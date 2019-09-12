const char* vertexShaderCode =
	"#version 430\r\n"
	""
	"in layout(location=0) vec2 position;"
	"out vec3 triColor;"
	"uniform vec2 startOffset;"	
	"uniform vec3 theColor;"
	"void main()"
	"{"
	"	gl_Position = vec4(position + startOffset, 0.0f, 1.0f);"
	"	triColor = theColor;"
	"}";

const char* fragmentShaderCode =
	"#version 430\r\n"
	""
	"out vec4 daColor;"
	"in vec3 triColor;"	
	"void main()"
	"{"
	"	daColor = vec4(triColor, 1.0f);"
	"}";