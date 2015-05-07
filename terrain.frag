#version 130

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex, tex2;


in vec3 exNormal;

in vec3 outPosition;
in vec3 outPositionCam;

in vec3 testpos;

uniform mat4 camMatrix;
uniform mat4 mdlMatrix;


uniform int skybox;
uniform int water;

uniform bool multitex;

in vec3 norm;
in vec3 vec;
vec3 colors;

vec3 s;
vec3 n;
vec3 eye;

float cos_angle;

vec3 diffuse;
vec3 specular;
vec3 tmp_colors;
vec3 reflection;


uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];


mat3 lightCamMatrix = mat3(camMatrix);


void main(void)
{
	vec4 test = (mdlMatrix)*vec4(outPosition.x, outPosition.y, outPosition.z, 1);
	//colors = vec3(0,0,0);
	colors = vec3(0.5, 0.5, 0.5);


	if(multitex)
	{
    	tmp_colors = vec3(texture(tex, texCoord));
    }
    else{
    	tmp_colors = vec3(texture(tex2, texCoord));
    }



	if(skybox==1)
	{
		outColor = texture(tex, texCoord);

	}
	else{

        for(int i=0;i<4;i++){
        	if(isDirectional[i])
        	{
        		s = normalize(lightCamMatrix * lightSourcesDirPosArr[i]);
        	}else{
                s = normalize(lightCamMatrix * lightSourcesDirPosArr[i]-outPosition);
        	}
        	n = normalize(exNormal);

        	float lambert = dot(n,s)-.001;

        	if(lambert > 0)
        	{
				diffuse = (lightSourcesColorArr[i]*tmp_colors)*lambert;
				colors += diffuse;

				reflection = reflect(s, n);
				eye = normalize(outPositionCam);

				cos_angle = dot(reflection, eye);
				cos_angle = max(0, cos_angle);

				specular = (lightSourcesColorArr[i]*tmp_colors)*pow(cos_angle, specularExponent[i]);
				colors += specular;

        	}
		}

		if(testpos.y < 0.2 && water==1)
		{
			outColor = vec4(colors.x*0, colors.y*0, colors.z, 1.0);
		}else{
			if((colors.y + colors.x + colors.z)/3 > 0.75)
				outColor = vec4(1, 1, 1, 1.0)*texture(tex, texCoord);
			else if((colors.y + colors.x + colors.z)/3 > 0.5)
				outColor = vec4(.4, .4, .4, 1.0)*texture(tex, texCoord);
			else
				outColor = vec4(0.2, 0.2, 0.2, 1.0)*texture(tex, texCoord);
		}
		
	}


	//outColor = texture(tex, texCoord);
}
