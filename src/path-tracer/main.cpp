#include <unistd.h>
#include <filesystem>
#include <format>

int main() {
	while (true) {
		std::filesystem::path dir = "path-tracer/requests";
		int current_request = -1;

		for (const auto& entry : std::filesystem::directory_iterator(dir)) {
			if (entry.is_regular_file() && entry.path().extension() == ".json") {
				std::string name = entry.path().stem().filename();
				if (current_request < 0 || current_request > std::stoi(name)) { current_request = std::stoi(name);}
			}
		}

		// No work to do = sleep and wait
		if (current_request < 0) { sleep(5); continue; }
		
		// Do work here...
		
		std::system(std::format("rm path-tracer/requests/{}.json", current_request).c_str());
	}

	return 1;
}
