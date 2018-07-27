#version 330

uniform sampler2D u_texture;

in vec2 vf_UV;
out vec4 FragColor;


void main()
{
	vec2 tc = vf_UV;	 //	tc.y = 1.0 - vf_UV.y;
	FragColor = vec4(texture(u_texture, tc).xyz, 1) * 0.05;
}