#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform mat4 worldSpaceMatrix;
uniform mat4 fullTransformMatrix;

out vec3 PositionToFrag;
out vec3 lightVector;
out vec3 camera;
out vec3 worldSpaceNormalPos;

void main()
{
	PositionToFrag = position;
	
	vec4 v = vec4(position, 1.0);
	gl_Position = fullTransformMatrix * v;

	//Get light vector
	vec4 worldSpaceVertex = worldSpaceMatrix * v;
	vec3 worldSpaceVertexPos = vec3(worldSpaceVertex);
	lightVector = normalize(lightPosition - worldSpaceVertexPos);

	//Get camera vector
	vec4 n = vec4(normal, 0);
	vec4 worldSpaceNormal = worldSpaceMatrix * n;
	worldSpaceNormalPos = vec3(worldSpaceNormal);	
	camera = normalize(cameraPosition - worldSpaceVertexPos);	
}