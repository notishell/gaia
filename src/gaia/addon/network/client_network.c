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
#include <gaia/addon/network/client_network.h>

enum {
	ADDON_ID_SIMPLE_NETWORK        = 0x200012340001,
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
    STATUS_CONNECT       = 0,
    STATUS_DISCONNECT    = 1,
};

static int sock = 0;
static int buff_size = BUFF_SIZE;
static int recv_size;
static char *recv_buff;
static char *send_buff;
static struct addrinfo *res = 0, *cur = 0;
static int status = STATUS_DISCONNECT;
static struct config_func_t *config;
static pthread_mutex_t network_rlock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t network_wlock = PTHREAD_MUTEX_INITIALIZER;

struct addrinfo *get_server_addr() {
	struct addrinfo *addr, mask;

	if (!cur) {
	    memset(&mask, 0, sizeof(struct addrinfo));
	    mask.ai_family = AF_INET;
	    mask.ai_flags = AI_PASSIVE;
	    mask.ai_protocol = 0;
	    mask.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(config->remote_server_hostname(), config->remote_server_service(), &mask, &res)) {
			return (0);
		}
		cur = res;
	}

	addr = cur;
	cur = cur->ai_next;

	if (!cur) {
	    freeaddrinfo(res);
	    res = 0;
	}
	return (addr);
}

int connect_server() {
	struct addrinfo *addr;

	addr = get_server_addr();
	if (!addr) {
		return (-1);
	}

	if (sock > 0) {
		close(sock);
	}

	sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sock < 0) {
		return (-2);
	}

	if (0 < connect(sock, addr->ai_addr, addr->ai_addrlen)) {
		return (-3);
	}

	status = STATUS_CONNECT;
	return (0);
}

static int receive_message(struct gaia_message_t *msg) {
	int i, ret = 0;
	u4 size = 0;
	struct pollfd recvfd;
	struct gaia_message_t *tmp = 0;

	pthread_mutex_lock(&network_rlock);
	if (status == STATUS_DISCONNECT) {
		ret = connect_server();
	}
	if (ret == 0 && status == STATUS_CONNECT) {
		recvfd.fd = sock;
		recvfd.events = POLLIN;
		recvfd.revents = 0;

		while (1) {
			if (poll(&recvfd, 1, 100) < 0) {
				continue;
			}

			if (!(recvfd.revents & POLLIN)) {
				continue;
			}

			i = recv(sock, recv_buff + recv_size, BUFF_SIZE - recv_size, MSG_DONTWAIT);
			if (i > 0) {
				recv_size += i;
				tmp = (struct gaia_message_t *)recv_buff;
				if (recv_size < 16) {
					continue;
				}

				size = n4_to_u4(tmp->size);
				if (recv_size < size) {
					continue;
				}
				tmp->addon_id = n8_to_u8(tmp->addon_id);
				tmp->type = n4_to_u4(tmp->type);
				tmp->size = n4_to_u4(tmp->size);

				memcpy(msg, tmp, size);

	    		recv_size -= size;
	    		memmove(recv_buff, recv_buff + size, recv_size);
	    		break;
			}

			if (i < 0 || size > sizeof(struct gaia_message_t)) {
				ret = -1;
				status = STATUS_DISCONNECT;
				break;
			}
		}
	} else {
		ret = -1;
	}
	pthread_mutex_unlock(&network_rlock);

	return (ret == 0 ? size : -1);
}

static int send_message(struct gaia_message_t *msg) {
	int ret = 0;
	ssize_t size = 0;
	struct gaia_message_t *tmp;

	pthread_mutex_lock(&network_wlock);
	if (status == STATUS_DISCONNECT) {
		ret = connect_server();
	}
	if (ret == 0 && status == STATUS_CONNECT) {
		memcpy(send_buff, msg, msg->size);

		tmp = (struct gaia_message_t *)send_buff;
		tmp->addon_id = u8_to_n8(tmp->addon_id);
		tmp->type = u4_to_n4(tmp->type);
		tmp->size = u4_to_n4(tmp->size);

		size = send(sock, send_buff, msg->size, 0);
		if (size != msg->size) {
			ret = -1;
			status = STATUS_DISCONNECT;
		}
	}
	pthread_mutex_unlock(&network_wlock);

	return (ret);
}

static int network_init(struct gaia_func_t *func) {
	recv_buff = (char *)malloc(buff_size);
	send_buff = (char *)malloc(buff_size);
	config = (struct config_func_t *)func->get_addon_by_type(ADDON_TYPE_CONFIG);
	return (0);
}

static void network_exit(struct gaia_addon_t *addon) {
	if (sock > 0) {
		close(sock);
		sock= -1;
	}
	if (res) {
	    freeaddrinfo(res);
	    res = 0;
	}
	status = STATUS_DISCONNECT;
	free(recv_buff);
	free(send_buff);
}

static void network_handle_message(struct gaia_message_t *msg) {
}

struct gaia_addon_t *client_network_info() {
	static struct gaia_addon_t network;
	static struct client_network_func_t network_func;

	network.id = ADDON_ID_SIMPLE_NETWORK;
	network.type = ADDON_TYPE_CLIENT_NETWORK;
	network.func_size = sizeof(struct client_network_func_t);
	network.func = (struct gaia_addon_func_t *)&network_func;
	network_func.basic.init = network_init;
	network_func.basic.exit = network_exit;
	network_func.basic.handle_message = network_handle_message;
	network_func.receive_message = receive_message;
	network_func.send_message = send_message;

	return (&network);
}
