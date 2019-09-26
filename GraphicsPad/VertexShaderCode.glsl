#version 430

in layout(location=0) vec3 position;

out vec3 triColor;

uniform vec3 startOffset;
uniform vec3 theColor;

void main()
{
	gl_Position = vec4(position + startOffset, 1.0f);
	triColor = theColor;
}