#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct flag {
  int b, e, n, s, t, v;
} flags;

int get_flags(int argc, char **argv, flags *flag);
void file_processing(char **argv, flags *flag);

#endif