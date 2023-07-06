#version 330 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 incol;


uniform mat4 MVP;

out vec2 texcoord0;
out vec3 col;
out vec3 pos;

void main()
{
    col=incol;
    texcoord0=vec2(texcoord.x, texcoord.y);
    gl_Position = MVP*vec4(aPos.x, aPos.y, aPos.z, 1.0);
    pos = aPos;
}