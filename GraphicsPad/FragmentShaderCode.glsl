#version 430

out vec4 daColor;
in vec3 PositionToFrag;
in vec3 lightVector;
in vec3 camera;
in vec3 worldSpaceNormalPos;

uniform sampler2D JTexture;
uniform float ambient;


vec2 TexCoord;

void main()
{
	//Calculate texture coord
	if(PositionToFrag.y < 0)
		TexCoord = vec2(PositionToFrag.x/2 + 0.5f, PositionToFrag.z - 1.0f);
	else
		TexCoord = vec2(-PositionToFrag.x/2 +0.5f, PositionToFrag.z - 1.0f);
		
	//Normalize
	vec3 nlightVector = normalize(lightVector);
	vec3 ncamera = normalize(camera);
	vec3 nworldSpaceNormalPos = normalize(worldSpaceNormalPos);

	//Specular Reflectivity
	vec3 reflected = reflect(-1 * nlightVector, nworldSpaceNormalPos);
	float a = clamp(dot(reflected, ncamera), 0, 1);
	a = pow(a, 10);
	float specular = a;

	//Diffuse Reflectivity
	float diffuse = dot(nworldSpaceNormalPos, nlightVector) * 0.8f;


	daColor = (diffuse + ambient) * texture(JTexture, TexCoord) + vec4(specular * vec3(1.0f, 1.0f, 1.0f), 1.0f);
	//daColor = vec4(PositionToFrag, 1.0f);
}
