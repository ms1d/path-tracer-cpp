#pragma once

#include "httplib.h"

inline void submit_render(const httplib::Request& req, httplib::Response& res) {
	auto body = req.body;
	res.status = 500;
	res.set_content("Not implemented!", "text/plain");
}
