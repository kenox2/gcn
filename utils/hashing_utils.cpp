
#include "../libs/xxhash/xxhash.h"
#include <fstream>
#include <zlib.h>
#include "./hashing_utils.h"

using namespace std;

uint64_t get_hash(ifstream &file){
    // returns: hash if succseful 0 if file path is invalid and 1 if state couldnt be created
    if(!file){
       return 0;
    }

    XXH64_state_t* const state = XXH64_createState();
    if(!state) return 1;
    char buffer[4096];
    XXH64_reset(state, 0);
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0){
        XXH64_update(state, buffer, file.gcount());
    }

    uint64_t hash = XXH64_digest(state);
    XXH64_freeState(state);

    return hash;

}


string hash_to_path(const uint64_t&  hash){
    string hash_str = to_string(hash);
    string dir = hash_str.substr(0, 2);
    string filename = hash_str.substr(2);

    return "/" + dir + "/" + filename;    
}



void compress_zlib(ifstream& in_stream, ofstream& out_stream){
    const size_t SIZE = 4096;
    char in_buffer[SIZE];
    char out_buffer[SIZE];

    z_stream zs{};

    deflateInit(&zs, Z_DEFAULT_COMPRESSION); 

    int flush;
    do {
        in_stream.read(in_buffer, SIZE);
        std::streamsize read_count = in_stream.gcount();

        zs.next_in = reinterpret_cast<Bytef*>(in_buffer);
        zs.avail_in = static_cast<uInt>(read_count);

        flush = in_stream.eof() ? Z_FINISH : Z_NO_FLUSH;

        do {
            zs.next_out = reinterpret_cast<Bytef*>(out_buffer);
            zs.avail_out = SIZE;

            deflate(&zs, flush);

            std::streamsize compressed_size = SIZE - zs.avail_out;
            out_stream.write(out_buffer, compressed_size);
        } while (zs.avail_out == 0);

    } while (flush != Z_FINISH);

    deflateEnd(&zs);
}