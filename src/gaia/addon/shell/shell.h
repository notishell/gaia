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
#ifndef SRC_GAIA_ADDON_SHELL_SHELL_H_
#define SRC_GAIA_ADDON_SHELL_SHELL_H_

/**
 * @file
 */
#include <gaia/addon/addon.h>

/**
 * Shell functions set.
 */
struct shell_func_t {

	/**
	 * Basic functions set.
	 */
	struct gaia_addon_func_t basic;
};

/**
 * Shell add-on information.
 */
struct gaia_addon_t *shell_addon_info();

#endif /* SRC_GAIA_ADDON_SHELL_SHELL_H_ */
