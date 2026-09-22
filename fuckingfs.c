// fuckingfs.c
// fuckingfs v1.0.0
// implementations for functions and fields in fuckingfs.h
// under LGPL 3.0-or-later
// https://kittheconfusedcyborg.neocities.org/projs/fuckingfs/
// https://github.com/Matto58/fuckingfs

#include "fuckingfs.h"
#include <stdlib.h>
#include <string.h>

const char FUCK_GENERAL_MAGIC_STR[4] = {'F', 'U', 'C', 'K'};
const char FUCK_PARTITION_MAGIC_STR[4] = {'P', 'a', 'r', 't'};
const char FUCK_FILE_MAGIC_STR[4] = {'F', 'i', 'l', 'e'};
const uint32_t FUCK_VERSION_NUM = 0x010000; // 0xXXYYZZ XX=major YY=minor ZZ=patch

node_t *fuckListFiles(FILE *part) {
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
void fuckFreeLinkedList(node_t *first) {
	node_t *n = first;
	while (n != NULL) {
		if (n->data) free(n->data);
		node_t *next = n->next;
		free(n);
		n = next;
	}
}
