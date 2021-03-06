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

/**
 * @file
 */
#include <gaia/addon/addon.h>

int default_addon_init(struct gaia_func_t *func, struct gaia_para_t *para) {
	return (0);
}

void default_addon_exit(struct gaia_para_t *para) {
}

void default_addon_handle_message(struct gaia_message_t *msg, struct gaia_para_t *para) {
}
