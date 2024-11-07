#include "s21_cat.h"

int main(int argc, char **argv) {
  flags flag = {0};
  get_flags(argc, argv, &flag);
  while (optind < argc) {
    file_processing(argv, &flag);
    optind++;
  }
  return 0;
}

int get_flags(int argc, char **argv, flags *flag) {
  int f;
  const char *short_options = "TtEevsnb";
  static struct option options[] = {
      {"number-nonblank", 0, 0, 'b'},
      {"number", 0, 0, 'n'},
      {"squeeze-blank", 0, 0, 's'},
      {0, 0, 0, 0},
  };
  while ((f = getopt_long(argc, argv, short_options, options, NULL)) != -1) {
    switch (f) {
      case 'T':
        flag->t = 1;
        break;
      case 't':
        flag->t = 1;
        flag->v = 1;
        break;
      case 'E':
        flag->e = 1;
        break;
      case 'e':
        flag->e = 1;
        flag->v = 1;
        break;
      case 'v':
        flag->v = 1;
        break;
      case 's':
        flag->s = 1;
        break;
      case 'n':
        flag->n = 1;
        break;
      case 'b':
        flag->b = 1;
        break;
      default:
        break;
    }
    if (flag->b && flag->n) flag->n = 0;
  }
  return 0;
}

void file_processing(char **argv, flags *flag) {
  FILE *fp = fopen(argv[optind], "r");
  if (fp != NULL) {
    int count = 1;
    int empty_count = 0;
    int prev_symb = '\n';
    while (!feof(fp)) {
      int symb = fgetc(fp);
      if (symb == EOF) break;
      if (flag->s && symb == '\n' && prev_symb == '\n') {
        empty_count++;
        if (empty_count > 1) {
          continue;
        }
      } else {
        empty_count = 0;
      }
      if (prev_symb == '\n' && ((flag->b && symb != '\n') || flag->n))
        printf("%6d\t", count++);
      if (flag->t && symb == '\t') {
        printf("^");
        symb = 'I';
      }
      if (flag->e && symb == '\n' && !flag->b) printf("$");
      if (flag->e && symb == '\n' && flag->b && prev_symb == '\n')
        printf("      \t$");
      if (flag->e && symb == '\n' && flag->b && prev_symb != '\n') printf("$");
      if (flag->v) {
        if ((symb >= 0 && symb < 9) || (symb > 10 && symb < 32) ||
            (symb > 126 && symb <= 160)) {
          printf("^");
          if (symb > 126) {
            symb -= 64;
          } else {
            symb += 64;
          }
        }
      }
      printf("%c", symb);
      prev_symb = symb;
    }
    fclose(fp);
  } else {
    printf("No file");
  }
}