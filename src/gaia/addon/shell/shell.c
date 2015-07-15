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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <readline/history.h>
#include <readline/readline.h>

/**
 * @file
 *
 * Command shell.
 */
#include <gaia/addon/shell/shell.h>

/**
 * Shell add-on id.
 */
#define ADDON_ID_SHELL 0x1000123002

static struct gaia_func_t *gaia_func;

struct shell_cmd {
	const char *cmd;
	int (*func)(int argc, char * argv[]);
};

int cmd_help(int argc, char * argv[]) {
	fprintf(stdout, "ls xxxx\n");
	return (0);
}

int cmd_ls(int argc, char * argv[]) {
	fprintf(stdout, "ls client\n");
	return (0);
}

int cmd_addon(int argc, char * argv[]) {
	int i, ic, count, type;
	struct gaia_addon_t *addon = 0;

	opterr = 0;
    while ((ic = getopt(argc, argv, "lct:")) >= 0) {
        switch (ic) {
        case 'l':
        	for (i = 0; i <= gaia_func->get_last_index(); i++) {
        		addon = gaia_func->get_addon_info(i);
        		if (addon) {
        			fprintf(stdout, "0x%08X 0x%016X\n", addon->type, addon->id);
        		}
        	}
            break;
        case 'c':
    		fprintf(stdout, "Exist %d add-ons.\n", gaia_func->count_addon());
            break;
        case 't':
        	type = atoi(optarg);
    		fprintf(stdout, "Exist %d add-ons of type %d.\n", gaia_func->count_addon_by_type(type), type);
            break;
        default:
            break;
        }
    }
    optind = 0;
	return (0);
}

extern HIST_ENTRY **history_list();

int cmd_history(int argc, char * argv[]) {
	int idx = 0;
	HIST_ENTRY **entry;
	entry = history_list();
	if (entry) {
		while (*entry) {
			fprintf(stdout, "%d %s\n", idx++, (*entry++)->line);
		}
	}
	return (0);
}

int cmd_quit(int argc, char * argv[]) {
	struct gaia_message_t msg;
	msg.addon_id = ADDON_ID_INIT;
	msg.type = 1;
	msg.size = 16;
	gaia_func->handle_message(&msg);
	close(fileno(rl_instream));
	close(fileno(rl_outstream));
	close(fileno(stdout));
	return (0);
}

static int working = 1;
static pthread_t shell_thread;
static struct shell_cmd cmd_list[] = {
	{"help", cmd_help},
	{"ls", cmd_ls},
	{"addon", cmd_addon},
	{"history", cmd_history},
	{"quit", cmd_quit},
	{"exit", cmd_quit},
	{0, 0}
};

static int shell_process_line(char *line) {
	int i, argc;
	char *str; char *argv[32];

	argc = 0;
	str = strtok(line, " \r\n\0");
	while (str && argc < 32) {
		argv[argc++] = str;
		str = strtok(0, " \r\n\0");
	}

	if (argc < 1) {
		return (0);
	}

	i = 0;
	while (cmd_list[i].cmd) {
		if (!strcmp(cmd_list[i].cmd, argv[0])) {
			return (cmd_list[i].func(argc, argv));
		}
		i++;
	}

	fprintf(stdout, "GAIA command '%s' not found.\n", argv[0]);
	return (0);
}

static void *shell_work(void *para) {
	int ret;
	char *temp;

	fprintf(stdout, "Welcome to GAIA world.\n\n");

	rl_bind_key('\t', rl_complete);

	temp = 0;
	while (working) {
	    temp = readline("\\> ");
	    if (!temp) {
	    	break;
	    }
	    if (!*temp) {
	    	continue;
	    }

    	add_history(temp);

		ret = shell_process_line(temp);
		if (ret != 0) {
			fprintf(stdout, "Exit with code %d\r\n", ret);
		}
		free(temp);
	}
}

static int shell_init(struct gaia_func_t *func, struct gaia_para_t *para) {
	working = 1;
	pthread_create(&shell_thread, 0, shell_work, 0);
	gaia_func = func;
	return (0);
}

static void shell_exit(struct gaia_para_t *para) {
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
