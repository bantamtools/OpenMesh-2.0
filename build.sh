mkdir -p build
cd build
CXX='clang++ -std=c++11 -stdlib=libc++' `which cmake` ..
make "$@"
