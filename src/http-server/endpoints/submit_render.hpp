#pragma once



#include "asio/ip/address.hpp"
#include "httplib.h"
#include "../helpers/gen_content.hpp"
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
	constexpr auto mats = "mats";
	constexpr auto camera = "camera";
	constexpr auto fov = "fov";
	constexpr auto pos = "pos";
	constexpr auto rot = "rot";
	constexpr auto scale = "scale";
	constexpr auto emission = "emission";
	constexpr auto smoothness = "smoothness";
	constexpr auto metallic = "metallic";
	constexpr auto rgb = "rgb";
	constexpr auto objects = "objects";
	constexpr auto mesh_id = "mesh_id";
	constexpr auto mat_index = "mat_index";
}



constexpr int number_of_keys_on_json_body = 12;



inline void enforce_string(const nlohmann::json &json, const std::string &key) {
	if (!json.contains(key) || !json[key].is_string())
		throw std::runtime_error(std::format("Valid {} (string) not found on JSON body", key));
}


inline void enforce_valid_udp_details(const nlohmann::json &json) {
	enforce_string(json, keys::ip); enforce_string(json, keys::port);

	try { asio::ip::make_address(json[keys::ip].get<std::string>()); }
	catch (std::exception) {
		auto msg = std::format("Valid {} (string) not found on JSON body", keys::ip);
		throw std::runtime_error(msg);
	}

	if (json[keys::port].get<std::string>().length() != 4) {
		auto msg = std::format("Valid {} (string) of length 4 not found on JSON body", keys::port);
		throw std::runtime_error(msg);
	}
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


// Depends on height and width having been validated
inline void enforce_valid_pixels_to_skip(const nlohmann::json &json) {	
	if (!json.contains(keys::pixels_to_skip)) return;
	
	auto msg_prefix = std::format("Valid {} (uint16_t[][]) not found on JSON body", keys::pixels_to_skip);
	auto &pixels_to_skip = json[keys::pixels_to_skip];
	if (!pixels_to_skip.is_array() || pixels_to_skip.size() == 0 || !pixels_to_skip[0].is_array()) {
		auto msg = msg_prefix + ": could not find 2D array";
		throw std::runtime_error(msg);
	}

	if (!pixels_to_skip[0][0].is_number_unsigned()) {
		auto msg = msg_prefix + ": array does not include unsigned integers";
		throw std::runtime_error(msg);
	} 

	auto height = json[keys::height].get<uint16_t>();
    auto width = json[keys::width].get<uint16_t>();

	for (size_t i = 0; i < pixels_to_skip.size(); i++) {
		if (pixels_to_skip[i].size() != 2) {
			auto msg = msg_prefix + ": inner arrays are not all of length 2";
			throw std::runtime_error(msg);
		}

		if (!pixels_to_skip[i][0].is_number_unsigned() || !pixels_to_skip[i][1].is_number_unsigned()) {
			auto msg = msg_prefix + ": inner arrays do not all include unsigned integers";
			throw std::runtime_error(msg);
		}

		// cast to uint64_t to avoid overflow causing false positives
		if (pixels_to_skip[i][0].get<uint64_t>() >= width || pixels_to_skip[i][1].get<uint64_t>() >= height) {
			auto msg = msg_prefix + std::format(": element '[{},{}]' references pixels out of bounds",
					pixels_to_skip[i][0].get<uint64_t>(), pixels_to_skip[i][1].get<uint64_t>());
			throw std::runtime_error(msg);
		}
	}
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
	} catch (std::exception) {
		auto msg = std::format("Valid {}['{}'] & {}['{}'] (float[3]) not found on JSON body",
				keys::camera, keys::pos, keys::camera, keys::rot);
		throw std::runtime_error(msg);
	}
	
	try {
		float fov = cam_json[keys::fov].get<float>();
		if (fov < 0 || fov > 180) throw std::exception();
	} catch (std::exception) {
		auto msg = std::format("Valid {}['{}'] (float) not found on JSON body", keys::camera, keys::fov);
		throw std::runtime_error(msg);
	}
}


