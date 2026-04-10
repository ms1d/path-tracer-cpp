#pragma once



#include "httplib.h"
#include "../helpers/gen_content.hpp"
#include <cstdint>
#include <ctime>
#include <exception>
#include <format>
#include <fstream>
#include <stdexcept>
#include <asio.hpp>



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
// camera { float (fov), float[] (pos), float[] (rot) }
inline void submit_render(const httplib::Request& req, httplib::Response& res) {

	// Assert data validity
	try {
		nlohmann::json j = nlohmann::json::parse(req.body);

		try { std::string _ = j["render_id"].get<std::string>(); }
		catch (std::exception) { throw std::runtime_error("render_id (string) not found on JSON body"); }

		try { std::string ip = j["ip"]; asio::ip::make_address(ip); }
		catch (std::exception) { throw std::runtime_error("Valid ip (string) not found on JSON body"); }

		try { std::string port = j["port"]; if (port.length() != 4) throw std::exception(); }
		catch (std::exception) { throw std::runtime_error("Valid port (string) not found on JSON body"); }
		
		try {
			auto width = j["width"];
			if (!width.is_number_unsigned()
					|| !width.is_number_integer()
					|| width.get<int>() != width.get<uint16_t>()
					|| width.get<int>() == 0) throw std::exception();
		} catch (std::exception) { throw std::runtime_error("Valid width (uint16_t) not found on JSON body"); }

		try {
			auto height = j["height"];
			if (!height.is_number_unsigned()
					|| !height.is_number_integer()
					|| height.get<int>() != height.get<uint16_t>()
					|| height.get<int>() == 0) throw std::exception();
		} catch (std::exception) { throw std::runtime_error("Valid height (uint16_t) not found on JSON body"); }

		try {
			auto samples = j["samples"];
			if (!samples.is_number_unsigned()
					|| !samples.is_number_integer()
					|| samples.get<int>() != samples.get<uint16_t>()
					|| samples.get<int>() == 0) throw std::exception();
		} catch (std::exception) { throw std::runtime_error("Valid samples (uint16_t) not found on JSON body"); }

		try {
			auto pixels_to_skip = j["pixels_to_skip"];
			if (!pixels_to_skip.is_array()
					|| !pixels_to_skip[0].is_array()
					|| (pixels_to_skip[0].size() != 2 && pixels_to_skip[0].size() != 0)) throw std::exception();

			for (size_t i = 0; i < pixels_to_skip.size(); i++) {
				if (pixels_to_skip[i][0] >= j["height"].get<uint16_t>()
						|| pixels_to_skip[i][1] >= j["width"].get<uint16_t>()) throw std::exception();
				for (int j = 0; j < 2; j++)
					if (!pixels_to_skip[i][j].is_number_unsigned()
							|| pixels_to_skip[i][j].get<int>()
							!= pixels_to_skip[i][j].get<uint16_t>()) throw std::exception();
			if (!j.contains("pixels_to_skip")) return;
			}
		} catch (std::exception) { if(!j["pixels_to_skip"].is_null()) throw std::runtime_error("Valid pixels_to_skip (uint16_t[][]) not found on JSON body"); }

		try {
			float pos[3], rot[3];
			auto& json = j["camera"];
			if (json["pos"].size() != 3 || json["rot"].size() != 3) throw std::exception();
			pos[0] = json["pos"][0].get<float>(), rot[0] = json["rot"][0].get<float>();
			pos[1] = json["pos"][1].get<float>(), rot[1] = json["rot"][1].get<float>();
			pos[2] = json["pos"][2].get<float>(), rot[2] = json["rot"][2].get<float>();
		} catch (std::exception) { throw std::runtime_error("Valid camera position & rotation (float[3]) not found on JSON body"); }

		try { float _ = j["camera"]["fov"].get<float>(); } catch (std::exception) { throw std::runtime_error("Valid fov (float) not found on JSON body"); }

		uint32_t verts_len = j["verts"].size();
		uint32_t tris_len = j["tris"].size();

		if (verts_len % 3 != 0) throw std::runtime_error("Verts must have a length divisble by 3");
		if (tris_len % 3 != 0) throw std::runtime_error("Tris must have a length divisble by 3");

		auto& tris = j["tris"];
		for (uint32_t i = 0; i < tris_len; i++) {
			uint32_t tri = tris[i].get<uint32_t>();
			if (tri >= verts_len / 3) throw std::runtime_error("An element of tri references an invalid index on verts");
		}

		auto& mats_indices = j["mat_indices"], mats = j["mats"];
		uint32_t mats_len = mats.size(), mats_indices_len = j["mat_indices"].size();
		for (uint32_t i = 0; i < mats_indices_len; i++) {
			uint32_t index = mats_indices[i].get<uint32_t>();
			if (index >= mats_len) throw std::runtime_error("An element of mats_indices references an invalid index on mats");
		}

		// Ensure materials meet schema
		if (!mats[0].contains("smoothness") || !mats[0]["smoothness"].is_number_unsigned() || __builtin_fabs(mats[0]["smoothness"]) > 1
				|| !mats[0].contains("metallic") || !mats[0]["metallic"].is_number_unsigned() || __builtin_fabs(mats[0]["metallic"]) > 1
				|| !mats[0].contains("emission") || !mats[0]["emission"].is_number_unsigned()
				|| !mats[0].contains("rgb") || !mats[0]["rgb"].is_array() || mats[0]["rgb"].size() != 3) {
			throw std::runtime_error("Materials must have smoothness, metallic, emission and rgb values");
		}
	}

	catch (const std::exception& e) {
		res.set_content(gen_content(400, std::format("Invalid JSON. Exception: {}", e.what()).c_str()), "application/json");
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
