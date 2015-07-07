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
#include <gaia/addon/manager/client_manager.h>
#include <gaia/addon/config/direct_config.h>
#include <gaia/addon/network/server_network.h>

enum {
	ADDON_ID_CLIENT_MANAGER        = 0x1000000000000002,
};
/**
 * depend header files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct client_t {
	int fd;
};

static struct client_t *clients;
static struct simple_config_func_t *config;
static struct server_network_func_t *server_network;

void new_client(int fd) {
	struct gaia_message_t msg;
	printf("handle_new_connection: %s:%d\n", server_network->get_client_ip(fd), server_network->get_client_port(fd));
	msg.addon_id = 1;
	msg.type = 2;
	msg.size = sizeof(struct gaia_message_t);
	strcpy(msg.data, "hello, how old are you????");
	server_network->send_message(fd, &msg);
}


void new_message(int fd, struct gaia_message_t *msg) {
	printf("%lld - %d : %s\n", msg->addon_id, msg->type, msg->data);
	msg->addon_id = 1;
	msg->type = 2;
	strcpy(msg->data, "hello, how old are you????");
	server_network->send_message(fd, msg);
}

int client_manager_init(struct gaia_func_t *func) {
	config = (struct simple_config_func_t *)func->get_addon_by_type(ADDON_TYPE_CONFIG);
	server_network = (struct server_network_func_t *)func->get_addon_by_type(ADDON_TYPE_SERVER_NETWORK);
	printf("client_manager_init\n");
	return (0);
}

void client_manager_exit(struct gaia_addon_t *addon) {
	printf("client_manager_exit\n");
}

void client_manager_handle_message(struct gaia_message_t *msg) {
//	struct client_message_t *client = (struct client_message_t *)msg;
//
//	printf("%p\n", client);
//	printf("%p\n", server_network);
//	switch (client->type) {
//	case MSG_TYPE_NEW_CONN:
//		printf("client_manager_handle_message\n");
//		handle_new_connection(client->fd);
//		break;
//	}
}

struct gaia_addon_t *client_manager_info() {
	static struct gaia_addon_t client_manager;
	static struct client_manager_func_t client_manager_func;

	client_manager.id = ADDON_ID_CLIENT_MANAGER;
	client_manager.type = ADDON_TYPE_CLIENT_MANAGER;
	client_manager.func_size = sizeof(struct client_manager_func_t);
	client_manager.func = (struct gaia_addon_func_t *)&client_manager_func;
	client_manager_func.basic.init = client_manager_init;
	client_manager_func.basic.exit = client_manager_exit;
	client_manager_func.basic.handle_message = client_manager_handle_message;
	client_manager_func.new_client = new_client;
	client_manager_func.new_message = new_message;

	return (&client_manager);
}
