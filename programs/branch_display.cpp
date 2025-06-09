#include <iostream>
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <fstream>

#include "../utils/headers/searching_utils.h"
#include "../utils/headers/reading_utils.h"


namespace fs = std::filesystem;
using namespace std;



int branch_display(){
    fs::path gcn_dir =  find_gnc_dir(fs::current_path());
    unordered_map<string , fs::path> branches =  get_branches(gcn_dir);\
    for(auto& branch : branches){
        auto [branch_name, essa] = branch;
        cout << branch_name << endl;
    }
    return 0;
}

int main(int argc, char **argv){
    if(argc > 1){
        cerr << "Usage: branch <branch_name> optional<hash>";
        return 1;
    }
    try{
      return branch_display(); 
    }catch(runtime_error e){
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }
}