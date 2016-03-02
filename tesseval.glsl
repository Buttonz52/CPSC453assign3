#version 410

layout(isolines, equal_spacing) in;

in vec3 te_color[];

out vec3 color;

vec4 Bezier(vec4 a)
	{
		return a;
	}

vec4 squareBezier(vec4 a, vec4 b, float u)
	{
		return mix(a,b,u);
	}
	
vec4 quadracticBezier(vec4 a, vec4 b, vec4 c, float u)
	{
		return mix(squareBezier(a,b,u), squareBezier(b,c,u),u);
	}
	
vec4 cubicBezier(vec4 a, vec4 b, vec4 c, vec4 d, float u)
	{
		return mix(quadracticBezier(a,b,c,u), quadracticBezier(b,c,d,u),u);
	}
	
	
uniform int scene;
uniform int curveType;

void main()
{
	vec4 ans;
	vec4 p0;
	vec4 p1;
	vec4 p2;
	vec4 p3;
	float b0;
	float b1;
	float b2;
	float b3;
	
	float u = gl_TessCoord.x;
	
	if(scene == 1)
	{
		p0 = gl_in[0].gl_Position; //maybe vec2's
		p1 = gl_in[1].gl_Position;
		p2 = gl_in[2].gl_Position;
		
		ans = quadracticBezier(p0,p1,p2,u);
	}
	
	if(scene == 3)
	{	
		if(curveType == 0)
		{
			p0 = gl_in[0].gl_Position;		
			ans = Bezier(p0);
		}
		
		if(curveType == 1)
		{
			p0 = gl_in[0].gl_Position;		
			p1 = gl_in[1].gl_Position;	
			ans = squareBezier(p0,p1,u);
		}
		
		if(curveType == 2)
		{
			p0 = gl_in[0].gl_Position;		
			p1 = gl_in[1].gl_Position;
			p2 = gl_in[2].gl_Position;
			ans = quadracticBezier(p0,p1,p2,u);
		}
		
		if(curveType == 3)
		{
			p0 = gl_in[0].gl_Position;		
			p1 = gl_in[1].gl_Position;
			p2 = gl_in[2].gl_Position;
			p3 = gl_in[3].gl_Position;
			ans = cubicBezier(p0,p1,p2,p3,u);
		}

		
	}
	if(scene == 2)
	{
		p0 = gl_in[0].gl_Position;		
		p1 = gl_in[1].gl_Position;
		p2 = gl_in[2].gl_Position;
		p3 = gl_in[3].gl_Position;
		ans = cubicBezier(p0,p1,p2,p3,u);
	}
	
	
	gl_Position = ans;
	
	
	color = quadracticBezier(vec4(te_color[0],0), vec4(te_color[1],0),vec4(te_color[2],0),u).rgb;
	
	
}
