#pragma once



#include "json.hpp"
#include "httplib.h"
#include "../helpers/gen_content.hpp"



inline void check_json(const httplib::Request& req, httplib::Response& res) {
	// Allow empty bodies - some endpoints don't use them
	if (req.body.empty()) return;

	try { nlohmann::json j = nlohmann::json::parse(req.body); }
	catch(std::exception) {
		res.status = 400;
		res.set_content(gen_content(400, "Malformed JSON"), "application/json");
	}
}
