// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>

// specify that we want the OpenGL core profile before including GLFW headers
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "GlyphExtractor.h"

using namespace std;

int scene = 1;
int version = 1;
int curveType = 0;
int font = 1;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string shaderPath = "shaders/";
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tessControlShader, GLuint tessEvalShader);
GLuint LinkLineProgram(GLuint vertexShader, GLuint fragmentShader);
// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

struct MyShader
{
    // OpenGL names for vertex and fragment shaders, shader program
    GLuint vertex;
    GLuint fragment;
    GLuint tesscontrol;
    GLuint tesseval;
    GLuint program;

    // initialize shader and program names to zero (OpenGL reserved value)
    MyShader() : vertex(0), fragment(0), tesscontrol(0), tesseval(0), program(0)
    {}
};

// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader)
{
    // load shader source from files
    string vertexSource = LoadSource("vertex.glsl");
    string fragmentSource = LoadSource("fragment.glsl");
    string tessellationControlSource = LoadSource("tesscontrol.glsl");
    string tessellationEvalSource = LoadSource("tesseval.glsl");
    //***load tessel/ation shaders

    if (vertexSource.empty() || fragmentSource.empty() || tessellationControlSource.empty() || tessellationEvalSource.empty() ) return false;

    // compile shader source into shader objects
    shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
    shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    shader->tesscontrol = CompileShader(GL_TESS_CONTROL_SHADER, tessellationControlSource);
    shader->tesseval = CompileShader(GL_TESS_EVALUATION_SHADER, tessellationEvalSource);
    //***compile tessellation shaders

    // link shader program
    shader->program = LinkProgram(shader->vertex, shader->fragment,shader->tesscontrol, shader->tesseval/*Link tessellation shaders*/);

    // check for OpenGL errors and return false if error occurred
    return !CheckGLErrors();
}

// deallocate shader-related objects
void DestroyShaders(MyShader *shader)
{
    // unbind any shader programs and destroy shader objects
    glUseProgram(0);
    glDeleteProgram(shader->program);
    glDeleteShader(shader->vertex);
    glDeleteShader(shader->fragment);
    glDeleteShader(shader->tesscontrol);
    glDeleteShader(shader->tesseval);
}

bool InitializeLineShaders(MyShader *shader)
{
    // load shader source from files
    string vertexSource = LoadSource("vertex2.glsl");
    string fragmentSource = LoadSource("fragment2.glsl");
    if (vertexSource.empty() || fragmentSource.empty()) return false;

    // compile shader source into shader objects
    shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
    shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // link shader program
    shader->program = LinkLineProgram(shader->vertex, shader->fragment);

    // check for OpenGL errors and return false if error occurred
    return !CheckGLErrors();
}

