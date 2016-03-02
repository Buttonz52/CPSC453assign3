#version 410

layout (vertices = 4) out;

in vec3 ve_color[];

out vec3 te_color[];

void main()
{	
	gl_TessLevelOuter[0] = 1;
	gl_TessLevelOuter[1] = 32;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	te_color[gl_InvocationID] = ve_color[gl_InvocationID];
}
