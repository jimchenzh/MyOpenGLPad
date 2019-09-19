#version 430

in layout(location=0) vec2 position;

out vec3 triColor;

uniform vec2 startOffset;
uniform vec3 theColor;

void main()
{
	gl_Position = vec4(position + startOffset, 0.0f, 1.0f);
	triColor = theColor;
}