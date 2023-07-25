#ifndef CICFLOW_BASE_ATTRIBUTES_H_
#define CICFLOW_BASE_ATTRIBUTES_H_

// HAVE_ATTRIBUTE
#ifdef __has_attribute
#define CIC_HAVE_ATTRIBUTE(x) __has_attribute(x)
#else
#define CIC_HAVE_ATTRIBUTE(x) 0
#endif

// -----------------------------------------------------------------------------
// Function Attributes
//
// GCC: https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
// Clang: https://clang.llvm.org/docs/AttributeReference.html

// CIC_PRINTF_ATTRIBUTE
// CIC_SCANF_ATTRIBUTE
#if CIC_HAVE_ATTRIBUTE(format) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_PRINTF_ATTRIBUTE(string_index, first_to_check) \
  __attribute__((__format__(__printf__, string_index, first_to_check)))
#define CIC_SCANF_ATTRIBUTE(string_index, first_to_check) \
  __attribute__((__format__(__scanf__, string_index, first_to_check)))
#else
#define CIC_PRINTF_ATTRIBUTE(string_index, first_to_check)
#define CIC_SCANF_ATTRIBUTE(string_index, first_to_check)
#endif

// CIC_ATTRIBUTE_ALWAYS_INLINE
// CIC_ATTRIBUTE_NOINLINE
//
// Forces functions to either inline or not inline. Introduced in gcc 3.1.
#if CIC_HAVE_ATTRIBUTE(always_inline) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#define CIC_HAVE_ATTRIBUTE_ALWAYS_INLINE 1
#else
#define CIC_ATTRIBUTE_ALWAYS_INLINE
#endif

#if CIC_HAVE_ATTRIBUTE(noinline) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_ATTRIBUTE_NOINLINE __attribute__((noinline))
#define CIC_HAVE_ATTRIBUTE_NOINLINE 1
#else
#define CIC_ATTRIBUTE_NOINLINE
#endif

// CIC_ATTRIBUTE_NO_TAIL_CALL
//
// Prevents the compiler from optimizing away stack frames for functions which
// end in a call to another function.
#if CIC_HAVE_ATTRIBUTE(disable_tail_calls)
#define CIC_HAVE_ATTRIBUTE_NO_TAIL_CALL 1
#define CIC_ATTRIBUTE_NO_TAIL_CALL __attribute__((disable_tail_calls))
#elif defined(__GNUC__) && !defined(__clang__) && !defined(__e2k__)
#define CIC_HAVE_ATTRIBUTE_NO_TAIL_CALL 1
#define CIC_ATTRIBUTE_NO_TAIL_CALL __attribute__((optimize("no-optimize-sibling-calls")))
#else
#define CIC_ATTRIBUTE_NO_TAIL_CALL
#define CIC_HAVE_ATTRIBUTE_NO_TAIL_CALL 0
#endif

// CIC_ATTRIBUTE_WEAK
//
// Tags a function as weak for the purposes of compilation and linking.
// Weak attributes did not work properly in LLVM's Windows backend before
// 9.0.0, so disable them there. See
// https://bugs.llvm.org/show_bug.cgi?id=37598 for further information. The
// MinGW compiler doesn't complain about the weak attribute until the link
// step, presumably because Windows doesn't use ELF binaries.
#if (CIC_HAVE_ATTRIBUTE(weak) || (defined(__GNUC__) && !defined(__clang__))) && \
    (!defined(_WIN32) || (defined(__clang__) && __clang_major__ >= 9)) && !defined(__MINGW32__)
#undef CIC_ATTRIBUTE_WEAK
#define CIC_ATTRIBUTE_WEAK __attribute__((weak))
#define CIC_HAVE_ATTRIBUTE_WEAK 1
#else
#define CIC_ATTRIBUTE_WEAK
#define CIC_HAVE_ATTRIBUTE_WEAK 0
#endif

