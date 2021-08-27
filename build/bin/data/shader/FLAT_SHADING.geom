#version 120
#extension GL_EXT_geometry_shader4 : enable
varying in vec3 v[];
varying in vec3 N[];
varying in vec4 vC[];

varying out vec3 norm;
varying out vec3 vert;
varying out vec4 vertexColor;

// varying out vec3 baryCoords;

// varying out float fragmentDepth;

void main(void)
{
	vec3 finalNormal = N[0] + N[1] + N[2];
	finalNormal = normalize(finalNormal);
	
	// vec4 vecA = gl_PositionIn[0] - gl_PositionIn[1];
	// vec4 vecB = gl_PositionIn[0] - gl_PositionIn[2];
	// vec4 vecC = gl_PositionIn[1] - gl_PositionIn[2];
	
	// float s = length(vecA) + length(vecB) + length(vecC);
	// s /= 2.0;
	
	// float F = sqrt(s*(s-length(vecA))*(s-length(vecB))*(s-length(vecC)));
			
	// vec3 sum = gl_PositionIn[0].xyz + gl_PositionIn[1].xyz + gl_PositionIn[2].xyz;
	// vec3 baryCentrum = sum / 3.0;
		
	gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[0];
	norm = finalNormal;
	vert = v[0];
	vertexColor = vC[0];
	//baryCoords = vec3(1.0,0.0,0.0);
	//fragmentDepth = gl_Position.z;
	EmitVertex();

	gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[1];
	norm = finalNormal;
	vert = v[1];
	vertexColor = vC[1];
	//baryCoords = vec3(0.0,1.0,0.0);
	//fragmentDepth = gl_Position.z;
	EmitVertex();

	gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[2];
	norm = finalNormal;
	vert = v[2];
	vertexColor = vC[2];
	//baryCoords = vec3(0.0,0.0,1.0);
	//fragmentDepth = gl_Position.z;
	EmitVertex();

    EndPrimitive();
}