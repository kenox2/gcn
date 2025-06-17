
#!/bin/bash

set -e  # Stop on error

compile_add() {
    echo "Compiling add..."
    g++ ./programs/add.cpp ./utils/source_files/hashing_utils.cpp ./utils/source_files/creating_utils.cpp ./utils/source_files/searching_utils.cpp ./libs/xxhash/xxhash.c -lz -g -o add
}

compile_init() {
    echo "Compiling init..."
    g++ ./programs/init.cpp ./utils/source_files/searching_utils.cpp ./utils/source_files/hashing_utils.cpp ./libs/xxhash/xxhash.c -lz -o init
}

compile_commit() {
    echo "Compiling commit..."
    g++ ./programs/commit.cpp ./utils/source_files/reading_utils.cpp ./utils/source_files/searching_utils.cpp ./libs/xxhash/xxhash.c ./utils/source_files/hashing_utils.cpp ./utils/source_files/creating_utils.cpp -lz -g -o commit
}

compile_switch(){
    echo "Compiling switch..."
    g++ programs/switch.cpp utils/source_files/reading_utils.cpp utils/source_files/searching_utils.cpp utils/source_files/recreate_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c -g -lz -o switch
}

compile_debug(){
    echo "Compiling debug..."
    g++ debugging_programs/debug.cpp utils/source_files/searching_utils.cpp libs/xxhash/xxhash.c utils/source_files/hashing_utils.cpp -g -lz -o debug;
}


compile_branch(){
    echo "Compiling branch..."
    g++ programs/branch.cpp utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c -g -lz -o branch
}


compile_branch_rm(){
    echo "Compiling branch_rm..."
    g++ programs/branch_rm.cpp utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c -g -lz -o branch_rm
}

compile_print_head(){
    echo "Compiling print_head..."
    g++ programs/print_head.cpp utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c utils/source_files/reading_utils.cpp -g -lz -o print_head
}

compile_log(){
    echo "Compiling log..."
    g++ programs/log.cpp utils/source_files/traversing_utils.cpp utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c utils/source_files/reading_utils.cpp -g -lz -o log
}

compile_merge(){
    echo "Compiling merge..."
    g++ programs/merge.cpp utils/source_files/traversing_utils.cpp utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c utils/source_files/reading_utils.cpp -g -lz -o merge
}

compile_branch_display(){
    echo "Compiling branch_display..."
    g++ programs/branch_display.cpp  utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c utils/source_files/reading_utils.cpp -g -lz -o display    
}


compile_get_head(){
    echo "Compiling get_head..."
    g++ programs/get_head.cpp  utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c utils/source_files/reading_utils.cpp -g -lz -o get_head  
}



compile_get_head_hash(){
    echo "Compiling get_head_hash..."
    g++ programs/get_head_hash.cpp  utils/source_files/searching_utils.cpp utils/source_files/hashing_utils.cpp libs/xxhash/xxhash.c utils/source_files/reading_utils.cpp -g -lz -o get_head_hash
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
    compile_branch_display
    compile_print_head
    compile_log
    compile_merge
    compile_get_head
    compile_get_head_hash
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
            log)
                compile_log
                ;;
            merge)
                compile_merge
                ;;
            branch_display)
                compile_branch_display
                ;;
            get_head)
                compile_get_head
                ;;
            get_head_hash)
                compile_get_head_hash
                ;;
            *)
                echo "Unknown target: $arg"
                ;;
        esac
    done
fi
