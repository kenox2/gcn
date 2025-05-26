#include <filesystem>
#include "../headers/searching_utils.h"
#include <iostream>
#include "../headers/hashing_utils.h"

namespace fs = std::filesystem;
using namespace std;

fs::path find_gnc_dir(const fs::path& cur_dir){
    const string DIR = ".gcn";
    fs::path curr = fs::absolute(cur_dir);
    
    while(!curr.empty()){
        fs::path candidate = curr / DIR;
        if(fs::exists(candidate) && fs::is_directory(candidate)){
            return candidate;
        }
        curr = curr.parent_path();
         fs::path parent = curr.parent_path();
        if (parent == curr) {
            break;  // We've reached the root
        }
    curr = parent;
    }
    throw std::runtime_error("This is not a gcn repository.");
}


fs::path find_file(const fs::path& cur_dir, const string& relative_file_path, bool is_dir){
    fs::path gnc_dir = find_gnc_dir(cur_dir);
    fs::path path = gnc_dir / relative_file_path;
    if(fs::exists(path) && (is_dir ? fs::is_directory(path) : fs::is_regular_file(path))){
        return (path);
    }
    throw std::runtime_error("No file. It is not repostiry or repositroy is corrupted");
}


fs::path find_file_by_hash(const fs::path object_dir, const string& hash){
    string dir_part = hash.substr(0,2);
    string hash_part = hash.substr(2);
    fs::path branch_dir(object_dir / dir_part);
    if(!fs::exists(branch_dir)){
        throw std::runtime_error("File with this hash doesn't exist");
    }
    fs::directory_iterator it(branch_dir);
    for(auto entry : it){
        string filename = entry.path().filename().string();
        if(entry.is_regular_file() && filename.size() >= hash_part.size() && (filename.compare(0, hash_part.size(), hash_part) == 0) ){
            return entry.path();
        }
    }
    throw std::runtime_error("Commit with this hash doesn't exist");
}

fs::path find_file_by_hash(const fs::path object_dir,const uint64_t& hash){
    string hash_dir = object_dir.string() + hash_to_path(hash);
    fs::path path(hash_dir);
    if(fs::exists(path)) return path;
    throw std::runtime_error("File system corrupted. File doesn't exist even though it should");
}