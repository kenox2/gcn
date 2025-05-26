#include <iostream>
#include <cstdint>
#include <filesystem>
#include "../utils/headers/searching_utils.h"
#include <fstream>

namespace fs = std::filesystem;
using namespace std;


int branch_rm(const string& branch_name){
    fs::path branch_path = find_file(fs::current_path(), "refs/heads", true);
    if(!fs::exists(branch_path / branch_name)){
        cerr << "Branch of this name doesn't exist";
        return 1;
    }
    fs::remove(branch_path / branch_name);
    return 0;
}

int main(int argc, char **argv){
    if(argc != 2){
        cerr << "Usage: branch -rm <branch_name>";
        return 1;
    }
    try{
        return branch_rm(argv[1]);    
    }catch(runtime_error e){
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }
}