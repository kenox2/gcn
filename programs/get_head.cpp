#include <filesystem>
#include <cstdint>
#include "../utils/headers/searching_utils.h"
#include "../utils/headers/reading_utils.h"

namespace fs = std::filesystem;
using namespace std;

void get_head(){
    fs::path head_path = find_file(fs::current_path(), "HEAD");
    pair<uint64_t, string> head = get_hash_from_HEAD(head_path);
    cout << fs::path(head.second).filename();
}

int main(){
    get_head();
    return 0;
}