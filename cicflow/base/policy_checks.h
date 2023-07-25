#ifndef CICFLOW_BASE_POLICY_CHECKS_H_
#define CICFLOW_BASE_POLICY_CHECKS_H_

#include <limits.h>
#include <stddef.h>

// -----------------------------------------------------------------------------
// Operating System
//

#if defined(__CYGWIN__)
#error "Cygwin is not supported."
#endif

// -----------------------------------------------------------------------------
// Toolchain
//

#if defined(_MSC_VER) && _MSC_VER < 1910 && !defined(__clang__)
#error "This package requires Visual Studio 2017 (MSVC++ 15.0) or higher."
#endif

#if defined(__GNUC__) && !defined(__clang__)
#if __GNUC__ < 7
#error "This package requires GCC 7 or higher."
#endif
#endif

#if defined(__apple_build_version__) && __apple_build_version__ < 4211165
#error "This package requires __apple_build_version__ of 4211165 or higher."
#endif

// -----------------------------------------------------------------------------
// Standard Library
//

#if defined(_STLPORT_VERSION)
#error "STLPort is not supported."
#endif

// -----------------------------------------------------------------------------
// 'char' Size
//

#if CHAR_BIT != 8
#error "cicflow assumes CHAR_BIT == 8."
#endif

// -----------------------------------------------------------------------------
// 'int' Size
//

#if INT_MAX < 2147483647
#error "cicflow assumes that int is at least 4 bytes. "
#endif

#endif  // CICFLOW_BASE_POLICY_CHECKS_H_
