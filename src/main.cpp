#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include "json.hpp"

using nlohmann::json;

std::vector<pid_t> children;

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

pid_t spawn(const char *path) {
    pid_t pid = fork();

	if (pid == 0) {
		execl(path, path, (char *)NULL);
		perror("execl failed");
		_exit(1);
	}

	if (pid < 0) perror("fork failed");
    
	children.push_back(pid);

    return pid;
}

int main() {
	// Ensure that children are killed on exit + exit signals
	signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGKILL, sig_handler);
	atexit(kill_children);

    pid_t http_pid = spawn("./http-server/http-server");
    pid_t path_tracer_pid  = spawn("./path-tracer/path-tracer");

    while (true) {
        if (http_pid <= 0) http_pid = spawn("./http-server/http-server");

        if (path_tracer_pid <= 0) path_tracer_pid = spawn("./path-tracer/path-tracer");

        int status;
        pid_t r;

		// Detect exits
        while ((r = waitpid(-1, &status, WNOHANG)) > 0) {
            if (r == http_pid) http_pid = -1;
            else if (r == path_tracer_pid) path_tracer_pid = -1;
        }

		// From time to time, clear children that are not working (pid of -1)
		std::vector<pid_t> new_children;

		for (auto pid : children) {
			if (pid > 0) new_children.push_back(pid);
		}

		children = new_children;

        sleep(1);
    }
}
