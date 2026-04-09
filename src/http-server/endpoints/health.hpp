#pragma once



#include "httplib.h"
#include "json.hpp"
#include "../helpers/gen_content.hpp"



using nlohmann::json;



// Simple health check to see if the system is up
// Used internally by ASP.NET backend
inline void health(const httplib::Request& _, httplib::Response& res) {
	res.status = 200;
	res.set_content(gen_content(200, "Accepting HTTP requests."), "application/json");
}