// CIC_ATTRIBUTE_NONNULL
//
// Tells the compiler either (a) that a particular function parameter
// should be a non-null pointer, or (b) that all pointer arguments should
// be non-null.
//
// Note: As the GCC manual states, "[s]ince non-static C++ methods
// have an implicit 'this' argument, the arguments of such methods
// should be counted from two, not one."
//
// Args are indexed starting at 1.
//
// For non-static class member functions, the implicit `this` argument
// is arg 1, and the first explicit argument is arg 2. For static class member
// functions, there is no implicit `this`, and the first explicit argument is
// arg 1.
//
// Example:
//
//   /* arg_a cannot be null, but arg_b can */
//   void Function(void* arg_a, void* arg_b) CIC_ATTRIBUTE_NONNULL(1);
//
//   class C {
//     /* arg_a cannot be null, but arg_b can */
//     void Method(void* arg_a, void* arg_b) CIC_ATTRIBUTE_NONNULL(2);
//
//     /* arg_a cannot be null, but arg_b can */
//     static void StaticMethod(void* arg_a, void* arg_b)
//     CIC_ATTRIBUTE_NONNULL(1);
//   };
//
// If no arguments are provided, then all pointer arguments should be non-null.
//
//  /* No pointer arguments may be null. */
//  void Function(void* arg_a, void* arg_b, int arg_c)
//  CIC_ATTRIBUTE_NONNULL();
//
// NOTE: The GCC nonnull attribute actually accepts a list of arguments, but
// CIC_ATTRIBUTE_NONNULL does not.
#if CIC_HAVE_ATTRIBUTE(nonnull) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_ATTRIBUTE_NONNULL(arg_index) __attribute__((nonnull(arg_index)))
#else
#define CIC_ATTRIBUTE_NONNULL(...)
#endif

// CIC_ATTRIBUTE_NORETURN
//
// Tells the compiler that a given function never returns.
#if CIC_HAVE_ATTRIBUTE(noreturn) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_ATTRIBUTE_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define CIC_ATTRIBUTE_NORETURN __declspec(noreturn)
#else
#define CIC_ATTRIBUTE_NORETURN
#endif

// CIC_ATTRIBUTE_NO_SANITIZE_ADDRESS
//
// Tells the AddressSanitizer (or other memory testing tools) to ignore a given
// function. Useful for cases when a function reads random locations on stack,
// calls _exit from a cloned subprocess, deliberately accesses buffer
// out of bounds or does other scary things with memory.
// NOTE: GCC supports AddressSanitizer(asan) since 4.8.
// https://gcc.gnu.org/gcc-4.8/changes.html
#if defined(CIC_HAVE_ADDRESS_SANITIZER) && CIC_HAVE_ATTRIBUTE(no_sanitize_address)
#define CIC_ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#elif defined(CIC_HAVE_ADDRESS_SANITIZER) && defined(_MSC_VER) && _MSC_VER >= 1928
// https://docs.microsoft.com/en-us/cpp/cpp/no-sanitize-address
#define CIC_ATTRIBUTE_NO_SANITIZE_ADDRESS __declspec(no_sanitize_address)
#elif defined(CIC_HAVE_ADDRESS_SANITIZER) && CIC_HAVE_ATTRIBUTE(no_sanitize)
#define CIC_ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute((no_sanitize("hwaddress")))
#else
#define CIC_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif

// CIC_ATTRIBUTE_NO_SANITIZE_MEMORY
//
// Tells the MemorySanitizer to relax the handling of a given function. All
// "Use of uninitialized value" warnings from such functions will be
// suppressed, and all values loaded from memory will be considered fully
// initialized.  This attribute is similar to the
// CIC_ATTRIBUTE_NO_SANITIZE_ADDRESS attribute above, but deals with
// initialized-ness rather than addressability issues. NOTE:
// MemorySanitizer(msan) is supported by Clang but not GCC.
#if CIC_HAVE_ATTRIBUTE(no_sanitize_memory)
#define CIC_ATTRIBUTE_NO_SANITIZE_MEMORY __attribute__((no_sanitize_memory))
#else
#define CIC_ATTRIBUTE_NO_SANITIZE_MEMORY
#endif

