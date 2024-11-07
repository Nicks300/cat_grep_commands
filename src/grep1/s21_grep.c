#include "s21_grep.h"

int main(int argc, char *argv[]) {
  Input input;

  initialize_input(&input);
  parse_input(argc, argv, &input);
  correct_files(&input);
  if (make_golbal_pattern(&input) > 0) {
    process_files(&input);
  } else {
    fprintf(stderr, "s21_grep: No pattern\n");
  }

  return 0;
}

void initialize_input(Input *input) {
  input->e_flag = false;
  input->i_flag = false;
  input->v_flag = false;
  input->c_flag = false;
  input->l_flag = false;
  input->n_flag = false;
  input->h_flag = false;
  input->s_flag = false;
  input->f_flag = false;
  input->o_flag = false;

  input->matches_count = 0;
}

void correct_files(Input *input) {
  FILE *file_pointer = NULL;

  for (int i = 0; i < input->files_count; i++) {
    file_pointer = fopen(input->files[i], "r");
    if (file_pointer == NULL) {
      if (!input->s_flag)
        fprintf(stderr, "s21_grep: %s: No such file or directory\n",
                input->files[i]);
    }
    else{
      fclose(file_pointer);
    }
      
  }
}

void parse_input(int argc, char *argv[], Input *input) {
  int files_count = 0;
  int patterns_count = 0;
  int pattern_files_count = 0;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      int len = strlen(argv[i]);

      for (int j = 1; j < len; j++) {
        if (argv[i][j] == 'e') {
          input->e_flag = true;
          if (argv[i + 1]) input->patterns[patterns_count++] = argv[++i];
        } else if (argv[i][j] == 'i') {
          input->i_flag = true;
        } else if (argv[i][j] == 'v') {
          input->v_flag = true;
        } else if (argv[i][j] == 'c') {
          input->c_flag = true;
        } else if (argv[i][j] == 'l') {
          input->l_flag = true;
        } else if (argv[i][j] == 'n') {
          input->n_flag = true;
        } else if (argv[i][j] == 'h') {
          input->h_flag = true;
        } else if (argv[i][j] == 's') {
          input->s_flag = true;
        } else if (argv[i][j] == 'f') {
          input->f_flag = true;
          if (argv[i + 1])
            input->pattern_files[pattern_files_count++] = argv[++i];
        } else if (argv[i][j] == 'o') {
          input->o_flag = true;
        } else {
          fprintf(stderr, "s21_grep: %s: Incorrect options\n", argv[i]);
          exit(EXIT_FAILURE);
        }
      }
    } else if (!input->e_flag && !input->f_flag) {
      input->e_flag = true;
      input->patterns[patterns_count++] = argv[i];
    } else {
      input->files[files_count++] = argv[i];
    }
  }

  if (input->o_flag && (input->l_flag || input->v_flag || input->c_flag)) {
    input->o_flag = false;
  }

  if (input->n_flag && (input->l_flag || input->c_flag)) {
    input->n_flag = false;
  }

  input->files_count = files_count;
  input->patterns_count = patterns_count;
  input->pattern_files_count = pattern_files_count;
}

int make_golbal_pattern(Input *input) {
  int patterns_sum = 0;

  for (int i = 0; i < input->patterns_count; i++) {
    strcat(strcat(input->pattern, strlen(input->pattern) == 0 ? "" : "|"),
           strlen(input->patterns[i]) == 0 ? "," : input->patterns[i]);
    ++patterns_sum;
  }
  for (int i = 0; i < input->pattern_files_count; i++) {
    make_global_pattern_from_file(input, i);
    ++patterns_sum;
  }

  return patterns_sum;
}

void make_global_pattern_from_file(Input *input, int num) {
  char buf[BUF_SIZE];
  FILE *fp = fopen(input->pattern_files[num], "r");

  if (fp) {
    while (fgets(buf, BUF_SIZE, fp)) {
      char *c = buf + strlen(buf) - 1;

      if (*c == '\n') *c = '\0';

      strcat(strcat(input->pattern, strlen(input->pattern) == 0 ? "" : "|"),
             strlen(buf) == 0 ? "," : buf);
    }
  } else {
    if (!input->s_flag)
      fprintf(stderr, "s21_grep: %s: No such file or directory\n",
              input->pattern_files[num]);
  }
}

