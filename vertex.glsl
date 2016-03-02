// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec3 VertexColour;

// output to be interpolated between vertices and passed to the fragment stage
out vec3 ve_color;

uniform int scene;

void main()
{
	mat4 scaMatrix = mat4(0.9,0,0,0,
					0, 0.9,0,0,
					0,0,1,0,
					0,0,0,1);
									
	mat4 traMatrix = mat4(1,0,0,0,
					  0,1,0,0,
					  0,0,1,0,
					  0,0,0,1);
					  
	if(scene == 2)
	{
		scaMatrix = mat4(1.4,0,0,0,
					0, 1.4,0,0,
					0,0,1,0,
					0,0,0,1);
		
		traMatrix = mat4(1,0,0,0,
					  0,1,0,0,
					  0,0,1,0,
					  -0.7,-0.3,0,1);	
	}		

	
    // assign vertex position without modification
    gl_Position = traMatrix * scaMatrix * vec4(VertexPosition, 0.0, 1.0);

    // assign output colour to be interpolated
    ve_color = VertexColour;
}
