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

/**
 * depend header files
 */
#include <stdio.h>

static int simple_config_init(struct gaia_func_t *func) {
	printf("simple_config_init\n");
	return (0);
}

static void simple_config_exit(struct gaia_addon_t *addon) {
	printf("simple_config_exit\n");
}

static void simple_config_handle_message(struct gaia_message_t *msg) {
	printf("simple_config_handle_message\n");
}

static int check_flag(int flag) {
	return (flag & (CONFIG_FLAG_LOAD_ADDON | CONFIG_FLAG_RUN_AS_SERVER));
}

static unsigned int server_port() {
	return (9999);
}

static int server_max_conns() {
	return (1000);
}

static const char *server_hostname() {
	return ("127.0.0.1");
}

static const char *server_service() {
	return ("9999");
}

static struct gaia_addon_t *simple_config_info() {
	static struct gaia_addon_t simple_config;
	static struct config_func_t simple_config_func;

	simple_config.id = ADDON_ID_SIMPLE_CONFIG;
	simple_config.type = ADDON_TYPE_CONFIG;
	simple_config.func_size = sizeof(struct config_func_t);
	simple_config.func = (struct gaia_addon_func_t *)&simple_config_func;
	simple_config_func.basic.init = simple_config_init;
	simple_config_func.basic.exit = simple_config_exit;
	simple_config_func.basic.handle_message = simple_config_handle_message;
	simple_config_func.check_flag = check_flag;
	simple_config_func.get_server_port = server_port;
	simple_config_func.get_server_max_conns = server_max_conns;
	simple_config_func.remote_server_hostname = server_hostname;
	simple_config_func.remote_server_service = server_service;

	return (&simple_config);
}
