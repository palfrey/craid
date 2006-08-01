# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.6

bin_PROGRAMS = craid
craid_SOURCES = craid.c uuidhash.c
CFLAGS +=$(GLIB_CFLAGS) -Wall -Werror -D_FILE_OFFSET_BITS=64
LDFLAGS += $(GLIB_LIBS) $(FUSE_LIBS) $(UUID_LIBS)
