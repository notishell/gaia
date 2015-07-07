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


enum {
	ADDON_ID_DIRECT_CONFIG          = 0x1000000000000002,
};

static int check_flag(int flag) {
	return (flag & (CONFIG_FLAG_LOAD_ADDON | CONFIG_FLAG_RUN_AS_SERVER));
}

static const char **get_addon_list() {
	static const char *addons[] = {0};
	return (addons);
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

static int config_init(struct gaia_func_t *func) {
	return (0);
}

static void config_exit(struct gaia_addon_t *addon) {
}

struct gaia_addon_t *direct_config_info() {
	static struct gaia_addon_t config;
	static struct config_func_t config_func;

	config.id = ADDON_ID_DIRECT_CONFIG;
	config.type = ADDON_TYPE_CONFIG;
	config.func_size = sizeof(struct config_func_t);
	config.func = (struct gaia_addon_func_t *)&config_func;
	config_func.basic.init = config_init;
	config_func.basic.exit = config_exit;
	config_func.basic.handle_message = default_addon_handle_message;
	config_func.check_flag = check_flag;
	config_func.get_addon_list = get_addon_list;
	config_func.get_server_port = server_port;
	config_func.get_server_max_conns = server_max_conns;
	config_func.remote_server_hostname = server_hostname;
	config_func.remote_server_service = server_service;

	return (&config);
}
