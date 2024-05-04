find include tests src -name '*.cpp' -o -name '*.h' \
    | grep 'doctest.h' -v \
    | xargs clang-format -i -style=file