// CIC_ATTRIBUTE_NO_SANITIZE_THREAD
//
// Tells the ThreadSanitizer to not instrument a given function.
// NOTE: GCC supports ThreadSanitizer(tsan) since 4.8.
// https://gcc.gnu.org/gcc-4.8/changes.html
#if CIC_HAVE_ATTRIBUTE(no_sanitize_thread)
#define CIC_ATTRIBUTE_NO_SANITIZE_THREAD __attribute__((no_sanitize_thread))
#else
#define CIC_ATTRIBUTE_NO_SANITIZE_THREAD
#endif

// CIC_ATTRIBUTE_NO_SANITIZE_UNDEFINED
//
// Tells the UndefinedSanitizer to ignore a given function. Useful for cases
// where certain behavior (eg. division by zero) is being used intentionally.
// NOTE: GCC supports UndefinedBehaviorSanitizer(ubsan) since 4.9.
// https://gcc.gnu.org/gcc-4.9/changes.html
#if CIC_HAVE_ATTRIBUTE(no_sanitize_undefined)
#define CIC_ATTRIBUTE_NO_SANITIZE_UNDEFINED __attribute__((no_sanitize_undefined))
#elif CIC_HAVE_ATTRIBUTE(no_sanitize)
#define CIC_ATTRIBUTE_NO_SANITIZE_UNDEFINED __attribute__((no_sanitize("undefined")))
#else
#define CIC_ATTRIBUTE_NO_SANITIZE_UNDEFINED
#endif

// CIC_ATTRIBUTE_NO_SANITIZE_CFI
//
// Tells the ControlFlowIntegrity sanitizer to not instrument a given function.
// See https://clang.llvm.org/docs/ControlFlowIntegrity.html for details.
#if CIC_HAVE_ATTRIBUTE(no_sanitize)
#define CIC_ATTRIBUTE_NO_SANITIZE_CFI __attribute__((no_sanitize("cfi")))
#else
#define CIC_ATTRIBUTE_NO_SANITIZE_CFI
#endif

// CIC_ATTRIBUTE_NO_SANITIZE_SAFESTACK
//
// Tells the SafeStack to not instrument a given function.
// See https://clang.llvm.org/docs/SafeStack.html for details.
#if CIC_HAVE_ATTRIBUTE(no_sanitize)
#define CIC_ATTRIBUTE_NO_SANITIZE_SAFESTACK __attribute__((no_sanitize("safe-stack")))
#else
#define CIC_ATTRIBUTE_NO_SANITIZE_SAFESTACK
#endif

// CIC_ATTRIBUTE_RETURNS_NONNULL
//
// Tells the compiler that a particular function never returns a null pointer.
#if CIC_HAVE_ATTRIBUTE(returns_nonnull)
#define CIC_ATTRIBUTE_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
#define CIC_ATTRIBUTE_RETURNS_NONNULL
#endif

// CIC_HAVE_ATTRIBUTE_SECTION
//
// Indicates whether labeled sections are supported. Weak symbol support is
// a prerequisite. Labeled sections are not supported on Darwin/iOS.
#ifdef CIC_HAVE_ATTRIBUTE_SECTION
#error CIC_HAVE_ATTRIBUTE_SECTION cannot be directly set
#elif (CIC_HAVE_ATTRIBUTE(section) || (defined(__GNUC__) && !defined(__clang__))) && \
    !defined(__APPLE__) && CIC_HAVE_ATTRIBUTE_WEAK
#define CIC_HAVE_ATTRIBUTE_SECTION 1

// CIC_ATTRIBUTE_SECTION
//
// Tells the compiler/linker to put a given function into a section and define
// `__start_ ## name` and `__stop_ ## name` symbols to bracket the section.
// This functionality is supported by GNU linker.  Any function annotated with
// `CIC_ATTRIBUTE_SECTION` must not be inlined, or it will be placed into
// whatever section its caller is placed into.
//
#ifndef CIC_ATTRIBUTE_SECTION
#define CIC_ATTRIBUTE_SECTION(name) __attribute__((section(#name))) __attribute__((noinline))
#endif

