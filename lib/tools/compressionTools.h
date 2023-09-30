//
// Created by Triage on 5/28/2023.
//

#ifndef HALCYONICUS_COMPRESSIONTOOLS_H
#define HALCYONICUS_COMPRESSIONTOOLS_H
#include <iostream>
#include <vector>
#include <zlib.h>
#include "../logging/hlogger.h"
#include <lz4.h>

class CTOOLS{
public:


    static void zcomp(std::vector<char>* input, std::vector<char>* output){
        si;

        // Calculate the maximum possible size of compressed data (typically, it's less than or equal to the original size)
        int maxCompressedSize = LZ4_compressBound(input->size());

        output->clear();
        output->resize(maxCompressedSize);

        // Perform the compression
        int compressedSize = LZ4_compress_default(input->data(), output->data(), input->size(), maxCompressedSize);

        if (compressedSize <= 0) {
            std::cerr << "Compression failed." << std::endl;
            so;
            return;
        }

        // Resize the compressed vector to the actual compressed size
        output->resize(compressedSize);

        // Output the original and compressed data sizes
        std::cout << "Original Data Size: " << input->size() << " bytes" << std::endl;
        std::cout << "Compressed Data Size: " << compressedSize << " bytes" << std::endl;

        unsigned long uncompressedSize = input->size();
        unsigned char uncom[8];
        ::memcpy(uncom, &uncompressedSize, 8);
        std::cout << std::endl << std::hex;
        for (auto c : uncom){
            output->push_back(c);
            std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(c) << " ";
        }
        std::cout << std::endl << std::dec;

        so;
    }

    static void zuncomp(std::vector<char>* input, std::vector<char>* output) {
        si;

        if (input->size() < 8) {
            std::cerr << "Vector does not have enough elements." << std::endl;
            return;
        }
        unsigned char temp[8] {'\0'};
        // Copy the last 8 elements from the vector to the array
        for (int i = 7; i >= 0; i--) {
            temp[i] = input->back();
            input->pop_back();
        }

        unsigned long uncompressedSize;
        ::memcpy(&uncompressedSize, temp, 8);

        output->clear();
        output->resize(uncompressedSize);

        // Perform the decompression
        int decompressedSize = LZ4_decompress_safe(input->data(), output->data(), input->size(), uncompressedSize);

        if (decompressedSize < 0) {
            std::cerr << "Decompression failed." << std::endl;
            so;
            return;
        }

        // Resize the decompressed vector to the actual decompressed size
        output->resize(decompressedSize);

        // Output the decompressed data
        std::cout << "Decompressed Data: ";
        for (char c : *output) {
            std::cout << c;
        }
        std::cout << std::endl;

        so;
    }

    static unsigned long getExpectedSizeFromData(std::vector<unsigned char>* input){
        if (input->size() < 8) {
            err << "Input was not large enough to undergo decompression" << nl;
        }

        unsigned char sizeOfOutput[8];
        for (int i = 0; i <= 7; i++){
            sizeOfOutput[i] = input[0][i];
            std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(input->at(i)) << " " << std::dec;
        }
        std::cout << nl;

        unsigned long intSize;

        ::memcpy(&intSize, sizeOfOutput, 8);
        if (input->size() >= 8) {
            input->erase(input->begin(), input->begin() + 8);
        } else {
            input->clear();
        }
        return intSize;
    }

    static void compressV(std::vector<unsigned char>* input, std::vector<unsigned char>* output) {
        si;
        // Perform zlib operation (e.g., compression)
        z_stream stream;
        memset(&stream, 0, sizeof(stream));

        output->clear();

        int result = deflateInit(&stream, Z_DEFAULT_COMPRESSION);

        if (result != Z_OK) {
            std::cerr << "deflateInit failed" << std::endl;
            so;
            return;
        }

        char size[8];
        unsigned long sizeOfInput = input->size();
        ::memcpy(size, &sizeOfInput, 8);

        info << "Input Size: " << sizeOfInput << nl;
        for (auto c : size) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(c) << " " << std::dec;
        }
        std::cout << nl;

        for (auto c : size) output->push_back(c);

        output->resize(input->size() + 12);

        stream.avail_in = input->size();
        stream.next_in = input->data();

        // Initialize output buffer size
        stream.avail_out = output->size() + 12;
        stream.next_out = output->data() + 8;
        // Perform compression in a loop
        while (true) {
            result = deflate(&stream, Z_FINISH);
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
        info << "Output Size: " << (int) output->size() << nl;

        so;
    }

    static void decompressV(std::vector<unsigned char>* input, std::vector<unsigned char>* output, unsigned long expectedSize) {
        si;
        // Perform zlib decompression
        z_stream stream;
        memset(&stream, 0, sizeof(stream));


        output->clear();
        output->resize(expectedSize);

        stream.avail_in = input->size();
        // Dont try to modify this value, it ultimately results in a failure no matter what.
        stream.next_in = input->data();

        // Initialize output buffer size
        stream.avail_out = output->size();
        stream.next_out = output->data();

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
                info << "Resizing buffer" << nl;
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
        so;
    }
};
#endif //HALCYONICUS_COMPRESSIONTOOLS_H
