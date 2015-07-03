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
#include <gaia/addon/manager.h>
#include <gaia/addon/client_manager.h>
#include <gaia/addon/config/local_config.h>
#include <gaia/addon/config/simple_config.h>
#include <gaia/addon/network/simple_network.h>
#include <gaia/addon/network/server_network.h>

/**
 * depend header files
 */
#include <stdio.h>
#include <String.h>

enum {
	MSG_TYPE_START             = 0,
	MSG_TYPE_RESTART           = 1,
	MSG_TYPE_STOP              = 2,
};

static int working = 1;
static int local_server = 0;
static pthread_t network_thread;
static struct gaia_func_t *gaia_func;
static struct config_func_t *config;
static struct client_manager_func_t *client_manager;
static struct simple_network_func_t *network;
static struct server_network_func_t *server_network;


void *network_working(void *para) {
	struct gaia_message_t msg;

	while (working) {
		if (network->receive_message(&msg) > 0) {
			//gaia_func->handle_message(&msg);
			printf("%lld - %d : %s\n", msg.addon_id, msg.type, msg.data);
			msg.addon_id = 2;
			msg.type = 1;
			strcat(msg.data, "im fine thanks");
			network->send_message(&msg);
		}
	}
}

void manager_start() {
	working = 1;
	if (local_server) {
		local_server = 1;
		server_network->start_network();
	}
	pthread_create(&network_thread, 0, network_working, 0);
}

void manager_stop() {
	working = 0;
	if (local_server) {
		server_network->stop_network();
	}
	pthread_join(network_thread, 0);
}

void manager_restart() {
	manager_stop();
	manager_start();
}

void manager_init(struct gaia_func_t *func) {
	const char **path;
	struct gaia_message_t dowork_msg = {
	    ADDON_ID_MANAGER, MSG_TYPE_START, 0
	};

	func->install(func, local_config_info());
	func->install(func, simple_config_info());
	config = (struct simple_config_func_t *)func->get_addon_by_id(ADDON_ID_SIMPLE_CONFIG);
	if (!config) {
		return;
	}

	// load plugin
//	path = config->get_addon_list();
//	while (*path) {
//		;
//	}

	func->install(func, simple_network_info());
	network = (struct simple_network_func_t *)func->get_addon_by_id(ADDON_ID_SIMPLE_NETWORK);
	if (!network) {
		return;
	}

	if (config->check_flag(CONFIG_FLAG_RUN_AS_SERVER)) {
		local_server = 1;

		func->install(func, server_network_info());
		server_network = (struct server_network_func_t *)func->get_addon_by_id(ADDON_ID_SERVER_NETWORK);
		if (!server_network) {
			return;
		}

		func->install(func, client_manager_info());
		client_manager = (struct client_manager_func_t *)func->get_addon_by_id(ADDON_ID_CLIENT_MANAGER);
		if (!client_manager) {
			return;
		}
	}

	func->handle_message(&dowork_msg);
	gaia_func = func;
}

void manager_exit(struct gaia_addon_t *addon) {
	if (local_server) {
		gaia_func->uninstall(gaia_func, ADDON_ID_CLIENT_MANAGER);
		gaia_func->uninstall(gaia_func, ADDON_ID_SERVER_NETWORK);
	}
	gaia_func->uninstall(gaia_func, ADDON_ID_SIMPLE_NETWORK);
	gaia_func->uninstall(gaia_func, ADDON_ID_SIMPLE_CONFIG);
}

void manager_handle_message(struct gaia_message_t *msg) {
	switch(msg->type) {
	case MSG_TYPE_START:
		manager_start();
		break;
	case MSG_TYPE_STOP:
		manager_stop();
		break;
	case MSG_TYPE_RESTART:
		manager_restart();
		break;
	default:
		break;
	}
}
