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
#ifndef SRC_GAIA_ADDON_CLIENT_MANAGER_H_
#define SRC_GAIA_ADDON_CLIENT_MANAGER_H_

#include <gaia/gaia.h>
#include <gaia/addon/addon.h>

enum {
	ADDON_ID_CLIENT_MANAGER        = 0x1000000000000002,
};

enum {
	MSG_TYPE_NEW_CONN              = 0x00000001,
	MSG_TYPE_NEW_MSG               = 0x00000002,
};

struct client_message_t {
	uint64_t addon_id;
	uint32_t type;
	uint32_t size;
	int fd;
	uint8_t data[256];
};

struct client_manager_func_t {
	struct gaia_addon_func_t basic;
	void (*start_network)();
	void (*stop_network)();
};

struct gaia_addon_t *client_manager_info();
void client_manager_init(struct gaia_func_t *func);
void client_manager_exit(struct gaia_addon_t *addon);
void client_manager_handle_message(struct gaia_message_t *msg);

#endif /* SRC_GAIA_ADDON_CLIENT_MANAGER_H_ */