inline void enforce_valid_materials(const nlohmann::json &json) {
	auto mats = json[keys::mats];

	// Each object's mat_index is a uint8_t, allowing for only upto 256 materials to be used
	if (mats.size() > 256) throw std::runtime_error("Too many materials in JSON body (max is 256)");

	for (size_t i = 0; i < mats.size(); i++) {
		auto &smoothness = mats[i][keys::smoothness],
			 &metallic = mats[i][keys::metallic],
			 &emission = mats[i][keys::emission],
			 &rgb = mats[i][keys::rgb];
	
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


}


// Depends on materials already having been validated
inline void enforce_valid_objects(const nlohmann::json &json) {
	if (!json.contains(keys::objects)) throw std::runtime_error("Could not find objects key on JSON body.");

	auto &objs = json[keys::objects];
	if (!objs.is_array()) throw std::runtime_error("objects was not array on JSON body.");
	if (objs.size() == 0) throw std::runtime_error("objects array is empty.");

	auto mats_len = json[keys::mats].size();
	std::vector<const char*> pos_rot_scale = { keys::pos, keys::rot, keys::scale };
	for (size_t i = 0; i < objs.size(); i++) {
		auto &obj = objs[i];


		if (!obj.contains(keys::mesh_id) || !obj[keys::mesh_id].is_string()) {
			auto msg = std::format("mesh_id (string) not found at objects[{}]", i);
			throw std::runtime_error(msg);
		}

		std::fstream file(std::format("path-tracer/meshes/{}.mesh", obj[keys::mesh_id].get<std::string>()));
		if (!file) {
			auto msg = std::format("Could not find mesh with id '{}' at objects[{}]", obj[keys::mesh_id].get<std::string>(), i);
			throw std::runtime_error(msg);
		}
		
		if (!obj.contains(keys::mat_index) || obj[keys::mat_index].get<uint8_t>() >= mats_len) {
			auto msg = std::format("Valid mat_index not found at objects[{}]", i);
			throw std::runtime_error(msg);
		}

	
		for (const auto &property : pos_rot_scale) {
			if (!obj.contains(property) || !obj[property].is_array() || obj[property].size() != 3
					|| !obj[property][0].is_number() || !obj[property][1].is_number() || !obj[property][2].is_number()) {
				auto msg = std::format("Valid {} not found at objects[{}]", property, i);
				throw std::runtime_error(msg);
			}
		}
	}
}



inline void create_request_file(const nlohmann::json &content) {
	auto time_since_epoch = std::time(nullptr);
	std::string command = std::format("mkdir -p path-tracer/requests && touch path-tracer/requests/{}.json", time_since_epoch);
	std::system(command.c_str());

	std::ofstream file(std::format("path-tracer/requests/{}.json", time_since_epoch));
	file << content;
	file.close();
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
// mats: [{ smoothness: float, metallic: float, emission: float, rgb: float[3] }]
// camera { fov (between 0 and 180 exclusive): float, pos: float[3], rot: float[3] }
// objects [{ mesh_id: string, mat_index (references mats[index]): uint8_t, pos: float[3], rot: float[3], scale: float[3] }}]
inline void submit_render(const httplib::Request& req, httplib::Response& res) {
	if (!req.has_header("Content-Type") || req.get_header_value("Content-Type") != std::string("application/json")) {
		res.status = 400;
		res.set_content(gen_content(400, "Content-Type header was not found, or was not application/json."), "application/json");
		return;
	}

	nlohmann::json incoming_body = nlohmann::json::parse(req.body);

	// Assert data validity
	try {
		enforce_string(incoming_body, keys::render_id);

		enforce_valid_udp_details(incoming_body);
		
		enforce_nonzero_uint16_t(incoming_body, keys::width);
		enforce_nonzero_uint16_t(incoming_body, keys::height);
		enforce_nonzero_uint16_t(incoming_body, keys::samples);

		enforce_valid_pixels_to_skip(incoming_body);

		enforce_valid_camera(incoming_body);

		enforce_valid_materials(incoming_body);
		enforce_valid_objects(incoming_body);
	}

	catch (const std::exception& e) {
		res.set_content(gen_content(400, std::format("Invalid JSON. Exception: {}", e.what()).c_str()), "application/json");
		res.status = 400;
		return;
	}

	try { create_request_file(req.body); }
	catch (std::exception) {
		res.set_content(gen_content(500, "Failed to dump to file."), "application/json");
		res.status = 500; return;
	}
	

	res.set_content(gen_content(200, "On it's way!"), "application/json");
	res.status = 200;
}
