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

uniform int water;
uniform int fire;
uniform float time;
uniform float randwater;
uniform int hud;
uniform int game_over_sign;

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
	testpos = inPosition;

	if(hud==1)
	{
		if(game_over_sign==1)
			gl_Position = projMatrix * mdlMatrix * vec4(inPosition.x, inPosition.y, inPosition.z+10*sin(time*3), 1.0);
		else
			gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
	}
	else{

	if(water==1 && inPosition.y < 0.2)
		gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition.x, 2*sin(inPosition.x/6+time)/2, inPosition.z, 1.0);
	else if(fire==1 )
	{
		gl_Position = projMatrix * camMatrix * mdlMatrix *
		vec4(inPosition.x+sin(inPosition.x*time*20), inPosition.y*.8 +inPosition.y*.2*sin(60*time), inPosition.z+sin(50*time), 1.0);
	}else
		gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);

	}

	outPositionCam = vec3(camMatrix * mdlMatrix * vec4(inPosition, 1));
	outPosition = mat3(camMatrix) * vec3(mdlMatrix * vec4(inPosition, 1));

	

}