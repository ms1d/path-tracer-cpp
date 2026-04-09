#include "request_handler.hpp"
#include "json.hpp"
#include "structs.cuh"
#include <format>
#include <fstream>
#include <thread>
#include "renderer.cuh"



// sleep period to sleep for when waiting for the GPU to fill up buffers
constexpr int cpu_sleep_period = 1;



void parse_request(const nlohmann::json& request,
		vec<3>*& verts, uint32_t& verts_len,
		uint32_t*& tris, uint32_t& tris_len,
		Materials& mats,
		uint32_t*& mats_indices, uint32_t& mats_indices_len,
		vec<3> &camPos, vec<3> &camRot, float &camFov,
		uint8_t &tilesize) {

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

	auto& mats_json = request["mats"];
	uint32_t mats_len = mats_json.size();
	mats.length = mats_len;
	mats.smoothness = new float[mats_len];
	mats.metallic = new float[mats_len];
	mats.colors = new vec<3>[mats_len];

	for (uint32_t i = 0; i < mats_len; i++) {
		auto& current_mat = mats_json[i];
		mats.smoothness[i] = current_mat["smoothness"].get<float>();
		mats.metallic[i] = current_mat["metallic"].get<float>();
		auto& rgb = current_mat["rgb"];
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

	tilesize = request["tilesize"].get<uint8_t>();
}



void handle_request(int current_request, Pixel **buffers, bool &lock_state, int buffers_count) {
	std::string path_to_current_request = std::format("path-tracer/requests/in_progress/{}.json", current_request).c_str();
	std::ifstream file(path_to_current_request);
	nlohmann::json request;

	try { file >> request; }
	catch (std::exception) {
		perror("Failed to read file!");
		_exit(1);
	}

	vec<3>* verts;
	uint32_t verts_len;
	uint32_t* tris;
	uint32_t tris_len;
	Materials mats;
	uint32_t* mats_indices;
	uint32_t mats_indices_len;
	vec<3> camPos, camRot;
	float camFov;
	uint8_t tilesize;
	
	parse_request(request,
			verts, verts_len,
			tris, tris_len,
			mats,
			mats_indices, mats_indices_len,
			camPos, camRot, camFov,
			tilesize);

	std::atomic<int> curr_gpu_buffer = 0;

	std::thread(start_render,
			verts, verts_len,
			tris, tris_len,
			mats,
			mats_indices, mats_indices_len,
			camPos, camRot, camFov,
			tilesize,
			buffers, std::ref(curr_gpu_buffer), std::ref(buffers_count))
		.detach();

	int curr_cpu_buffer = 0;
	while (true) {
		if (curr_gpu_buffer.load() == -1) break; // in reality, just signal that the loop should end soon before finsihing off work. dont just break
		else if (curr_gpu_buffer.load() <= curr_cpu_buffer) { sleep(cpu_sleep_period); continue; }

		// for now, just print something for testing
		sleep(1);
		curr_cpu_buffer++;
		std::cout << "Buffer moved up! Now on buffer " << curr_cpu_buffer << std::endl;

		// work with this index
		int curr_cpu_buffer_index = curr_cpu_buffer % buffers_count;
	}

	// Once finished work, remove the request + delete heap data + mark lock as free
	std::system(std::format("rm {}", path_to_current_request).c_str());

	delete[] verts;
	delete[] tris;
	delete[] mats.smoothness;
	delete[] mats.metallic;
	delete[] mats.colors;
	delete[] mats_indices;

	std::cout << "done" << std::endl;
	lock_state = false;
}
