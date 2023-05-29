//
// Created by Triage on 5/28/2023.
//

#ifndef HALCYONICUS_COMPRESSIONTOOLS_H
#define HALCYONICUS_COMPRESSIONTOOLS_H
#include <iostream>
#include <vector>
#include <zlib.h>
class COMPRESSION_TOOLS{


    //I had ChatGPT generate these, so its worth testing first to see if it just works or now

    //FIXME: Needs testing
    static std::vector<char> decompressData(const std::vector<char>& compressedData, std::size_t uncompressedSize) {
        std::vector<char> decompressedData;

        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;

        // Initialize inflate (decompression) parameters
        if (inflateInit(&stream) != Z_OK) {
            std::cerr << "Failed to initialize inflate." << std::endl;
            return decompressedData;
        }

        // Set input data
        stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
        stream.avail_in = static_cast<uInt>(compressedData.size());

        // Decompressed output buffer
        std::vector<char> decompressedBuffer(uncompressedSize);

        // Decompress the input data
        do {
            stream.next_out = reinterpret_cast<Bytef*>(decompressedBuffer.data());
            stream.avail_out = static_cast<uInt>(decompressedBuffer.size());

            int inflateResult = inflate(&stream, Z_NO_FLUSH);

            if (inflateResult == Z_STREAM_ERROR) {
                std::cerr << "Error occurred during decompression." << std::endl;
                inflateEnd(&stream);
                return decompressedData;
            }

            std::size_t decompressedSize = decompressedBuffer.size() - stream.avail_out;
            decompressedData.insert(decompressedData.end(), decompressedBuffer.begin(), decompressedBuffer.begin() + decompressedSize);

        } while (stream.avail_out == 0);

        inflateEnd(&stream);

        return decompressedData;
    }

    //FIXME: Needs testing
    static std::vector<char> compressData(const std::vector<char>& input) {
        std::vector<char> compressedData;

        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;

        // Initialize deflate (compression) parameters
        if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
            std::cerr << "Failed to initialize deflate." << std::endl;
            return compressedData;
        }

        // Set input data
        stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.data()));
        stream.avail_in = static_cast<uInt>(input.size());

        // Compressed output buffer
        std::vector<char> compressedBuffer(1024);

        // Compress the input data
        do {
            stream.next_out = reinterpret_cast<Bytef*>(compressedBuffer.data());
            stream.avail_out = static_cast<uInt>(compressedBuffer.size());

            int deflateResult = deflate(&stream, Z_FINISH);

            if (deflateResult == Z_STREAM_ERROR) {
                std::cerr << "Error occurred during compression." << std::endl;
                deflateEnd(&stream);
                return compressedData;
            }

            std::size_t compressedSize = compressedBuffer.size() - stream.avail_out;
            compressedData.insert(compressedData.end(), compressedBuffer.begin(), compressedBuffer.begin() + compressedSize);

        } while (stream.avail_out == 0);

        deflateEnd(&stream);

        return compressedData;
    }
};
#endif //HALCYONICUS_COMPRESSIONTOOLS_H
