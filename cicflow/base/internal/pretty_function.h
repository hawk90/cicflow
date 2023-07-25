#ifndef CIC_BASE_INTERNAL_PRETTY_FUNCTION_H_
#define CIC_BASE_INTERNAL_PRETTY_FUNCTION_H_

#if defined(_MSC_VER)
#define CIC_PRETTY_FUNCTION __FUNCSIG__
#elif defined(__GNUC__)
#define CIC_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else 
#error Unsupported comiler
#endif


#endif
