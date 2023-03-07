#include <stdint.h>

#include "hashmap.h"
#include "test.h"
#include "typedefs.h"

uint32_t hash_u64(const u64 *val, const void *context) {
  return hash_eight_bytes(0, *val);
}

int cmp_u64(const u64 *a, const u64 *b) {
  return a - b;
}

a_hashmap mk_hm(void) {
  return ahm_new(u64, u64, cmp_u64, hash_u64, hash_u64);
}

void test_hashmap(test_state *state) {
  test_group_start(state, "HashMap");

  {
    test_start(state, "empty");
    a_hashmap hm = mk_hm();
    for (u64 i = 0; i < 1000; i++) {
      if (ahm_lookup(&hm, &i, NULL) != NULL) {
        failf(state, "Expected hashmap to be empty!");
      }
    }
    test_end(state);
  }

  {
    test_start(state, "reproduces");
    a_hashmap hm = mk_hm();
    for (u64 i = 0; i < 1000; i++) {
      u64 val = i + 1;
      ahm_upsert(&hm, &i, &i, &val, NULL);
    }
    for (u64 i = 0; i < 1000; i++) {
      u64 *res = ahm_lookup(&hm, &i, NULL);
      if (res == NULL) {
        failf(state, "Expected value %llu", i);
      } else if (*res != i + 1) {
        failf(state, "Wrong value %llu: %llu", i, *res);
      }
    }
    test_end(state);
  }

  test_group_end(state);
}
