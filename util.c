#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

void *memclone(void *src, size_t bytes) {
  void *dest = malloc(bytes);
  memcpy(dest, src, bytes);
  return dest;
}

void ss_init(stringstream *ss) {
  size_t bufferSize = 0;
  ss->stream = open_memstream(&ss->string, &bufferSize);
}

// Won't update copies of the stringstream that was used to `init`.
void ss_finalize(stringstream *ss) {
  putc(0, ss->stream);
  fclose(ss->stream);
}

int vasprintf(char **buf, const char *restrict fmt, va_list rest) {
  stringstream ss;
  ss_init(&ss);
  int res = vfprintf(ss.stream, fmt, rest);
  ss_finalize(&ss);
  *buf = ss.string;
  return res;
}

int asprintf(char **buf, const char *restrict fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int res = vasprintf(buf, fmt, ap);
  va_end(ap);
  return res;
}

char *join(size_t str_amt, char **strs) {
  size_t space = 0;
  size_t *lens = __builtin_alloca(str_amt * sizeof(size_t));
  for (size_t i = 0; i < str_amt; i++) {
    lens[i] = strlen(strs[i]);
    space += lens[i];
  }
  char *res = malloc(space + 1);
  res[space] = '\0';
  size_t ind = 0;
  for (size_t i = 0; i < str_amt; i++) {
    memcpy(res + ind, strs[i], lens[i]);
    ind += lens[i];
  }
  return res;
}
