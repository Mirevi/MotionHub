#extension GL_EXT_gpu_shader4 : require
varying vec3 N;
varying vec3 v;
varying vec4 vC;

void main(void)
{
	vC = gl_Color;
	v = vec3(gl_ModelViewMatrix * gl_Vertex);       
	N = normalize(gl_NormalMatrix * gl_Normal);
	gl_Position = gl_Vertex;
}