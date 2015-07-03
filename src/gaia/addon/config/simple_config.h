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
#ifndef SRC_GAIA_ADDON_CONFIG_SIMPLE_CONFIG_H_
#define SRC_GAIA_ADDON_CONFIG_SIMPLE_CONFIG_H_

#include <gaia/gaia.h>
#include <gaia/addon/addon.h>
#include <gaia/addon/config/config.h>

enum {
	ADDON_ID_SIMPLE_CONFIG          = 0x1000000000000001,
};

struct gaia_addon_t *simple_config_info();

#endif /* SRC_GAIA_ADDON_CONFIG_SIMPLE_CONFIG_H_ */
