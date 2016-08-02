#version 420 core

in vec2 texCoordsExport;

smooth in vec4 colorsExport;
uniform sampler2D grassTex;
uniform int ObjectTree;
out vec4 colorsOut;

void main(void)
{
	if (ObjectTree == 1){
		colorsOut = texture(grassTex, texCoordsExport) * colorsExport;
		//colorsOut = vec4(240.0f / 256.0f, 74.0f / 256.0f, 207.0f / 256.0f, 1.0f)*colorsExport;
	}
	else {
		colorsOut = colorsExport;
	}
}