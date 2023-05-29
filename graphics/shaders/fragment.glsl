#version 400 core

out vec4 color;

uniform sampler2D myAtlas;
uniform int atlasRows;
uniform int atlasColumns;

flat in int type;
in vec3 normal;
in vec2 uv;

void main()
{
    vec2 uvSize = vec2(atlasColumns,atlasRows);
    uvSize = vec2(1.0,1.0) / uvSize;

    int column = (type-1) % atlasColumns;
    int row = (type-1) / atlasColumns;

    vec2 uvStart = vec2(column,row);
    uvStart *= uvSize;

    vec2 actualUV = uv * uvSize + uvStart;

    color = texture2D(myAtlas,actualUV);

    vec3 sunDirection = normalize(vec3(1,0.5,0.2));
    float cosTheta = clamp(dot(sunDirection,normal),0.0,1.0);
    cosTheta += 0.3;
    cosTheta = clamp(cosTheta,0.0,1.0);

    color.rgb = cosTheta * color.rgb;
    color.a = 1.0;

    //Tone maping
    //color.rgb = color.rgb / (color.rgb + vec3(1.0));
    //Gamma correction
    //color.rgb = pow(color.rgb, vec3(1.0/2.2));
}
