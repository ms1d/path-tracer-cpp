#include <cstdint>
#include <fstream>
#include <unistd.h>
#include <filesystem>
#include <format>
#include <asio.hpp>
#include "json.hpp"
#include "renderer.cuh"



#ifndef __CUDACC__
#define __host__
#define __device__
#define __global__
#endif



void stream_result(nlohmann::json res, std::string ip, std::string port) {
	try { assert(port.length() == 4); asio::ip::make_address(ip); std::stoi(port); }
	catch (std::exception) { perror("JSON did not have valid 'ip' and 'port'!"); return; }

	asio::io_context io;
	asio::ip::udp::socket socket(io);
	socket.open(asio::ip::udp::v4());

	asio::ip::udp::endpoint target(
		asio::ip::make_address(ip), std::stoi(port)
	);

	// Replace with a while polling loop
	socket.send_to(asio::buffer(res.dump(4)), target);
}



void find_earliest_request(int& current_request) {
	std::filesystem::path dir = "path-tracer/requests";
	
	current_request = -1;

	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
		if (entry.is_regular_file() && entry.path().extension() == ".json") {
			std::string name = entry.path().stem().filename();
			if (current_request < 0 || current_request > std::stoi(name)) { current_request = std::stoi(name);}
		}
	}
}



void parse_request(const nlohmann::json& request,
		vec<3>*& verts, uint32_t& verts_len,
		uint32_t*& tris, uint32_t& tris_len,
		Materials& mats, uint32_t& mats_len,
		uint32_t*& mats_indices, uint32_t& mats_indices_len,
		vec<3> &camPos, vec<3> &camRot, float &camFov) {

	verts_len = request["verts"].size() / 3;
	verts = new vec<3>[verts_len];
	auto& verts_json = request["verts"];
	for (uint32_t i = 0; i < verts_len; i++) {
		auto x = verts_json[3 * i + 0].get<float>(),
			 y = verts_json[3 * i + 1].get<float>(),
			 z = verts_json[3 * i + 2].get<float>();
		verts[i] = vec<3>(x,y,z);
	}

	tris_len = request["tris"].size();
	tris = new uint32_t[tris_len];
	auto& tris_json = request["tris"];
	for (uint32_t i = 0; i < tris_len; i++) tris[i] = tris_json[i].get<uint32_t>();

	mats_len = request["mats"].size();
	mats.length = mats_len;
	mats.smoothness = new float[mats_len];
	mats.metallic = new float[mats_len];
	mats.colors = new vec<3>[mats_len];

	auto& mats_json = request["mats"];

	for (uint32_t i = 0; i < mats_len; i++) {
		auto& current_mat = mats_json[i];
		mats.smoothness[i] = current_mat["smoothness"].get<float>();
		mats.metallic[i] = current_mat["metallic"].get<float>();
		auto& rgb = current_mat["color"];
		auto r = rgb[0].get<float>(),
			 g = rgb[1].get<float>(),
			 b = rgb[2].get<float>();
		mats.colors[i] = vec<3>(r,g,b);
	}

	auto& mat_indices_json = request["mat_indices"];
	mats_indices_len = mat_indices_json.size();
	mats_indices = new uint32_t[mats_indices_len];
	for (uint32_t i = 0; i < mats_indices_len; i++) mats_indices[i] = mat_indices_json[i].get<uint32_t>();

	auto &camera_json = request["camera"],
	&pos_json = camera_json["pos"],
	&rot_json = camera_json["rot"];
	camPos.x = pos_json[0].get<float>(), camRot.x = rot_json[0].get<float>();
	camPos.y = pos_json[1].get<float>(), camRot.y = rot_json[1].get<float>();
	camPos.z = pos_json[2].get<float>(), camRot.z = rot_json[2].get<float>();
	camFov = camera_json["fov"].get<float>();
}



int main() {
	while (true) {
		int current_request;
		find_earliest_request(current_request);

		if (current_request < 0) { sleep(5); continue; }
		
		// Read the request
		std::string path_to_current_request = std::format("path-tracer/requests/{}.json", current_request).c_str();
		std::ifstream file(path_to_current_request);
		nlohmann::json request;

		try { file >> request; }
		catch (std::exception) {
			perror("Failed to read file!");
			return 1;
		}

		// Parse data + Trigger work start_render
		vec<3>* verts;
		uint32_t verts_len;
		uint32_t* tris;
		uint32_t tris_len;
		Materials mats;
		uint32_t mats_len;
		uint32_t* mats_indices;
		uint32_t mats_indices_len;
		vec<3> camPos, camRot;
		float camFov;
		
		parse_request(request, verts, verts_len, tris, tris_len, mats, mats_len, mats_indices, mats_indices_len, camPos, camRot, camFov);
		start_render(verts, verts_len, tris, tris_len, mats, mats_indices, mats_indices_len, camPos, camRot, camFov);

		// Trigger stream start - this will poll the buffer and stream back changes made
		stream_result(nlohmann::json(), request["ip"], request["port"]);

		// Once finished work, remove the request
		std::system(std::format("rm {}", path_to_current_request).c_str());

		delete[] verts;
		delete[] tris;
		delete[] mats.smoothness;
		delete[] mats.metallic;
		delete[] mats.colors;
		delete[] mats_indices;

	}

	return 1;
}
