#version 330 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

out gl_PerVertex
{
	vec4 gl_Position;
};

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in mat4 inMdlMtx;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormalQuat;
layout (location = 2) out vec3 outView;

layout (std140, binding = 0) uniform Transformation
{
	mat4 mdlvMtx;
	mat4 projMtx;
};

void main()
{
	// Calculate position
	vec4 pos = mdlvMtx * inMdlMtx * vec4(inPos, 1.0);
	outView = -pos.xyz;
	gl_Position = projMtx * pos;

	// Calculate normalquat
	vec3 normal = normalize(mat3(mdlvMtx) * inNormal);
	float z = (1.0 + normal.z) / 2.0;
	outNormalQuat = vec4(1.0, 0.0, 0.0, 0.0);
	if (z > 0.0)
	{
		outNormalQuat.z = sqrt(z);
		outNormalQuat.xy = normal.xy / (2.0 * outNormalQuat.z);
	}

	// Calculate color
	outColor = vec4(1.0);
}
