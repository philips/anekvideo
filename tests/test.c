#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "defines.h"

int setup(void)
{
	int s, t, len;
	struct sockaddr_un remote;
	char str[MAX_MSG_SIZE]; fd_set fds, tfds; FD_ZERO(&fds);

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCK_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(s, (struct sockaddr *)&remote, len) == -1) {
		perror("connect");
		exit(1);
	}

	return s;
}

int main(void)
{
	int s;
	char bad[MAX_MSG_SIZE * 2];
	char msg[] = "Test\n";

	s = setup();

	send(s, bad, sizeof(bad), 0);

	s = setup();

	send(s, bad, 10, 0);
	sleep(1);
	send(s, bad, 160, 0);

	s = setup();

	send(s, msg, 2, 0);
	sleep(2);
	send(s, msg + 2, 3, 0);

	return 0;
}