// CIC_ATTRIBUTE_SECTION_VARIABLE
//
// Tells the compiler/linker to put a given variable into a section and define
// `__start_ ## name` and `__stop_ ## name` symbols to bracket the section.
// This functionality is supported by GNU linker.
#ifndef CIC_ATTRIBUTE_SECTION_VARIABLE
#ifdef _AIX
// __attribute__((section(#name))) on AIX is achived by using the `.csect`
// psudo op which includes an additional integer as part of its syntax
// indcating alignment. If data fall under different alignments then you might
// get a compilation error indicating a `Section type conflict`.
#define CIC_ATTRIBUTE_SECTION_VARIABLE(name)
#else
#define CIC_ATTRIBUTE_SECTION_VARIABLE(name) __attribute__((section(#name)))
#endif
#endif

// CIC_DECLARE_ATTRIBUTE_SECTION_VARS
//
// A weak section declaration to be used as a global declaration
// for CIC_ATTRIBUTE_SECTION_START|STOP(name) to compile and link
// even without functions with CIC_ATTRIBUTE_SECTION(name).
// CIC_DEFINE_ATTRIBUTE_SECTION should be in the exactly one file; it's
// a no-op on ELF but not on Mach-O.
//
#ifndef CIC_DECLARE_ATTRIBUTE_SECTION_VARS
#define CIC_DECLARE_ATTRIBUTE_SECTION_VARS(name)   \
  extern char __start_##name[] CIC_ATTRIBUTE_WEAK; \
  extern char __stop_##name[] CIC_ATTRIBUTE_WEAK
#endif
#ifndef CIC_DEFINE_ATTRIBUTE_SECTION_VARS
#define CIC_INIT_ATTRIBUTE_SECTION_VARS(name)
#define CIC_DEFINE_ATTRIBUTE_SECTION_VARS(name)
#endif

#define CIC_ATTRIBUTE_SECTION_START(name) ((void *)(__start_##name))
#define CIC_ATTRIBUTE_SECTION_STOP(name) ((void *)(__stop_##name))

#else  // !CIC_HAVE_ATTRIBUTE_SECTION
#define CIC_HAVE_ATTRIBUTE_SECTION 0

// provide dummy definitions
#define CIC_ATTRIBUTE_SECTION(name)
#define CIC_ATTRIBUTE_SECTION_VARIABLE(name)
#define CIC_INIT_ATTRIBUTE_SECTION_VARS(name)
#define CIC_DEFINE_ATTRIBUTE_SECTION_VARS(name)
#define CIC_DECLARE_ATTRIBUTE_SECTION_VARS(name)
#define CIC_ATTRIBUTE_SECTION_START(name) ((void *)(0))
#define CIC_ATTRIBUTE_SECTION_STOP(name) ((void *)(0))

#endif  // CIC_ATTRIBUTE_SECTION

// CIC_ATTRIBUTE_STACK_ALIGN_FOR_OLD_LIBC
//
// Support for aligning the stack on 32-bit x86.
#if CIC_HAVE_ATTRIBUTE(force_align_arg_pointer) || (defined(__GNUC__) && !defined(__clang__))
#if defined(__i386__)
#define CIC_ATTRIBUTE_STACK_ALIGN_FOR_OLD_LIBC __attribute__((force_align_arg_pointer))
#define CIC_REQUIRE_STACK_ALIGN_TRAMPOLINE (0)
#elif defined(__x86_64__)
#define CIC_REQUIRE_STACK_ALIGN_TRAMPOLINE (1)
#define CIC_ATTRIBUTE_STACK_ALIGN_FOR_OLD_LIBC
#else  // !__i386__ && !__x86_64
#define CIC_REQUIRE_STACK_ALIGN_TRAMPOLINE (0)
#define CIC_ATTRIBUTE_STACK_ALIGN_FOR_OLD_LIBC
#endif  // __i386__
#else
#define CIC_ATTRIBUTE_STACK_ALIGN_FOR_OLD_LIBC
#define CIC_REQUIRE_STACK_ALIGN_TRAMPOLINE (0)
#endif

