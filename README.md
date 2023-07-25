# Cicflow - C Common Libraries

The repository copies the Abseil C++ library code. Abseil is an open-source
collection of C++ code (compliant to C++14) designed to augment the C++
standard library. 

## Table of Contents

- [About Cicflow](#about)
- [Building Cicflow](#build)
- [Codemap](#codemap)
- [License](#license)
- [Project convention](#convention)

<a name="about"></a>
## About Cicflow

Cicflow is an open-source collection of C library code designed to augment
the C standard library. The Cicflow library code is copied Google's Abseil C++ Common Libraries

<a name="build"></a>
## Building Cicflow

```bash
mkdir build
cd build
cmake ../cicflow  # configure the project
cmake --build . --target cicflow  # build the executable
```

<a name="codemap"></a>
## Codemap

Cicflow contains the following C library components:

* [x] [`base`](cicflow/base/)
  <br /> The `base` library contains initialization code and other code which
  all other Cicflow code depends on. Code within `base` may not depend on any
  other code (other than the C standard library).
* [ ] [`debugging`](cicflow/debugging/)
  <br /> The `debugging` library contains code useful for enabling leak
  checks, and stacktrace and symbolization utilities.
* [ ] [`flags`](cicflow/flags/)
  <br /> The `flags` library contains code for handling command line flags for
  libraries and binaries built with Cicflow.
* [ ] [`hash`](cicflow/hash/)
  <br /> The `hash` library contains the hashing framework and default hash
  functor implementations for hashable types in Cicflow.
* [ ] [`log`](cicflow/log/)
  <br /> The `log` library contains `LOG` and `CHECK` macros and facilities
  for writing logged messages out to disk, `stderr`, or user-extensible
  destinations.
* [ ] [`profiling`](cicflow/profiling/)
  <br /> The `profiling` library contains utility code for profiling C
  entities.  It is currently a private dependency of other Cicflow libraries.
* [ ] [`random`](cicflow/random/)
  <br /> The `random` library contains functions for generating psuedorandom
  values.
* [ ] [`status`](cicflow/status/)
  <br /> The `status` library contains abstractions for error handling.
* [ ] [`synchronization`](cicflow/synchronization/)
  <br /> The `synchronization` library contains concurrency primitives and a variety of
  synchronization abstractions.
* [ ] [`time`](cicflow/time/)
  <br /> The `time` library contains abstractions for computing with absolute
  points in time, durations of time, and formatting and parsing time within
  time zones.
* [ ] [`utility`](cicflow/utility/)
  <br /> The `utility` library contains utility and helper code.

<a name="license"></a>
## License

The Cicflo C library is licensed under the terms of the Apache
license. See [LICENSE](LICENSE) for more information.

<a name="convention"></a>
## Project Convention
> GNU or Linux Kernel  

