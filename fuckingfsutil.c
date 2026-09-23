#include "fuckingfs.h"
#include "fuckingfsutil.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>

void ffsuInitPartition(FILE *part, char *label) {
	struct fuckPartitionHeader p = {
		"abc", // magicStr1
		"xyz", // magicStr2
		FUCK_VERSION_NUM, // versionID
		0, // placeholder
		"", // label
	};
	strncpy(p.magicStr1, FUCK_GENERAL_MAGIC_STR, 4);
	strncpy(p.magicStr2, FUCK_PARTITION_MAGIC_STR, 4);
	strncpy(p.label, label, strnlen(label, 48));
	fwrite(&p, sizeof(struct fuckPartitionHeader), 1, part);
}

bool ffsuAddFile(FILE *part, char *fullPath, char *filename) {
	struct stat s;
	if (stat(fullPath, &s) == -1) {
		int j = errno;
		printf("ffsuAddFile: WARNING: stat for %s failed, skipping (errno=%d)\n", fullPath, j);
		return false;
	}
	if (S_ISDIR(s.st_mode)) return false;

	struct fuckFileHeader f = {
		"abc", // magicStr1
		"xyz", // magicStr2
		s.st_mode & 0x1ff, // permissions
		0, // placeholder
		"meow.txt", // filename
		0, // fileSizeDisplayed
		0, // sectorNextCount
		"foo", // owner
		0, // unixMsSinceCreation
		0, // unixMsSinceModification
	};
	strncpy(f.magicStr1, FUCK_GENERAL_MAGIC_STR, 4);
	strncpy(f.magicStr2, FUCK_FILE_MAGIC_STR, 4);
	strncpy(f.filename, filename, strnlen(filename, 12));

	struct passwd *p = getpwuid(s.st_uid); // how sketchy is this actually
	strncpy(f.owner, p->pw_name, 8);

	f.unixMsSinceCreation = s.st_ctim.tv_nsec/1000;
	f.unixMsSinceModification = s.st_mtim.tv_nsec/1000;

	f.fileSizeDisplayed = s.st_size;
	f.sectorNextCount = (s.st_size / 64) * 64;
	if ((s.st_size - f.sectorNextCount) > 0) f.sectorNextCount++;

	fwrite(&f, sizeof(struct fuckFileHeader), 1, part);

	FILE *srcFile = fopen(fullPath, "r");
	char buffer[64];
	size_t readBytes = 0;
	do {
		readBytes = fread(buffer, 1, 64, srcFile);
		if (readBytes == 0) break;
		fwrite(buffer, 1, 64, part);
	}
	while (readBytes == 64);

	fclose(srcFile);
	return true;
}

node_t *ffsuGetFileListing(FILE *part) {
	// todo: move linked list ops into separate functions
	node_t *list = malloc(sizeof(node_t));
	list->next = NULL;
	list->data = NULL;
	node_t *currNode = list;
	// todo?: maybe the partition header isn't going to be at the start, so seek until we encounter it and only then continue
	fseek(part, sizeof(struct fuckPartitionHeader), SEEK_SET);
	const size_t fhSize = sizeof(struct fuckFileHeader);
	struct fuckFileHeader h;
	while (fread(&h, fhSize, 1, part)) {
		if (strncmp(h.magicStr1, FUCK_GENERAL_MAGIC_STR, 4) || strncmp(h.magicStr2, FUCK_FILE_MAGIC_STR, 4))
			continue;
		if (currNode->data != NULL) {
			currNode->next = malloc(sizeof(node_t));
			currNode = currNode->next;
			currNode->next = NULL;
		}
		currNode->data = malloc(fhSize);
		memcpy(currNode->data, &h, fhSize);
		fseek(part, h.sectorNextCount*64, SEEK_CUR);
	}
	return list;
}

void ffsuListFiles(FILE *part) {
	node_t *fileFirst = ffsuGetFileListing(part);
	node_t *file = fileFirst;
	while (file != NULL) {
		struct fuckFileHeader *h = file->data;
		printf(
			"* %s [size: %lu bytes, size on disk: %lu bytes], owned by %s, permissions: %o\n",
			h->filename,
			h->fileSizeDisplayed, h->sectorNextCount*64,
			h->owner, h->permissions);
		file = file->next;
	}
	ffsuFreeLinkedList(fileFirst);
}

void ffsuFreeLinkedList(node_t *first) {
	node_t *n = first;
	while (n != NULL) {
		if (n->data) free(n->data);
		node_t *next = n->next;
		free(n);
		n = next;
	}
}