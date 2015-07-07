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
#ifndef SRC_GAIA_GAIA_H_
#define SRC_GAIA_GAIA_H_

#include <util/util.h>

/**
 * @file
 * @mainpage GAIA
 *
 * Gaia is a tool to manager distributed computers.
 */

struct gaia_addon_t;

/**
 * Each message contains destination add-on id, message type, message size and data.
 *
 * Example:
 *
 * @code
 * 		struct gaia_message_t msg;
 * 		msg.id = ADDON_ID_INIT;
 * 		msg.type = 0;
 * 		msg.size = 16;
 * 		gaia_func->handle_message(&msg);
 * @endcode
 */
struct gaia_message_t {

	/**
	 * Destination add-on id.
	 *
	 * Such as:
	 *
	 * @code
	 * 		msg.id = ADDON_ID_INIT;
	 * @endcode
	 */
	u8 addon_id;

	/**
	 * User defined message type, unsigned 4 bytes.
	 *
	 * Such as:
	 *
	 * @code
	 * 		msg.type = 1;
	 * @endcode
	 */
	u4 type;

	/**
	 * Size of a message. Contains the size of add-on id, type, size, and data.
	 *
	 * Such as:
	 *
	 * @code
	 * 		msg.size = sizeof(msg.addon_id) + sizeof(msg.type) + sizeof(msg.size) + data_size;
	 * @endcode
	 */
	u4 size;

	/**
	 * Data of a message. Maximum size of data is 1024 bytes.
	 *
	 * Such as:
	 *
	 * @code
	 * 		strcpy(msg.data, "hello, world!");
	 * @endcode
	 */
	u1 data[1024];
};

/**
 * Functions provided by add-on controller to run the system.
 *
 * Example:
 *
 * @code
 * 		config_func_t *config;
 * 		struct gaia_func_t *gaia_func = global_gaia_func;
 *
 * 		if (gaia_func->install(gaia_func, simple_config_info()) == 0) {
 * 			config = gaia_func->get_addon_by_id(simple_config_info()->id);
 * 			if (config) {
 * 				gaia_func->handle_message(&msg);
 * 			}
 * 		}
 * @endcode
 */
struct gaia_func_t {

	/**
	 * A method to install add-on. The first argument must be the 'gaia_func' pointer.
	 * Once installed using current add-on's 'gaia_func', current add-on became the
	 * father add-on of new install. Return non-zero if not success.
	 *
	 * Example:
	 *
	 * @code
	 * 		gaia_func->install(gaia_func, simple_config_info());
	 * @endcode
	 */
	int (*install)(struct gaia_func_t *obj, struct gaia_addon_t *addon);

	/**
	 * Method to remove add-on by add-on id. Only add-on's direct or indirect father
	 * can remove it. Return non-zero if not success.
	 *
	 * Example:
	 *
	 * @code
	 * 		gaia_func->uninstall(gaia_func, ADDON_ID_INIT);
	 * @endcode
	 */
	int (*uninstall)(struct gaia_func_t *obj, u8 id);

	/**
	 * Get function sets of a add-on by id. Use the highest version if exist 2+ add-on
	 * with same id.
	 *
	 * Example:
	 *
	 * @code
	 * 		struct config_func_t *config;
	 * 		config = gaia_func->get_addon_by_id(ADDON_ID_SIMPLE_CONFIG);
	 * 		if (config) {
	 * 			printf("server: %s\n", config->remote_server_hostname());
	 * 		}
	 * @endcode
	 */
	struct gaia_addon_func_t *(*get_addon_by_id)(u8 id);

	/**
	 * Get function sets of a add-on by type. Use the last installed version if exist 2+
	 * add-on with same type.
	 *
	 * Example:
	 *
	 * @code
	 * 		struct config_func_t *config;
	 * 		config = gaia_func->get_addon_by_type(ADDON_TYPE_CONFIG);
	 * 		if (config) {
	 * 			printf("server: %s\n", config->remote_server_hostname());
	 * 		}
	 * @endcode
	 */
	struct gaia_addon_func_t *(*get_addon_by_type)(u4 type);

	/**
	 * Get add-on function sets list by type. Return N or -N if find N add-ons and saved N.
	 * -N means buffer is not enough.
	 *
	 * Example:
	 *
	 * @code
	 * 		int len;
	 * 		struct config_func_t *config[5];
	 *
	 * 		len = gaia_func->get_addon_list_by_type(ADDON_TYPE_CONFIG, 5, config);
	 *
	 * 		printf("find %d add-on of type config.\n", len >= 0 ? len : -len);
	 * @endcode
	 */
	int (*get_addon_list_by_type)(u4 type, int len, struct gaia_addon_func_t **addon);

	/**
	 * Handle a message. Return non-zero if not success.
	 *
	 * Example:
	 *
	 * @code
	 * 		struct gaia_message_t msg;
	 * 		msg.id = ADDON_ID_INIT;
	 * 		msg.type = 0;
	 * 		msg.size = 16;
	 * 		gaia_func->handle_message(&msg);
	 * @endcode
	 */
	int (*handle_message)(struct gaia_message_t *msg);
};

/**
 * Method to run GAIA. Never stop until a quit message received.
 */
void gaia_running();


#endif /* SRC_GAIA_GAIA_H_ */