// deallocate shader-related objects
void DestroyLineShaders(MyShader *shader)
{
    // unbind any shader programs and destroy shader objects
    glUseProgram(0);
    glDeleteProgram(shader->program);
    glDeleteShader(shader->vertex);
    glDeleteShader(shader->fragment);
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct MyGeometry
{
    // OpenGL names for array buffer objects, vertex array object
    GLuint  vertexBuffer;
    GLuint  colourBuffer;
    GLuint  vertexArray;
    GLsizei elementCount;

    // initialize object names to zero (OpenGL reserved value)
    MyGeometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
    {}
};

// create buffers and fill with geometry data, returning true if successful
bool InitializeGeometry(MyGeometry *geometry)
{
    // three vertex positions and assocated colours of a triangle 
		
	GLfloat vertices[][2] = {
		//quadratic
		{ 1.0/2.5,  1.0/2.5},
		{ 2.0/2.5, -1.0/2.5},
		{ 0.0/2.5, -1.0/2.5},
		{ 0, 0 },//d
		{ 0.0/2.5, -1.0/2.5},
		{ -2.0/2.5,-1.0/2.5},
		{ -1.0/2.5, 1.0/2.5},
		{ 0, 0 },//d
		{ -1.0/2.5, 1.0/2.5},
		{ 0.0/2.5, 1.0/2.5 },
		{ 1.0/2.5, 1.0/2.5 },
		{ 0, 0 },//d
		{ 1.2/2.5, 0.5/2.5 },
		{ 2.5/2.5, 1.0/2.5},
		{ 1.3/2.5, -0.4/2.5},
		{ 0, 0 },//d
		//cubic
		{ 1.0/9.0, 1.0/9.0 },
		{ 4.0/9.0, 0.0/9.0 },
		{ 6.0/9.0, 2.0/9.0 },
		{ 9.0/9.0, 1.0/9.0 },
		{ 8.0/9.0, 2.0/9.0 },
		{ 0.0/9.0, 8.0/9.0 },
		{ 0.0/9.0,-2.0/9.0 },
		{ 8.0/9.0, 4.0/9.0 },
		{ 5.0/9.0, 3.0/9.0 },
		{ 3.0/9.0, 2.0/9.0 },
		{ 3.0/9.0, 3.0/9.0 },
		{ 5.0/9.0, 2.0/9.0 },
		{ 3.0/9.0, 2.2/9.0 },
		{ 3.5/9.0, 2.7/9.0 },
		{ 3.5/9.0, 3.3/9.0 },
		{ 3.0/9.0, 3.8/9.0 },
		{ 2.8/9.0, 3.5/9.0 },
		{ 2.4/9.0, 3.8/9.0 },
		{ 2.4/9.0, 3.2/9.0 },
		{ 2.8/9.0, 3.5/9.0 }
		};    

    GLfloat curveColours[][3] = {
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },        
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },        
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },        
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },             
        { 1.0, 0.0, 0.0 },             
        { 1.0, 0.0, 0.0 }        
    };

    geometry->elementCount = 20;

    // these vertex attribute indices correspond to those specified for the
    // input variables in the vertex shader
    const GLuint VERTEX_INDEX = 0;
    const GLuint COLOUR_INDEX = 1;

    // create an array buffer object for storing our vertices
    glGenBuffers(1, &geometry->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create another one for storing our colours
    glGenBuffers(1, &geometry->colourBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(curveColours), curveColours, GL_STATIC_DRAW);
    
    // create a vertex array object encapsulating all our vertex attributes
    glGenVertexArrays(1, &geometry->vertexArray);
    glBindVertexArray(geometry->vertexArray);

    // associate the position array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VERTEX_INDEX);

    // assocaite the colour array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(COLOUR_INDEX);

    // unbind our buffers, resetting to default state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

    // check for OpenGL errors and return false if error occurred
    return !CheckGLErrors();
}

