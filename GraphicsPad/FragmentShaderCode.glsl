#version 430

out vec4 daColor;
in vec3 triColor;	
void main()
{
	daColor = vec4(triColor, 1.0f);
}
