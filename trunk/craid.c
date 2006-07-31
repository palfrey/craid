/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2006  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

#define FUSE_USE_VERSION 25
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <stdlib.h>

typedef struct {
	const char *name;
} craid_dir;

craid_dir *dirs= NULL;
uint8_t dir_count=0;

static int craid_getattr(const char *path, struct stat *stbuf)
{
    int res = 0,i;
	off_t fullsize=0;

	char * cwd = g_get_current_dir();
	printf("cwd: %s\n",cwd);
	free(cwd);
	printf("getattr: %s\n",path);
    memset(stbuf, 0, sizeof(struct stat));
    res = -ENOENT;
	for(i=0;i<dir_count;i++)
	{
		char* fullpath = g_strdup_printf("%s%s",dirs[i].name,path);
		int ret;
		struct stat mybuf;
		printf("fullpath: '%s'\n",fullpath);
		ret = stat(fullpath,&mybuf);
		if (ret==0)
		{
			res = ret;
			memcpy(stbuf,&mybuf,sizeof(struct stat));
			fullsize+=mybuf.st_size;
		}
		else
			perror("stat");
		free(fullpath);	
	}
	stbuf->st_size = fullsize;
    return res;
}

static int craid_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

	printf("readdir: %s\n",path);
    if(strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    //filler(buf, craid_path + 1, NULL, 0);

    return 0;
}

static int craid_open(const char *path, struct fuse_file_info *fi)
{
	printf("open: %s\n",path);
	return -EACCES;
}

static int craid_create (const char *path, mode_t mode, struct fuse_file_info * ffi)
{
	int i;
	printf("create: %s\n",path);
	for(i=0;i<dir_count;i++)
	{
		char* fullpath = g_strdup_printf("%s%s",dirs[i].name,path);
		printf("fullpath: '%s'\n",fullpath);
		open(fullpath,O_EXCL|O_CREAT,mode);
	}
	return -ENOENT;
}

static int craid_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    //size_t len;
    (void) fi;
	printf("read: %s\n",path);

    /*len = strlen(craid_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, craid_str + offset, size);
    } else*/
        size = 0;

    return size;
}

static struct fuse_operations craid_oper = {
    .getattr	= craid_getattr,
    .readdir	= craid_readdir,
    .open	= craid_open,
    .read	= craid_read,
	.create = craid_create,
};

static const struct fuse_opt craid_opts[] = {
	{"dirs=%s",0,0},
	FUSE_OPT_END
};

int main(int argc, char *argv[])
{
	char *dir = NULL,*place,*cwd;
	struct fuse_args fa = FUSE_ARGS_INIT(argc,argv);
	fuse_opt_parse(&fa,&dir,craid_opts,NULL);
	printf("dirs = %s\n",dir);
	place =strtok(dir,":");
	cwd = g_get_current_dir();
	while(place!=NULL)
	{
		printf("place: %s\n",place);
		dirs = (craid_dir*)realloc(dirs,(dir_count+1)*sizeof(craid_dir));
		dirs[dir_count].name = g_strdup_printf("%s/%s",cwd,place);
		dir_count++;
		place =strtok(NULL,":");
	}
	return fuse_main(fa.argc, fa.argv, &craid_oper);
}
