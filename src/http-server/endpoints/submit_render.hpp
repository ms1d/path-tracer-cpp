#pragma once

#include "httplib.h"
#include "../helpers/gen_content.hpp"

inline void submit_render(const httplib::Request& req, httplib::Response& res) {
	auto body = req.body;
	res.set_content(gen_content(500, "Not implemented!"), "application/json");
	res.status = 500;
}
