#version 330

uniform sampler2D u_texture;
uniform vec2 u_texelSize;
uniform float u_offset;

in vec2 vf_UV;
out vec4 FragColor;

void main()
{
	vec2 tc = vf_UV;	 

//	FragColor = texture(u_texture, tc);


	vec2 duv = u_offset * u_texelSize;

//	vec2 duv = vec2(1/60f, 1/60f);

	vec4 OUT = vec4(0,0,0,0);
	OUT += 0.25 * texture(u_texture, vec2(tc.x + duv.x, tc.y + duv.y));
	OUT += 0.25 * texture(u_texture, vec2(tc.x + duv.x, tc.y - duv.y));
	OUT += 0.25 * texture(u_texture, vec2(tc.x - duv.x, tc.y + duv.y));
	OUT += 0.25 * texture(u_texture, vec2(tc.x - duv.x, tc.y - duv.y));

	FragColor = OUT;

}