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
#ifndef SRC_GAIA_ADDON_NETWORK_NETWORK_H_
#define SRC_GAIA_ADDON_NETWORK_NETWORK_H_

#include <gaia/addon/addon.h>

/**
 * Network.
 */
struct network_func_t {

	/**
	 * Basic.
	 */
	struct gaia_addon_func_t basic;

	/**
	 * Start local network server.
	 */
	int (*start_server)(const char *address, int max_conns);

	/**
	 * Stop local network server.
	 */
	void (*stop_server)();

	/**
	 * Connect to remote server.
	 */
	int (*connect)(const char *address);

	/**
	 * Close a connection.
	 */
	void (*disconnect)(int fd);

	/**
	 * Send a message.
	 */
	int (*send)(int fd, struct gaia_message_t *msg);

	/**
	 * Receive a message.
	 */
	int (*recv)(int fd, struct gaia_message_t *msg);

	/**
	 * Get client address.
	 */
	const char *(*get_client_address)(int fd);
};

#endif /* SRC_GAIA_ADDON_NETWORK_NETWORK_H_ */
