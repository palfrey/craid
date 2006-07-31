CC=gcc
CFLAGS=`pkg-config --cflags glib-2.0 fuse` -g -Wall -Werror
LDFLAGS=`pkg-config --libs glib-2.0 fuse`

all: craid

run:: craid
	-fusermount -u test
	./craid -d test -odirs=blocka:blockb
	@fusermount -u test

craid: craid.o
