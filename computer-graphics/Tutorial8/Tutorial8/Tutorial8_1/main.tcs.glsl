#version 410 core

layout(vertices = 4) out;

uniform float uOuter0, uOuter1;

void main(){
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	// set tessellation levels
	gl_TessLevelOuter[0] = uOuter0;
	gl_TessLevelOuter[1] = uOuter1;
}