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
#ifndef SRC_GAIA_ADDON_CONFIG_DIRECT_CONFIG_H_
#define SRC_GAIA_ADDON_CONFIG_DIRECT_CONFIG_H_

/**
 * @file
 *
 * Direct config add-on.
 */
#include <gaia/addon/addon.h>

/**
 * Config flag.
 */
enum config_flag {
	CONFIG_FLAG_LOAD_ADDON				= 0x00000001,
	CONFIG_FLAG_USE_NETWORK				= 0x00000002,
	CONFIG_FLAG_RUN_AS_SERVER			= 0x00000003,
};

/**
 * Get config information from config center like this.
 *
 * Example:
 * @code
 *    struct config_func_t *config;
 *    const char **file_list;
 *
 *    config = (struct config_func_t *)func->get_addon_by_type(ADDON_TYPE_CONFIG);
 *
 *    if (config->check_flag(CONFIG_FLAG_LOAD_ADDON)) {
 *        file_list = config->get_addon_list();
 *        while (*file_list) {
 *            printf("load addon : %s\n", *file_list++);
 *        }
 *    }
 *
 *    if (config->check_flag(CONFIG_FLAG_RUN_AS_SERVER)) {
 *        printf("local server listen port : %d\n", config->get_server_port());
 *        printf("local server maximum connections : %d\n", config->get_server_max_conns());
 *    }
 *
 *    printf("remote server(%s:%s)\n", config->remote_server_hostname(),
 *        config->remote_server_service());
 * @endcode
 *
 */
struct config_func_t {

	/**
	 * Basic.
	 */
	struct gaia_addon_func_t basic;

	/**
	 * Check if flag marked.
	 */
	int (*check_flag)(int flag);

	/**
	 * Use this interface to get addon list.
	 */
	const char **(*get_addon_list)();

	/**
	 * Get local server listen port.
	 */
	unsigned int (*get_server_port)();

	/**
	 * Get local server maximum connections.
	 */
	int (*get_server_max_conns)();

	/**
	 * Get remote server host name.
	 */
	const char *(*remote_server_hostname)();

	/**
	 * Get remote server service name.
	 */
	const char *(*remote_server_service)();
};

/**
 * Return direct config add-on information.
 */
struct gaia_addon_t *direct_config_info();

#endif /* SRC_GAIA_ADDON_CONFIG_DIRECT_CONFIG_H_ */
