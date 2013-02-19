//color noise
if (true) {
  
	else if (lighteffect == 3) {
	  
	  vec4 _mypos = gl_ModelViewMatrix * myvertex;
	  vec3 eyepos = vec3(0,0,0);
	  
	  float _variationsx = rand(vec2(_mypos.x,_mypos.y));
	  float _variationsy = rand(vec2(_mypos.x,_mypos.z));
	  float _variationsz = rand(vec2(_mypos.z,_mypos.y));
	  
	  vec3 mypos = _mypos.xyz / _mypos.w; // Dehomogenize current location 
	  vec3 eyedirn = normalize(eyepos - mypos); 
	  vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz; 
	  vec3 normal = normalize(_normal);
	  
	  finalcolor += vec4(_variationsx, _variationsy, _variationsz, 1);
	  
	  gl_FragColor = finalcolor;
	}
	
	//bw noise interacting with light
	else if (lighteffect == 4) {
	  
	  vec4 _mypos = gl_ModelViewMatrix * myvertex;
	  vec3 eyepos = vec3(0,0,0);
	  
	  float _variationsx = rand(vec2(_mypos.x,_mypos.y));
	  float _variationsy = rand(vec2(_mypos.x,_mypos.z));
	  float _variationsz = rand(vec2(_mypos.z,_mypos.y));
	  
	  vec3 mypos = _mypos.xyz / _mypos.w; // Dehomogenize current location 
	  vec3 eyedirn = normalize(eyepos - mypos); 
	  vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz; 
	  vec3 normal = normalize(_normal);
	  
	  finalcolor += vec4(_variationsx, _variationsx, _variationsx, 1);
	  
	  
	  for ( int n = 0; n < numLights; n++ ) {
	    vec4 ligcolor = lightcolor[n];
	    vec4 lightposition = lightposn[n];
	    
	    vec3 direction;
	    if (lightposition.w != 0) {
	      direction = normalize(lightposition.xyz / lightposition.w - mypos);
	    } else {
	      direction = normalize(lightposition.xyz);
	    }
	    vec3 halfvec = normalize (direction + eyedirn) ;  
	    
	    float nDotL = dot(normal, direction)  ;         
	    vec4 lambert = diffuse * ligcolor * max (nDotL, 0.0) ;  
	    
	    float nDotH = dot(normal, halfvec) ; 
	    vec4 phong = specular * ligcolor * pow (max(nDotH, 0.0), shininess) ; 
	    finalcolor += lambert + phong;
	    
	  }
	  
	  finalcolor += ambient;
	  gl_FragColor = finalcolor;
	}
 }
