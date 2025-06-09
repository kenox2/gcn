#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include "../utils/headers/creating_utils.h"
#include "../utils/headers/searching_utils.h"
#include "../utils/headers/constants.h"
#include "../utils/headers/hashing_utils.h"

namespace fs = std::filesystem;
using namespace std;

template <typename Stream>
void write_index_entry(Stream& out, const fs::path& file_path, const fs::path& root, const fs::path& objects, uint64_t& hash) {
    string path = fs::relative(file_path, root).string();
    uint16_t path_len = path.size();
    out.write(reinterpret_cast<char*>(&path_len), sizeof(path_len));
    out.write(path.data(), path.size());

    uint32_t mode;
#ifdef _WIN32
    mode = static_cast<uint32_t>(FileMode::Blob);
#else
    auto perms = fs::status(file_path).permissions();
    mode = static_cast<uint32_t>((perms & fs::perms::owner_exec) ? FileMode::Exec : FileMode::Blob);
#endif

    out.write(reinterpret_cast<char*>(&mode), sizeof(mode));
    out.write(reinterpret_cast<char*>(&hash), sizeof(hash));
}

void delete_blob(fs::path object_path, uint64_t hash){
    string file_str = hash_to_path(hash);
    string object_str = object_path.string();
    string file_path = object_str + file_str;
    //  debugging 
    cout << "file_str content: " << file_str << " len: " << file_str.size();
    cout << "object_stre content: "  << object_str << " len: " << object_str.size();
    cout << "added: " << file_path << "len: " <<  file_path.size();
    
    
    fs::remove_all(fs::path(file_path));
}







void delete_non_existing_index_entries(uint32_t& count, fs::path objects_path, vector<IndexEntry>& index_entries){
    for(auto it = index_entries.begin(); it != index_entries.end(); ){
        fs::path absolute_path = fs::absolute(it->path);
        if(!fs::exists(absolute_path)){
            index_entries.erase(it);
            delete_blob(objects_path, it->hash);
            --count;
        }else it++;
        
    }

}

unordered_map<fs::path, IndexEntry> load_curr_ind(uint32_t& count, string index_path, fs::path root, fs::path objects_path){
    vector<IndexEntry> entries = read_index(index_path);
    delete_non_existing_index_entries( count, objects_path,entries);
    //  change count 
    fstream index_file(index_path, ios::binary | ios::in | ios::out);
    index_file.write(reinterpret_cast<char*>(&count), sizeof(count));
    index_file.seekp(0,ios::end);
    
    unordered_map<fs::path, IndexEntry> map;
    for (const auto& entry : entries) {
        map[entry.path] = entry;
    }
    return map;
} 






template <typename Stream>
void rewrite_index(Stream& index_file, const unordered_map<fs::path, IndexEntry>& index_entries, const fs::path& root, const fs::path& objects){
    for(auto& entry : index_entries){
        auto [path, index_entry] = entry;
        write_index_entry(index_file, path, root, objects, index_entry.hash);
    }
}


uint32_t get_count_from_ind(fs::path index_path){
    fstream index_file(index_path, ios::binary | ios::in | ios::out);
        // update count 
    uint32_t count;
    index_file.read(reinterpret_cast<char*>(&count), sizeof(count));
    return count;
}




// i need to create a list of all files that are supposed to be stored in index 
//  then for a singl file i check if its already in index and does it have the same hash. 
// if yes and yes i rewrite entire index if yes and not i dont do shit
// if no then i just append new file at the end
int add_to_index(fs::path cur_path) {
    fs::path curr_dir = fs::current_path();
    fs::path index = find_file(curr_dir, "INDEX");
    fs::path root = index.parent_path().parent_path();
    fs::path objects = find_file(curr_dir, "objects", true);
    
    uint32_t count = 0;
    unordered_map<fs::path, IndexEntry> index_entries;
    if(!fs::is_empty(index)){
        count = get_count_from_ind(index);
        index_entries = load_curr_ind(count, index.string(), root, objects);
    }

    if (!(fs::is_directory(cur_path))) {
        // get count
        
        uint64_t hash = create_blob(cur_path.string(), objects.string());
        fs::path rel_path = fs::relative(cur_path, root);
        if(index_entries.count(rel_path) > 0){
            IndexEntry& entry = index_entries[rel_path];
            if(entry.hash == hash) return 0;
            else{
                entry.hash = hash;
                ofstream index_file(index, ios::binary);
                 if (!index_file) {
                    cerr << "Failed to open index file.\n";
                    return 1;
                }
                index_file.write(reinterpret_cast<char*>(&count), sizeof(count));
                rewrite_index(index_file, index_entries, root, objects); 
                delete_blob(objects, hash);
            }
        }else{
            ++count;
            fstream index_file(index, ios::binary | ios::in | ios::out);
             if (!index_file) {
                cerr << "Failed to open index file.\n";
                return 1;
            }
            index_file.write(reinterpret_cast<char*>(&count), sizeof(count));
            index_file.seekp(0,ios::end);
            write_index_entry(index_file, cur_path, root, objects, hash);
        }
        return 0;
    } else {

        for (fs::recursive_directory_iterator it(cur_path), end; it != end; ++it) {
            if (it->is_directory() && it->path().filename().string() == ".gcn" &&
                fs::absolute(it->path().parent_path()) == root) {
                it.disable_recursion_pending();
                continue;
            }

            if (it->is_regular_file()) { 
                fs::path rel_path = fs::relative(it->path(), root);
                uint64_t hash = create_blob(it->path().string(), objects.string());
                if(index_entries.count(rel_path) > 0){
                    IndexEntry entry = index_entries[rel_path];
                    if(entry.hash == hash) continue;
                    else{
                        entry.hash = hash;
                        ofstream index_file(index, ios::binary);
                        if (!index_file) {
                            cerr << "Failed to open index file.\n";
                            return 1;
                        }
                        index_file.write(reinterpret_cast<char*>(&count), sizeof(count));
                        delete_blob(objects, hash);
                    }
                }
                else{
                    ++count;
                    string rel_path_str = rel_path.string();
                    uint16_t path_len = rel_path_str.size();
                    uint32_t mode;
#ifdef _WIN32
                    mode = static_cast<uint32_t>(FileMode::Blob);
#else
                    auto perms = fs::status(it_path->).permissions();
                    mode = static_cast<uint32_t>((perms & fs::perms::owner_exec) ? FileMode::Exec : FileMode::Blob);
#endif

                    IndexEntry entry = {rel_path_str ,mode, hash};
                    index_entries[rel_path] = entry;
                }
                
            }
        }
        ofstream index_file(index, ios::binary);
        if (!index_file) {
            cerr << "Failed to open index file.\n";
            return 1;
        }

        index_file.write(reinterpret_cast<char *>(&count), sizeof(count));
        rewrite_index(index_file, index_entries, root, objects); 
         
        return 0;
    }
}

int main(int argc, char* argv[]){
    if(argc != 2){
        cerr << "Usage: add <directory_name>\n";
        exit(1);
    }
    string dir = argv[1];
    try {
        return add_to_index(fs::absolute(fs::path(dir)));
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << '\n';
        return 1;
    }
}