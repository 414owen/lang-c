#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "consts.h"
#include "source.h"
#include "vec.h"
#include "parser.h"

#define T_EOF 0

typedef unsigned char token_type;

typedef struct {
  token_type type;
  BUF_IND_T start;
  BUF_IND_T end;
} token;

typedef struct {
  bool succeeded;
  token token;
} token_res;

VEC_DECL(token);

typedef struct {
  bool succeeded;
  union {
    vec_token tokens;
    BUF_IND_T error_pos;
  };
} tokens_res;

token_res scan(source_file file, BUF_IND_T pos);
