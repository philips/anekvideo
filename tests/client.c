#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "defines.h"

int main(void)
{
	int s, t, len;
	struct sockaddr_un remote;
	char str[MAX_MSG_SIZE];
	fd_set fds, tfds;

	FD_ZERO(&fds);

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

	FD_SET(fileno(stdin), &fds);
	FD_SET(s, &fds);

	for (;;) {
		tfds = fds;
		printf("What are you doing right now?\n");

		if (select(s + 1, &tfds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}

		if (FD_ISSET(s, &tfds)) {
			if ((t = recv(s, str, MAX_MSG_SIZE, 0)) > 0) {
				str[t] = '\0';
				printf("%s", str);
			} else {
				if (t < 0) perror("recv");
				else printf("Server closed connection\n");
				exit(1);
			}
		}

		if (FD_ISSET(fileno(stdin), &tfds)) {
			if (feof(stdin))
				exit(0);
			fgets(str, MAX_MSG_SIZE, stdin);
			if (send(s, str, strlen(str), 0) == -1) {
				perror("send");
				exit(1);
			}
		}

	}

	close(s);

	return 0;
}
