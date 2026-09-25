// main.c
// fuckingfs v1.0.3
// the main program, also serves as a usage example. this will do more later but right now it just takes the files in the specified folder and creates a .bin file with the folder contents (minus subfolders) in a fuckingfs partition and then lists them
// under LGPL 3.0-or-later
// https://kittheconfusedcyborg.neocities.org/projs/fuckingfs/
// https://github.com/Matto58/fuckingfs

#include <dirent.h>
#include "fuckingfsutil.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: %s [path to folder]\ncreates a file called '[path to folder].bin' and encodes the files inside the folder into a fuckingfs partition, which will be in this bin file, then prints a file listing\n", argv[0]);
		return 1;
	}
	char binFilePath[256];
	snprintf(binFilePath, 256, "%s.bin", argv[1]);

	FILE *f = fopen(binFilePath, "w");
	ffsuInitPartition(f, argv[1]);

	DIR *d = opendir(argv[1]);
	struct dirent *de;
	while ((de = readdir(d)) != NULL) {
		char fullFilePath[256] = {0};
		snprintf(fullFilePath, 256, "%s/%s", argv[1], de->d_name);
		ffsuAddFile(f, fullFilePath, de->d_name);
	}
	closedir(d);
	fclose(f);

	f = fopen(binFilePath, "r");
	fclose(f);
	return 0;
}