//
#if defined(__clang__) && CIC_HAVE_ATTRIBUTE(warn_unused_result)
#define CIC_MUST_USE_RESULT __attribute__((warn_unused_result))
#else
#define CIC_MUST_USE_RESULT
#endif

//
#if CIC_HAVE_ATTRIBUTE(hot) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_ATTRIBUTE_HOT __attribute__((hot))
#else
#define CIC_ATTRIBUTE_HOT
#endif

#if CIC_HAVE_ATTRIBUTE(cold) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_ATTRIBUTE_COLD __attribute__((cold))
#else
#define CIC_ATTRIBUTE_COLD
#endif

// -----------------------------------------------------------------------------
// Variable Attributes
//

//
#if CIC_HAVE_ATTRIBUTE(unused) || (defined(__GNUC__) && !defined(__clang__))
#undef CIC_ATTRIBUTE_UNUSED
#define CIC_ATTRIBUTE_UNUSED __attribute__((__unused__))
#else
#define CIC_ATTRIBUTE_UNUSED
#endif

//
#if CIC_HAVE_ATTRIBUTE(tls_model) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_ATTRIBUTE_INITIAL_EXEC __attribute__((tls_model("initial-exec")))
#else
#define CIC_ATTRIBUTE_INITIAL_EXEC
#endif

//
#if CIC_HAVE_ATTRIBUTE(packed) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_ATTRIBUTE_PACKED __attribute__((__packed__))
#else
#define CIC_ATTRIBUTE_PACKED
#endif

//
#if CIC_HAVE_ATTRIBUTE(aligned) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_ATTRIBUTE_FUNC_ALIGN(bytes) __attribute__((aligned(bytes)))
#else
#define CIC_ATTRIBUTE_FUNC_ALIGN(bytes)
#endif

// TODO:
// https://stackoverflow.com/questions/44511436/how-to-do-an-explicit-fall-through-in-c
#ifdef CIC_FALLTHROUGH_INTENDED
#error "CIC_FALLTHROUGH_INTENDED should not be defined."
#else
#define CIC_FALLTHROUGH_INTENDED \
  do {                           \
  } while (0)
#endif

//
#if CIC_HAVE_ATTRIBUTE(deprecated)
#define CIC_DEPRECATED(message) __attribute__((deprecated(message)))
#else
#define CIC_DEPRECATED(message)
#endif

#if defined(__GNUC__) || defined(__clang__)
// Clang also supports these GCC pragmas.
#define CIC_INTERNAL_DISABLE_DEPRECATED_DECLARATION_WARNING \
  _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define CIC_INTERNAL_RESTORE_DEPRECATED_DECLARATION_WARNING _Pragma("GCC diagnostic pop")
#else
#define CIC_INTERNAL_DISABLE_DEPRECATED_DECLARATION_WARNING
#define CIC_INTERNAL_RESTORE_DEPRECATED_DECLARATION_WARNING
#endif  // defined(__GNUC__) || defined(__clang__)

#define CIC_CONST_INIT

#if CIC_HAVE_ATTRIBUTE(lifetimebound)
#define CIC_ATTRIBUTE_LIFETIME_BOUND __attribute__((lifetimebound))
#else
#define CIC_ATTRIBUTE_LIFETIME_BOUND
#endif

#if CIC_HAVE_ATTRIBUTE(trivial_abi)
#define CIC_ATTRIBUTE_TRIVIAL_ABI __attribute__((trivial_abi))
#define CIC_HAVE_ATTRIBUTE_TRIVIAL_ABI 1
#else
#define CIC_ATTRIBUTE_TRIVIAL_ABI
#endif

#define ABSL_ATTRIBUTE_NO_UNIQUE_ADDRESS

#endif  // CICFLOW_BASE_ATTRIBUTES_H_
