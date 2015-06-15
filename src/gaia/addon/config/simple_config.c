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

int run_as_server() {
	return (1);
}

unsigned int server_port() {
	return (9999);
}

int server_max_conns() {
	return (1000);
}

const char *server_hostname() {
	return ("127.0.0.1");
}

const char *server_service() {
	return ("9999");
}

struct gaia_addon_t *simple_config_info() {
	static struct gaia_addon_t simple_config;
	static struct simple_config_func_t simple_config_func;

	simple_config.id = ADDON_ID_SIMPLE_CONFIG;
	simple_config.type = ADDON_TYPE_CONFIG;
	simple_config.func_size = sizeof(struct simple_config_func_t);
	simple_config.func = (struct gaia_addon_func_t *)&simple_config_func;
	simple_config_func.basic.init = simple_config_init;
	simple_config_func.basic.exit = simple_config_exit;
	simple_config_func.basic.handle_message = simple_config_handle_message;
	simple_config_func.run_as_server = run_as_server;
	simple_config_func.server_port = server_port;
	simple_config_func.server_max_conns = server_max_conns;
	simple_config_func.server_hostname = server_hostname;
	simple_config_func.server_service = server_service;

	return (&simple_config);
}

void simple_config_init(struct gaia_func_t *func) {
	printf("simple_config_init\n");
}

void simple_config_exit(struct gaia_addon_t *addon) {
	printf("simple_config_exit\n");
}

void simple_config_handle_message(struct gaia_message_t *msg) {
	printf("simple_config_handle_message\n");
}
