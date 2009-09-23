/**
 * server - socket server routines
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <linux/videodev.h>
#include <fcntl.h>
#include <unistd.h>

#include "defines.h"
#include "server.h"

int debuglevel = 1;

int av_server_destroy(struct av_server *srv)
{
	/* Koii bat nahii. Server dies so memory can leak */
	return 0;
}

struct av_server *av_server_new(void)
{
	struct av_server *srv = talloc(NULL, struct av_server);
	
	if (srv < 0) {
		talloc_free(srv);
		return NULL;
	}

	talloc_set_destructor(srv, av_server_destroy);
	memset(srv, 0, sizeof(*srv));
	return srv;
}

#if 0

int av_server_client_add_by_fd(struct av_server *srv, int fd)
{
	int i;
	struct av_server_client *client = &srv->clients[fd];

	FD_SET(fd, &srv->set);

	if (fd > srv->max)
		srv->max = fd;

	client->fd = fd;
	client->msg = buffer_init();

	return i;
}

void av_server_client_remove_by_fd(struct av_server *srv, int fd)
{
	int i;
	//struct av_server_client *client = &srv->clients[fd];

	FD_CLR(fd, &srv->set);

	//buffer_free(srv->msg);
	//srv->fd = -1;

	if (fd != srv->max)
		return;

	for (i = srv->max - 1; i >= 0; i--) {
		if (FD_ISSET(i, &srv->set)) {
			srv->max = i;
			return;
		}
	}
}

int av_server_socket_create(char *path)
{
	int s, len;
	struct sockaddr_un local;
	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, path);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(s, 5) == -1) {
		perror("listen");
		exit(1);
	}

	return s;
}

int av_server_client_accept(struct av_server *srv)
{
	int ret;
	socklen_t t;
	struct sockaddr_un remote;

	t = sizeof(remote);
	ret = accept(srv->fd, (struct sockaddr *)&remote, &t);
	if (ret == -1) {
		perror("accept");
		exit(1);
	}
	av_server_client_add_by_fd(srv, ret);

	av_log_debug("Connected client %i.\n", ret);

	return ret;
}

int av_server_client_handle_request(struct av_server *srv, int fd)
{
	int n, t;
	int toread;

	struct av_buffer *buffer = srv->clients[fd].msg;

	char *str;

        if (ioctl(fd, FIONREAD, &toread)) {
		av_log_debug("%i: FIONREAD\n", errno);
		return -1;
        }

	if (buffer->used + toread >= MAX_MSG_SIZE) {
		av_log_debug("client sent %d bytes, too much data!\n", 
				buffer->used + toread);
		av_client_remove_by_fd(srv, fd);
		close(fd);
		return -1;
	}

	buffer_grow(buffer, toread);

	n = recv(fd, buffer->buf + buffer->used, MAX_MSG_SIZE - buffer->used, 0);
	buffer->used += n;

	if (n <= 0) {
		if (n < 0) perror("recv");
		client_remove(cls, fd);
		close(fd);
		return;
	}

	if (buffer->buf[buffer->used - 1] != '\n')
		return 0;

	buffer->buf[buffer->used] = '\0';

	str = malloc(MAX_MSG_SIZE);
	if (!str)
		perror("malloc");

	/* TODO: Woah!  Lots more safety needed here */
	av_log_debug("IOCTL %d\n", (int)buffer->buf);
	ioctl(cls->device, (int)buffer->buf, (void *)(buffer->buf + sizeof(int)));
	buffer->used = 0;
	free(str);
}

#endif
