DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
MAKE_TARGETS+=" OpenMeshCore OpenMeshTools"
#MAKE_FLAGS="VERBOSE=1"
CMAKE_FLAGS+=" -DBUILD_APPS=OFF"

if [ "$1" = "clean" ]; then
  if [ -d "$DIR/build" ]; then
    (cd $DIR/build && make clean && cd $DIR && rm -r build)
  fi
  exit 0
fi

if [ "$1" = release ]; then 
  CMAKE_FLAGS+=" -DCMAKE_BUILD_TYPE=Release"
elif [ "$1" = debug ]; then
  CMAKE_FLAGS+=" -DCMAKE_BUILD_TYPE=Debug"
else
  echo "Missing or unrecognized metabuild argument \"$1\""
  exit 1
fi

if [ -z "$2" ]; then
  ARCH=x86-64
else
  ARCH=$2
fi
echo Architecture is set to $ARCH

SYSTEM_NAME=`uname -s`
if [ "$SYSTEM_NAME" = "Darwin" -o "$SYSTEM_NAME" = "Linux" ]; then
echo "Building for $SYSTEM_NAME"
(cd $DIR && \
    mkdir -p build && \
    cd build && \
    echo "Running cmake in `pwd`" && \
    `which cmake` $CMAKE_FLAGS -DARCH:STRING=$ARCH .. && \
    echo "Running make in `pwd`" && \
    make $MAKE_FLAGS $MAKE_TARGETS && \
    echo "Success!")
elif echo "$SYSTEM_NAME" | grep -q "MINGW64_NT"; then
echo "Building for Windows"
(cd $DIR && \
    mkdir -p build && \
    cd build && \
    echo "Running cmake in `pwd`" && \
    cmake $CMAKE_FLAGS -DWIN32=1 -G "MSYS Makefiles" .. && \
    echo "Running make in `pwd`" && \
    make $MAKE_FLAGS $MAKE_TARGETS && \
    echo "Performing manual copy step hack" && \
    cp src/OpenMesh/Core/libOpenMeshCore.a Build/lib/ && \
    cp src/OpenMesh/Tools/libOpenMeshTools.a Build/lib/ && \
    echo "Success!")
elif echo "$SYSTEM_NAME" | grep -q "MSYS"; then
echo "ERROR: the MSYS shell is not supported. Please use the MinGW-w64 Win64 Shell instead."
exit 1
else
echo "ERROR: Unrecognized or unsupported platform: $SYSTEM_NAME!"
exit 1
fi
