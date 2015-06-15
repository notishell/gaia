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

/**
 * depend header files
 */
#include <stdint.h>

/**
 *
 */
struct gaia_addon_t;

struct gaia_message_t {
	uint64_t addon_id;
	uint32_t type;
	uint32_t size;
	uint8_t data[1024];
};

struct gaia_func_t {
	int (*install)(struct gaia_func_t *obj, struct gaia_addon_t *addon);
	int (*uninstall)(struct gaia_func_t *obj, uint64_t id);
	struct gaia_addon_func_t *(*get_addon_by_id)(uint64_t id);
	int (*get_addon_by_type)(uint32_t type, int len, struct gaia_addon_func_t **addon);
	int (*handle_message)(struct gaia_message_t *msg);
};

void gaia_start_up();

#endif /* SRC_GAIA_GAIA_H_ */
