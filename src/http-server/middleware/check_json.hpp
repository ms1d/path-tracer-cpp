#pragma once



#include "json.hpp"
#include "httplib.h"
#include "../helpers/gen_content.hpp"



inline void check_json(const httplib::Request& req, httplib::Response& res) {
	try { nlohmann::json j = nlohmann::json::parse(req.body); }
	catch(std::exception) {
		res.status = 400;
		res.set_content(gen_content(400, "Malformed JSON"), "application/json");
	}
}
