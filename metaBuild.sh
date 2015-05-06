DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
if [ "$1" = "build" ]; then
  (cd $DIR && \
    mkdir -p build && \
    cd build && \
    echo "Running cmake in `pwd`" && \
    cmake -DWIN32=1 -G "MSYS Makefiles" .. && \
    echo "Running make in `pwd`" && \
    make && \
    echo "Performing manual copy step hack" && \
    cp src/OpenMesh/Core/libOpenMeshCore.a Build/lib/ && \
    cp src/OpenMesh/Tools/libOpenMeshTools.a Build/lib/ && \
    echo "Success!")
elif [ "$1" = "clean" ]; then
  if [ -d "$DIR/build" ]; then
    (cd $DIR/build && make clean && cd $DIR && rm -r build)
  fi
else
  echo "Missing or unrecognized metabuild argument \"$1\""
  exit 1
fi



