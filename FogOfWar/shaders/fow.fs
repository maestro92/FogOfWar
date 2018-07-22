#version 330

uniform sampler2D u_texture;

in vec2 vf_UV;
out vec4 FragColor;


void main()
{
	vec2 tc = vf_UV;	 //	tc.y = 1.0 - vf_UV.y;

	float alpha = 1 - texture(u_texture, tc).r;
//	float alpha = texture(u_texture, tc).r;

	FragColor = vec4(0, 0, 0, alpha);
//	FragColor = vec4(texture(u_texture, tc).rgb, alpha);
}