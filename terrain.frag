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


uniform int landing_point;
uniform int skybox;
uniform int hud;
uniform int fuel_full;
uniform int game_over_sign;
uniform int water;
uniform int exhaust;
uniform int fire;
uniform vec3 highest;

uniform float world_angle0;
uniform float world_angle1;


uniform float time;
uniform float randwater;

uniform vec3 spaceship_pos;
uniform vec3 landing_point_pos;

uniform bool multitex;

in vec3 norm;
in vec3 vec;
vec4 colors;

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

uniform vec3 cam_vector;

mat3 lightCamMatrix = mat3(camMatrix);

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void draw_shadow()
{
	if((-spaceship_pos.x-testpos.x)*(-spaceship_pos.x-testpos.x) +
	(-spaceship_pos.z-testpos.z)*(-spaceship_pos.z-testpos.z) < 
	min(1000, (spaceship_pos.y-testpos.y)*60))
	//if((testpos.x)*(testpos.x) +
	//(testpos.z)*(testpos.z) < 
	//min(1000, (testpos.y)*60))
	{
		//if(testpos.y > .2)
			colors -= vec4(0.5,0.5,0.5,0)*texture(tex, texCoord);
		//else
			//outColor = vec4(0,0,0.1,1);
			//colors -= vec4(0,0,0.1,0);
	}
}

float dist(vec3 p1, vec3 p2)
{
	float xx = pow(p1.x-p2.x, 2);
	float yy = pow(p1.y-p2.y, 2);
	float zz = pow(p1.z-p2.z, 2);

	return sqrt(xx+yy+zz);
}

void main(void)
{

	//vec4 test = (mdlMatrix)*vec4(outPosition.x, outPosition.y, outPosition.z, 1);
	//colors = vec3(0,0,0);
	colors = vec4(0, 0, 0, 0);

	n = normalize(mat3(mdlMatrix)*exNormal);
	//s = normalize(lightCamMatrix*cam_vector);
	//s = normalize(lightCamMatrix*vec3(0,1,0));
	//s = vec3(cos(world_angle1),sin(world_angle1),0);
	//s = normalize(spaceship_pos);
	//s = normalize(vec3(cos(world_angle0)*sin(world_angle1),sin(world_angle0)*sin(world_angle1),cos(world_angle1)));

    //float lambert = pow(dot(n,s),2)-.001;
	float lambert = dot(n,s)-.001;


	if(skybox==1)
	{
		//color gradient, blue sky
		colors += vec4(0.7*(1-testpos.y*.5)+.2,(1-testpos.y*.5)+.2,1,1);

	}
	else if(hud==1)
	{
		if(fuel_full==1)
			colors += vec4(.1,.1,.1,.6);
		else if(game_over_sign==1)
		{
			float lambert = dot(n,vec3(0,0,1));
			float f = smoothstep(.1, .8, lambert);
			vec4  pixcolor1 = vec4(.2, .2, .2, 1);
			vec4  pixcolor2 = vec4(.7,.7,.7,1);
			colors += mix(pixcolor1, pixcolor2, f);
		}
		else
			colors += vec4(.7,.1,.1,1);
	}
	else{
        
        	
		float f = smoothstep(.45, 0, lambert);
        if(exhaust==0)
        {
        	vec4 pixcol1 = vec4(1,1,1,1)*texture(tex, texCoord);
        	vec4 pixcol2 = vec4(.8,.8,.8,1)*texture(tex, texCoord);
        	colors += mix(pixcol1, pixcol2, f);
        	
        }else //exhaust == 1, 
        {
        	vec4 pixcol1 = vec4(.8,.8,.8,1);
        	vec4 pixcol2 = vec4(.6,.6,.6,1);
        	colors += mix(pixcol1, pixcol2, f);
        }
        	
        if(fire==1)
        {
			//s = normalize(lightCamMatrix*cam_vector);
			s = vec3(0,0,1);

        	f = smoothstep(.6, .9, dot(n,s));
        	vec4 pixcol1 = vec4(1,0,0,1);
        	vec4 pixcol2 = vec4(1,.7,0,1);
			
			colors = mix(pixcol1, pixcol2, f);
        }

		if(landing_point==1)
		{
			vec4 pixcol1 = vec4(.8,.5,.5,1);
        	vec4 pixcol2 = vec4(.6,.3,.3,1);
			
			if((landing_point_pos.x-testpos.x-10)*(landing_point_pos.x-testpos.x-10) + 
			(landing_point_pos.z-testpos.z-10)*(landing_point_pos.z-testpos.z-10) < 1000)
			{
				colors = vec4(1,0,0,1)*texture(tex, texCoord);
			}else{
				colors = mix(pixcol1, pixcol2, f)*texture(tex,texCoord);
			}
		}

		
		//if(testpos.y < 0.2 && water==1)
		if(length(testpos+vec3(0,12800,0)) < 12800 && water==1)
		{
			float f = smoothstep(.1,.3,testpos.y);
			vec4 pixcol = texture(tex, texCoord/20.0);
			pixcol.rgb = (pixcol.rgb-0.5)*max(pixcol.rgb*2,0)+.5;
			vec4 col1 = vec4(0, 0.2+sin(testpos.x/60+time)/30.0, 0.5, 1)
			+pixcol/2.0;
			vec4 col2 = vec4(1,1,1,1);
			colors = mix(vec4(col1.x-.3,col1.y-.3,col1.z, 1),col2,f);
		}
		//else if(testpos.y < .5 && water==1){
		else if(length(testpos+vec3(0,12800,0)) < 12810 && water==1){
			float f = smoothstep(.1,.8,testpos.y);
			colors = mix(vec4(1,1,1,1), vec4(1,1,1,.1), f);
		}
		else{ //ground
			
		
		}
		
	}

	

	
	draw_shadow();
	outColor = colors;

}

