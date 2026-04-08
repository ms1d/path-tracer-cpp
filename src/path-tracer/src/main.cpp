#include <string>
#include <unistd.h>
#include <filesystem>
#include <asio.hpp>
#include "request_handler.hpp"
#include "structs.cuh"



// Number of buffers per multi-buffer
constexpr int buffers_count = 3;
// Default size of each buffer
constexpr int buffer_size = 1e8;
// Maximum + current number of requests being handled at once. Determines how many mutli-buffers are allocated at program start
constexpr int max_requests = 3;
// Period to sleep if no requests are available
constexpr int sleep_period = 5;



void stream_result(Pixel* buffer, std::string ip, std::string port) {
	try { assert(port.length() == 4); asio::ip::make_address(ip); std::stoi(port); }
	catch (std::exception) { perror("JSON did not have valid 'ip' and 'port'!"); exit(1); }

	asio::io_context io;
	asio::ip::udp::socket socket(io);
	socket.open(asio::ip::udp::v4());

	asio::ip::udp::endpoint target(
		asio::ip::make_address(ip), std::stoi(port)
	);

	// Replace with a while polling loop
	socket.send_to(asio::buffer(std::to_string(buffer[0].b)), target);
}



void find_earliest_request(int& current_request) {
	std::filesystem::path dir = "path-tracer/requests";
	
	current_request = -1;

	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
		if (entry.is_regular_file() && entry.path().extension() == ".json") {
			std::string name = entry.path().stem().filename();
			if (current_request < 0 || current_request > std::stoi(name)) { current_request = std::stoi(name);}
		}
	}
}



int main() {
	// Buffers setup
	// buffers is a 3D array. Each element is a multi-buffer for 1 thread ONLY
	Pixel*** buffers = new Pixel**[max_requests];
	for (int i = 0; i < max_requests; i++) {
		buffers[i] = new Pixel*[buffers_count];
		for (int j = 0; j < buffers_count; j++) {
			buffers[i][j] = new Pixel[buffer_size];
		}
	}

	// The ith buffer owns the ith lock state (used by the ith thread running)
	bool lock_states[max_requests] = { };
	int current_request = -1;

	// If the system crashes, move all previous in progress requests back to the default requests dir
	std::system("mkdir -p path-tracer/requests/in_progress && mv path-tracer/requests/in_progress/* path-tracer/requests/");

	while (true) {
		find_earliest_request(current_request);

		if (current_request < 0) { sleep(sleep_period); continue; }

		// Pass it onto another thread
		int buffer_to_use = -1;
		for (int i = 0; i < max_requests; i++) {
            if (lock_states[i]) continue;
            buffer_to_use = i;
            lock_states[i] = true;
            break;
        }

		if (buffer_to_use < 0) { sleep(sleep_period); continue; }
		// MOVE the current request to an "in_progrss" dir!
		std::system(std::format("mv path-tracer/requests/{}.json path-tracer/requests/in_progress/{}.json", current_request, current_request).c_str());
		std::thread(handle_request,
				current_request, buffers[buffer_to_use],
				std::ref(lock_states[buffer_to_use]), buffers_count)
			.detach();
	}

	for (int i = 0; i < max_requests; i++) {
		for (int j = 0; j < buffers_count; j++) {
			delete[] buffers[i][j];
		}
		delete[] buffers[i];
	}
	delete[] buffers;

	return 1;
}
