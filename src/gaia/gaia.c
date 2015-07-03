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
#include <util/list.h>
#include <gaia/gaia.h>
#include <gaia/addon/addon.h>
#include <gaia/addon/manager.h>

/**
 * depend header files
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/**
 * addon information
 */
struct gaia_addon_info_t {
	u8 id;
	u4 type;
	u4 version;
	struct gaia_func_t func;
	struct gaia_addon_info_t *parent;
	struct linked_list ref_list;
	struct linked_list addon_list;
	struct gaia_addon_t *addon_orig;
	struct gaia_addon_func_t *addon_func;
};

/**
 * message list entry
 */
struct gaia_message_list_t {
	struct linked_list list;
	struct gaia_message_t message;
};

/**
 * context
 */
struct gaia_context_t {
	u1 working;
	pthread_rwlock_t addon_lock;
	struct linked_list addon_list;
	pthread_mutex_t message_lock;
	struct linked_list message_list;
};

/**
 * declares
 */
extern struct gaia_context_t gaia_global_context;

/**
 *
 */
static int install(struct gaia_func_t *obj, struct gaia_addon_t *addon);
static int uninstall(struct gaia_func_t *obj, u8 id);
static struct gaia_addon_func_t *get_addon_by_id(u8 id);
static struct gaia_addon_func_t *get_addon_by_type(u4 type);
static int get_addon_list_by_type(u4 type, int len, struct gaia_addon_func_t **addon);
static int handle_message(struct gaia_message_t *msg);

/**
 * gaia init addon handle message function
 */
void gaia_init_handle_message(struct gaia_message_t *msg);

/**
 * Since init addon work as the first addon, has no parent and no need to
 * install.
 *
 *
 */
struct gaia_addon_func_t gaia_init_func = {
    0, 0, gaia_init_handle_message
};

struct gaia_addon_info_t gaia_init_addon = {
	0,
	ADDON_TYPE_INIT,
	0,
	{install, uninstall},
	0, // no parent
	{&gaia_init_addon.ref_list, &gaia_init_addon.ref_list},
	{&gaia_global_context.addon_list, &gaia_global_context.addon_list},
	0,
	&gaia_init_func,
};

struct gaia_message_list_t gaia_msg_init = {
	{&gaia_global_context.message_list, &gaia_global_context.message_list},
	{ADDON_ID_INIT, 0, 0}
};

struct gaia_context_t gaia_global_context = {
	1,
	PTHREAD_RWLOCK_INITIALIZER,
	{&gaia_init_addon.addon_list, &gaia_init_addon.addon_list},
	PTHREAD_MUTEX_INITIALIZER,
	{&gaia_msg_init.list, &gaia_msg_init.list},
};


/**
 * install & uninstall are used to
 *
 *
 *
 */
static int install(struct gaia_func_t *obj, struct gaia_addon_t *addon) {
	int valid = 0;
	struct gaia_addon_info_t *info, *src = 0;

	if (!obj || !addon || !addon->func || !addon->func->init || !addon->func->exit) {
		return (-1);
	}

	info = (struct gaia_addon_info_t *)malloc(sizeof(struct gaia_addon_info_t));
	if (!info) {
		return (-2);
	}

	info->id = addon->id;
	info->type = addon->type;
	info->version = addon->version;
	info->func.install = install;
	info->func.uninstall = uninstall;
	info->func.get_addon_by_id = get_addon_by_id;
	info->func.get_addon_by_type = get_addon_by_type;
	info->func.get_addon_list_by_type = get_addon_list_by_type;
	info->func.handle_message = handle_message;
	info->addon_orig = addon;
	info->addon_func = addon->func;

	/**
	 * check caller
	 */
	pthread_rwlock_wrlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (&src->func == obj) {
			valid = 1;
			info->parent = src;
			linked_list_add(&gaia_global_context.addon_list, &info->addon_list);
			break;
		}
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	if (!valid) {
		free(info);
		return (-3);
	}
	return (addon->func->init(&info->func));
}

