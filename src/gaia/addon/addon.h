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
#ifndef SRC_GAIA_ADDON_ADDON_H_
#define SRC_GAIA_ADDON_ADDON_H_

/**
 * @file
 *
 * Add-on type and basic functions set define here.
 */
#include <gaia/gaia.h>

/**
 * Initial add-on id is 0.
 */
enum addon_id {
	ADDON_ID_INIT 						= 0,
};

/**
 * Add-on types define here.
 */
enum addon_type {
	ADDON_TYPE_INIT         		  	= 0x00000000,
	ADDON_TYPE_CONFIG  	    		   	= 0x00000010,
	ADDON_TYPE_SERVER_NETWORK        	= 0x00000100,
	ADDON_TYPE_CLIENT_NETWORK        	= 0x00000200,
	ADDON_TYPE_MANAGER  	  	    	= 0x00001000,
	ADDON_TYPE_CLIENT_MANAGER  	  	    = 0x00002000,
	ADDON_TYPE_USER_INTERFACE  	  	    = 0x00010000,
};

struct gaia_addon_t;

/**
 * Add-on functions set.
 */
struct gaia_addon_func_t {

	/**
	 * Initialize add-on. Call after install.
	 */
	int (*init)(struct gaia_func_t *func);

	/**
	 * Cleanup add-on. Call before uninstall.
	 */
	void (*exit)(struct gaia_addon_t *addon);

	/**
	 * Handle message.
	 */
	void (*handle_message)(struct gaia_message_t *msg);
};

/**
 * Add-on information.
 */
struct gaia_addon_t {

	/**
	 * Add-on id.
	 */
	u8 id;

	/**
	 * Add-on type.
	 */
	u4 type;

	/**
	 * Add-on version.
	 */
	u4 version;

	/**
	 * Functions set size.
	 */
	u4 func_size;

	/**
	 * Pointer to functions set.
	 */
	struct gaia_addon_func_t *func;
};

/**
 * Default add-on init method.
 */
int default_addon_init(struct gaia_func_t *func);

/**
 * Default add-on exit method.
 */
void default_addon_exit(struct gaia_addon_t *addon);

/**
 * Default add-on handle message method.
 */
void default_addon_handle_message(struct gaia_message_t *msg);

#endif /* SRC_GAIA_ADDON_ADDON_H_ */
