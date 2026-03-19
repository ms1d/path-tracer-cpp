#pragma once

#include "httplib.h"
#include "json.hpp"
#include <fstream>

using nlohmann::json;

// Simple health check to see if the http-server is up
inline void health(const httplib::Request& _, httplib::Response& res) {
	json status;
	std::ifstream file("state/status.json");

	if (!file.is_open()) {
		std::cerr << "Failed to open file 'state/status.json'.\n";
		res.status = 500;
		res.set_content("Failed to open file 'state/status.json'.\n", "text/plain");
		return;
	}

	try { file >> status; }
	catch (std::exception) {
		res.status = 500;
		res.set_content("Internal JSON was malformed!", "text/plain");
		return;
	}

	res.status = 200;
	res.set_content(status.dump(4), "application/json");
}
