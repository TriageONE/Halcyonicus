#version 400 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUVs;
layout(location = 3) in ivec3 cubeData; //Changed to only hold the cube XYZ and not the type
layout(location = 4) in int typeIn;

uniform float time;
uniform mat4 cameraViewMatrix;
uniform mat4 cameraProjectionMatrix;
uniform vec3 cameraPosition;

uniform float chunkX;
uniform float chunkZ;

out vec3 normal;
out vec2 uv;
flat out int type;

void main()
{
    type = typeIn;
    uv = vertexUVs;
    normal = vertexNormal;

    vec3 worldPos = vertexPosition + cubeData + vec3(chunkX*16.0,0.0,chunkZ*16.0);//Should be ok, i hope. not much has changed other than ive
    gl_Position = cameraProjectionMatrix * cameraViewMatrix * vec4(worldPos,1.0);
}
