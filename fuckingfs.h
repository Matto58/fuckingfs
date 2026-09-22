// fuckingfs.h
// fuckingfs v1.0.0
// it's a fucking file system. it doesn't get any more complex than that
// first try at writing a somewhat functional file system
// it is the least practical thing in the world but i lowkey don't give a shit lmao

// this file system comes with NO WARRANTY, and by using it, you agree that:
// all data stored with it is NOT IMPORTANT and all lost with it is ENTIRELY YOUR FAULT, because i warned you.
// just to reiterate, by using the file system, ALL DATA LOSS (from storing your files with said file system) IS YOUR FAULT. you agree that I TAKE NO BLAME.
// okay? got it? good. this file system fucking sucks so really, don't use it for important stuff. YOU HAVE BEEN WARNED!

// under LGPL 3.0-or-later
// https://kittheconfusedcyborg.neocities.org/projs/fuckingfs/
// https://github.com/Matto58/fuckingfs

#ifndef _FUCKINGFS_H
#define _FUCKINGFS_H

#pragma once

#include <stdio.h>
#include <stdint.h>

// defined in fuckingfs.c
extern const char FUCK_GENERAL_MAGIC_STR[4];
extern const char FUCK_PARTITION_MAGIC_STR[4];
extern const char FUCK_FILE_MAGIC_STR[4];
extern const uint32_t FUCK_VERSION_NUM;

// permission flags, can be OR'd together
enum fuckPerms : uint16_t {
	PERMS_NONE = 0,
	PERMS_OTHERS_EXECUTE = 01,
	PERMS_OTHERS_WRITE = 02,
	PERMS_OTHERS_READ = 04,
	PERMS_GROUP_EXECUTE = 010,
	PERMS_GROUP_WRITE = 020,
	PERMS_GROUP_READ = 040,
	PERMS_OWNER_EXECUTE = 0100,
	PERMS_OWNER_WRITE = 0200,
	PERMS_OWNER_READ = 0400,
};

struct fuckPartitionHeader {
	char magicStr1[4];
	char magicStr2[4];
	uint32_t versionID;
	uint32_t placeholder;
	char label[40];
	uint64_t firstFilePtr;
};
struct fuckFileHeader {
	char magicStr1[4];
	char magicStr2[4];
	uint16_t permissions;
	uint16_t placeholder;
	char filename[12];
	uint64_t fileSizeDisplayed;
	uint64_t sectorNextCount;
	char owner[8];
	int64_t unixMsSinceCreation;
	int64_t unixMsSinceModification;
};

struct node { void *data, *next; };
typedef struct node node_t;

node_t *fuckListFiles(FILE *part);
void fuckFreeLinkedList(node_t *first);

#endif
