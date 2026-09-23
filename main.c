// main.c
// fuckingfs v1.0.1
// the main program, also serves as a usage example. this will do more later but right now it just takes the files in the specified folder and creates a .bin file with the folder contents (minus subfolders) in a fuckingfs partition and then lists them
// under LGPL 3.0-or-later
// https://kittheconfusedcyborg.neocities.org/projs/fuckingfs/
// https://github.com/Matto58/fuckingfs

#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include "fuckingfs.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: %s [path to folder]\ncreates a file called '[path to folder].bin' and encodes the files inside the folder into a fuckingfs partition, which will be in this bin file, then prints a file listing\n", argv[0]);
		return 1;
	}
	char binFilePath[256];
	snprintf(binFilePath, 256, "%s.bin", argv[1]);

	// the writing portion
	// todo: move writing and reading portion into functions in fuckingfsutil.c/.h
	FILE *f = fopen(binFilePath, "w");
	struct fuckPartitionHeader p1 = {
		"abc", // magicStr1
		"xyz", // magicStr2
		FUCK_VERSION_NUM, // versionID
		0, // placeholder
		"", // label
		0, // firstFilePtr
	};
	strncpy(p1.magicStr1, FUCK_GENERAL_MAGIC_STR, 4);
	strncpy(p1.magicStr2, FUCK_PARTITION_MAGIC_STR, 4);

	fwrite(&p1, sizeof(struct fuckPartitionHeader), 1, f);

	DIR *d = opendir(argv[1]);
	struct dirent *de;
	while ((de = readdir(d)) != NULL) {
		char fullFilePath[256] = {0};
		snprintf(fullFilePath, 256, "%s/%s", argv[1], de->d_name);

		struct stat s;
		if (stat(fullFilePath, &s) == -1) {
			int j = errno;
			printf("WARNING: stat for %s failed, skipping (errno=%d)\n", fullFilePath, j);
			continue;
		}
		if (S_ISDIR(s.st_mode)) continue;

		struct fuckFileHeader h = {
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
		strncpy(h.magicStr1, FUCK_GENERAL_MAGIC_STR, 4);
		strncpy(h.magicStr2, FUCK_FILE_MAGIC_STR, 4);
		strncpy(h.filename, de->d_name, 12);

		struct passwd *p = getpwuid(s.st_uid); // how sketchy is this actually
		strncpy(h.owner, p->pw_name, 8);

		h.unixMsSinceCreation = s.st_ctim.tv_nsec/1000;
		h.unixMsSinceModification = s.st_mtim.tv_nsec/1000;

		h.fileSizeDisplayed = s.st_size;
		h.sectorNextCount = (s.st_size / 64) * 64;
		if ((s.st_size - h.sectorNextCount) > 0) h.sectorNextCount++;

		fwrite(&h, sizeof(struct fuckFileHeader), 1, f);

		FILE *currentFile = fopen(fullFilePath, "r");
		char buffer[64];
		size_t readBytes = 0;
		do {
			readBytes = fread(buffer, 1, 64, currentFile);
			if (readBytes == 0) break;
			fwrite(buffer, 1, 64, f);
		}
		while (readBytes == 64);

		fclose(currentFile);
	}
	closedir(d);
	fclose(f);

	// the reading portion
	f = fopen(binFilePath, "r");
	struct fuckPartitionHeader p2;
	fread(&p2, sizeof(struct fuckPartitionHeader), 1, f);
	if (strncmp(p2.magicStr1, FUCK_GENERAL_MAGIC_STR, 4) || strncmp(p2.magicStr2, FUCK_PARTITION_MAGIC_STR, 4)) {
		printf("ERROR: partition header is invalid\n");
		fclose(f);
		return 1;
	}
	node_t *fileFirst = fuckListFiles(f);
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
	fclose(f);
	fuckFreeLinkedList(fileFirst);
	return 0;
}
