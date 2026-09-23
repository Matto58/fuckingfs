// fuckingfs.c
// fuckingfs v1.0.1
// implementations for fields in fuckingfs.h
// under LGPL 3.0-or-later
// https://kittheconfusedcyborg.neocities.org/projs/fuckingfs/
// https://github.com/Matto58/fuckingfs

#include "fuckingfs.h"

const char FUCK_GENERAL_MAGIC_STR[4] = {'F', 'U', 'C', 'K'};
const char FUCK_PARTITION_MAGIC_STR[4] = {'P', 'a', 'r', 't'};
const char FUCK_FILE_MAGIC_STR[4] = {'F', 'i', 'l', 'e'};
const uint32_t FUCK_VERSION_NUM = 0x010001; // 0xXXYYZZ XX=major YY=minor ZZ=patch
