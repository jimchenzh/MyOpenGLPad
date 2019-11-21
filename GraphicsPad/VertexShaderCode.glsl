#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float ambientLight;

uniform mat4 worldSpaceMatrix;
uniform mat4 fullTransformMatrix;

out vec3 theColor;

void main()
{
	vec4 v = vec4(position, 1.0);
	gl_Position = fullTransformMatrix * v;

	//Get light vector
	vec4 worldSpaceVertex = v * worldSpaceMatrix;
	vec3 worldSpaceVertexPos = vec3(worldSpaceVertex.x, worldSpaceVertex.y, worldSpaceVertex.z);
	vec3 lightVector = normalize(lightPosition - worldSpaceVertexPos);

	//Specular Light
	vec3 reflected = reflect(-1 * lightVector, normal);
	vec3 camera = normalize(cameraPosition - position);
	float a = clamp(dot(reflected, camera), 0, 1);
	a = pow(a, 60);
	float specularLight = a;

	//Diffuse Light
	float diffuseLight = dot(normal, -lightVector);

	float totalLight = ambientLight + diffuseLight + specularLight;
	theColor = vertexColor * totalLight;
}