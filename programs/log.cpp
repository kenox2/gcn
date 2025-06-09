#include <iostream>
#include "../utils/headers/traversing_utils.h"
#include <cstdint>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;
using namespace std;

int log_all(){
    vector<CommitEntry> log =  traverse_DAG();
    reverse(log.begin(), log.end());
    for (const auto& commit : log) {
        std::cout << "\033[33mcommit " << commit.hash << "\033[0m\n"; // Yellow
        std::cout << "Author: " << commit.author << "\n";
        std::cout << commit.message << "\n\n";
    }

    return 0;
}

int log_branch(string branch_name){
    vector<CommitEntry> log =  traverse_DAG(branch_name);
    reverse(log.begin(), log.end());
    for (const auto& commit : log) {
        std::cout << "\033[33mcommit " << commit.hash << "\033[0m\n"; // Yellow
        std::cout << "Author: " << commit.author << "\n";
        std::cout << commit.message << "\n\n";
    }

    return 0;
}


int main(int argc, char** argv){
    if(argc != 2){
        cerr << "Usage: log <branch> or <all>\n";
        exit(1);
    }
    string branch = argv[1];
    if(branch == "all"){
        log_all();
    }else{
        log_branch(branch);
    }
    return 0;
}