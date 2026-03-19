#pragma once



#include "httplib.h"
#include <vector>
#include <unordered_set>



using httplib::Request;
using httplib::Response;



// Super simple implementation of middleware. LIMITATIONS:
//	- No true "short circuit" behaviour: if the request status has been set by a layer, ALL next layers are skipped (including on the way back!)
//	- No true "next()" behaviour - separate incoming and outgoing layers
//	Since this server API is mainly used internally these tradeoffs are perfectly adequate
//	The main purpose here is to enforce basic validation of requests and not to enforce complicated API logic

class middleware {
	


	private:
		std::vector<std::function<void(const Request&, Response&)>> incoming_layers;



		std::vector<std::function<void(const Request&, Response&)>> outgoing_layers;



		std::unordered_set<std::string> routes;



		void run_middleware_incoming(const Request& req, Response& res) {
			for (auto layer : incoming_layers) if(res.status == -1) layer(req, res);
		}

		void run_middleware_outgoing(const Request& req, Response& res) {
			for (int i = outgoing_layers.size() - 1; i >= 0; i--) if (res.status == -1) outgoing_layers[i](req, res);
		}



	public:



		void add_middleware(
				std::function<void(const Request&, Response&)> before,
				std::function<void(const Request&, Response&)> after
				) {

			incoming_layers.push_back(before);
			outgoing_layers.push_back(after);
		}



		// ALL API routes declared in controller.cpp MUST be wrapped to use middleware
		std::function<void(const Request&, Response&)>
			wrap_endpoint(const std::string& route, std::function<void(const Request& req, Response& res)> handler) {

			routes.emplace(route);

			return [handler, this](const Request& req, Response& res){
				run_middleware_incoming(req, res);

				// Only run handler if response has not already been set (basic short circuit)
				if (res.status == -1) handler(req, res);

				run_middleware_outgoing(req, res);
			};
		}



};
