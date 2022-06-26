#!/bin/zsh
mkdir -p bin

EXECUTABLE_EXTENSION=""

FLAGS=()
if [[ "$OSTYPE" == "msys" ]]; then
    FLAGS+="-lkernel32.lib"
    FLAGS+=" -D_CRT_SECURE_NO_WARNINGS"

    EXECUTABLE_EXTENSION=".exe"
fi

# sh ./build.sh && ./bin/wlang test/foreign.w -o bin/foreign.w.o && otool -xvjr ./bin/foreign.w.o
clang++ -g -std=c++20 -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-missing-designated-field-initializers -nostdlib++ -nostdinc++ -fno-exceptions -fno-rtti -fsanitize=address ${FLAGS[@]} tools/coffdump.cpp -o "bin/coffdump$EXECUTABLE_EXTENSION" &&
clang++ -g -std=c++20 -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-missing-designated-field-initializers -nostdlib++ -nostdinc++ -fno-exceptions -fno-rtti -fsanitize=address ${FLAGS[@]} tools/machodump.cpp -o "bin/machodump$EXECUTABLE_EXTENSION" &&
clang++ -g -std=c++20 -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-missing-designated-field-initializers -nostdlib++ -nostdinc++ -fno-exceptions -fno-rtti -fsanitize=address ${FLAGS[@]} src/wlang.cpp -o "bin/wlang$EXECUTABLE_EXTENSION"
