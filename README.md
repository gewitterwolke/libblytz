libblytz
========

C++ library for simple BLYTZ access. Still buggy as hell and not fully
functional. 

Needed libraries for compiling are the dev versions of `libqrencode`, `libcurl` (e.g. `libcurl4-gnutls-dev`), `libcrypto` and `libpng`.

Compile via
`cmake .`
`make`

and test using
`./blytz-test`

For building under GNU/Linux you need a copy of the Google Test sources locally in a subdirectory (currently hardcoded to `gtest-1.7.0`). You may 
obtain the sources [here](https://code.google.com/p/googletest/downloads/list).

Under FreeBSD the precompiled gtest library is used (although this is not recommended by the developers of Google Test). Install it e.g. via `pkg install googletest`.
