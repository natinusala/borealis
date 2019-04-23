#version 330 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec4 inNormalQuat;
layout (location = 2) in vec3 inView;

layout (location = 0) out vec4 outColor;

layout (std140, binding = 1) uniform Lighting
{
	vec4 lightPos;
	vec3 ambient;
	vec3 diffuse;
	vec4 specular; // w component is shininess
};

// Rotate the vector v by the quaternion q
vec3 quatrotate(vec4 q, vec3 v)
{
	return v + 2.0*cross(q.xyz, cross(q.xyz, v) + q.w*v);
}

void main()
{
	// Extract normal from quaternion
	vec4 normquat = normalize(inNormalQuat);
	vec3 normal = quatrotate(normquat, vec3(0.0, 0.0, 1.0));

	vec3 lightVec;
	if (lightPos.w != 0.0)
		lightVec = normalize(lightPos.xyz + inView);
	else
		lightVec = normalize(lightPos.xyz);

	vec3 viewVec = normalize(inView);
	vec3 halfVec = normalize(viewVec + lightVec);
	float diffuseFactor = max(dot(lightVec, normal), 0.0);
	float specularFactor = pow(max(dot(normal, halfVec), 0.0), specular.w);

	vec3 fragLightColor = ambient + diffuseFactor*diffuse + specularFactor*specular.xyz;

	outColor = vec4(min(fragLightColor, 1.0), 1.0);
}
