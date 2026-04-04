#pragma once

#include "httplib.h"
#include "../helpers/gen_content.hpp"
#include <ctime>
#include <format>
#include <fstream>

// Body has the form:
// render_id: string
// ip: int
// port: int
inline void submit_render(const httplib::Request& req, httplib::Response& res) {
	auto body = req.body;

	auto time_since_epoch = std::time(nullptr);
	std::string command = std::format("mkdir -p path-tracer/requests && touch path-tracer/requests/{}.json", time_since_epoch);

	std::system(command.c_str());

	std::ofstream file(std::format("path-tracer/requests/{}.json", time_since_epoch));

	try { file << req.body; }
	catch(std::exception) {
		res.set_content(gen_content(500, "Failed to dump to file"), "application/json");
		res.status = 500; return;
	}

	file.close();

	res.set_content(gen_content(200, "On it's way!"), "application/json");
	res.status = 200;
}
