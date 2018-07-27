#version 330

uniform sampler2D u_fogFadeTexture;
uniform sampler2D u_noiseTexture;
uniform float u_time;
uniform float u_noiseSpeed;

// how much do you want to break up the edge shape
// the larger the number, the less defined you have with your edge
uniform float u_edgeShape;

in vec2 vf_noiseUV;
in vec2 vf_UV;
out vec4 FragColor;



float fbmNoise(sampler2D noiseTex, vec2 uv, float time)
{
	float n = 0;
	n += texture(noiseTex, (uv + time * vec2(1,1))).r - 0.5;
	n += texture(noiseTex, (-uv + time * vec2(-1,1))).r - 0.5;
	return n;
}


void main()
{
	float noise = fbmNoise(u_noiseTexture, vf_noiseUV, u_time * u_noiseSpeed);

	// if you want to debug the noise, 
	// fixed(noise.xxx + 0.5, 1)
	vec2 fogUV = vf_UV + noise * u_edgeShape;

//	FragColor = vec4( fract(fogUV), 0, 1);
//	FragColor = vec4( noise + 0.5, noise + 0.5, noise + 0.5, 1);


//	FragColor = vec4(noise + 0.5, noise+0.5, noise+0.5, 1);
//	FragColor = texture2D(u_noiseTexture, vf_noiseUV);
//	FragColor.a = 1;
//	return;
//	vec2 tc = vf_UV;	 //	tc.y = 1.0 - vf_UV.y;
//	FragColor = vec4(tc, 0, 1);
//	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
//	return;

	// so we are interpolating between the whites parts in the noise texture
	// and the blackest part in the noise texture
	vec4 color0 = vec4(0, 14/255f, 29/255f, 229/255f);
	vec4 color1 = vec4(3/255f, 0/255f, 19/255f, 244/255f);



	vec4 fogColor = mix(color0, color1, noise + 0.5f);

	fogColor = vec4(0,0,0,1);
	float alpha = 1 - texture(u_fogFadeTexture, fogUV).r;
//	FragColor =  vec4(alpha, alpha, alpha, alpha);
	FragColor = fogColor * alpha;
}