#include <iostream>
#include <filesystem>
#include <fstream>
#include "../utils/creating_utils.h"
#include "../utils/searching_utils.h"
#include "../utils/constants.h"

namespace fs = std::filesystem;
using namespace std;


int add_to_index(fs::path cur_dir){
    fs::recursive_directory_iterator it(cur_dir);
    fs::path index = find_file("INDEX");
    fs::path objects = find_file("objects", true);
    ofstream ind_file(index, ios::out | ios::binary);
    ostringstream buffer;
    uint32_t count = 0;

    for (const auto& entry : it) {
        if (entry.is_regular_file()) {
            count++;
            uint64_t hash = create_blob(entry.path().string(), objects.string());
            string path = entry.path().string();
            uint16_t path_len = path.size();
            buffer.write(reinterpret_cast<char*>(&path_len), sizeof(path_len));
            buffer.write(path.data(), path.size());

            auto perms = entry.status().permissions();

            uint32_t mode;
#ifdef _WIN32
                mode = static_cast<uint32_t>(FileMode::Blob);
#else
                mode = static_cast<uint32_t>((perms & fs::perms::owner_exec) ? FileMode::Exec : FileMode::Blob);
#endif

            buffer.write(reinterpret_cast<char*>(&mode), sizeof(mode));
            buffer.write(reinterpret_cast<char*>(&hash), sizeof(hash));
        }
    }

    ind_file.write(reinterpret_cast<char*>(&count), sizeof(count));
    if (!ind_file) {
        cerr << "Failed to open index file for writing: " << index << endl;
        return 1;
    }
    string data = buffer.str();
    ind_file.write(data.data(), data.size());
    return 0;
}

int main(int argc, char* argv[]){
    if(argc != 2){
        cerr << "Usage: add <directory_name>\n";
        exit(1);
    }
    string dir = argv[1];
    try {
        return add_to_index(fs::path(dir));
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << '\n';
        return 1;
    }
}