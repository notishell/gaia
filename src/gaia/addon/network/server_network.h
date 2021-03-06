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
#ifndef SRC_GAIA_ADDON_NETWORK_SERVER_NETWORK_H_
#define SRC_GAIA_ADDON_NETWORK_SERVER_NETWORK_H_

#include <gaia/addon/addon.h>

struct server_network_func_t {
	struct gaia_addon_func_t basic;
	void (*start_network)();
	void (*stop_network)();
	char *(*get_client_ip)(int fd);
	int (*get_client_port)(int fd);
	int (*send_message)(int fd, struct gaia_message_t *msg);
};

struct gaia_addon_t *server_network_info();

#endif /* SRC_GAIA_ADDON_NETWORK_SERVER_NETWORK_H_ */
