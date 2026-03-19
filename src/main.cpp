#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	// Spin up the http server by cloning this process and running the http-server exe
	pid_t http_pid = fork();
	if (http_pid == 0) {
		execl("./http-server", "http-server", nullptr);
		_exit(1);
	}

	// Spin up the udp server the same way
	pid_t udp_pid = fork();
	if (udp_pid == 0) {
		execl("./udp-server", "udp-server", nullptr);
		_exit(1);
	}

	while (true) {
		// Keep restarting if child dies
		if (http_pid <= 0) {
			http_pid = fork();
			if (http_pid == 0) {
				execl("./proc1", "./proc1", (char*)nullptr);
				_exit(1);
			}
		}

		if (udp_pid <= 0) {
			udp_pid = fork();
			if (udp_pid == 0) {
				execl("./proc2", "./proc2", (char*)nullptr);
				_exit(1);
			}
		}

		// Handle exits
		int status;
		pid_t r;

		while ((r = waitpid(-1, &status, WNOHANG)) > 0) {
			if (r == http_pid) {
				http_pid = -1;  // mark dead → will restart next loop
			} else if (r == udp_pid) {
				udp_pid = -1;
			}
		}

		sleep(5);
	}

	return 1;
}
