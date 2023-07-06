#version 330

out vec4 FragColor;
in vec2 texcoord0;
in vec3 col;
in vec3 pos;

uniform sampler2D tex0;
uniform vec3 maxpos;
void main()
{

   FragColor  = vec4(pos.x/maxpos.x,pos.y/maxpos.y,pos.z/maxpos.z, 1.0f);

} 