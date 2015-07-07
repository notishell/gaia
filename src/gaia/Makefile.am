AUTOMAKE_OPTIONS = foreign subdir-objects
noinst_LIBRARIES = libgaia.a 
libgaia_a_SOURCES = gaia.c addon/addon.c addon/manager/manager.c addon/manager/client_manager.c addon/config/direct_config.c \
	addon/network/client_network.c addon/network/server_network.c
libgaia_a_CFLAGS = -I.. $(AM_CFLAGS)