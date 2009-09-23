#include "defines.h"

int main(void)
{
	int s, i, v;
	int ret;
        fd_set fds;
	struct av_clients cli;

	client_init(&cli);
	s = av_server_socket_setup(SOCK_PATH);
	v = video_setup(VIDEO_PATH);

	cli.self = s;
	client_add(&cli, s);
	cli.device = v;
	client_add(&cli, v);

	for (;;) {
		fds = cli.set;
		if (select(cli.max + 1, &fds, NULL, NULL, NULL) == -1) {
		    perror("select");
		    exit(1);
		}

		for (i = 0; i <= cli.max; i++) {
			if (!FD_ISSET(i, &fds))
				continue;

			if (i == cli.self) {
				client_accept(&cli, s);
				continue;
			}
			if (i == cli.device) {
				debugprintf("video ready %i\n", v);
				continue;
			}
			client_handle_request(&cli, i);
		}
	}

	return 0;
}
