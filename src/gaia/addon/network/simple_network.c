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

/**
 * Simple network.
 */
#include <gaia/addon/network/simple_network.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

enum {
	ADDON_ID_SIMPLE_NETWORK        = 0x20000032100002,
};


enum {
	BUFF_SIZE = 8192
};

enum {
    STATUS_RUNNING       = 0,
    STATUS_STOPPED       = 1,
};


static struct config_func_t *config;
static struct client_manager_func_t *client_manager;
static struct gaia_func_t *gaia_func;

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


struct simple_network_client_t {
	char *addr;
	int recv_size;
	char recv_buff[BUFF_SIZE];
	int send_size;
	char send_buff[BUFF_SIZE];
	pthread_mutex_t network_lock;
};

struct simple_network_context_t {
	int server_sock = 0;
	int max_conns = 0;
	struct pollfd *fds;
	struct simple_network_client_t *clients;
	int working = 1;
	int server_status = STATUS_STOPPED;
	pthread_t server_network_thread;
};

static struct simple_network_context_t context;

static int start_server(const char *address, int max_conns) {
	int i;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		return (-1);
	}

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons();

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    	close(sock);
    	return (-2);
    }

    if (listen(sock, max_conns) < 0) {
    	close(sock);
    	return (-3);
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

void stop_server() {

}

static int simple_network_connect(const char *address) {
	return (0);
}

static void simple_network_disconnect(int fd) {
	;
}

static int simple_network_send(int fd, struct gaia_message_t *msg) {

	return (0);
}

static int simple_network_recv(int fd, struct gaia_message_t *msg) {

	return (0);
}

static const char *get_client_address(int fd) {

	return (0);
}

static int addon_init(struct gaia_func_t *func, struct gaia_para_t *para) {
	return (0);
}

static void addon_exit(struct gaia_para_t *para) {
}

struct gaia_addon_t *simple_network_info() {
	static struct gaia_addon_t addon;
	static struct network_func_t addon_func;

	addon.id = ADDON_ID_SIMPLE_NETWORK;
	addon.type = ADDON_TYPE_NETWORK;
	addon.func_size = sizeof(struct config_func_t);
	addon.func = (struct gaia_addon_func_t *)&addon_func;
	addon_func.basic.init = addon_init;
	addon_func.basic.exit = addon_exit;
	addon_func.basic.handle_message = default_addon_handle_message;
	addon_func.start_server = start_server;
	addon_func.stop_server = stop_server;
	addon_func.connect = simple_network_connect;
	addon_func.disconnect = simple_network_disconnect;
	addon_func.send = simple_network_send;
	addon_func.recv = simple_network_recv;
	addon_func.get_client_address = get_client_address;

	return (&addon);
}
