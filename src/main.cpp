#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <fstream>
#include "json.hpp"

using nlohmann::json;

std::vector<pid_t> children;
json status_json;

void kill_children() {
	for (auto pid : children) {
		if (pid > 0) kill(pid, SIGKILL);
	}
	_exit(1);
}

void sig_handler(int _) {
	kill_children();
	_exit(1);
}

pid_t spawn(const char *path, const char* nice_name) {
	status_json[nice_name] = "starting...";
    pid_t pid = fork();
    if (pid == 0) {
		status_json[nice_name] = "running";
        execl(path, path, (char *)NULL);
        perror("execl failed");
        _exit(1);
		status_json[nice_name] = "execl failed";
    }
    if (pid < 0) {
        perror("fork failed");
		status_json[nice_name] = "fork failed";
    }

	children.push_back(pid);

    return pid;
}

int main() {
	signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
	atexit(kill_children);

	std::system("mkdir -p state && touch state/status.json");
    pid_t http_pid = spawn("./http-server/http-server", "http");
    pid_t udp_pid  = spawn("./udp-server/udp-server", "http");

    while (1) {
        if (http_pid <= 0)
            http_pid = spawn("./http-server/http-server", "http");

        if (udp_pid <= 0)
            udp_pid = spawn("./udp-server/udp-server", "http");

        int status;
        pid_t r;

        while ((r = waitpid(-1, &status, WNOHANG)) > 0) {
            if (r == http_pid) http_pid = -1;
            else if (r == udp_pid) udp_pid = -1;
        }

		// From time to time, clear children that are not working (pid of -1)
		std::vector<pid_t> new_children;

		for (auto pid : children) {
			if (pid > 0) new_children.push_back(pid);
		}

		children = new_children;

		// Write status to status.json for the http server
		std::ofstream file("state/status.json");
		if (!file.is_open()) {
			std::cerr << "Failed to open file 'state/status.json'.\n";
			return 1;
		}

		file << status_json.dump(4);

		file.close();

        sleep(5);
    }
}
