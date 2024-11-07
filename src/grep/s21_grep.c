#include "s21_grep.h"

int main(int argc, char **argv) {
  flags flag = {0};
  flags *f = &flag;
  char template_array[argc][500];
  char file_array[argc][500];
  char file_array_temp[argc][500];
  int count_template = 0;
  int count_file = 0;
  int count_file_temp = 0;
  if (argc > 2 &&
      parsing(argv, f, argc, template_array, &count_template, file_array,
              &count_file, file_array_temp, &count_file_temp) != 1) {
    template(template_array, &count_template, file_array_temp,
             &count_file_temp);
    grep_processing(template_array, count_template, file_array, count_file, f);
  }
  return 0;
}

int parsing(char **str, flags *f, int count, char (*template_array)[500],
            int *count_template, char (*file_array)[500], int *count_file,
            char (*file_array_temp)[500], int *count_file_temp) {
  const char flags[11] = "eivclnhsfo";
  int checkup = 0;
  int pat_temp = 0;
  for (int x = 1; x < count; x++) {
    int check_file = 0;
    if (str[x][0] == '-') {
      for (size_t i = 1; i < strlen(str[x]) && check_file == 0; i++) {
        if (strchr(flags, (int)str[x][i]) == NULL && check_file == 0) {
          checkup = 1;
          break;
        }
        if (str[x][i] == 'e')
          e_case(str, &check_file, count, template_array, &count_template,
                 &pat_temp, &checkup);
        else if (str[x][i] == 'i')
          f->i = 1;
        else if (str[x][i] == 'v')
          f->v = 1;
        else if (str[x][i] == 'c')
          f->c = 1;
        else if (str[x][i] == 'l')
          f->l = 1;
        else if (str[x][i] == 'n')
          f->n = 1;
        else if (str[x][i] == 'h')
          f->h = 1;
        else if (str[x][i] == 's')
          f->s = 1;
        else if (str[x][i] == 'f')
          f_extra(str, &check_file, count, file_array_temp, &count_file_temp,
                  &pat_temp, &checkup);
        else if (str[x][i] == 'o')
          f->o = 1;
      }
    } else if (pat_temp == 1 && str[x][0] != '-' && check_file == 0) {
      strcpy(file_array[(*count_file)++], str[x]);
      check_file = 1;
      continue;
    } else if (pat_temp == 0 && str[x][0] != '-' && check_file == 0) {
      pat_temp = 1;
      check_file = 1;
      strcpy(template_array[(*count_template)++], str[x]);
      continue;
    }
  }
  if (f->v) f->o = 0;
  return checkup;
}

void template(char (*template_array)[500], int *count_template,
              char (*file_array_temp)[500], int *count_file_temp) {
  for (int i = 0; i < (*count_file_temp); i++) {
    FILE *fp = fopen(file_array_temp[i], "r");
    if (fp != NULL) {
      char str[500];
      while (fgets(str, 500, fp) != NULL) {
        if (str[strlen(str) - 1] == '\n' && strlen(str) > 1)
          str[strlen(str) - 1] = '\0';
        strcpy(template_array[(*count_template)++], str);
      }
    } else
      fclose(fp);
  }
}

