#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "../utils/searching_utils.h"

#ifdef _WIN32
    #include <windows.h>
#endif

namespace fs = std::filesystem;
using namespace std;

int create_gcn_dir(const fs::path& dir, const string& username, const string& email){
    fs::path gcn_dir(dir / ".gcn");
    fs::create_directory(gcn_dir);
    #ifdef _WIN32
        if (!SetFileAttributesA(gcn_dir.string().c_str(), FILE_ATTRIBUTE_HIDDEN)) {
            cerr << "Failed to set hidden attribute\n";
            return 1;
        }
    #endif
    
    fs::path main_branch = gcn_dir / "refs" / "heads";
    
    fs::create_directories(main_branch);
    fs::create_directory(gcn_dir / "objects");

    // create index file 
    ofstream((gcn_dir / "INDEX"), ios::binary | ios::out);

    // create head file 
    ofstream head((gcn_dir / "HEAD"), ios::binary | ios::out);
    // add relative path to the main branch 
    if (!head) {
        cerr << "Failed to open output file: " << gcn_dir / "HEAD" << "\n";
        return 3;
    }
    main_branch = main_branch / "main";
    ofstream main(main_branch, ios::binary | ios::out);
    string id = "path";
    head.write(reinterpret_cast<char *>(id.data()), id.size());
    auto rel_main_branch = fs::relative(main_branch, gcn_dir).string();
    head.write(reinterpret_cast<char *>(rel_main_branch.data()),  rel_main_branch.size());

    // create user data file with nusername and email
    ofstream user_data(gcn_dir / "user_data", ios::out);
    if (!user_data) {
        cerr << "Failed to open output file: " << gcn_dir / "user_data" << "\n";
        return 3;
    } 
    user_data << "username=" << username << "\nemail=" << email << "\n";
    
    return 0;
}



int init(const fs::path& dir, const string& username, const string& email){
    // create a hidden directory for diffrent operating systems
    try{
        find_gnc_dir(fs::current_path());
        cout << "Already a gcn repository";
        return 0;   
    }catch(const std::runtime_error& e){
        return create_gcn_dir(dir, username, email);
    }
    
}


int main(int argc, char *argv[]){
    if(argc != 3){
        cerr << "Usage: init <username> <email>\n";
        exit(1);
    }
    string username = argv[1];
    string email = argv[2];
    return init(fs::current_path(), username, email);
}