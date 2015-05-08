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
uniform int exhaust;
uniform int fire;

uniform float time;
uniform float randwater;

uniform vec3 spaceship_pos;

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

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

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
		outColor = vec4(0.7*(1-testpos.y*.5)+.2,(1-testpos.y*.5)+.2,1,1);

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
        	
        	float f = smoothstep(.1, .45, lambert);
        	if(exhaust==0)
        	{
        		vec4 pixcol1 = vec4(1,1,1,1)*texture(tex, texCoord);
        		vec4 pixcol2 = vec4(.9,.9,.9,1)*texture(tex, texCoord);
        		outColor = mix(pixcol1, pixcol2, f);
        	
        	}else
        	{
        		vec4 pixcol1 = vec4(.8,.8,.8,1);
        		vec4 pixcol2 = vec4(.6,.6,.6,1);
        		outColor = mix(pixcol1, pixcol2, f);
        	}
        	
        	if(fire==1)
        	{
        		f = smoothstep(.1, .3, lambert);
        		vec4 pixcol1 = vec4(1,0,0,1);
        		vec4 pixcol2 = vec4(1,.7,0,1);
        		outColor = mix(pixcol1, pixcol2, f);
        	}

		}

		if(testpos.y < 0.2 && water==1)
		{
			float f = smoothstep(.1,.3,testpos.y);
			vec4 pixcol = texture2D(tex, texCoord/20.0);
			pixcol.rgb = (pixcol.rgb-0.5)*max(pixcol.rgb*2,0)+.5;

			vec4 col1 = vec4(colors.x*0, colors.y*0.2+sin(testpos.x/3+time)/30.0, colors.z, 1)
			+pixcol/2.0;
			vec4 col2 = vec4(1,1,1,1);
			outColor = mix(vec4(col1.x-.3,col1.y-.3,col1.z, 1),col2,f);
		}else if(testpos.y < .5 && water==1){
			float f = smoothstep(.1,.8,testpos.y);
			outColor = mix(vec4(1,1,1,1), vec4(1,.9,.3,1), f);
		}else{
			//if((colors.y + colors.x + colors.z)/3 > 0.5)
			//	outColor = vec4(1, 1, 1, 1.0)*texture(tex, texCoord);
			//else
			//	outColor = vec4(0.9, 0.9, 0.9, 1.0)*texture(tex, texCoord);
		}
		
	}

	if((spaceship_pos.x-testpos.x)*(spaceship_pos.x-testpos.x) + (spaceship_pos.z-testpos.z)*(spaceship_pos.z-testpos.z) < 
	min(100, (spaceship_pos.y-testpos.y)*6))
	{
		if(testpos.y > .2)
			outColor = vec4(0.2,0.2,0.2,1)*texture(tex, texCoord);
		else
			outColor = vec4(0,0,0.1,1);
	}

	
}

