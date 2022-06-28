/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2022          rw-r-r-0644 <r.r.qwertyuiop.r.r@gmail.com>
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include <sys/iosupport.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>
#include "isfs.h"
#include "super.h"

static void _isfsdev_fst_to_stat(const isfs_fst* fst, struct stat* st)
{
    memset(st, 0, sizeof(struct stat));

    st->st_uid = fst->uid;
    st->st_gid = fst->gid;

    st->st_mode = isfs_fst_is_dir(fst) ? S_IFDIR : 0;
    st->st_size = fst->size;

    st->st_nlink = 1;
    st->st_rdev = st->st_dev;
    st->st_mtime = 0;
}

static int _isfsdev_close_r(struct _reent* r, void* fd)
{
    isfs_file* fp = (isfs_file*) fd;

    int res = isfs_close(fp);
    if(res) {
        r->_errno = EIO;
        return -1;
    }

    return 0;
}

static int _isfsdev_open_r(struct _reent* r, void* fileStruct, const char* path, int flags, int mode)
{
    isfs_file* fp = (isfs_file*) fileStruct;

    if (flags & (O_WRONLY | O_RDWR | O_CREAT | O_EXCL | O_TRUNC)) {
        r->_errno = ENOSYS;
        return -1;
    }

    int res = isfs_open(fp, path);
    if(res) {
        r->_errno = EIO;
        return -1;
    }

    return 0;
}

static ssize_t _isfsdev_read_r(struct _reent* r, void* fd, char* ptr, size_t len)
{
    isfs_file* fp = (isfs_file*) fd;

    size_t read = 0;
    int res = isfs_read(fp, ptr, len, &read);
    if(res) {
        r->_errno = EIO;
        return -1;
    }

    return read;
}

static off_t _isfsdev_seek_r(struct _reent* r, void* fd, off_t pos, int dir)
{
    isfs_file* fp = (isfs_file*) fd;

    int res = isfs_seek(fp, (s32)pos, dir);

    if(res) {
        r->_errno = EIO;
        return -1;
    }

    return fp->offset;
}


static int _isfsdev_stat_r(struct _reent* r, const char* file, struct stat* st)
{
    isfs_fst* fst = isfs_stat(file);
    if(!fst) {
        r->_errno = ENOENT;
        return -1;
    }

    _isfsdev_fst_to_stat(fst, st);

    return 0;
}

static int _isfsdev_dirclose_r(struct _reent* r, DIR_ITER* dirState)
{
    isfs_dir* dir = (isfs_dir*) dirState->dirStruct;

    int res = isfs_dirclose(dir);
    if(res) {
        r->_errno = EIO;
        return -1;
    }

    return 0;
}

static DIR_ITER* _isfsdev_diropen_r(struct _reent* r, DIR_ITER* dirState, const char* path)
{
    isfs_dir* dir = (isfs_dir*) dirState->dirStruct;

    int res = isfs_diropen(dir, path);
    if(res) {
        r->_errno = EIO;
        return 0;
    }

    return dirState;
}

static int _isfsdev_dirnext_r(struct _reent* r, DIR_ITER* dirState, char* filename, struct stat* st)
{
    isfs_dir* dir = (isfs_dir*) dirState->dirStruct;

    isfs_fst* fst = NULL;
    int res = isfs_dirread(dir, &fst);
    if(res) {
        r->_errno = EIO;
        return -1;
    }

    if(!fst) return -1;

    _isfsdev_fst_to_stat(fst, st);

    memcpy(filename, fst->name, sizeof(fst->name));
    filename[sizeof(fst->name)] = '\0';

    return 0;
}

static int _isfsdev_dirreset_r(struct _reent* r, DIR_ITER* dirState)
{
    isfs_dir* dir = (isfs_dir*) dirState->dirStruct;

    int res = isfs_dirreset(dir);
    if(res) {
        r->_errno = EIO;
        return -1;
    }

    return 0;
}

static int _isfsdev_stub_r();

int isfs_add_dotab(isfs_ctx* ctx)
{
    devoptab_t* dotab = &ctx->devoptab;
    memset(dotab, 0, sizeof(devoptab_t));

    dotab->name = ctx->name;
    dotab->deviceData = ctx;
    dotab->structSize = sizeof(isfs_file);
    dotab->dirStateSize = sizeof(isfs_dir);

    dotab->chdir_r = _isfsdev_stub_r;
    dotab->chmod_r = _isfsdev_stub_r;
    dotab->fchmod_r = _isfsdev_stub_r;
    dotab->fstat_r = _isfsdev_stub_r;
    dotab->fsync_r = _isfsdev_stub_r;
    dotab->ftruncate_r = _isfsdev_stub_r;
    dotab->link_r = _isfsdev_stub_r;
    dotab->mkdir_r = _isfsdev_stub_r;
    dotab->rename_r = _isfsdev_stub_r;
    dotab->rmdir_r = _isfsdev_stub_r;
    dotab->statvfs_r = _isfsdev_stub_r;
    dotab->unlink_r = _isfsdev_stub_r;
    dotab->write_r = _isfsdev_stub_r;

    dotab->close_r = _isfsdev_close_r;
    dotab->open_r = _isfsdev_open_r;
    dotab->read_r = _isfsdev_read_r;
    dotab->seek_r = _isfsdev_seek_r;
    dotab->stat_r = _isfsdev_stat_r;
    dotab->dirclose_r = _isfsdev_dirclose_r;
    dotab->diropen_r = _isfsdev_diropen_r;
    dotab->dirnext_r = _isfsdev_dirnext_r;
    dotab->dirreset_r = _isfsdev_dirreset_r;

    return AddDevice(dotab);
}

static int _isfsdev_stub_r(struct _reent *r)
{
    r->_errno = ENOSYS;
    return -1;
}

void isfs_remove_dotab(isfs_ctx* ctx)
{
    RemoveDevice(ctx->name);
}
