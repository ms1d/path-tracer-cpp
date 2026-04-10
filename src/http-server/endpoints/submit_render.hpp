#pragma once



#include "asio/ip/address.hpp"
#include "httplib.h"
#include "../helpers/gen_content.hpp"
#include "../helpers/decompress.hpp"
#include <cstdint>
#include <ctime>
#include <exception>
#include <format>
#include <fstream>
#include <stdexcept>
#include <asio.hpp>



namespace keys {
	constexpr auto render_id = "render_id";
	constexpr auto ip = "ip";
	constexpr auto port = "port";
	constexpr auto width = "width";
	constexpr auto height = "height";
	constexpr auto samples = "samples";
	constexpr auto pixels_to_skip = "pixels_to_skip";
	constexpr auto verts = "verts";
	constexpr auto tris = "tris";
	constexpr auto mats = "mats";
	constexpr auto mat_indices = "mat_indices";
	constexpr auto camera = "camera";
	constexpr auto fov = "fov";
	constexpr auto pos = "pos";
	constexpr auto rot = "rot";
	constexpr auto emission = "emission";
	constexpr auto smoothness = "smoothness";
	constexpr auto metallic = "metallic";
	constexpr auto rgb = "rgb";
}



constexpr int number_of_keys_on_json_body = 12;



inline void enforce_string(const nlohmann::json &json, const std::string &key) {
	if (!json.contains(key) || !json[key].is_string())
		throw std::runtime_error(std::format("Valid {} (string) not found on JSON body", key));
}



inline void enforce_valid_udp_details(const nlohmann::json &json) {
	enforce_string(json, keys::ip); enforce_string(json, keys::port);

	try { asio::ip::make_address(json[keys::ip].get<std::string>()); }
	catch (std::exception) { throw std::runtime_error("Valid ip (string) not found on JSON body"); }

	if (json[keys::port].get<std::string>().length() != 4)
		throw std::runtime_error("Valid port (string) not found on JSON body");
}



inline void enforce_nonzero_uint16_t(const nlohmann::json &json, const std::string &key) {
	auto msg = std::format("Valid {} (uint16_t) not found on JSON body", key);
	if (!json.contains(key)) throw std::runtime_error(msg);

	auto &val_json = json[key];
    if (!val_json.is_number_unsigned() || !val_json.is_number_integer())
		throw std::runtime_error(msg);

	int val = val_json.get<int>();
	if (val > 65535 || val == 0) throw std::runtime_error(msg);
}


inline void enforce_valid_pixels_to_skip(const nlohmann::json &json) {	
	if (!json.contains(keys::pixels_to_skip)) return;
	
	try {
		auto pixels_to_skip = json[keys::pixels_to_skip];
		if (!pixels_to_skip.is_array()
				|| !pixels_to_skip[0].is_array()
				|| (pixels_to_skip[0].size() != 2 && pixels_to_skip[0].size() != 0)) throw std::exception();

		for (size_t i = 0; i < pixels_to_skip.size(); i++) {
			if (pixels_to_skip[i][0] >= json[keys::height].get<uint16_t>()
					|| pixels_to_skip[i][1] >= json[keys::width].get<uint16_t>()) throw std::exception();
			for (int j = 0; j < 2; j++)
				if (!pixels_to_skip[i][j].is_number_unsigned()
						|| pixels_to_skip[i][j].get<int>()
						!= pixels_to_skip[i][j].get<uint16_t>()) throw std::exception();
		}
	} catch (std::exception) { throw std::runtime_error("Valid pixels_to_skip (uint16_t[][]) not found on JSON body"); }
}



inline void enforce_valid_camera(const nlohmann::json &json) {
	auto& cam_json = json[keys::camera];

	try {
		float pos[3], rot[3];
		auto pos_json = cam_json[keys::pos], rot_json = cam_json[keys::rot];
		if (!pos_json.is_array() || !rot_json.is_array() ||
				pos_json.size() != 3 || rot_json.size() != 3) throw std::exception();

		pos[0] = pos_json[0].get<float>(), rot[0] = rot_json[0].get<float>();
		pos[1] = pos_json[1].get<float>(), rot[1] = rot_json[1].get<float>();
		pos[2] = pos_json[2].get<float>(), rot[2] = rot_json[2].get<float>();
	} catch (std::exception) { throw std::runtime_error("Valid camera position & rotation (float[3]) not found on JSON body"); }
	
	try {
		float fov = cam_json[keys::fov].get<float>();
		if (fov < 0 || fov > 180) throw std::exception();
	} catch (std::exception) { throw std::runtime_error("Valid fov (float) not found on JSON body"); }
}



inline void enforce_valid_geometry(const nlohmann::json &json) {
	uint32_t verts_len = json[keys::verts].size();
	uint32_t tris_len = json[keys::tris].size();

	if (verts_len % 3 != 0) throw std::runtime_error("Verts must have a length divisble by 3");
	if (tris_len % 3 != 0) throw std::runtime_error("Tris must have a length divisble by 3");

	auto& tris = json[keys::tris];
	for (uint32_t i = 0; i < tris_len; i++) {
		uint32_t tri = tris[i].get<uint32_t>();
		if (tri >= verts_len / 3) throw std::runtime_error("An element of tri references an invalid index on verts");
	}
}