void process_files(Input *input) {
  regex_t regex;
  int regex_flags = 0;

  if (input->i_flag) {
    regex_flags |= REG_ICASE;
  }
  if (input->e_flag) {
    regex_flags |= REG_EXTENDED;
  }

  if (regcomp(&regex, input->pattern, regex_flags)) {
    fprintf(stderr, "Failled to compile regex\n");
    exit(EXIT_FAILURE);
  }

  for (int k = 0; k < input->files_count; k++) {
    FILE *fp = fopen(input->files[k], "r");

    if (fp != NULL) {
      grep_files(fp, input, &regex, k);
      fclose(fp);
    }
  }
  regfree(&regex);
}

void grep_files(FILE *file, Input *input, regex_t *regex, int k) {
  int line_number = 0;
  int matching_lines_count = 0;
  regmatch_t match;
  char line[BUF_SIZE];

  line_number = 0;
  matching_lines_count = 0;

  while (fgets(line, sizeof(line), file) != NULL) {
    line_number++;
    int len = strlen(line);
    if (line[len - 1] != '\n') {
      line[len] = '\n';
      line[len + 1] = '\0';
    }

    if (input->v_flag) {
      if (regexec(regex, line, 1, &match, 0)) {
        matching_lines_count++;
        if (!input->h_flag && input->files_count > 1) {
          if (input->c_flag && matching_lines_count == 1) {
            printf("%s", input->files[k]);
          } else if (!input->c_flag && !input->l_flag) {
            printf("%s:", input->files[k]);
          }
        }

        if (input->c_flag) {
          if (input->files_count > 1 && matching_lines_count == 1 &&
              !input->h_flag) {
            printf(":");
          }
          if (input->l_flag) {
            printf("%d\n", matching_lines_count);
            printf("%s\n", input->files[k]);
            break;
          } else {
            continue;
          }
        } else if (input->l_flag) {
          printf("%s\n", input->files[k]);
          break;
        } else if (input->n_flag) {
          printf("%d:%s", line_number, line);
        } else {
          printf("%s", line);
        }
      }

    } else {
      if (!regexec(regex, line, 1, &match, 0)) {
        matching_lines_count++;
        if (!input->h_flag && input->files_count > 1) {
          if (input->c_flag && matching_lines_count == 1) {
            printf("%s", input->files[k]);
          } else if (!input->c_flag && !input->l_flag) {
            printf("%s:", input->files[k]);
          }
        }

        if (input->c_flag) {
          if (input->files_count > 1 && matching_lines_count == 1 &&
              !input->h_flag) {
            printf(":");
          }
          if (input->l_flag) {
            printf("%d\n", matching_lines_count);
            printf("%s\n", input->files[k]);
            break;
          } else {
            continue;
          }
        } else if (input->l_flag) {
          printf("%s\n", input->files[k]);
          break;
        } else if (!input->o_flag) {
          if (input->n_flag) {
            printf("%d:%s", line_number, line);
          } else {
            printf("%s", line);
          }
        } else {
          if (input->n_flag) {
            int precision = (int)match.rm_eo - match.rm_so;
            printf("%d:%.*s\n", line_number, precision, line + match.rm_so);
          } else {
            int precision = (int)match.rm_eo - match.rm_so;
            printf("%.*s\n", precision, line + match.rm_so);
          }
        }
      }
    }
  }

  if (input->c_flag) {
    if (!input->h_flag && input->files_count > 1 && matching_lines_count == 0) {
      printf("%s:", input->files[k]);
    }
    if (!input->l_flag ||
        (matching_lines_count == 0 || matching_lines_count > 1))
      printf("%d\n", matching_lines_count);
  }

  matching_lines_count = 0;
  
}
