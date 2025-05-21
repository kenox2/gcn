#include <filesystem>
#include "searching_utils.h"
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

fs::path find_gnc_dir(){
    const string DIR = ".gcn";
    fs::path curr = fs::current_path();
    
    while(!curr.empty()){
        fs::path candidate = curr / DIR;
        if(fs::exists(candidate) && fs::is_directory(candidate)){
            return candidate;
        }
        curr = curr.parent_path();
    }
    throw std::runtime_error("This is not a gcn repository.");
}


fs::path find_file(const string& relative_file_path, bool is_dir){
    fs::path gnc_dir = find_gnc_dir();
    fs::path path = gnc_dir / relative_file_path;
    if(fs::exists(path) && (is_dir ? fs::is_directory(path) : fs::is_regular_file(path))){
        return (path);
    }
    throw std::runtime_error("No INDEX file. Repository might be corrupted.");
}


