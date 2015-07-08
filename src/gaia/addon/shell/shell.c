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
#include <stdio.h>

/**
 * @file
 *
 * Command shell.
 */
#include <gaia/addon/shell/shell.h>

/**
 * Shell add-on id.
 */
#define ADDON_ID_SHELL 0x1000000000000002


static pthread_t shell_thread;

static void *shell_work(void *para) {
	char buff[256];

	while (1) {
		scanf("%s", buff);
		printf("%s\n", buff);
	}
}

static int shell_init() {
	pthread_create(&shell_thread, 0, shell_work, 0);
	return (0);
}

static void shell_exit() {
	pthread_join(shell_thread, 0);
}

struct gaia_addon_t *shell_addon_info(struct gaia_func_t *func) {
	static struct gaia_addon_t addon;
	static struct shell_func_t addon_func;

	addon.id = ADDON_ID_SHELL;
	addon.type = ADDON_TYPE_USER_INTERFACE;
	addon.func_size = sizeof(struct shell_func_t);
	addon.func = (struct gaia_addon_func_t *)&addon_func;
	addon_func.basic.init = shell_init;
	addon_func.basic.exit = shell_exit;
	addon_func.basic.handle_message = default_addon_handle_message;

	return (&addon);
}
