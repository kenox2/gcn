
#!/bin/bash

set -e  # Stop on error

compile_add() {
    echo "Compiling add..."
    g++ ./commands/add.cpp ./utils/source_files/hashing_utils.cpp ./utils/source_files/creating_utils.cpp ./utils//source_files/searching_utils.cpp ./libs/xxhash/xxhash.c -lz -g -o add
}

compile_init() {
    echo "Compiling init..."
    g++ ./commands/init.cpp ./utils/source_files/searching_utils.cpp ./utils/source_files/hashing_utils.cpp ./libs/xxhash/xxhash.c -lz -o init
}

compile_commit() {
    echo "Compiling commit..."
    g++ ./commands/commit.cpp ./utils/source_files/reading_utils.cpp ./utils/source_files/searching_utils.cpp ./libs/xxhash/xxhash.c ./utils/source_files/hashing_utils.cpp ./utils/source_files/creating_utils.cpp -lz -g -o commit
}

compile_switch(){
    echo "Compiling switch..."
    g++ commands/switch.cpp utils/source_files/reading_utils.cpp utils/source_files/searching_utils.cpp utils/source_files/recreate_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c -g -lz -o switch
}

compile_debug(){
    echo "Compiling debug..."
    g++ debugging_commands/debug.cpp utils/source_files/searching_utils.cpp libs/xxhash/xxhash.c utils/source_files/hashing_utils.cpp -g -lz -o debug;
}


compile_branch(){
    echo "Compiling branch..."
    g++ commands/branch.cpp utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c -g -lz -o branch
}


compile_branch_rm(){
    echo "Compiling branch_rm..."
    g++ commands/branch_rm.cpp utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c -g -lz -o branch_rm
}

compile_print_head(){
    echo "Compiling print_head..."
    g++ commands/print_head.cpp utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c utils/source_files/reading_utils.cpp -g -lz -o print_head
}

# If no arguments, compile all
if [ $# -eq 0 ]; then
    compile_add
    compile_init
    compile_commit
    compile_switch
    compile_debug
    compile_branch
    compile_branch_rm
    compile_print_head
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
            switch)
                compile_switch
                ;;
            debug)
                compile_debug
                ;;
            branch)
                compile_branch
                ;;
            branch_rm)
                compile_branch_rm
                ;;
            print_head)
                compile_print_head
                ;;
            *)
                echo "Unknown target: $arg"
                ;;
        esac
    done
fi
