
#!/bin/bash

set -e  # Stop on error

compile_add() {
    echo "Compiling add..."
    g++ ./commands/add.cpp ./utils/hashing_utils.cpp ./utils/creating_utils.cpp ./utils/searching_utils.cpp ./libs/xxhash/xxhash.c -lz -g -o add
}

compile_init() {
    echo "Compiling init..."
    g++ ./commands/init.cpp ./utils/searching_utils.cpp -lz -o init
}

compile_commit() {
    echo "Compiling commit..."
    g++ ./commands/commit.cpp ./utils/reading_utils.cpp ./utils/searching_utils.cpp ./libs/xxhash/xxhash.c ./utils/hashing_utils.cpp ./utils/creating_utils.cpp -lz -g -o commit
}

# If no arguments, compile all
if [ $# -eq 0 ]; then
    compile_add
    compile_init
    compile_commit
else
    for arg in "$@"; do
        case $arg in
            add)
                compile_add
                ;;
            init)
                compile_init
                ;;
            commit)
                compile_commit
                ;;
            *)
                echo "Unknown target: $arg"
                ;;
        esac
    done
fi
