//
// Created by Triage on 5/28/2023.
//

#ifndef HALCYONICUS_COMPRESSIONTOOLS_H
#define HALCYONICUS_COMPRESSIONTOOLS_H
#include <iostream>
#include <vector>
#include <zlib.h>

class CTOOLS{
public:

    static void compressV(std::vector<unsigned char>* input, std::vector<unsigned char>* output) {

        // Perform zlib operation (e.g., compression)
        z_stream stream;
        memset(&stream, 0, sizeof(stream));

        output->clear();
        output->resize(128);

        stream.avail_in = input->size();
        stream.next_in = input->data();

        // Initialize output buffer size
        stream.avail_out = output->size();
        stream.next_out = output->data();

        int result = deflateInit(&stream, Z_DEFAULT_COMPRESSION);

        if (result != Z_OK) {
            std::cerr << "deflateInit failed" << std::endl;
            return;
        }

        // Perform compression in a loop
        while (true) {
            result = deflate(&stream, Z_FINISH);
            info << "Result of deflate is " << result << nl;
            if (result == Z_STREAM_END) {
                // Compression completed
                break;
            } else if (result == Z_BUF_ERROR) {
                // Output buffer too small, resize it and continue
                size_t newBufferSize = output->size() * 2; // Double the buffer size (adjust as needed)
                output->resize(newBufferSize);
                stream.avail_out = newBufferSize - (stream.next_out - reinterpret_cast<Bytef*>(output->data()));
            } else if (result != Z_OK) {
                // Handle other compression errors
                std::cerr << "deflate failed: " << stream.msg << std::endl;
                break;
            }
        }

        deflateEnd(&stream);
// Resize the compressedData vector to the actual compressed size
        output->resize(stream.total_out);
        // Print the compressed data (in binary)
        std::cout << "Compressed Data:\t";
        for (unsigned char byte : *output) {
            std::cout << std::hex << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::endl;
        std::cout << "Uncompressed Data:\t";
        for (unsigned char byte : *input) {
            std::cout << std::hex << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    static void decompressV(std::vector<unsigned char>* input, std::vector<unsigned char>* output) {

        // Perform zlib decompression
        z_stream stream;
        memset(&stream, 0, sizeof(stream));

        output->clear();
        output->resize(1024);

        stream.avail_in = input->size();
        stream.next_in = reinterpret_cast<Bytef*>(input->data());

        // Initialize output buffer size
        stream.avail_out = output->size();
        stream.next_out = reinterpret_cast<Bytef*>(output->data());

        int result = inflateInit(&stream);

        if (result != Z_OK) {
            std::cerr << "inflateInit failed" << std::endl;
            return;
        }

        // Perform decompression in a loop
        while (true) {
            result = inflate(&stream, Z_FINISH);

            if (result == Z_STREAM_END) {
                // Decompression completed
                break;
            } else if (result == Z_BUF_ERROR) {
                // Output buffer too small, resize it and continue
                size_t newBufferSize = output->size() * 2; // Double the buffer size (adjust as needed)
                output->resize(newBufferSize);
                stream.avail_out = newBufferSize - (stream.next_out - reinterpret_cast<Bytef*>(output->data()));
            } else if (result != Z_OK) {
                // Handle other decompression errors
                std::cerr << "inflate failed: " << stream.msg << std::endl;
                break;
            }
        }

        inflateEnd(&stream);
        output->resize(stream.total_out);
        // Print the compressed data (in binary)
        std::cout << "Compressed Data:\t";
        for (unsigned char byte : *input) {
            std::cout << std::hex << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::endl;
        std::cout << "Uncompressed Data:\t";
        for (unsigned char byte : *output) {
            std::cout << std::hex << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::endl;
    }
};
#endif //HALCYONICUS_COMPRESSIONTOOLS_H
