#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 greenish;
uniform vec3 reddish;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
    FragColor.rgb *= greenish + reddish;
}