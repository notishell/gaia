AUTOMAKE_OPTIONS = foreign subdir-objects
noinst_LIBRARIES = libgaia.a 
libgaia_a_SOURCES = gaia.c addon/addon.c addon/manager/manager.c addon/config/direct_config.c \
	addon/shell/shell.c
libgaia_a_CFLAGS = -I.. $(AM_CFLAGS)