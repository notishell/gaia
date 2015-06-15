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
#include <gaia/addon/config/simple_config.h>
#include <gaia/addon/network/simple_network.h>

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

enum {
	BUFF_SIZE = 8192
};

enum {
    STATUS_CONNECT       = 0,
    STATUS_DISCONNECT    = 1,
};

int sock = 0;
int buff_size = BUFF_SIZE;
char *buffer = 0;
struct addrinfo *res = 0, *cur = 0;
int status = STATUS_DISCONNECT;
struct simple_config_func_t *config;
pthread_mutex_t network_rlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t network_wlock = PTHREAD_MUTEX_INITIALIZER;

struct addrinfo *get_server_addr() {
	struct addrinfo *addr, mask;

	if (!cur) {
	    memset(&mask, 0, sizeof(struct addrinfo));
	    mask.ai_family = AF_INET;     /* Allow IPv4 */
	    mask.ai_flags = AI_PASSIVE;   /* For wild card IP address */
	    mask.ai_protocol = 0;         /* Any protocol */
	    mask.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(config->server_hostname(), config->server_service(), &mask, &res)) {
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
	int ret = 0;
	uint16_t length = 0;
	ssize_t tmp = 0, size = 0;

	pthread_mutex_lock(&network_rlock);
	if (status == STATUS_DISCONNECT) {
		ret = connect_server();
	}
	if (ret == 0 && status == STATUS_CONNECT) {
		size = 0;
		do {
			tmp = recv(sock, ((uint8_t *)&length) + size, sizeof(uint16_t) - size, MSG_DONTWAIT);
			if (tmp < 0) {
				break;
			}
			size += tmp;
		} while (size < sizeof(uint16_t));

		length = ntohs(length);
		if (sizeof(uint16_t) == size && length < BUFF_SIZE) {
			size = 0;
			do {
				tmp = recv(sock, ((uint8_t *)buffer) + size, length - size, MSG_DONTWAIT);
				if (tmp < 0) {
					break;
				}
				size += tmp;
			} while (size < length);
		}
	}
	pthread_mutex_unlock(&network_rlock);

	return (size == length ? 0 : -1);
}

static int send_message(struct gaia_message_t *msg) {
	int ret = 0;
	ssize_t size = 0;

	pthread_mutex_lock(&network_wlock);
//	if (status == STATUS_DISCONNECT) {
//		ret = connect_server();
//		printf("connect_server\n");
//	}
//	if (ret == 0 && status == STATUS_CONNECT) {
//		do {
//			printf("recv\n");
//			size = recv(sock, buffer, buff_size, MSG_DONTWAIT);//MSG_DONTWAIT
//			if (size > 0) {
//				printf("%s\n", buffer);
//				break;
//			}
//			break;
//		} while (1);
//	}
	pthread_mutex_unlock(&network_wlock);

	return (ret);
}

struct gaia_addon_t *simple_network_info() {
	static struct gaia_addon_t simple_network;
	static struct simple_network_func_t simple_network_func;

	simple_network.id = ADDON_ID_SIMPLE_NETWORK;
	simple_network.type = ADDON_TYPE_NETWORK;
	simple_network.func_size = sizeof(struct simple_network_func_t);
	simple_network.func = (struct gaia_addon_func_t *)&simple_network_func;
	simple_network_func.basic.init = simple_network_init;
	simple_network_func.basic.exit = simple_network_exit;
	simple_network_func.basic.handle_message = simple_network_handle_message;
	simple_network_func.receive_message = receive_message;

	return (&simple_network);
}

void simple_network_init(struct gaia_func_t *func) {
	buffer = (char *)malloc(buff_size);
	config = (struct simple_config_func_t *)func->get_addon_by_id(ADDON_ID_SIMPLE_CONFIG);
	printf("simple_network_init\n");
}

void simple_network_exit(struct gaia_addon_t *addon) {
	free(buffer);
	printf("simple_network_exit\n");
}

void simple_network_handle_message(struct gaia_message_t *msg) {
	printf("simple_network_handle_message\n");
}