bool InitializeGlyphGeometry(MyGeometry *geometry, vector<MyGlyph> &fNameGlyphs)
{					
	int u = 0; //current vertex index
	float advance = 0;
	
	GLfloat vertices[512][2] = {
		{ 1.0/2.5,  1.0/2.5},
		{ 2.0/2.5, -1.0/2.5},
		{ 0.0/2.5, -1.0/2.5},
		{ 0, 0 }//d 
		};   
		
    GLfloat curveColours[512][3] = {
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },             
        { 1.0, 0.0, 0.0 },             
        { 1.0, 0.0, 0.0 } 
		};    
	
	//Get ith Glyph
	for(uint i = 0; i < fNameGlyphs.size(); i++)
	{

		//Get jth Contour
		for(uint j = 0; j < fNameGlyphs[i].contours.size(); j++)
		{
					
			//Get kth Segment from jth contour
			for(uint k = 0; k < fNameGlyphs[i].contours[j].size(); k++)
			{	
				int segDegree = fNameGlyphs[i].contours[j][k].degree;
				
				if(segDegree == 0)
				{
					vertices[u][0] = fNameGlyphs[i].contours[j][k].x[0]+advance;
					vertices[u][1] = fNameGlyphs[i].contours[j][k].y[0];
					vertices[u+1][0] = 0;
					vertices[u+1][1] = 0;
					vertices[u+2][0] = 0;	//pad
					vertices[u+2][1] = 0;
					vertices[u+3][0] = 0;
					vertices[u+3][1] = 0;
				}
				
				if(segDegree == 1)
				{
					vertices[u][0] = fNameGlyphs[i].contours[j][k].x[0]+advance;
					vertices[u][1] = fNameGlyphs[i].contours[j][k].y[0];
					vertices[u+1][0] = fNameGlyphs[i].contours[j][k].x[1]+advance;
					vertices[u+1][1] = fNameGlyphs[i].contours[j][k].y[1];
					vertices[u+2][0] = 0;
					vertices[u+2][1] = 0;	//pad
					vertices[u+3][0] = 0;
					vertices[u+3][1] = 0;
					//cout << "test1" << endl;
					
				}
				
				if(segDegree == 2)
				{
					vertices[u][0] = fNameGlyphs[i].contours[j][k].x[0]+advance;
					vertices[u][1] = fNameGlyphs[i].contours[j][k].y[0];
					vertices[u+1][0] = fNameGlyphs[i].contours[j][k].x[1]+advance;
					vertices[u+1][1] = fNameGlyphs[i].contours[j][k].y[1];
					vertices[u+2][0] = fNameGlyphs[i].contours[j][k].x[2]+advance;
					vertices[u+2][1] = fNameGlyphs[i].contours[j][k].y[2];
					vertices[u+3][0] = 0;
					vertices[u+3][1] = 0;	//pad
				}
				
				if(segDegree == 3)
				{
					vertices[u][0] = fNameGlyphs[i].contours[j][k].x[0]+advance;
					vertices[u][1] = fNameGlyphs[i].contours[j][k].y[0];
					vertices[u+1][0] = fNameGlyphs[i].contours[j][k].x[1]+advance;
					vertices[u+1][1] = fNameGlyphs[i].contours[j][k].y[1];
					vertices[u+2][0] = fNameGlyphs[i].contours[j][k].x[2]+advance;
					vertices[u+2][1] = fNameGlyphs[i].contours[j][k].y[2];
					vertices[u+3][0] = fNameGlyphs[i].contours[j][k].x[3]+advance;
					vertices[u+3][1] = fNameGlyphs[i].contours[j][k].y[3];
				}
				u = u + 4;
				
			}
			
		}
		advance += fNameGlyphs[i].advance;
	}
	
	for(int i = 0; i < u; i++)
	{
		curveColours[i][0] = 1;
		curveColours[i][1] = 0;
		curveColours[i][2] = 0;
	}
	/*
	MyGlyph B = ge->ExtractGlyph('B');
		
	cout << B.contours[0][0].degree << endl; // degree of segment
	cout << B.contours.size() << endl;		// number of contours
	
	cout << B.contours[0].size() << endl;	// number of segments in first contour
	
	cout << B.contours[0][0].x[0] << endl; // x position of point 0
	cout << B.contours[0][0].y[0] << endl; // y position of point 0
	cout << B.contours[0][0].x[1] << endl; // x position of point 1
	cout << B.contours[0][0].y[1] << endl; // y position of point 1
	
	*/

	//vertices[i][0] = xpoint;
	//vertices[i][1] = ypoint;


    geometry->elementCount = 20;

    // these vertex attribute indices correspond to those specified for the
    // input variables in the vertex shader
    const GLuint VERTEX_INDEX = 0;
    const GLuint COLOUR_INDEX = 1;

    // create an array buffer object for storing our vertices
    glGenBuffers(1, &geometry->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create another one for storing our colours
    glGenBuffers(1, &geometry->colourBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(curveColours), curveColours, GL_STATIC_DRAW);
    
    // create a vertex array object encapsulating all our vertex attributes
    glGenVertexArrays(1, &geometry->vertexArray);
    glBindVertexArray(geometry->vertexArray);

    // associate the position array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VERTEX_INDEX);

    // assocaite the colour array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(COLOUR_INDEX);

    // unbind our buffers, resetting to default state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

    // check for OpenGL errors and return false if error occurred
    return !CheckGLErrors();
}
// deallocate geometry-related objects
void DestroyGeometry(MyGeometry *geometry)
{
    // unbind and destroy our vertex array object and associated buffers
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &geometry->vertexArray);
    glDeleteBuffers(1, &geometry->vertexBuffer);
    glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderGlyphs(MyGeometry *geometry, MyShader *shader, vector<int> *degrees)
{
	glUseProgram(shader->program);
    
    int sceLoc = glGetUniformLocation(shader->program, "scene");    
    int curLoc = glGetUniformLocation(shader->program, "curveType");    
    glUniform1i(sceLoc, scene);
    
    glBindVertexArray(geometry->vertexArray);
	
	if(scene == 3)
	{
		for(uint i = 0; i < degrees->size(); i++)
		{
			glUniform1i(curLoc, degrees->at(i));
			glDrawArrays(GL_PATCHES, i*4, 4);
		}
	}
		

    // reset state to default (no shader or geometry bound)
    glBindVertexArray(0);
    glUseProgram(0);

    // check for an report any OpenGL errors
    CheckGLErrors();
}
void RenderGlyphLine(MyGeometry *geometry, MyShader *shader, vector<int> *degrees)
{
	 // clear screen to a dark grey colour
    
    // bind our shader program and the vertex array object containing our
    // scene geometry, then tell OpenGL to draw our geometry
    glUseProgram(shader->program);
    glBindVertexArray(geometry->vertexArray);
	
	int sceLoc = glGetUniformLocation(shader->program, "scene");           
	int colLoc = glGetUniformLocation(shader->program, "colorType");           
    glUniform1i(sceLoc, scene);
	
	if((version == 2) && (scene == 3))
	{
		//tangent lines		
		for(int i = 0; i <  (4 * degrees->size()); i++)
		{
			glUniform1f(colLoc, 0.7);
			if((i % 4) == 0) 
				glDrawArrays(GL_LINES, i, 3);
		}
		
		//off line control points
		glPointSize(4);
		for(int i = 0; i < (4 * degrees->size()); i++)
		{
			glUniform1f(colLoc, 0.7);
			if((i % 4) == 1)
				glDrawArrays(GL_POINTS, i, 1);
		}
		
		//on line control points
		glPointSize(4);
		for(int i = 0; i < (4 * degrees->size()); i++)
		{
			glUniform1f(colLoc, 0.0);
			if(((i % 4) == 0) || ((i % 4) == 2))
				glDrawArrays(GL_POINTS, i, 1);
		}
	}	
	
    // reset state to default (no shader or geometry bound)
    glBindVertexArray(0);
    glUseProgram(0);

    // check for an report any OpenGL errors
    CheckGLErrors();
}
void RenderScene(MyGeometry *geometry, MyShader *shader)
{
    // bind our shader program and the vertex array object containing our
    // scene geometry, then tell OpenGL to draw our geometry
    glUseProgram(shader->program);
    
    int sceLoc = glGetUniformLocation(shader->program, "scene");    
    glUniform1i(sceLoc, scene);

    glBindVertexArray(geometry->vertexArray);
    if(scene == 1)
		glDrawArrays(GL_PATCHES, 0, 16);
	if(scene == 2)
		glDrawArrays(GL_PATCHES, 16, 20);
	if(scene == 3)
		glDrawArrays(GL_PATCHES, 0, 3);

    // reset state to default (no shader or geometry bound)
    glBindVertexArray(0);
    glUseProgram(0);

    // check for an report any OpenGL errors
    CheckGLErrors();
}

