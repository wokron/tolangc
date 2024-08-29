# This script is used to build tolangc compiler with options.

# Usage: ./build.sh [llvm|pcode]

# Check if the number of arguments is correct
if [ "$#" -ne 1 ]; then
    echo "Usage: ./build.sh [llvm|pcode]"
    exit 1
fi

# Check if the argument is valid
if [ "$1" != "llvm" ] && [ "$1" != "pcode" ]; then
    echo "Invalid argument! Use [llvm|pcode]."
    exit 1
fi

backend=$1

# Check required tools (cmake, make, clang)
if ! command -v cmake &>/dev/null; then
    echo "Command cmake not found! Please install CMake."
    exit 1
fi
if ! command -v make &>/dev/null; then
    echo "Command make not found! Please install Make."
    exit 1
fi
if ! command -v clang &>/dev/null; then
    echo "Command clang not found! Please install Clang."
    exit 1
fi

# Check if BUILD_DIR env variable is set
if [ -z "$BUILD_DIR" ]; then
    BUILD_DIR="build"
fi

# Create build directory if it doesn't exist
if [ ! -d $BUILD_DIR ]; then
    mkdir -p $BUILD_DIR
fi

# Build the compiler
opt=""
if [ "$backend" = "pcode" ]; then
    opt="$opt -DPCODE_BACKEND=ON"
fi

# If BUILD_TYPE is set, use it
if [ ! -z "$BUILD_TYPE" ]; then
    opt="$opt -DCMAKE_BUILD_TYPE=$BUILD_TYPE"
fi

cmake $opt -B$BUILD_DIR .

make -C $BUILD_DIR
