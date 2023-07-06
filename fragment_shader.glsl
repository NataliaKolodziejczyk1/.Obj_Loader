#version 330

out vec4 FragColor;
in vec2 texcoord0;
in vec3 col;

uniform sampler2D tex0;
void main()
{
  
   FragColor  = vec4(col, 1.0f);

} 
