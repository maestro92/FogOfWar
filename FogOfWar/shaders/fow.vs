#version 330

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 UV;

uniform float u_noiseTexSamplingLocationScale;
uniform mat4 u_modelViewProjMat;
uniform mat4 u_modelMat;
uniform vec2 u_fogMeshVertexOrigin;
uniform vec4 u_fogMeshVertex2UVMat;

out vec2 vf_noiseUV;
out vec2 vf_UV;

void main()
{
	gl_Position = u_modelViewProjMat * vec4(position, 1.0);

	// in local pos (without the modelview matrix)
	vec2 worldPos2D = vec2(position.x, position.y);

	vec2 offset = worldPos2D - u_fogMeshVertexOrigin;

	// convert from worldPos to UV
	mat2 fogMat = mat2(u_fogMeshVertex2UVMat);
	vf_UV = fogMat * offset;


	// compute the noise texture
	vec4 modelWorldPos = u_modelMat * vec4(position, 1.0);	
	vf_noiseUV = modelWorldPos.xy * u_noiseTexSamplingLocationScale;
}



