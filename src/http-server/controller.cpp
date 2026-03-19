#include "httplib.h"
#include "json.hpp"
#include "endpoints/health.hpp"
#include "endpoints/submit_render.hpp"

using nlohmann::json;

int main() {
	httplib::Server svr;

	// Call endpoints
	svr.Get("/health", health);
	svr.Post("/submit-render", submit_render);

	svr.listen("localhost", 8080);

	return 1;
}
