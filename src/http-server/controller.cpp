#include "httplib.h"
#include "json.hpp"
#include "endpoints/health.hpp"
#include "endpoints/submit_render.hpp"
#include "middleware/middleware_base.hpp"

using nlohmann::json;

int main() {
	httplib::Server svr;
	middleware mw;

	// Register middleware here
	mw.add_middleware([](const Request&, Response& res) {
		std::cout << "hello from layer 1!" << std::endl;
		res.status = 200;
	}, [](const Request&, Response&) {
	std::cout << "goodbye from layer 1!" << std::endl;
	});

	mw.add_middleware([](const Request&, Response&) {
		std::cout << "hello from layer 2!" << std::endl;
	}, [](const Request&, Response&) {
	std::cout << "goodbye from layer 2!" << std::endl;
	});

	// Call endpoints
	svr.Get("/health", mw.wrap_endpoint("/health", health));
	svr.Post("/submit-render", mw.wrap_endpoint("/submit-render", submit_render));

	svr.listen("localhost", 8080);

	return 1;
}
