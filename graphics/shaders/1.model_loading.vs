#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aHeight;
layout (location = 3) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 ColorMap;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 worldOffset;

void main()
{
    TexCoords = aTexCoords;
    float color = float(int(aHeight)-256)/1024.0;
    float coffset = 0.0;
    if (((int(gl_InstanceID % 64) + int(gl_InstanceID / 64) % 2 ) % 2) == 1){
        coffset = 0.2;
    }
    vec3 instancePos = vec3((gl_InstanceID % 64) + worldOffset.y*64, aHeight/2.0, (gl_InstanceID / 64) + worldOffset.x*64);

    ColorMap = vec3(color + coffset, color + coffset, color + coffset);

    gl_Position = projection * view * model * vec4(aPos * vec3(1.0,1.0,1.0) + instancePos, 1.0);
}


