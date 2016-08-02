#version 420 core

layout(location = 0) in vec4 terrainCoords;
layout(location = 1) in vec3 terrainNormals;
layout(location = 2) in vec2 terrainTexCoords;

layout(location = 3) in vec4 squareCoords;
layout(location = 4) in vec4 squareColors;



uniform mat3 normalMat;
uniform int ObjectTree;
uniform mat4 Translation;

struct Light
{
	vec4 ambCols;
	vec4 difCols;
	vec4 specCols;
	vec4 coords;
};
uniform Light light0;

vec3 normal;
vec3 lightDirection;

uniform mat4 projMat;
uniform mat4 modelViewMat;
uniform vec4 globAmb;

out vec2 texCoordsExport;

smooth out vec4 colorsExport;

struct Material{
	vec4 ambRefl;
	vec4 difRefl;
	vec4 specRefl;
	vec4 emitCols;
	float shininess;
};
uniform Material terrainFandB;

void main(void)
{
	if (ObjectTree == 1){
	normal = normalize(normalMat * terrainNormals);
	lightDirection = normalize(vec3(light0.coords));
	colorsExport = (globAmb*5) * terrainFandB.ambRefl * max(dot(normal, lightDirection), 0.0f) * (light0.difCols * terrainFandB.difRefl);
	gl_Position = projMat * modelViewMat * terrainCoords;
	texCoordsExport = terrainTexCoords;
	}
	else {
		gl_Position = projMat * modelViewMat *Translation * squareCoords;
		colorsExport = squareColors;
	}
	// colorsExport = globAmb * terrainFandB.ambRefl;
   //colorsExport = terrainFandB.ambRefl;
}