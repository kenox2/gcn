#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace fs = std::filesystem;
using namespace std;

int init(const fs::path& dir, const string& username, const string& email){
    // create a hidden directory for diffrent operating systems
    fs::path gcn_dir(dir / ".gcn");
    fs::create_directory(gcn_dir);
    #ifdef _WIN32
        if (!SetFileAttributesA(gcn_dir.string().c_str(), FILE_ATTRIBUTE_HIDDEN)) {
            cerr << "Failed to set hidden attribute\n";
            return 1;
        }
    #endif

    fs::create_directories(gcn_dir / "refs" / "heads" / "main");
    fs::create_directory(gcn_dir / "objects");

    // create index file 
    ofstream((gcn_dir / "INDEX"), ios::binary | ios::out);

    // create head file 
    ofstream((gcn_dir / "HEAD"), ios::binary | ios::out);

    // create user data file with nusername and email
    ofstream user_data(gcn_dir / "user_data", ios::out);
    user_data << "username=" << username << "\nemail=" << email << "\n";


    return 0;
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