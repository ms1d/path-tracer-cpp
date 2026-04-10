#pragma once



#include <string>
#include <stdexcept>
#include <zlib.h>



// Standard gzip decompression algorithm I found online
inline std::string gzip_decompress(const std::string& input) {
    z_stream zs{};
    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.data()));
    zs.avail_in = static_cast<uInt>(input.size());

    if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK)
        throw std::runtime_error("inflateInit failed");

    std::string out;
    char buffer[8192];

    int ret;
    do {
        zs.next_out = reinterpret_cast<Bytef*>(buffer);
        zs.avail_out = sizeof(buffer);

        ret = inflate(&zs, Z_NO_FLUSH);

        if (ret != Z_OK && ret != Z_STREAM_END) {
            inflateEnd(&zs);
            throw std::runtime_error("gzip inflate failed");
        }

        out.append(buffer, sizeof(buffer) - zs.avail_out);

    } while (ret != Z_STREAM_END);

    inflateEnd(&zs);
    return out;
}
