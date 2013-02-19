# version 120 

// Mine is an old machine.  For version 130 or higher, do 
// in vec4 color ;  
// in vec4 mynormal ; 
// in vec4 myvertex ;
// That is certainly more modern

varying vec4 color ;
varying vec3 mynormal ; 
varying vec4 myvertex ;
uniform vec4 eye;

uniform int fogFlag;
varying float fogFactor;

uniform sampler2D tex ; 
uniform int shadow ; 
uniform float shadowatten;
uniform int istex ; 

uniform int lighteffect ; // are we lighting.
uniform int numused; 

const int numLights = 20 ; 
uniform vec4 lightposn[numLights] ; // positions of lights 
uniform vec4 lightcolor[numLights] ; // colors of lights

// Now, set the material parameters.  These could be varying and/or bound to 
// a buffer.  But for now, I'll just make them uniform.  
// I use ambient, diffuse, specular, shininess as in OpenGL.  
// But, the ambient is just additive and doesn't multiply the lights.  

uniform vec4 ambient ; 
uniform vec4 diffuse ; 
uniform vec4 specular ; 
uniform float shininess ; 

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 ComputeLight (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {

	float nDotL = dot(normal, direction)  ;         
	vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

	float nDotH = dot(normal, halfvec) ; 
	vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

	vec4 retval = lambert + phong ; 
	return retval ;            
}       

vec4 getLighting (const in int n,const in vec4 texcol,const in vec3 eyedirn,const in vec3 normal) {
     vec4 finalcolor = vec4(0, 0, 0, 1);

     float atten = 1.0;
     if (n > numused) return finalcolor;
     
     vec4 ligcolor = lightcolor[n];	
     vec4 lightposition = lightposn[n];
     vec3 direction;	  

	  if (lightposition.w != 0) {
	    //torches
	    float mag = dot(lightposition, lightposition);
	    if (mag > 16) return finalcolor; //torches go out 4 spaces
	    float attenFactor = (16-mag)/16.0;
	    atten = atten * attenFactor * attenFactor;
	    finalcolor += texcol * atten * (ligcolor * 2);
	    return finalcolor;
	    //ignore normal directional lights
	    //direction = normalize(lightposition.xyz / lightposition.w - mypos);	    
	  } else {
	    //sun
	    if (shadow == 1) return finalcolor;
	    direction = normalize(lightposition.xyz);
	  }
	  vec3 halfvec = normalize (direction + eyedirn) ;  

	  float nDotL = dot(normal, direction)  ;         
	  vec4 lambert = diffuse * max (nDotL, 0.0);  

          float nDotH = dot(normal, halfvec) ; 
	  vec4 phong = specular * pow (max(nDotH, 0.0), shininess); 
	  finalcolor += (lambert + phong) * atten * ligcolor;
	  return finalcolor;
}	     
void main (void) 
{       

	vec4 finalcolor = vec4(0,0,0,1);
	vec4 texcolor = vec4(0,0,0,1);

	//Texture	
    	  if (istex > 0) {
	   texcolor = texture2D(tex, gl_TexCoord[0].xy);
	   finalcolor = texcolor * shadowatten;
	   gl_FragColor = finalcolor;
	 }	  

	//No Lighting
	if (lighteffect == 0) gl_FragColor = finalcolor + color;
	//No color or Lighting 
	else if (lighteffect == 5)  gl_FragColor = finalcolor;
	//Normal Lighting
	else if (lighteffect == 1 || lighteffect == 2) {
	
	if (lighteffect == 1) {
	   finalcolor += color; //color and lighting effects
	} 
	//lighteffect 2 means no color object with only lighting effects

	const vec3 eyepos = vec3(0,0,0) ;

	vec4 _mypos = gl_ModelViewMatrix * myvertex; 
	vec3 mypos = _mypos.xyz / _mypos.w; // Dehomogenize current location 
        vec3 eyedirn = normalize(eyepos - mypos); 
	vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz; 
 	vec3 normal = normalize(_normal); 
	//for each light

	int n = 0;
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 1; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 2; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 3; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 4; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 5; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 6; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 7; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 8; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 9; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 10; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 11; 
	finalcolor += getLighting(n, texcolor, eyedirn, normal);
	n = 12; 
 

        clamp(finalcolor.xyz, 0.0, 1.0);
	finalcolor += ambient;
	gl_FragColor = finalcolor;
	}

    if (fogFlag == 1) {
        clamp(fogFactor, 0.0, 1.0);
        finalcolor = mix(vec4(.3,.5,.8, 1), finalcolor, fogFactor);
    	clamp(finalcolor, 0.0, 1.0);
    	gl_FragColor = finalcolor;	
   }
}
