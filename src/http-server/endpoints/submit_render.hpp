#pragma once

#include "httplib.h"
#include "../helpers/gen_content.hpp"
#include <ctime>
#include <exception>
#include <format>
#include <fstream>

// Body has the form:
// render_id: string
// ip: string
// port: string
// tilesize: int (usually 64)
// samples: int
// tiles_to_render: bool[][], where tiles_to_render[i][j] = true means
//	that the (i+1)th tile to the right and (j+1)th tile down should be rendered
//
// verts: float[]. every 3 floats = 1 vertex
// tris: int[]. every 3 ints = 1 triangle. each element is an index of verts
// mats: { float (smoothness), float (metallic), float3 (rgb) }
// mat_indices: int[]. each element is an index of materials and represents a triangle in order
// camera { float (fov), float3 (pos), float3 (rot) }
inline void submit_render(const httplib::Request& req, httplib::Response& res) {
	auto body = req.body;

	// Assert data validity
	try {
		nlohmann::json j = nlohmann::json::parse(body);
		int verts_len = j["verts"].size();
		int tris_len = j["tris"].size();
		auto& tris = j["tris"];
		assert(verts_len % 3 == 0);
		assert(tris_len % 3 == 0);
		for (int i = 0; i < tris_len; i++) { int tri = tris[i].get<int>(); assert(tri < verts_len / 3 && tri >= 0); }
	} catch (std::exception) {
		res.set_content(gen_content(400, "Invalid JSON"), "application/json");
		res.status = 400;
		return;
	}


	auto time_since_epoch = std::time(nullptr);
	std::string command = std::format("mkdir -p path-tracer/requests && touch path-tracer/requests/{}.json", time_since_epoch);
	std::system(command.c_str());

	std::ofstream file(std::format("path-tracer/requests/{}.json", time_since_epoch));
	try { file << req.body; }
	catch(std::exception) {
		res.set_content(gen_content(500, "Failed to dump to file."), "application/json");
		res.status = 500; return;
	}
	file.close();

	res.set_content(gen_content(200, "On it's way!"), "application/json");
	res.status = 200;
}
