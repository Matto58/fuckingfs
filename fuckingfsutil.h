// fuckingfsutil.h
// fuckingfs v1.0.3
// utility functions for directly working with partitions
// under LGPL 3.0-or-later
// https://kittheconfusedcyborg.neocities.org/projs/fuckingfs/
// https://github.com/Matto58/fuckingfs

#include <stdbool.h>
#include <stdio.h>

struct node { void *data, *next; };
typedef struct node node_t;

node_t *ffsuInitLinkedList(void *data);
void ffsuCopyAppendToLinkedList(node_t *first, void *data, size_t size);
void ffsuInitPartition(FILE *part, char *label);
bool ffsuAddFile(FILE *part, char *fullPath, char *filename);
node_t *ffsuGetFileListing(FILE *part);
void ffsuListFiles(FILE *part);
void ffsuFreeLinkedList(node_t *first);
