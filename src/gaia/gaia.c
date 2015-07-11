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
#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/**
 * @file
 *
 * Core of GAIA.
 */
#include <util/list.h>
#include <gaia/gaia.h>
#include <gaia/addon/addon.h>
#include <gaia/addon/manager/manager.h>

/**
 * Add-on information.
 */
struct gaia_addon_info_t {

	/**
	 * Add-on index.
	 */
	int index;

	/**
	 * Function sets. Each add-on has a private sets.
	 *
	 * @see gaia_func_t
	 */
	struct gaia_func_t func;

	/**
	 * Pointer to parent add-on.
	 */
	struct gaia_addon_info_t *parent;

	/**
	 * Son list lock.
	 */
	pthread_mutex_t list_lock;

	/**
	 * Son list.
	 */
	struct linked_list son_list;

	/**
	 * All add-ons has same parent linked as a list.
	 */
	struct linked_list bro_list;

	/**
	 * All add-ons linked as a list.
	 */
	struct linked_list addon_list;

	/**
	 * Add-on information for reader.
	 *
	 * @see gaia_addon_t
	 */
	struct gaia_addon_t addon_info;

	/**
	 * Original add-on information.
	 *
	 * @see gaia_addon_t
	 */
	struct gaia_addon_t *addon_orig;

	/**
	 * Function table of add-on.
	 *
	 * Example:
	 * @code
	 * 		struct config_func_t {
	 * 			struct gaia_addon_func_t basic;
	 * 			int (*check_flag)(int flag);
	 * 			...
	 * 		};
	 * @endcode
	 */
	struct gaia_addon_func_t *addon_func;
};

/**
 * Message list entry. Used by GAIA global context.
 */
struct gaia_message_list_t {

	/**
	 * List head of message list.
	 */
	struct linked_list list;

	/**
	 * Message.
	 *
	 * @see gaia_message_t
	 */
	struct gaia_message_t message;
};

/**
 * GAIA running context.
 */
struct gaia_context_t {

	/**
	 * Working flag. Exit if set false.
	 */
	u1 working;

	/**
	 * Last add-on index.
	 */
	int index;

	/**
	 * Read-write lock for add-on list .
	 */
	pthread_rwlock_t addon_lock;

	/**
	 * Add-on list.
	 */
	struct linked_list addon_list;

	/**
	 * Lock for message list.
	 */
	pthread_mutex_t message_lock;

	/**
	 * Message list.
	 *
	 * @todo use cache
	 */
	struct linked_list message_list;
};

/*
 * The GAIA global context declared here.
 */
extern struct gaia_context_t gaia_global_context;

/*
 * The global GAIA core functions declared here.
 */

/**
 * Install add-on.
 */
static int install(struct gaia_func_t *obj, struct gaia_addon_t *addon);

/**
 * Remove add-on.
 */
static int uninstall(struct gaia_func_t *obj, u8 id);

/**
 * Count how many add-on exist. Return negative value if failed.
 */
static int count_addon();

/**
 * Count how many add-on of certain type exist. Return negative value if failed.
 */
static int count_addon_by_type(u4 type);

/**
 * Get last installed add-on index. Never failed.
 */
static int get_last_index();

/**
 * Get the idx-th add-on information. Return 0 if not find.
 */
static struct gaia_addon_t *get_addon_info(int idx);

/**
 * Get add-on by id.
 */
static struct gaia_addon_func_t *get_addon_by_id(u8 id);

/**
 * Get add-on by type.
 */
static struct gaia_addon_func_t *get_addon_by_type(u4 type);

/**
 * Get add-on list by type.
 */
static int get_addon_list_by_type(u4 type, int len, struct gaia_addon_func_t **addon);

/**
 * Handle message.
 */
static int handle_message(struct gaia_message_t *msg);

/**
 * GAIA initial add-on handle message function.
 */
static void gaia_init_handle_message(struct gaia_message_t *msg);

/**
 * Function set for initial add-on.
 *
 * @see gaia_init_addon
 */
struct gaia_addon_func_t gaia_init_func = {
    0, 0, gaia_init_handle_message
};

/**
 * Information of initial add-on.
 */
struct gaia_addon_info_t gaia_init_addon = {
	0,
	{install, uninstall},
	0, // no parent
	PTHREAD_MUTEX_INITIALIZER,
	{&gaia_init_addon.son_list, &gaia_init_addon.son_list},
	{&gaia_init_addon.bro_list, &gaia_init_addon.bro_list},
	{&gaia_global_context.addon_list, &gaia_global_context.addon_list},
	{0, 0, ADDON_TYPE_INIT, 0, 0},
	0,
	&gaia_init_func,
};

/**
 * Initial message list.
 */
struct gaia_message_list_t gaia_msg_init = {
	{&gaia_global_context.message_list, &gaia_global_context.message_list},
	{ADDON_ID_INIT, 0, 0}
};

/**
 * GAIA running context.
 */
struct gaia_context_t gaia_global_context = {
	1,
	0,
	PTHREAD_RWLOCK_INITIALIZER,
	{&gaia_init_addon.addon_list, &gaia_init_addon.addon_list},
	PTHREAD_MUTEX_INITIALIZER,
	{&gaia_msg_init.list, &gaia_msg_init.list},
};

