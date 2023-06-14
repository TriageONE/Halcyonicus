#version 400 core

out vec4 color;

/*uniform sampler2D myAtlas;
uniform int atlasRows;
uniform int atlasColumns;*/
uniform sampler2DArray myAtlas;

flat in int type;
in vec3 normal;
in vec2 uv;

void main()
{
    if(type == 0)
        discard;

    color = texture(myAtlas,vec3(uv,type-1));

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
