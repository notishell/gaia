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
#include <gaia/addon/loader/direct_loader.h>

/**
 * Direct config add-on id.
 */
#define ADDON_ID_DIRECT_LOADER 0x100000000002222

struct gaia_addon_t *direct_loader_info() {
	static struct gaia_addon_t addon;
	static struct loader_func_t addon_func;

	addon.id = ADDON_ID_DIRECT_LOADER;
	addon.type = ADDON_TYPE_CONFIG;
	addon.func_size = sizeof(struct config_func_t);
	addon.func = (struct loader_func_t *)&addon_func;
	addon_func.basic.init = default_addon_init;
	addon_func.basic.exit = default_addon_exit;
	addon_func.basic.handle_message = default_addon_handle_message;

	return (&addon);
}