/**
 * @see gaia_func_t
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

	info->func.install = install;
	info->func.uninstall = uninstall;
	info->func.count_addon = count_addon;
	info->func.count_addon_by_type = count_addon_by_type;
	info->func.get_last_index = get_last_index;
	info->func.get_addon_info = get_addon_info;
	info->func.get_addon_by_id = get_addon_by_id;
	info->func.get_addon_by_type = get_addon_by_type;
	info->func.get_addon_list_by_type = get_addon_list_by_type;
	info->func.handle_message = handle_message;
	memcpy(&info->addon_info, addon, sizeof(struct gaia_addon_t));
	info->addon_orig = addon;
	info->addon_func = addon->func;
	pthread_mutex_init(&info->list_lock, 0);
	info->son_list.prev = &info->son_list;
	info->son_list.next = &info->son_list;
	info->bro_list.prev = &info->bro_list;
	info->bro_list.next = &info->bro_list;

	/**
	 * Check caller.
	 * Check if exists.
	 */
	pthread_rwlock_wrlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (&src->func == obj) {
			valid = 1;
			info->parent = src;
		}
		if (src->addon_info.id == info->addon_info.id && &src->func != obj) {
			valid = 0;
			break;
		}
	}
	if (valid) {
		info->index = ++gaia_global_context.index;
		linked_list_add(&info->parent->son_list, &info->bro_list);
		linked_list_add(&gaia_global_context.addon_list, &info->addon_list);
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	if (!valid) {
		free(info);
		return (-3);
	}

	valid = addon->func->init(&info->func);
	if (valid != 0) {
		pthread_rwlock_wrlock(&gaia_global_context.addon_lock);
		linked_list_del(&info->bro_list);
		linked_list_del(&info->addon_list);
		pthread_rwlock_unlock(&gaia_global_context.addon_lock);
		free(info);
	}
	return (valid);
}

/**
 * @see gaia_func_t
 */
static int uninstall(struct gaia_func_t *obj, u8 id) {
	int valid = 0;
	struct gaia_addon_info_t *tmp, *addon = 0, *parent = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(addon, &gaia_global_context.addon_list, addon_list) {
		if (addon->addon_info.id == id) {
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

	pthread_mutex_lock(&addon->list_lock);
	list_for_each(tmp, &addon->son_list, son_list) {
		uninstall(&addon->func, tmp->addon_info.id);
	}
	pthread_mutex_unlock(&addon->list_lock);

	pthread_rwlock_wrlock(&gaia_global_context.addon_lock);
	linked_list_del(&addon->bro_list);
	linked_list_del(&addon->addon_list);
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	addon->addon_func->exit(addon->addon_orig);
	pthread_mutex_destroy(&addon->list_lock);
	free(addon);
	return (0);
}

/**
 * @see gaia_func_t
 */
static int count_addon() {
	int count = 0;
	struct gaia_addon_info_t *src = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		count++;
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	return (count);
}

/**
 * @see gaia_func_t
 */
static int count_addon_by_type(u4 type) {
	int count = 0;
	struct gaia_addon_info_t *src = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (src->addon_info.type == type) {
			count++;
		}
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	return (count);
}

/**
 * @see gaia_func_t
 */
static int get_last_index() {
	return (gaia_global_context.index);
}

/**
 * @see gaia_func_t
 */
static struct gaia_addon_t *get_addon_info(int idx) {
	struct gaia_addon_t *addon = 0;
	struct gaia_addon_info_t *src = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (src->index == idx) {
			addon = &src->addon_info;
			break;
		}
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	return (addon);
}

/**
 * @see gaia_func_t
 */
static struct gaia_addon_func_t *get_addon_by_id(u8 id) {
	struct gaia_addon_info_t *src = 0, *addon = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (src->addon_info.id == id && (addon == 0 || addon->addon_info.version <=
				src->addon_info.version)) {
			addon = src;
		}
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	return (addon->addon_func);
}

/**
 * @see gaia_func_t
 */
static struct gaia_addon_func_t *get_addon_by_type(u4 type) {
	struct gaia_addon_info_t *src = 0, *addon = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (src->addon_info.type == type) {
			addon = src;
		}
	}
	pthread_rwlock_unlock(&gaia_global_context.addon_lock);

	if (!addon) {
		return (0);
	}
	return (addon->addon_func);
}

/**
 * @see gaia_func_t
 */
static int get_addon_list_by_type(u4 type, int len, struct gaia_addon_func_t **addon) {
	int count = 0, enough = 1;
	struct gaia_addon_info_t *src = 0;

	pthread_rwlock_rdlock(&gaia_global_context.addon_lock);
	list_for_each(src, &gaia_global_context.addon_list, addon_list) {
		if (src->addon_info.type == type) {
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

/**
 * @see gaia_func_t
 */
static int handle_message(struct gaia_message_t *msg) {
	struct gaia_message_list_t *msg_entry;

	msg_entry = (struct gaia_message_list_t *)malloc(sizeof(struct gaia_message_list_t));
	if (!msg_entry) {
		return (-1);
	}
	memcpy(&(msg_entry->message), msg, msg->size);

	pthread_mutex_lock(&gaia_global_context.message_lock);
	linked_list_add(&gaia_global_context.message_list, &msg_entry->list);
	pthread_mutex_unlock(&gaia_global_context.message_lock);

	return (0);
}

static void gaia_init_handle_message(struct gaia_message_t *msg) {
	static struct gaia_addon_t *addon;

	if (msg->type == 0) {
		addon = manager_addon_info();
		if (!addon) {
			return;
		}

		if (install(&gaia_init_addon.func, addon) != 0) {
			gaia_global_context.working = 0;
		}
	} else if (msg->type == 1) {
		if (uninstall(&gaia_init_addon.func, ADDON_ID_MANAGER) == 0) {
			gaia_global_context.working = 0;
		}
	}
}

void gaia_running(void) {
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
			if (addon->addon_info.id == msg->addon_id) {
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
