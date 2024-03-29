#version 400 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUVs;
layout(location = 3) in ivec3 cubeData; //Changed to only hold the cube XYZ and not the type
layout(location = 4) in int typeIn;

uniform float time;
uniform mat4 modelMatrix;
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
    /*if(typeIn == 0)
    {
        gl_Position = vec4(0, 0, 0, 0);
        return;
    }

    type = typeIn;
    uv = vertexUVs;
    normal = (modelMatrix * vec4(vertexNormal,0.0)).xyz;*/

    //vec3 worldPos = vertexPosition + cubeData + vec3(chunkX*16.0,0.0,chunkZ*16.0);//Should be ok, i hope. not much has changed other than ive
    vec4 worldPos = modelMatrix * vec4(vertexPosition,1.0);
    gl_Position = cameraProjectionMatrix * cameraViewMatrix * modelMatrix * worldPos;
}
