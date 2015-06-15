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
#ifndef SRC_GAIA_ADDON_ADDON_H_
#define SRC_GAIA_ADDON_ADDON_H_


#include <gaia/gaia.h>

enum {
	ADDON_ID_INIT = 0,
	ADDON_ID_MANAGER = 1,
};

enum {
	ADDON_TYPE_INIT           = 0,
	ADDON_TYPE_MANAGER        = 1,
	ADDON_TYPE_CONFIG         = 2,
	ADDON_TYPE_NETWORK        = 3,
	ADDON_TYPE_ETC            = 4,
};

struct gaia_addon_t;

struct gaia_addon_func_t {
	void (*init)(struct gaia_func_t *func);
	void (*exit)(struct gaia_addon_t *addon);
	void (*handle_message)(struct gaia_message_t *msg);
};

struct gaia_addon_t {
	uint64_t id;
	uint32_t type;
	uint32_t version;
	uint32_t func_size;
	struct gaia_addon_func_t *func;
};

#endif /* SRC_GAIA_ADDON_ADDON_H_ */
