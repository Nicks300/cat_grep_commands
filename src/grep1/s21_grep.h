#ifndef S21_GREP_H
#define S21_GREP_H

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 128
#define BUF_SIZE 2048

typedef struct Input {
  char *patterns[SIZE];
  char *pattern_files[SIZE];
  char *files[SIZE];
  char pattern[BUF_SIZE];
  bool e_flag, i_flag, v_flag, c_flag, l_flag, n_flag, h_flag, s_flag, f_flag,
      o_flag;
  int files_count;
  int patterns_count;
  int pattern_files_count;
  int matches_count;
} Input;

void initialize_input(Input *input);
void correct_files(Input *input);
void parse_input(int argc, char *argv[], Input *input);
int make_golbal_pattern(Input *input);
void make_global_pattern_from_file(Input *input, int num);
void process_files(Input *input);
void grep_files(FILE *file, Input *input, regex_t *regex, int k);

#endif