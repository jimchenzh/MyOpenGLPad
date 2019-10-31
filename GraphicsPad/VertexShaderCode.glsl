#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

uniform mat4 modelTransformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 fullTransformMatrix;

out vec3 theColor;

void main()
{
	vec4 v = vec4(position, 1.0f);
	
	gl_Position = fullTransformMatrix * v;

	theColor = vertexColor;
	//theColor = vec3(1.0f, 0.0f, 0.0f);
}