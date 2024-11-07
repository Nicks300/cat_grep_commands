#ifndef SRC_CAT_S21_GREP_H_
#define SRC_CAT_S21_GREP_H_
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct flag {
  int i, v, c, l, n, h, s, o;
} flags;

int parsing(char *str[], flags *flag, int count, char (*template_array)[500],
            int *count_template, char (*file_array)[500], int *count_file,
            char (*file_array_temp)[500], int *count_file_temp);
void template(char (*template_array)[500], int *count_template,
              char (*file_array_temp)[500], int *count_file_temp);
void grep_processing(char (*template_array)[500], int count_template,
                     char (*file_array)[500], int count_file, flags *flag);
void e_case(char **str, int *check_file, int count, char (*template_array)[500],
            int **template, int *pat_temp, int *checkup);
void o_case(int coincidence, int *for_o, int string_number, flags *f,
            regmatch_t *match, char *text, int count_file,
            char (*file_array)[500], int i);
void n_case(int coincidence, flags *f, int string_number, char *text,
            int count_file, char (*file_array)[500], int i);
void f_case(flags *f, int count_file, int i, char (*file_array)[500],
            int string);
void f_extra(char **str, int *check_file, int count,
             char (*file_array_temp)[500], int **count_temp, int *pat_temp,
             int *checkup);

#endif