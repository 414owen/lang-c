CFLAGS ?= -O1 -Wall -Wno-unused-result

SRCS := $(wildcard *.c)
DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

tokenizer.o : tokenizer.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

parser.o : parser.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

parser.h: parser.c

TEST_OBJS := test.o test_scanner.o test_parser.o parse_tree.o util.o tokenizer.o parser.o

test: run_tests.c $(TEST_OBJS)
	$(CC) $(CFLAGS) -o test run_tests.c $(TEST_OBJS)

clean:
	rm -f *.so *.o test parser.c tokenizer.c

parser.c: parser.y
	lemon parser.y
	sed -i 's/^static \(const char \*.*yyTokenName\[\].*\)$$/\1/g' parser.c

tokenizer.c: tokenizer.re
	re2c -o tokenizer.c tokenizer.re

include $(wildcard $(DEPFILES))
