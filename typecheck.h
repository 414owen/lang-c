#pragma once

#include "parse_tree.h"
#include "vec.h"
#include "types.h"

typedef enum {
  INT_LARGER_THAN_MAX,
  TYPE_NOT_FOUND,
  AMBIGUOUS_TYPE,
  BINDING_NOT_FOUND,

  // TODO Do we need this?
  TYPE_MISMATCH,

  TYPE_HEAD_MISMATCH,
  LITERAL_MISMATCH,
  WRONG_ARITY,
  MISSING_SIG,
} tc_error_type;

typedef struct {
  tc_error_type type;
  NODE_IND_T pos;

  union {
    struct {
      NODE_IND_T expected;
      union {
        NODE_IND_T got;
        type_tag type_head;
      };
    };
    struct {
      NODE_IND_T exp_param_amt;
      NODE_IND_T got_param_amt;
    };
  };

} tc_error;

VEC_DECL(tc_error);

typedef struct {
  vec_tc_error errors;
  // all the types
  vec_type types;
  // type sub-indices
  vec_node_ind type_inds;
  // index into types, one per parse_node
  NODE_IND_T *node_types;

  parse_tree tree;
  source_file source;
} tc_res;

tc_res typecheck(source_file source, parse_tree tree);
void free_tc_res(tc_res res);