inline void enforce_valid_materials(const nlohmann::json &json) {
	auto mats_indices = json[keys::mat_indices], mats = json[keys::mats];
	uint32_t mats_len = mats.size(), mats_indices_len = json[keys::mat_indices].size();
	for (uint32_t i = 0; i < mats_indices_len; i++) {
		uint32_t index = mats_indices[i].get<uint32_t>();
		if (index >= mats_len) throw std::runtime_error("An element of mats_indices references an invalid index on mats");
	}

	auto smoothness = mats[0][keys::smoothness],
		 metallic = mats[0][keys::metallic],
		 emission = mats[0][keys::emission],
		 rgb = mats[0][keys::rgb];
	std::vector<nlohmann::json> v = {smoothness, metallic, emission};

	for (auto &ele : v) {
		if (ele.is_null()) throw std::runtime_error("Could not find smoothness, metallic and/or emission on provided materials.");
		if (!ele.is_number()) throw std::runtime_error("Smoothness, metallic and/or emission is not a number");
		if (ele.get<float>() < 0) throw std::runtime_error("Smoothness, metallic and/or emission is negative");
	}

	if (__builtin_fabs(smoothness.get<float>()) > 1 || __builtin_fabs(metallic.get<float>()) > 1)
		throw std::runtime_error("Materials must have smoothness + metallic values between 0 and 1");

	if (rgb.is_null() || !rgb.is_array() || rgb.size() != 3) throw std::runtime_error("Materials must have rgb values (float[3])");
}



inline void create_request_file(const nlohmann::json &content) {
	auto time_since_epoch = std::time(nullptr);
	std::string command = std::format("mkdir -p path-tracer/requests && touch path-tracer/requests/{}.json", time_since_epoch);
	std::system(command.c_str());

	std::ofstream file(std::format("path-tracer/requests/{}.json", time_since_epoch));
	file << content;
	file.close();
}



inline void populate_parsed_body(nlohmann::json &out, const nlohmann::json &in, const std::array<std::string, number_of_keys_on_json_body> &keys) {
	for (auto& key : keys) if (in.contains(key)) out[key] = in[key]; 
}



// Body has the form:
// render_id: string
// ip: string
// port: string
// width: uint16_t
// height: uint16_t
// samples: uint16_t
// OPTIONAL pixels_to_skip: uint16_t[][]. co-ordinates of all pixels to skip
//	pixel[i][0] is the ith pixels x co-ordinate. pixel[i][1] is the ith pixels y co-ordinate
//
// verts: float[]. every 3 floats = 1 vertex
// tris: uint32_t[]. every 3 ints = 1 triangle. each element is an index of verts
// mats: [{ float (smoothness), float (metallic), float (emission), float[] (rgb) }]
// mat_indices: uint8_t[]. each element is an index of materials and represents a triangle in order
// camera { float (fov between 0 and 180 exclusive), float[] (pos), float[] (rot) }
inline void submit_render(const httplib::Request& req, httplib::Response& res) {
	nlohmann::json parsed_body, incoming_body;

	// Accept compressed payloads via gzip
	if (req.has_header("Content-Encoding") && req.get_header_value("Content-Encoding") == "gzip") {
		std::cout << "here!" << std::endl;
		try { incoming_body = nlohmann::json::parse(gzip_decompress(req.body)); }
		catch (const std::exception &e) {
			res.status = 400;
			res.set_content(gen_content(400, "Could not decompress incoming body"), "application/json");
			return;
		}
	}

	else { incoming_body = nlohmann::json::parse(req.body); }


	// Assert data validity
	try {
		enforce_string(incoming_body, keys::render_id);

		enforce_valid_udp_details(incoming_body);
		
		enforce_nonzero_uint16_t(incoming_body, keys::width);
		enforce_nonzero_uint16_t(incoming_body, keys::height);
		enforce_nonzero_uint16_t(incoming_body, keys::samples);

		enforce_valid_pixels_to_skip(incoming_body);

		enforce_valid_camera(incoming_body);

		enforce_valid_geometry(incoming_body);
		enforce_valid_materials(incoming_body);
	}

	catch (const std::exception& e) {
		res.set_content(gen_content(400, std::format("Invalid JSON. Exception: {}", e.what()).c_str()), "application/json");
		res.status = 400;
		return;
	}

	std::array<std::string, number_of_keys_on_json_body> keys = {
		keys::render_id,
		keys::ip, keys::port,
		keys::width, keys::height, keys::samples,
		keys::pixels_to_skip,
		keys::verts, keys::tris, keys::mats, keys::mat_indices,
		keys::camera
	};

	populate_parsed_body(parsed_body, incoming_body, keys);

	try { create_request_file(parsed_body); }
	catch (std::exception) {
		res.set_content(gen_content(500, "Failed to dump to file."), "application/json");
		res.status = 500; return;
	}
	

	res.set_content(gen_content(200, "On it's way!"), "application/json");
	res.status = 200;
}
