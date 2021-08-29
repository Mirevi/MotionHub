#extension GL_EXT_gpu_shader4 : require
//flat varying vec3 N;
//varying vec3 v;

varying in vec3 norm;
varying in vec3 vert;
varying in vec4 vertexColor;

// varying in vec3 baryCoords;

// varying in float fragmentDepth;

void main(void)
{
	vec3 L0 = normalize(gl_LightSource[0].position.xyz - vert);  
	vec4 Idiff;
	vec4 IdiffColor;
	if (gl_FrontFacing) // is the fragment part of a front face?
    {
		Idiff = max(dot(norm,L0), 0.0);
		if(vertexColor.r > 0.5){
			Idiff.g = 0.0;
			Idiff.b = 0.0;
		}

    }
	else
	{
		Idiff = max(dot(-norm,L0), 0.0);
		if(vertexColor.r > 0.5){
			Idiff.g = 0.0;
			Idiff.b = 0.0;
		}
	}
   Idiff = clamp(Idiff, 0.05, 0.95);
   IdiffColor =  clamp(Idiff + vertexColor, 0.05, 0.95);
	gl_FragColor = Idiff;
	gl_FragColor.a = 1.0;
	
}

// void main(){
        // vec3 normal = normalize(N); 

        // // voila, we can use our flat normal! This will colour our fragment with the current normal for debug purposes.
        // gl_FragColor = vec4((normal + vec3(1.0, 1.0, 1.0)) / 2.0,1.0);
// }