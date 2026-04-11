#include "httplib.h"
#include "endpoints/health.hpp"
#include "endpoints/submit_render.hpp"
#include "endpoints/upload_mesh.hpp"
#include "middleware/middleware_base.hpp"
#include "middleware/check_json.hpp"



constexpr int max_payload_length = 50e6;



int main() {
	httplib::Server svr;
	middleware mw;

	// Register middleware here
	// Order matters! (FILO)
	mw.add_middleware(check_json, [](const Request&, Response&) {});

	// Call endpoints. MUST wrap to use middleware
	svr.Get("/health", mw.wrap_endpoint("/health", health));
	svr.Post("/submit-render", mw.wrap_endpoint("/submit-render", submit_render));
	svr.Post("/upload-mesh", mw.wrap_endpoint("/upload-mesh", upload_mesh));

	svr.set_payload_max_length(max_payload_length);

	svr.listen("localhost", 8080);

	return 1;
}