static int uninstall(struct gaia_func_t *obj, u8 id) {
	int valid = 0;
	struct gaia_addon_info_t *addon = 0, *parent = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(addon, &gaia_global_context.addon_list, addon_list) {
		if (addon->id == id) {
			parent = addon->parent;
			if (&(parent->func) == obj) {
				valid = 1;
				break;
			}
		}
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	if (!valid) {
		return (-1);
	}

	addon->addon_func->exit(addon->addon_orig);
	pthread_rwlock_wrlock(&gaia_global_context.addon_lock);
	linked_list_del(&addon->addon_list);
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);
	free(addon);
	return (0);
}

static struct gaia_addon_func_t *get_addon_by_id(u8 id) {
	struct gaia_addon_info_t *src = 0, *addon = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (src->id == id && (addon == 0 || addon->version <= src->version)) {
			addon = src;
		}
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	return (addon->addon_func);
}

static struct gaia_addon_func_t *get_addon_by_type(u4 type) {
	struct gaia_addon_info_t *src = 0, *addon = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (src->type == type) {
			addon = src;
		}
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	if (!addon) {
		return (0);
	}
	return (addon->addon_func);
}

static int get_addon_list_by_type(u4 type, int len, struct gaia_addon_func_t **addon) {
	int count = 0, enough = 1;
	struct gaia_addon_info_t *src = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (src->type == type) {
			if (count >= len) {
				enough = 0;
				break;
			} else {
				addon[count++] = src->addon_func;
			}
		}
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	return (enough ? count : -count);
}

static int handle_message(struct gaia_message_t *msg) {
	struct gaia_message_list_t *msg_entry;

	msg_entry = (struct gaia_message_list_t *)malloc(sizeof(struct gaia_message_list_t));
	if (!msg_entry) {
		return (-1);
	}
	memcpy(&(msg_entry->message), msg, sizeof(struct gaia_message_t));

	pthread_mutex_lock(&gaia_global_context.message_lock);
	linked_list_add(&gaia_global_context.message_list, &msg_entry->list);
	pthread_mutex_unlock(&gaia_global_context.message_lock);

	return (0);
}

/**
 * start gaia
 * everything become nice
 *
 * always handle message after gaia start up
 */
void gaia_start_up(void) {
	int valid = 0;
	struct gaia_message_t *msg;
	struct gaia_message_list_t *msg_entry;
	struct gaia_addon_info_t *addon = 0;

	while (gaia_global_context.working) {
		valid = 0;
		pthread_mutex_lock(&gaia_global_context.message_lock);
		list_for_each(msg_entry, &gaia_global_context.message_list, list) {
			valid = 1;
			linked_list_del(&msg_entry->list);
			break;
		}
		pthread_mutex_unlock(&gaia_global_context.message_lock);

		if (!valid) {
			usleep(1);
			continue;
		}
		msg = &msg_entry->message;

		valid = 0;
		pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
		list_for_each(addon, &gaia_global_context.addon_list, addon_list) {
			if (addon->id == msg->addon_id) {
				valid = 1;
				break;
			}
		}
		pthread_rwlock_unlock(&gaia_global_context.addon_lock);

		if (valid) {
			addon->addon_func->handle_message(msg);
		}

		if (&gaia_msg_init != msg_entry) {
			free(msg_entry);
		}
	}
}

void gaia_init_handle_message(struct gaia_message_t *msg) {
	struct gaia_addon_t *addon;

	if (msg->type == 0) {
		addon = (struct gaia_addon_t *)malloc(sizeof(struct gaia_addon_t));
		if (!addon) {
			return;
		}
		addon->id = ADDON_ID_MANAGER;
		addon->type = ADDON_TYPE_MANAGER;

		addon->func = (struct gaia_addon_func_t *)malloc(sizeof(struct gaia_addon_func_t));
		if (!addon->func) {
			free(addon);
			return;
		}
		addon->func->init = manager_init;
		addon->func->exit = manager_exit;
		addon->func->handle_message = manager_handle_message;

		if (install(&gaia_init_addon.func, addon) != 0) {
			free(addon);
		}
	}
}
