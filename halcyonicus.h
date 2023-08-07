//
// Created by Triage on 7/16/2023.
//

#ifndef HALCYONICUS_HALCYONICUS_H
#define HALCYONICUS_HALCYONICUS_H
#define STB_IMAGE_IMPLEMENTATION
//Basic C++ Headers
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <string>
#include <map>
#include <cassert>
#include <cstdio>
#include <iomanip>
#include "lib/tools/streamtools.h"

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Graphics
#include <glad/glad.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


//ZLIB
#include <zlib.h>

//Crypto
#include <sodium.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ecdh.h>

#endif //HALCYONICUS_HALCYONICUS_H
