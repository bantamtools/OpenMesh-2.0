DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
MAKE_TARGETS+=" OpenMeshCore OpenMeshTools"
#MAKE_FLAGS="VERBOSE=1"
CMAKE_FLAGS+=" -DBUILD_APPS=OFF"

if [ -z "$3" ]; then
  ARCH=x86-64
else
  ARCH=$3
fi
echo Architecture is set to $ARCH

if [ "$1" = "build" ]; then
  SYSTEM_NAME=`uname -s` 
  if [ "$2" = release ]; then 
    CMAKE_FLAGS+=" -DCMAKE_BUILD_TYPE=Release"
  elif [ "$2" = debug ]; then
    CMAKE_FLAGS+=" -DCMAKE_BUILD_TYPE=Debug"
  fi 

  if [ "$SYSTEM_NAME" = "Darwin" ]; then
    echo "Building for OSX"
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
elif [ "$1" = "clean" ]; then
  if [ -d "$DIR/build" ]; then
    (cd $DIR/build && make clean && cd $DIR && rm -r build)
  fi
else
  echo "Missing or unrecognized metabuild argument \"$1\""
  exit 1
fi



