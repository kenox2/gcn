#include <iostream>
#include <cstdint>
#include <filesystem>
#include "../utils/headers/searching_utils.h"
#include <fstream>

namespace fs = std::filesystem;
using namespace std;


bool is_commit(const fs::path& commit_path){
    ifstream commit(commit_path, ios::binary );
    if(!commit){
        throw runtime_error("Failed to open commit file \n");
    }
    uint32_t mode;
    uint64_t hash;
    commit.read(reinterpret_cast<char *>(&mode), sizeof(mode));
    commit.read(reinterpret_cast<char *>(&hash), sizeof(hash));
    string identifier("\0", 6);
    commit.read(identifier.data(), 6);
    if(identifier == "author" || identifier == "parent") return true;
    return false;
}




int branch(const string& branch_name,const string& hash){
    fs::path heads_dir =  find_file(fs::current_path(), "refs/heads",true);
    fs::path object_dir = find_file(fs::current_path(), "objects", true);
    fs::path branch_path(heads_dir / branch_name);
    if(fs::exists(branch_path)){
        cout << "Branch of given name already exist" << endl;
        return 0;
    }
    ofstream branch(branch_path, ios::binary);
    if(hash != ""){
        fs::path commit_file = find_file_by_hash(object_dir , hash);
        // get hash from file name
        string hash_str = commit_file.parent_path().filename().string() + commit_file.filename().string();
        uint64_t hash = stoull(hash_str);
        // check if its commit
        if(is_commit(commit_file)){
            branch.write(reinterpret_cast<char *>(&hash), sizeof(hash));
        }else{
            throw runtime_error("This is not commit hash");
        }
    }
    return 0;
}

int main(int argc, char **argv){
    if(argc > 3 || argc == 1){
        cerr << "Usage: branch <branch_name> optional<hash>";
        return 1;
    }
    try{
        if(argc == 2){
            return branch(argv[1], "");
        }
        return branch(argv[1], argv[2]);   
        
    }catch(runtime_error e){
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }
}