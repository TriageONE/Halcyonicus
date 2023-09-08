//
// Created by Triage on 7/16/2023.
//

#ifndef HALCYONICUS_HALCYONICUS_H
#define HALCYONICUS_HALCYONICUS_H

//Graphics
#include <glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

//Basic C++ Headers
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <map>
#include <cassert>
#include <cstdio>
#include <iomanip>
#include <random>
#include <set>
#include <variant>

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>




//ZLIB
#include <zlib.h>

//Crypto
#include <sodium.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ecdh.h>

//Files
#include <filesystem>
#include <ctime>
#include <sys/stat.h>

#endif //HALCYONICUS_HALCYONICUS_H