void grep_processing(char (*template_array)[500], int count_template,
                     char (*file_array)[500], int count_file, flags *f) {
  int regular = REG_EXTENDED;
  if (f->i) regular = REG_ICASE;
  for (int i = 0; i < count_file; i++) {
    FILE *fp = fopen(file_array[i], "r");
    if (fp != NULL) {
      int string = 0, string_number = 1;
      char text[4096] = {0};
      while (fgets(text, 4095, fp) != NULL) {
        int coincidence = 0, for_o = 0;
        regex_t current;
        int string_check = 0;
        for (int x = 0; x < count_template; x++) {
          regmatch_t match;
          regcomp(&current, template_array[x], regular);
          int successfully = regexec(&current, text, 1, &match, 0);
          if (strchr(text, '\n') == NULL) strcat(text, "\n");
          if (successfully == 0 && !f->v) coincidence = 1;
          if (successfully == 0 && f->v) string_check = 1;
          if ((successfully = REG_NOMATCH) && f->v) coincidence = 1;
          o_case(coincidence, &for_o, string_number, f, &match, text,
                 count_file, file_array, i);
          if (string_check == 1) coincidence = 0;
          regfree(&current);
        }
        n_case(coincidence, f, string_number, text, count_file, file_array, i);
        string += coincidence;
        string_number++;
      }
      f_case(f, count_file, i, file_array, string);
      if (f->l && string > 0) printf("%s\n", file_array[i]);
      if (f->c && !f->l) {
        if (!f->h && !f->l && count_file > 1) printf("%s:", file_array[i]);
        printf("%d\n", string);
      }
      fclose(fp);
    } else {
      if (!f->s)
        fprintf(stderr, "grep_processing: %s: No such file or directory\n",
                file_array[i]);
    }
  }
}

void e_case(char **str, int *check_file, int count, char (*template_array)[500],
            int **count_template, int *pat_temp, int *checkup) {
  for (int x = 1; x < count; x++) {
    for (size_t i = 1; i < strlen(str[x]) && *check_file == 0; i++) {
      if (strlen(str[x]) == i + 1 && *check_file == 0) {
        if (!(x == count - 1)) {
          strcpy(template_array[(**count_template)++], str[++x]);
          *pat_temp = 1;
          *check_file = 1;
        } else {
          *checkup = 1;
        }
      } else if (check_file == 0) {
        *check_file = 1;
        strcpy(template_array[(**count_template)++], (str[x] + i + 1));
        *pat_temp = 1;
      }
    }
  }
}

void o_case(int coincidence, int *for_o, int string_number, flags *f,
            regmatch_t *match, char *text, int count_file,
            char (*file_array)[500], int i) {
  if (coincidence == 1 && f->o && !f->l && !f->c) {
    if (f->n && *for_o == 0) {
      printf("%d:", string_number);
      *for_o = 1;
    }
    for (int z = match->rm_so; z < match->rm_eo; z++) printf("%c", text[z]);
    if (match->rm_eo != match->rm_so) printf("\n");
    if (count_file > 1 && !f->h && (match->rm_eo != match->rm_so))
      printf("%s:", file_array[i]);
  }
}

void n_case(int coincidence, flags *f, int string_number, char *text,
            int count_file, char (*file_array)[500], int i) {
  if (coincidence == 1 && !f->l && !f->c && !f->o) {
    if (!f->h && !f->l && count_file > 1) printf("%s:", file_array[i]);
    if (coincidence == 1 && !f->l && !f->c && f->n)
      printf("%d:", string_number);
    printf("%s", text);
  }
}

void f_case(flags *f, int count_file, int i, char (*file_array)[500],
            int string) {
  if (f->l && f->c) {
    if (count_file > 1 && !f->h) printf("%s:", file_array[i]);
    if (string >= 1)
      printf("1\n");
    else
      printf("0\n");
  }
}

void f_extra(char **str, int *check_file, int count,
             char (*file_array_temp)[500], int **count_file_temp, int *pat_temp,
             int *checkup) {
  for (int x = 1; x < count; x++) {
    for (size_t i = 1; i < strlen(str[x]) && *check_file == 0; i++) {
      if (strlen(str[x]) == i + 1 && *check_file == 0) {
        if (!(x == count - 1)) {
          strcpy(file_array_temp[(**count_file_temp)++], str[++x]);
          *check_file = 1;
          *pat_temp = 1;
        } else {
          *checkup = 1;
        }
      } else if (check_file == 0) {
        *check_file = 1;
        strcpy(file_array_temp[(**count_file_temp)++], (str[x] + i + 1));
        *pat_temp = 1;
      }
    }
  }
}