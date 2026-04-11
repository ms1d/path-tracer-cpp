#pragma once



#include "httplib.h"
#include "../helpers/gen_content.hpp"
#include <exception>
#include <format>
#include <fstream>
#include <stdexcept>



// Supports only little endian layouts
// Enforces that all triangle indices
// are not out of range of verts ONLY
inline void validate_mesh(const char *body) {
	// how many vertices are in the body
	// each vertex is 3 * 4 byte floats
	uint32_t verts_len =
		(uint32_t)body[0]
		| ((uint32_t)body[1] << 8)
		| ((uint32_t)body[2] << 16)
		| ((uint32_t)body[3] << 24);
	
	uint64_t tris_len_index = 4 + 4 * 3 * verts_len;
	// how many triangles are in the body
	// each triangle is 3 * 4 byte uints
	uint32_t tris_len =
		(uint32_t)body[tris_len_index]
		| ((uint32_t)body[tris_len_index + 1] << 8)
		| ((uint32_t)body[tris_len_index + 2] << 16)
		| ((uint32_t)body[tris_len_index + 3] << 24);

	for (uint64_t i = tris_len_index; i < tris_len_index + tris_len; i += 4) {
		uint32_t index =
			(uint32_t)body[i]
			| ((uint32_t)body[i+1] << 8)
			| ((uint32_t)body[i+2] << 16)
			| ((uint32_t)body[i+3] << 24);
		if (index >= verts_len) throw std::runtime_error("Element of tris references an invalid index on verts.");
	}
}



inline void upload_mesh(const httplib::Request& req, httplib::Response& res) {
	if (req.body.empty()) {
		res.status = 400;
		res.set_content(gen_content(400, "No data provided"), "application/json");
		return;
	}

	try { validate_mesh(req.body.data()); }
	catch (const std::exception &e) {
		res.status = 400;
		res.set_content(gen_content(400, std::format("Malformed data recieved. Details: {}", e.what())), "application/json");
		return;
	}

	constexpr uint8_t id_length = 32;
	constexpr char charset[] = "abcdefABCDEF";
	// sizeof charset includes the terminator
	constexpr size_t charset_len = sizeof(charset) - 1;

	std::string id, raw_id(id_length, '\0');
	id.reserve(id_length);
	try {
		std::filesystem::create_directories("path-tracer/meshes");
		std::ifstream("/dev/urandom", std::ios::binary).read(&raw_id[0], id_length);

		for (char &c : raw_id) id.push_back(charset[(int)c % charset_len]);

		std::ofstream output("path-tracer/meshes/" + id + ".mesh", std::ios::binary);
		output << req.body;
		output.close();

	} catch (std::exception) {
		res.status = 500;
		res.set_content(gen_content(500, "Unable to save mesh to disk"), "application/json");
		return;
	}

	res.status = 200;
	res.set_content(gen_content(200, "OK. Id: " + id), "application/json");
}