void RenderLineScene(MyGeometry *geometry, MyShader *shader)
{
	 // clear screen to a dark grey colour
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // bind our shader program and the vertex array object containing our
    // scene geometry, then tell OpenGL to draw our geometry
    glUseProgram(shader->program);
    glBindVertexArray(geometry->vertexArray);
	
	int sceLoc = glGetUniformLocation(shader->program, "scene");           
	int colLoc = glGetUniformLocation(shader->program, "colorType");           
    glUniform1i(sceLoc, scene);
	
	if((version == 2) && (scene == 1))
	{
		//tangent lines		
		for(int i = 0; i <  16; i++)
		{
			glUniform1f(colLoc, 0.7);
			if((i % 4) == 0) 
				glDrawArrays(GL_LINE_STRIP, i, 3);
		}
		
		//off line control points
		glPointSize(4);
		for(int i = 0; i < 16; i++)
		{
			glUniform1f(colLoc, 0.7);
			if((i % 4) == 1)
				glDrawArrays(GL_POINTS, i, 1);
		}
		
		//on line control points
		glPointSize(4);
		for(int i = 0; i < 16; i++)
		{
			glUniform1f(colLoc, 0.0);
			if(((i % 4) == 0) || ((i % 4) == 2))
				glDrawArrays(GL_POINTS, i, 1);
		}
	}
	
	if((version == 2) && (scene == 2))
	{
		//tangent lines
		for(int i = 0; i <  20; i++)
		{
			glUniform1f(colLoc, 0.7);
			if((i % 4) == 0) 
				glDrawArrays(GL_LINE_STRIP, i+16, 4);
		}
		
		//off line control points
		glPointSize(4);
		for(int i = 0; i < 20; i++)
		{
			glUniform1f(colLoc, 0.7);
			if(((i % 4) == 1) || ((i % 4) == 2))
				glDrawArrays(GL_POINTS, i+16, 1);
		}
		
		//on line control points
		glPointSize(4);
		for(int i = 0; i < 20; i++)
		{
			glUniform1f(colLoc, 0.0);
			if(((i % 4) == 0) || ((i % 4) == 3))
				glDrawArrays(GL_POINTS, i+16, 1);
		}
	}
	
	
    // reset state to default (no shader or geometry bound)
    glBindVertexArray(0);
    glUseProgram(0);

    // check for an report any OpenGL errors
    CheckGLErrors();
}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		scene = 1;		
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		scene = 2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		scene = 3;		
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		version = 1;		
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		version = 2;	
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		version = 3;	
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		font = 1;	
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		font = 2;	
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		font = 3;	
}

// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
    // initialize the GLFW windowing system
    if (!glfwInit()) {
        cout << "ERROR: GLFW failed to initilize, TERMINATING" << endl;
        return -1;
    }
    glfwSetErrorCallback(ErrorCallback);

    // attempt to create a window with an OpenGL 4.1 core profile context
    GLFWwindow *window = 0;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES,4);
    window = glfwCreateWindow(512, 512, "CPSC 453 OpenGL Boilerplate", 0, 0);
    if (!window) {
        cout << "Program failed to create GLFW window, TERMINATING" << endl;
        glfwTerminate();
        return -1;
    }

    // set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, KeyCallback);
    glfwMakeContextCurrent(window);

    //Load Inconsolata font and print some info
  	GlyphExtractor* ge = new GlyphExtractor();
    if(!ge->LoadFontFile("Lora-Regular.ttf"))
      cout << "font Inconsolata.otf loaded" << endl;
      
   	GlyphExtractor* ge2 = new GlyphExtractor();
    if(!ge->LoadFontFile("SourceSansPro-Regular.otf"))
      cout << "font Inconsolata.otf loaded" << endl;
      
  	GlyphExtractor* ge3 = new GlyphExtractor();
    if(!ge->LoadFontFile("Inconsolata.otf"))
      cout << "font Inconsolata.otf loaded" << endl;

    // query and print out information about our OpenGL environment
    QueryGLVersion();

    // call function to load and compile shader programs
    MyShader shader;
    if (!InitializeShaders(&shader)) {
        cout << "Program could not initialize shaders, TERMINATING" << endl;
        return -1;
    }
    MyShader lineShader;
    if (!InitializeLineShaders(&lineShader)) {
        cout << "Program could not initialize shaders, TERMINATING" << endl;
        return -1;
    }
	
	vector<MyGlyph> fNameGlyphs;
	vector<MyGlyph> fNameGlyphs2;
	vector<MyGlyph> fNameGlyphs3;
	vector<int> degrees;
	vector<int> degrees2;
	vector<int> degrees3;

	string fName = "Petras";
	
	//First name array init
	for(uint i = 0; i < fName.size(); i++)
	{
		MyGlyph newGlyph = ge->ExtractGlyph(fName[i]);
		fNameGlyphs.push_back(newGlyph);
		MyGlyph newGlyph2 = ge2->ExtractGlyph(fName[i]);
		fNameGlyphs2.push_back(newGlyph2);
		MyGlyph newGlyph3 = ge3->ExtractGlyph(fName[i]);
		fNameGlyphs3.push_back(newGlyph3);	
	}
	
	//find each degree of each seg from each contour from each glyph
	for(uint i = 0; i < fNameGlyphs.size(); i++)
	{
		for(uint j = 0; j < fNameGlyphs[i].contours.size(); j++)
		{
			for(uint k = 0; k < fNameGlyphs[i].contours[j].size(); k++)
			{	
				degrees.push_back(fNameGlyphs[i].contours[j][k].degree);
			}
		}
	}
	
	for(uint i = 0; i < fNameGlyphs2.size(); i++)
	{
		for(uint j = 0; j < fNameGlyphs2[i].contours.size(); j++)
		{
			for(uint k = 0; k < fNameGlyphs2[i].contours[j].size(); k++)
			{	
				degrees2.push_back(fNameGlyphs2[i].contours[j][k].degree);
			}
		}
	}
	
	for(uint i = 0; i < fNameGlyphs3.size(); i++)
	{
		for(uint j = 0; j < fNameGlyphs3[i].contours.size(); j++)
		{
			for(uint k = 0; k < fNameGlyphs3[i].contours[j].size(); k++)
			{	
				degrees3.push_back(fNameGlyphs3[i].contours[j][k].degree);
			}
		}
	}

    // call function to create and fill buffers with geometry data
    MyGeometry geometry, glyphGeometry;
    if (!InitializeGeometry(&geometry))
        cout << "Program failed to intialize geometry!" << endl;
       
    if (!InitializeGlyphGeometry(&glyphGeometry, fNameGlyphs))
        cout << "Program failed to intialize geometry!" << endl;

    // run an event-triggered main loop
    while (!glfwWindowShouldClose(window))
    {        
        
        RenderLineScene(&geometry, &lineShader);
        
        RenderScene(&geometry, &shader);
        
        if(font == 1)
        {
			if (!InitializeGlyphGeometry(&glyphGeometry, fNameGlyphs))
				cout << "Program failed to intialize geometry!" << endl;
				
			RenderGlyphs(&glyphGeometry, &shader, &degrees);
        
			RenderGlyphLine(&glyphGeometry, &lineShader, &degrees);
		}
        if(font == 2)
        {
			if (!InitializeGlyphGeometry(&glyphGeometry, fNameGlyphs2))
				cout << "Program failed to intialize geometry!" << endl;
				
			RenderGlyphs(&glyphGeometry, &shader, &degrees2);
        
			RenderGlyphLine(&glyphGeometry, &lineShader, &degrees2);
		}
        if(font == 3)
        {
			if (!InitializeGlyphGeometry(&glyphGeometry, fNameGlyphs3))
				cout << "Program failed to intialize geometry!" << endl;
				
			RenderGlyphs(&glyphGeometry, &shader, &degrees3);
        
			RenderGlyphLine(&glyphGeometry, &lineShader, &degrees3);
		}
		       
        RenderGlyphs(&glyphGeometry, &shader, &degrees);
        
        RenderGlyphLine(&glyphGeometry, &lineShader, &degrees);
        
        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(window);

        // sleep until next event before drawing again
        glfwWaitEvents();
        //pullevents
        //glfwGetTime(); //grabs clock
    }

    // clean up allocated resources before exit
    DestroyGeometry(&geometry);
    DestroyGeometry(&glyphGeometry);
    DestroyShaders(&shader);
    DestroyLineShaders(&lineShader);
    glfwDestroyWindow(window);
    glfwTerminate();

    cout << "Goodbye!" << endl;
    return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
    // query opengl version and renderer information
    string version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    string glslver  = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    cout << "OpenGL [ " << version << " ] "
         << "with GLSL [ " << glslver << " ] "
         << "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
    bool error = false;
    for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
    {
        cout << "OpenGL ERROR:  ";
        switch (flag) {
        case GL_INVALID_ENUM:
            cout << "GL_INVALID_ENUM" << endl; break;
        case GL_INVALID_VALUE:
            cout << "GL_INVALID_VALUE" << endl; break;
        case GL_INVALID_OPERATION:
            cout << "GL_INVALID_OPERATION" << endl; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
        case GL_OUT_OF_MEMORY:
            cout << "GL_OUT_OF_MEMORY" << endl; break;
        default:
            cout << "[unknown error code]" << endl;
        }
        error = true;
    }
    return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
    string source;

    ifstream input(filename);
    if (input) {
        copy(istreambuf_iterator<char>(input),
             istreambuf_iterator<char>(),
             back_inserter(source));
        input.close();
    }
    else {
        cout << "ERROR: Could not load shader source from file "
             << filename << endl;
    }

    return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
    // allocate shader object name
    GLuint shaderObject = glCreateShader(shaderType);

    // try compiling the source as a shader of the given type
    const GLchar *source_ptr = source.c_str();
    glShaderSource(shaderObject, 1, &source_ptr, 0);
    glCompileShader(shaderObject);

    // retrieve compile status
    GLint status;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
        cout << "ERROR compiling shader:" << endl << endl;
        cout << source << endl;
        cout << info << endl;
    }

    return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tessControlShader, GLuint tessEvalShader)
{
    // allocate program object name
    GLuint programObject = glCreateProgram();

    // attach provided shader objects to this program
    if (vertexShader)   glAttachShader(programObject, vertexShader);
    if (fragmentShader) glAttachShader(programObject, fragmentShader);
    if (tessControlShader) glAttachShader(programObject, tessControlShader);
    if (tessEvalShader) glAttachShader(programObject, tessEvalShader);
    //***attach tessellation shaders

    // try linking the program with given attachments
    glLinkProgram(programObject);

    // retrieve link status
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        cout << "ERROR linking shader program:" << endl;
        cout << info << endl;
    }

    return programObject;
}

GLuint LinkLineProgram(GLuint vertexShader, GLuint fragmentShader)
{
    // allocate program object name
    GLuint programObject = glCreateProgram();

    // attach provided shader objects to this program
    if (vertexShader)   glAttachShader(programObject, vertexShader);
    if (fragmentShader) glAttachShader(programObject, fragmentShader);

    // try linking the program with given attachments
    glLinkProgram(programObject);

    // retrieve link status
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        cout << "ERROR linking shader program:" << endl;
        cout << info << endl;
    }

    return programObject;
}

// ==========================================================================
