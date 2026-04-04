#pragma once



#include "httplib.h"
#include "json.hpp"
#include "../helpers/gen_content.hpp"
#include <fstream>



using nlohmann::json;



// Simple health check to see if the system is up
// Used internally by ASP.NET backend
inline void health(const httplib::Request& _, httplib::Response& res) {
	json status;
	std::ifstream file("state/status.json");

	try { file >> status; }
	catch (std::exception) {
		res.status = 500;
		res.set_content(gen_content(500, "Failed to read status.json"), "text/plain");
		return;
	}

	res.status = 200;
	res.set_content(status.dump(4), "application/json");

	file.close();
}
