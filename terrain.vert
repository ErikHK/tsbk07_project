#version 130

out vec3 exNormal;
out vec3 NormalTex;

out vec3 outPosition;
out vec3 outPositionCam;

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;
out vec2 texCoord;

out vec3 testpos;

// NY
uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 viewMatrix;
uniform mat4 camMatrix;


mat3 normalMatrix = mat3(camMatrix * mdlMatrix);
vec3 transformedNormal = normalMatrix * inNormal;

void main(void)
{

	//NormalTex = inNormal;

	exNormal = transformedNormal;

	mat3 normalMatrix1 = mat3(mdlMatrix);
	texCoord = inTexCoord;
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);


	outPositionCam = vec3(camMatrix * mdlMatrix * vec4(inPosition, 1));
	outPosition = mat3(camMatrix) * vec3(mdlMatrix * vec4(inPosition, 1));

	testpos = inPosition;


}