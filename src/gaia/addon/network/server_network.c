/* Copyright 2015 9x6.me. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: Notis Hell (notishell@gmail.com)
 */
#include <gaia/addon/config/direct_config.h>
#include <gaia/addon/manager/client_manager.h>
#include <gaia/addon/network/server_network.h>

enum {
	ADDON_ID_SERVER_NETWORK        = 0x2000000000000002,
};

/**
 * depend header files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/poll.h>


enum {
	BUFF_SIZE = 8192
};

enum {
    STATUS_RUNNING       = 0,
    STATUS_STOPPED       = 1,
};

struct client_t {
	int port;
	char ip[16];
	int recv_size;
	char recv_buff[BUFF_SIZE];
	char send_buff[BUFF_SIZE];
	pthread_mutex_t network_lock;
};

static int sock = 0;
static int max_conns = 0;
static int working = 1;
static int server_status = STATUS_STOPPED;
static struct pollfd *conns = 0;
static struct client_t *clients = 0;
static pthread_t server_network_thread;
static struct config_func_t *config;
static struct client_manager_func_t *client_manager;
static struct gaia_func_t *gaia_func;

void server_network_receive(int fd) {
	int i;
	u4 size;
	struct gaia_message_t *msg;

	i = recv(conns[fd].fd, clients[fd].recv_buff + clients[fd].recv_size, BUFF_SIZE - clients[fd].recv_size, MSG_DONTWAIT);
	if (i > 0) {
		clients[fd].recv_size += i;
		msg = (struct gaia_message_t *)clients[fd].recv_buff;
		if (clients[fd].recv_size < 16) {
			return;
		}

		size = n4_to_u4(msg->size);

		if (size > sizeof(struct gaia_message_t)) {
			close(conns[fd].fd);
			conns[fd].fd = -1;
			return;
		}

		if (clients[fd].recv_size < size) {
			return;
		}

		msg->addon_id = n8_to_u8(msg->addon_id);
		msg->type = n4_to_u4(msg->type);
		msg->size = n4_to_u4(msg->size);

		if (!client_manager) {
			client_manager = (struct client_manager_func_t *)gaia_func->get_addon_by_type(ADDON_TYPE_CLIENT_MANAGER);
		}

		if (client_manager) {
			client_manager->new_message(fd, msg);
		}

		clients[fd].recv_size -= size;
		memmove(clients[fd].recv_buff, clients[fd].recv_buff + size, clients[fd].recv_size);
	} else {
		close(conns[fd].fd);
		conns[fd].fd = -1;
	}
}

void server_network_accept(int fd, struct sockaddr_in *addr) {
	int i;

    for (i = 1; i <= max_conns; i++) {
    	if (conns[i].fd != -1) {
    		continue;
    	}

		conns[i].fd = fd;
		clients[i].port = ntohs(addr->sin_port);
		inet_ntop(AF_INET, (void *)&addr->sin_addr, clients[i].ip, 16);

		if (!client_manager) {
			client_manager = (struct client_manager_func_t *)gaia_func->get_addon_by_type(ADDON_TYPE_CLIENT_MANAGER);
		}

		if (client_manager) {
			client_manager->new_client(i);
		}

		fd = 0;
		break;
    }

	if (fd > 0) {
		close(fd);
	}
}

void *server_network_working(void *para) {
	int i, j, fd;
	struct sockaddr_in addr;

	while (working) {

		if (poll(conns, max_conns + 1, 1000) < 0) {
			continue;
		}

	    for (i = 1; i <= max_conns; i++) {
	    	if (conns[i].fd > 0 && (conns[i].revents & POLLIN)) {
		    	server_network_receive(i);
	    	}
	    }

		if (conns[0].revents & POLLIN) {
    		j = sizeof(struct sockaddr_in);
			fd = accept(conns[0].fd, (struct sockaddr *)&addr, (socklen_t *)&j);
			if (fd > 0) {
				server_network_accept(fd, &addr);
			}
		}
	}

	return (0);
}

void start_network() {
	int i;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		return;
	}

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(config->get_server_port());

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    	return;
    }

    max_conns = config->get_server_max_conns();
    if (listen(sock, max_conns) < 0) {
    	return;
    }

    clients = (struct client_t *)malloc((max_conns + 1) * sizeof(struct client_t));
    if (!clients) {
    	return;
    } else {
		for (i = 0; i <= max_conns; i++) {
			pthread_mutex_init(&clients[i].network_lock, 0);
		}
    }

    conns = (struct pollfd *)malloc((max_conns + 1) * sizeof(struct pollfd));
    if (!conns) {
    	return;
    } else {
		conns[0].fd = sock;
		conns[0].events = POLLIN;
		conns[0].revents = 0;
		for (i = 1; i <= max_conns; i++) {
			conns[i].fd = -1;
			conns[i].events = POLLIN;
			conns[i].revents = 0;
		}
    }

    working = 1;
	pthread_create(&server_network_thread, 0, server_network_working, 0);

	server_status = STATUS_RUNNING;
}

void stop_network() {
	int i;

	working = 0;
	if (STATUS_RUNNING == server_status) {
		pthread_join(server_network_thread, 0);
	}

	if (clients) {
	    for (i = 0; i <= max_conns; i++) {
	    	pthread_mutex_destroy(&clients[i].network_lock);
	    }
		free(clients);
		clients = 0;
	}

	if (conns) {
	    for (i = 1; i <= max_conns; i++) {
	    	if (conns[i].fd != -1) {
	    		close(conns[i].fd);
	    	}
	    }
		free(conns);
		conns = 0;
	}

	if (sock > 0) {
		close(sock);
		if (conns) {
			free(conns);
		}
		sock = 0;
	}
}

char *get_client_ip(int fd) {
	if (fd <= max_conns && conns[fd].fd > 0) {
		return (clients[fd].ip);
	}
	return ("");
}

int get_client_port(int fd) {
	if (fd <= max_conns && conns[fd].fd > 0) {
		return (clients[fd].port);
	}
	return (-1);
}

static int send_message(int fd, struct gaia_message_t *msg) {
	int i, ret;
	ssize_t size;
	char *buff = 0;
	struct gaia_message_t *tmp;

	if (fd <= max_conns && conns[fd].fd > 0) {
		buff = clients[fd].send_buff;
		pthread_mutex_lock(&clients[fd].network_lock);

		memcpy(buff, msg, msg->size);

		tmp = (struct gaia_message_t *)buff;
		tmp->addon_id = u8_to_n8(tmp->addon_id);
		tmp->type = u4_to_n4(tmp->type);
		tmp->size = u4_to_n4(tmp->size);

		size = send(conns[fd].fd, buff, msg->size, 0);
		if (size != msg->size) {
			ret = -1;
			close(conns[fd].fd);
			conns[fd].fd = -1;
		}
		pthread_mutex_unlock(&clients[fd].network_lock);
	}

	return (ret);
}

int server_network_init(struct gaia_func_t *func) {
	config = (struct config_func_t *)func->get_addon_by_type(ADDON_TYPE_CONFIG);
	gaia_func = func;
	return (0);
}

void server_network_exit(struct gaia_addon_t *addon) {
}

void server_network_handle_message(struct gaia_message_t *msg) {
}

struct gaia_addon_t *server_network_info() {
	static struct gaia_addon_t server_network;
	static struct server_network_func_t server_network_func;

	server_network.id = ADDON_ID_SERVER_NETWORK;
	server_network.type = ADDON_TYPE_SERVER_NETWORK;
	server_network.func_size = sizeof(struct server_network_func_t);
	server_network.func = (struct gaia_addon_func_t *)&server_network_func;
	server_network_func.basic.init = server_network_init;
	server_network_func.basic.exit = server_network_exit;
	server_network_func.basic.handle_message = server_network_handle_message;
	server_network_func.start_network = start_network;
	server_network_func.stop_network = stop_network;
	server_network_func.get_client_ip = get_client_ip;
	server_network_func.get_client_port = get_client_port;
	server_network_func.send_message = send_message;

	return (&server_network);
}
