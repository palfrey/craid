/*
    CRAID: Consumer-friendly RAID
	Copyright (C) 2006 Tom Parker <palfrey@tevp.net>

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
#include <stdbool.h>
#include <uuid/uuid.h>
#include <assert.h>
#include "uuidhash.h"

typedef struct
{
	uuid_t uuid;
	const char *name;
} craid_dir;

typedef struct
{
	uuid_t next_drive;
	uint8_t idx;
	uint64_t block_size;
} craid_file;

GHashTable* dirs = NULL;
uint8_t dir_count=0;

bool fexists(const char *fullpath)
{
	struct stat mybuf;
	return stat(fullpath,&mybuf)==0;
}

typedef struct
{
	const char *path;
	int64_t ret;
	void *data;
} pathop;

static void getsize(gpointer key, gpointer value, gpointer user_data)
{
	craid_dir *dir = (craid_dir*)value;
	pathop *po = (pathop*)user_data;
	char* fullpath = g_strdup_printf("%s%s",dir->name,po->path);
	int ret;
	struct stat mybuf;
	printf("fullpath: '%s'\n",fullpath);
	ret = stat(fullpath,&mybuf);
	if (ret==0)
	{
		po->ret+=mybuf.st_size-sizeof(craid_file);
	}
	else
		perror("stat");
	free(fullpath);	
}

static int craid_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;
	pathop po;
	po.ret = 0;
	po.path = path;
	char * cwd = g_get_current_dir();
	printf("cwd: %s\n",cwd);
	free(cwd);
	printf("getattr: %s\n",path);
    memset(stbuf, 0, sizeof(struct stat));
	g_hash_table_foreach(dirs,getsize,&po);
	stbuf->st_size = po.ret;
    //res = -ENOENT;
    res = 0;
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

static void createfile(gpointer key, gpointer value, gpointer user_data)
{
	craid_dir *dir = (craid_dir*)value;
	pathop *po = (pathop*)user_data;
	char* fullpath = g_strdup_printf("%s%s",dir->name,po->path);
	mode_t *mode = po->data;
	printf("fullpath: '%s'\n",fullpath);
	open(fullpath,O_EXCL|O_CREAT,*mode);
}

static int craid_create (const char *path, mode_t mode, struct fuse_file_info * ffi)
{
	printf("create: %s\n",path);
	pathop po;
	po.ret = 0;
	po.path = path;
	po.data = &mode;
	g_hash_table_foreach(dirs,createfile,&po);
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

#define UUID_FILE ".uuid"

int main(int argc, char *argv[])
{
	char *dir = NULL,*place,*cwd;
	struct fuse_args fa = FUSE_ARGS_INIT(argc,argv);
	fuse_opt_parse(&fa,&dir,craid_opts,NULL);
	printf("dirs = %s\n",dir);
	place =strtok(dir,":");
	cwd = g_get_current_dir();
	dirs = g_hash_table_new(qsf_uuid_hash, (GCompareFunc)qsf_uuid_compare);
	while(place!=NULL)
	{
		printf("place: %s ",place);
		craid_dir *dir = (craid_dir*)malloc(sizeof(craid_dir));
		dir->name = g_strdup_printf("%s/%s",cwd,place);
		if (!fexists(dir->name))
		{
			printf("Panic! Source dir %s doesn't exist!\n",dir->name);
			exit(EXIT_FAILURE);
		}
		char* uuid_path = g_strdup_printf("%s/%s",dir->name,UUID_FILE);
		char tuid[37];
		if (!fexists(uuid_path))
		{
			uuid_generate(dir->uuid);
			uuid_unparse(dir->uuid,tuid);
			FILE *f = fopen(uuid_path,"w");
			int ret = fwrite(&tuid,1,36,f);
			assert(ret == 36);
			fclose(f);
		}
		else
		{
			FILE *f = fopen(uuid_path,"r");
			fread(&tuid,1,37,f);
			tuid[36] = '\0';
			fclose(f);
			uuid_parse(tuid,dir->uuid);
		}
		printf("uuid: %s\n",tuid);
		free(uuid_path);
		g_hash_table_insert(dirs,dir->uuid,dir);
		place =strtok(NULL,":");
	}
	return fuse_main(fa.argc, fa.argv, &craid_oper);
}
