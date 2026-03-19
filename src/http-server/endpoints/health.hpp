#pragma once

#include "httplib.h"

// Simple health check to see if the http-server is up
inline void health(const httplib::Request& _, httplib::Response& res) { res.status = 200; res.set_content("Hello World!", "text/plain"); }
