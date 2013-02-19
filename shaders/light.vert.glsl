# version 120 


// Mine is an old machine.  For version 130 or higher, do 
// out vec4 color ;  
// out vec4 mynormal ; 
// out vec4 myvertex ;
// That is certainly more modern

uniform sampler2D heightMap;
uniform int displace;
uniform vec3 centerOfDisplacement;
uniform vec4 eye;

varying vec4 color ; 
varying vec3 mynormal ; 
varying vec4 myvertex ; 

uniform int fog;
varying float fogFactor;

void main() {

	gl_TexCoord[0].xy = gl_MultiTexCoord0.xy ; 
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex ; 
	color = gl_Color ;  
	mynormal = gl_Normal ;

	if (displace == 1) {
	   vec3 dispToOrigin = (gl_Vertex.xyz - centerOfDisplacement);
	   vec4 newVertexPos = gl_Vertex + 
	   		       vec4(0,0,sin((dispToOrigin * 90).x)/6,
			       1);
	   gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * newVertexPos;
	}
	if (displace == 2) {
	   vec3 dispToOrigin =  gl_Vertex.xyz;
	   float val = dispToOrigin.x + dispToOrigin.y + dispToOrigin.z;
	   float sinval = sin(val * 30) * sin(val * 30);
	   vec4 newVertexPos = vec4((dispToOrigin*sinval),1);
	   gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * newVertexPos;
	}
	myvertex = gl_Vertex ; 
	
           float distAway = length(gl_ModelViewMatrix * gl_Vertex);	
	   fogFactor = 1 - distAway/10;
	   clamp(fogFactor, 0.0f, 1.0f);
}
