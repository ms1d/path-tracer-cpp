#include <string>
#include <unistd.h>
#include <filesystem>
#include <asio.hpp>
#include "request_handler.hpp"
#include "structs.cuh"
#include "buffer_setup.cuh"



// Maximum + current number of requests being handled at once. Determines how many mutli-buffers are allocated at program start
constexpr uint max_requests = 3;
// Number of buffers per multi-buffer
constexpr uint buffer_count = 2;
// Default size of each buffer
constexpr uint buffer_size = 1e8;
// Period to sleep if no requests are available
constexpr uint sleep_period = 5;



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
	// buffers is a 2D array. Each element is a buffer for 1 thread ONLY
	Pixel*** multi_buffers = new Pixel**[max_requests];
	for (uint i = 0; i < max_requests; i++) {
		multi_buffers[i] = new Pixel*[buffer_count];
		for (uint j = 0; j < buffer_count; j++) {
			multi_buffers[i][j] = new Pixel[buffer_size];
		}
	}

	Pixel* cuda_buffers;
	alloc(cuda_buffers, max_requests, buffer_size, buffer_count);


	// The ith buffer owns the ith lock state (used by the ith thread running)
	// Each thread will control the lock state, eventually setting it to false when it is finished
	bool lock_states[max_requests] = { };
	int current_request = -1;

	// If the system crashes, move all previous in progress requests back
	// to the default requests dir, ready to start processing again
	std::system("mkdir -p path-tracer/requests && cd path-tracer/requests && mkdir -p in_progress && mv in_progress/* ./");

	while (true) {
		find_earliest_request(current_request);

		if (current_request < 0) { sleep(sleep_period); continue; }

		// Pass it onto another thread
		int buffer_to_use = -1;
		for (uint i = 0; i < max_requests; i++) {
            if (lock_states[i]) continue;
            buffer_to_use = i;
            lock_states[i] = true;
            break;
        }

		if (buffer_to_use < 0) { sleep(sleep_period); continue; }

		std::system(std::format("cd path-tracer/requests && mv {}.json in_progress/{}.json", current_request, current_request).c_str());
		std::thread(handle_request,
				current_request, multi_buffers[buffer_to_use], cuda_buffers,
				std::ref(lock_states[buffer_to_use]))
			.detach();
	}

	dealloc(cuda_buffers);

	for (uint i = 0; i < max_requests; i++) {
		for (uint j = 0; j < buffer_count; j++) delete[] multi_buffers[i][j];
		delete[] multi_buffers[i];
	}

	delete[] multi_buffers;

	return 1;
}
