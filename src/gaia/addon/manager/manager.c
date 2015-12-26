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
 * @file
 */
#include <gaia/addon/manager/manager.h>
#include <gaia/addon/manager/client_manager.h>
#include <gaia/addon/config/direct_config.h>
#include <gaia/addon/loader/direct_loader.h>
#include <gaia/addon/network/simple_network.h>
#include <gaia/addon/shell/shell.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Flags.
 */
enum {
	FLAG_INIT                    = 0x00000001,
	FLAG_WORKING                 = 0x00000002,
	FLAG_LOCAL_SERVER            = 0x00000010,
};

/**
 * Manager work context.
 */
struct manager_context_t {

	/**
	 * Flags.
	 */
	int flag;

	/**
	 * Thread.
	 */
	pthread_t working_thread;

	/**
	 * GAIA functions set.
	 */
	struct gaia_func_t *gaia_func;

	/**
	 * Configure add-on functions set.
	 */
	struct config_func_t *config;

	/**
	 * Loader functions set.
	 */
	struct loader_func_t *loader;

	/**
	 * Network add-on functions set.
	 */
	struct network_func_t *network;
};

/**
 * Manager working thread function.
 */
void *manager_working(struct manager_context_t *ctx) {
	struct gaia_message_t msg;
	struct config_func_t *config = ctx->config;
	struct network_func_t *network = ctx->network;

	if (ctx->flag & FLAG_LOCAL_SERVER) {
		network->start_server(config->local_server_address(), config->local_server_max_conns());
	}

	while (ctx->flag & FLAG_WORKING) {

		//network->connect(config->remote_server_address());
		printf("xxxxx\r\n");
		sleep(1);
//		if (client_network->receive_message(&msg) > 0) {
//			sleep(1);
//			//gaia_func->handle_message(&msg);
//			printf("%lld - %d : %s\n", msg.addon_id, msg.type, msg.data);
//			msg.addon_id = 2;
//			msg.type = 1;
//			strcat(msg.data, "im fine thanks");
//			client_network->send_message(&msg);
//		}
	}

	if (ctx->flag & FLAG_LOCAL_SERVER) {
		network->stop_server();
	}

	return (0);
}

/**
 * Start.
 */
void manager_start(struct manager_context_t *ctx) {
	const char **path;
	struct gaia_func_t *func = ctx->gaia_func;

	if (ctx->flag & FLAG_INIT) {
		if (ctx->flag & FLAG_WORKING) {
			return;
		}
	} else {
		func->install(func, direct_config_info());
		func->install(func, simple_network_info());

		ctx->config = (struct config_func_t *)func->get_addon_by_type(ADDON_TYPE_CONFIG);
		if (!ctx->config) {
			return;
		}

//		ctx->loader = (struct loader_func_t *)func->get_addon_by_type(ADDON_TYPE_LOADER);
//		if (!ctx->loader) {
//			return;
//		}
//
//		path = ctx->config->get_addon_list();
//		while (*path) {
//			ctx->loader->load_addon(*path);
//			path++;
//		}

		if (ctx->config->check_flag(CONFIG_FLAG_USE_SHELL)) {
			func->install(func, shell_addon_info());
		}

		if (ctx->config->check_flag(CONFIG_FLAG_USE_NETWORK)) {
			ctx->network = (struct network_func_t *)func->get_addon_by_type(ADDON_TYPE_NETWORK);
			if (!ctx->network) {
				return;
			}
			if (ctx->config->check_flag(CONFIG_FLAG_RUN_LOCAL_SERVER)) {
				ctx->flag |= FLAG_LOCAL_SERVER;
			}
		}

		ctx->flag |= FLAG_INIT;
	}

	ctx->flag |= FLAG_WORKING;
	pthread_create(&ctx->working_thread, ctx, manager_working, 0);
}

/**
 * Stop.
 */
void manager_stop(struct manager_context_t *ctx) {
	if (ctx->flag & FLAG_WORKING) {
		ctx->flag &= ~FLAG_WORKING;
		pthread_join(ctx->working_thread, 0);
	}
}

/**
 * Restart.
 */
void manager_restart(struct manager_context_t *ctx) {
	manager_stop(ctx);
	manager_start(ctx);
}

/**
 * Init.
 */
int manager_init(struct gaia_func_t *func, struct gaia_para_t *para) {
	const char **path;
	struct manager_context_t *ctx;

	struct gaia_message_t dowork_msg = {
	    ADDON_ID_MANAGER, MSG_TYPE_START, 16
	};

	para->size = sizeof(struct manager_context_t);
	ctx = (struct manager_context_t *)malloc(para->size);
	if (!ctx) {
		return (-1);
	}
	memset(ctx, 0, para->size);
	para->para = ctx;
	ctx->flag = 0;
	ctx->gaia_func = func;

	func->handle_message(&dowork_msg);

	return (0);
}

/**
 * Exit.
 */
void manager_exit(struct gaia_para_t *para) {
	manager_stop(para->para);
	free(para->para);
}

/**
 * Handle message.
 */
void manager_handle_message(struct gaia_message_t *msg, struct gaia_para_t *para) {
	switch(msg->type) {
	case MSG_TYPE_START:
		manager_start(para->para);
		break;
	case MSG_TYPE_STOP:
		manager_stop(para->para);
		break;
	case MSG_TYPE_RESTART:
		manager_restart(para->para);
		break;
	default:
		break;
	}
}

struct gaia_addon_t *manager_addon_info() {
	static struct gaia_addon_t addon;
	static struct manager_func_t addon_func;

	addon.id = ADDON_ID_MANAGER;
	addon.type = ADDON_TYPE_MANAGER;
	addon.func_size = sizeof(struct client_manager_func_t);
	addon.func = (struct gaia_addon_func_t *)&addon_func;
	addon_func.basic.init = manager_init;
	addon_func.basic.exit = manager_exit;
	addon_func.basic.handle_message = manager_handle_message;

	return (&addon);
}
