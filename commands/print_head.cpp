#include <filesystem>
#include <cstdint>
#include "../utils/headers/searching_utils.h"
#include "../utils/headers/reading_utils.h"

namespace fs = std::filesystem;
using namespace std;

void print_head(){
    fs::path head_path = find_file(fs::current_path(), "HEAD");
    pair<uint64_t, string> head = get_hash_from_HEAD(head_path);
      cout << "Current branch: " << head.second
         << "\nTop branch commit: " << (head.first != 0 ? to_string(head.first) : "none") << endl;

}

int main(){
    print_head();
    return 0;
}