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
#include <string.h>

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

struct shell_cmd {
	const char *cmd;
	int (*func)(int argc, char const *argv[]);
};

int cmd_help(int argc, char const *argv[]) {
	fprintf(stdout, "ls xxxx\n");
	return (0);
}

int cmd_ls(int argc, char const *argv[]) {
	fprintf(stdout, "ls client\n");
	return (0);
}

int cmd_quit(int argc, char const *argv[]) {

	return (0);
}

static int working = 1;
static pthread_t shell_thread;
static struct shell_cmd cmd_list[] = {
	{"help", cmd_help},
	{"ls", cmd_ls},
	{"quit", cmd_quit},
	{"exit", cmd_quit},
	{0, 0}
};

static int shell_process_line(char *line) {
	int i, argc;
	char *str; char const *argv[16];

	argc = 0;
	str = strtok(line, " \r\n\0");
	while (str && argc < 16) {
		argv[argc++] = str;
		str = strtok(0, " \r\n\0");
	}

	i = 0;
	while (cmd_list[i].cmd) {
		if (!strcmp(cmd_list[i].cmd, argv[0])) {
			return (cmd_list[i].func(argc, argv));
		}
		i++;
	}

	fprintf(stdout, "gaia command '%s' not found\n", argv[0]);
	return (0);
}

static void *shell_work(void *para) {
	char *buff;
	int ret, buff_size = 1024;

	buff = (char *)malloc(buff_size);
	if (!buff) {
		return (0);
	}

	while (working) {
		fprintf(stdout, "\\> ");
		fflush(stdout);

		if (fgets(buff, buff_size, stdin)) {
			ret = shell_process_line(buff);
			if (ret != 0) {
				fprintf(stdout, "Exit with code %d\n", ret);
			}
		} else {
			break;
		}
	}

	free(buff);
}

static int shell_init() {
	working = 1;
	pthread_create(&shell_thread, 0, shell_work, 0);
	return (0);
}

static void shell_exit() {
	working = 0;
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
