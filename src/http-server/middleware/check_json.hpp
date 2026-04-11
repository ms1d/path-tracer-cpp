#pragma once



#include "json.hpp"
#include "httplib.h"
#include "../helpers/gen_content.hpp"



inline void check_json(const httplib::Request& req, httplib::Response& res) {
	// Allow empty && non-json bodies
	if (req.body.empty()
			|| (req.has_header("Content-Type") && req.get_header_value("Content-Type") != std::string("application/json"))) return;

	try { nlohmann::json j = nlohmann::json::parse(req.body); }
	catch(std::exception) {
		res.status = 400;
		res.set_content(gen_content(400, "Malformed JSON"), "application/json");
	}
}
