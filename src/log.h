#pragma once

#include <hedley.h>
#include <stdarg.h>
#include <stdio.h>

#include "attrs.h"
#include "global_settings.h"
#include "util.h"

/**
 * Log something to stdout when verbosity is >= VERBOSE_SOME
 */
NON_NULL_PARAMS
HEDLEY_PRINTF_FORMAT(1, 2)
void log_warning(const char *restrict fmt, ...);

/**
 * Log something to stdout when verbosity is >= VERBOSE_VERY
 */
NON_NULL_PARAMS
HEDLEY_PRINTF_FORMAT(1, 2)
void log_debug(const char *restrict fmt, ...);
