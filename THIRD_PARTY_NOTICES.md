# Third-party notices

## Decremental hull chain maintenance

`src/decremental_hull.cpp` adapts the forward/backward bridge repair and
split-chain representation from:

- Project: Luzhiled's Library, by GitHub user ei1333
- File: `structure/others/decremental-upper-hull.hpp`
- Source: <https://github.com/ei1333/library/blob/master/structure/others/decremental-upper-hull.hpp>
- Documentation: <https://ei1333.github.io/library/structure/others/decremental-upper-hull.hpp>
- Documentation reports last file update 2025-08-16; consulted 2026-09-23.
- License: <https://github.com/ei1333/library/blob/master/LICENSE> (Unlicense).

Changes include index-based storage, explicit subtree occupancy, generic filtered
exact predicates, retained bridge endpoint pairs, logarithmic selection queries,
and integration with duplicate groups. No upstream dependency is downloaded at
build time. No code from the unlicensed dacin21 codebook is vendored.

The algorithm is attributed to J. Hershberger and S. Suri,
*Applications of a semi-dynamic convex hull algorithm*, BIT (1992).

### Upstream license text

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>

## Boost.Multiprecision

Boost is a system/build dependency, not vendored. It is distributed under the
Boost Software License 1.0: <https://www.boost.org/LICENSE_1_0.txt>.
Preserve applicable Boost notices when redistributing Boost sources or binaries.

## pybind11

The Python extension is built with pybind11, a build dependency. Its BSD-3-Clause
notice is reproduced here for binary distributions:

Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>, All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Upstream: <https://github.com/pybind/pybind11>.
