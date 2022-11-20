#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "test.h"
#include "tests.h"
#include "args.h"

const char *byte_suffixes[] = {"B", "KB", "MB", "GB", "TB"};
const long mega = 1e6;

// TODO move to utils, if we need it again
static void print_byte_amount(FILE *f, uint64_t bytes) {
	unsigned long i = 0;
	double dblBytes = bytes;

	if (bytes > 1024) {
		for (i = 0; (bytes / 1024) > 0 && i < STATIC_LEN(byte_suffixes) - 1; i++, bytes /= 1024) {
			dblBytes = bytes / 1024.0;
    }
	}

	fprintf(f, "%.02lf %s", dblBytes, byte_suffixes[i]);
}

const char *qualtity_suffixes[] = {"", "thousand", "million", "billion"};

static void print_amount(FILE *f, uint64_t amt) {
  stringstream ss;
  ss_init_immovable(&ss);
  fprintf(ss.stream, "%" PRIu64, amt);
  ss_finalize(&ss);
  size_t len = strlen(ss.string);
  for (size_t i = 0; i < len; i++) {
    if (i > 0 && (len - i) % 3 == 0) {
      putc('_', f);
    }
    putc(ss.string[i], f);
  }
  free(ss.string);
}

static double timespec_to_seconds(struct timespec s) {
  double res = 0;
  res += s.tv_sec;
  res += s.tv_nsec / 1e9;
  return res;
}

static double timespec_to_millis(struct timespec s) {
  double res = 0;
  res += s.tv_sec * 1e3;
  res += s.tv_nsec / 1e6;
  return res;
}

static double timespec_to_micros(struct timespec s) {
  double res = 0;
  res += s.tv_sec * 1e6;
  res += s.tv_nsec / 1e3;
  return res;
}

static void print_nanos(FILE *f, long unsigned ns) {
  fprintf(f, "%luns", ns);
}

static void print_seconds(FILE *f, double s) {
  fprintf(f, "%.3fs", s);
}

static void print_millis(FILE *f, double ms) {
  fprintf(f, "%.3fms", ms);
}

static void print_micros(FILE *f, double micros) {
  if (micros < 1) {
    print_nanos(f, (long unsigned) (micros * 1000));
  } else {
    fprintf(f, "%.3fµs", micros);
  }
}

static void print_timespec(FILE *f, struct timespec ts) {
  if (ts.tv_sec > 0) {
    print_seconds(f, timespec_to_seconds(ts));
  } else if (ts.tv_nsec > 1e6) {
    print_millis(f, timespec_to_millis(ts));
  } else if (ts.tv_nsec > 1e3) {
    print_micros(f, timespec_to_micros(ts));
  } else {
    print_nanos(f, ts.tv_nsec);
  }
}

static void run_tests(test_state *state) {
  test_vec(state);
  test_bitset(state);
  test_strint(state);
  test_utils(state);
  test_scanner(state);
  test_parser(state);
  test_typecheck(state);
  test_ir(state);
  test_llvm(state);
}

int main(int argc, const char **argv) {
  test_config conf = {
    .junit = false,
    .lite = false,
    .filter_str = NULL,
  };

  int times = 1;

  argument args[] = {
    {
      .tag = ARG_FLAG,
      .long_name = "lite",
      .short_name = 'l',
      .flag_data = &conf.lite,
      .description = "Turn off stress tests",
    },
    {.tag = ARG_FLAG,
     .long_name = "junit",
     .short_name = 'j',
     .flag_data = &conf.junit,
     .description = "Create JUnit compatible test-results.xml file"},
    {
      .tag = ARG_INT,
      .long_name = "times",
      .short_name = 't',
      .int_data = &times,
      .description = "Run the test suite more than once (for benchmarking)",
    },
    {
      .tag = ARG_STRING,
      .long_name = "match",
      .short_name = 'm',
      .string_data = &conf.filter_str,
      .description = "filter tests by name. Matches on <group>.<group>.<test>",
    },
  };

  argument_bag root = {
    .amt = STATIC_LEN(args),
    .args = args,
  };

  program_args pa = {
    .root = &root,
    .preamble = "Lang tests v1.0.0\n",
  };

  parse_args(pa, argc, argv);

  test_state state = test_state_new(conf);
  state.print_streaming = false;

  for (int i = 0; i < times; i++) {
    if (i == times - 1) {
      state.print_streaming = true;
    }
    run_tests(&state);
  }

  test_state_finalize(&state);

  print_failures(&state);

  printf("Tests passed: %" PRIu32 "/%" PRIu32 "\n",
         state.tests_passed,
         state.tests_run);

#ifdef TIME_ANY
  puts("\n------------");
  puts("--- Timings:");
  puts("------------\n");
#endif

#ifdef TIME_TOKENIZER
  fputs("Total bytes tokenized: ", stdout);
  print_byte_amount(stdout, state.total_bytes_tokenized);
  puts("");

  fputs("Total tokens produced: ", stdout);
  print_amount(stdout, state.total_tokens);
  puts("");

  fputs("Time spent tokenizing: ", stdout);
  print_timespec(stdout, state.total_tokenization_time);
  puts("");

  {
    fputs("Tokenization time per token produced: ", stdout);
    double micros_per_token = timespec_to_micros(state.total_tokenization_time) / state.total_tokens;
    print_micros(stdout, micros_per_token);
    puts("");
  }
  {
    fputs("Tokenization time per byte: ", stdout);
    double millis_per_byte = timespec_to_micros(state.total_tokenization_time) / state.total_bytes_tokenized;
    print_micros(stdout, millis_per_byte);
    puts("");
  }
#endif

#ifdef TIME_PARSER
  fputs("Time spent parsing: ", stdout);
  print_timespec(stdout, state.total_parser_time);
  puts("");

  fputs("Total tokens parsed: ", stdout);
  print_amount(stdout, state.total_tokens_parsed);
  puts("");

  fputs("Total parse nodes produced: ", stdout);
  print_amount(stdout, state.total_parse_nodes_produced);
  puts("");

  {
    fputs("Parse time per token: ", stdout);
    double micros_per_token = timespec_to_micros(state.total_parser_time) / state.total_tokens_parsed;
    print_micros(stdout, micros_per_token);
    puts("");
  }
  {
    fputs("Parse time per parse node produced: ", stdout);
    double micros_per_token = timespec_to_micros(state.total_parser_time) / state.total_parse_nodes_produced;
    print_micros(stdout, micros_per_token);
    puts("");
  }
#endif

  if (conf.junit) {
    write_test_results(&state);
    VEC_FREE(&state.actions);
    VEC_FREE(&state.strs);
  }

  for (uint32_t i = 0; i < state.failures.len; i++) {
    failure f = VEC_GET(state.failures, i);
    free(f.reason);
    VEC_FREE(&f.path);
  }

  VEC_FREE(&state.failures);
  VEC_FREE(&state.path);
}
