#pragma once

#include "httplib.h"
#include "../helpers/gen_content.hpp"
#include <ctime>
#include <exception>
#include <format>
#include <fstream>
#include <stdexcept>

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
		if (verts_len % 3 != 0) throw std::runtime_error("Verts must have a length divisble by 3");
		if (tris_len % 3 != 0) throw std::runtime_error("Tris must have a length divisble by 3");
		for (int i = 0; i < tris_len; i++) { int tri = tris[i].get<int>(); if (tri >= verts_len / 3 || tri < 0) throw std::runtime_error("An element of tri references an invalid index on verts"); }
	} catch (const std::exception& e) {
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
