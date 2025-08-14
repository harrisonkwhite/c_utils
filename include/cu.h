#ifndef CU_H
#define CU_H

#include "cu_mem.h"
#include "cu_math.h"
#include "cu_io.h"

#if defined(__GNUC__) || defined(__clang__)
#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
#include <sal.h>
#define WARN_UNUSED_RESULT _Check_return_
#else
#define WARN_UNUSED_RESULT
#endif

#endif
