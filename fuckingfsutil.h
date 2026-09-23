#include <stdbool.h>
#include <stdio.h>

struct node { void *data, *next; };
typedef struct node node_t;

void ffsuInitPartition(FILE *part, char *label);
bool ffsuAddFile(FILE *part, char *fullPath, char *filename);
node_t *ffsuGetFileListing(FILE *part);
void ffsuListFiles(FILE *part);
void ffsuFreeLinkedList(node_t *first);