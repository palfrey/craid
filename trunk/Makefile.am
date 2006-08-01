# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.6

SUBDIRS = src

CFLAGS+= -g -Wall -Werror
run:: src/craid
	-fusermount -u test
	@mkdir -p test
	gdb --args ./src/craid -d test -odirs=blocka:blockb
	@fusermount -u test
