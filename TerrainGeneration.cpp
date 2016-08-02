#include <iostream>
#include <fstream>
#include <vector>
#include "getbmp.h"
#include "Tree.h"

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#pragma comment(lib, "glew32.lib") 
int RandList[10];
int RandList1[10];

using namespace std;

using namespace glm;

// Size of the terrain

float cameraTheta = 0;
float cameraPhi = 0;
vec3 cameraPosition;
vec3 losUnitVector = vec3(0.0,0.0,1.0);
vec3 upDirection = vec3(0.0, 1.0, 0.0);

mat4 Translation;

static BitMapFile *image[1];

const int MAP_SIZE = 33;

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;

float terrain[MAP_SIZE][MAP_SIZE] = {};
vec3 Normal1[MAP_SIZE][MAP_SIZE];
vec3 Normal2[MAP_SIZE][MAP_SIZE];

float High = 0.5;
float Low = -0.5;

struct Vertex
{
	float coords[4];
	vec3 normals;
	vec2 texcoords;
};

static mat3 normalMat = mat3(1.0);
int ObjectTree = 0;

struct Matrix4x4
{
	float entries[16];
};

static mat4 projMat = mat4(1.0);

static const Matrix4x4 IDENTITY_MATRIX4x4 =
{
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	}
};

struct Light{
	vec4 ambCols;
	vec4 difCols;
	vec4 specCols;
	vec4 coords;
};

static const Light light0 =
{
	vec4(0.0, 0.0, 0.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 0.0, 0.0)
};

struct Material{
	vec4 ambRefl;
	vec4 difRefl;
	vec4 specRefl;
	vec4 emitCols;
	float shininess;
};

static const Material terrainFandB =
{
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 0.0, 1.0),
	50.0f
};

static const vec4 globAmb = vec4(0.2, 0.2, 0.2, 1.0);

static enum buffer { SQUARE_VERTICES, TREE_VERTICES };
static enum object { SQUARE, TREE };

// Globals
static Vertex terrainVertices[MAP_SIZE*MAP_SIZE] = {};

const int numStripsRequired = MAP_SIZE - 1;
const int verticesPerStrip = 2 * MAP_SIZE;

unsigned int terrainIndexData[numStripsRequired][verticesPerStrip];

Tree BasicTree;

static unsigned int
texture[1],
grassTexLoc,
programId,
vertexShaderId,
fragmentShaderId,
modelViewMatLoc,
projMatLoc,
normalMatLoc,
transMatLoc,
buffer[2],
vao[2];

static Vertex1 squareVertices[1000];
int indexBuffer[10000];

// Function to read text file, used to read shader files
char* readTextFile(char* aTextFile)
{
	FILE* filePointer = fopen(aTextFile, "rb");
	char* content = NULL;
	long numVal = 0;

	fseek(filePointer, 0L, SEEK_END);
	numVal = ftell(filePointer);
	fseek(filePointer, 0L, SEEK_SET);
	content = (char*)malloc((numVal + 1) * sizeof(char));
	fread(content, 1, numVal, filePointer);
	content[numVal] = '\0';
	fclose(filePointer);
	return content;
}

// Function to replace GluPerspective provided by NEHE
// http://nehe.gamedev.net/article/replacement_for_gluperspective/21002/
void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;

	fH = tan(fovY / 360 * pi) * zNear;
	fW = fH * aspect;

	projMat = frustum(-fW, fW, -fH, fH, zNear, zFar);
}
/*
void ProcedualGen(int startx, int starty,int currentNum, float L, float H){
//	cout << currentNum << endl;
	if (currentNum < 1){	
	}
	else
	{
		terrain[0][0] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
		terrain[currentNum - 1][0] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
		terrain[0][currentNum - 1] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
		terrain[currentNum - 1][currentNum - 1] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));

		float terrAvg = terrain[startx][starty] + terrain[currentNum - 1][starty] + terrain[startx][starty + currentNum - 1] + terrain[currentNum - 1][starty + currentNum - 1];
		terrAvg = (terrAvg / 4) + L + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (H - L)));

		terrain[(currentNum - 1) / 2][(currentNum - 1) / 2] = terrAvg;


		terrain[startx + (currentNum - 1) / 2][starty] = (terrain[startx + (currentNum - 1) / 2][starty + (currentNum - 1) / 2] + terrain[startx][starty] + terrain[startx + currentNum - 1][starty]) / 3;
		terrain[startx][starty + (currentNum - 1) / 2] = (terrain[(currentNum - 1) / 2][(currentNum - 1) / 2] + terrain[startx][starty] + terrain[startx][starty + currentNum - 1]) / 3;
		terrain[startx + currentNum - 1][starty + (currentNum - 1) / 2] = (terrain[startx + (currentNum - 1) / 2][starty + (currentNum - 1) / 2] + terrain[startx + currentNum - 1][starty] + terrain[startx + currentNum - 1][starty + currentNum - 1]) / 3;
		terrain[startx + (currentNum - 1) / 2][starty + currentNum - 1] = (terrain[startx + (currentNum - 1) / 2][starty + (currentNum - 1) / 2] + terrain[startx][starty + currentNum - 1] + terrain[startx + currentNum - 1][starty + currentNum - 1]) / 3;

		currentNum = currentNum / 2;

		L = L / 2;
		H = H / 2;
		/*ProcedualGen(startx/2,starty/2,currentNum, L, H);
		ProcedualGen(currentNum - 1, 0, currentNum , L, H);
		ProcedualGen(0, currentNum, currentNum , L, H);
		ProcedualGen(currentNum, currentNum, currentNum, L, H);
		ProcedualGen(startx/2, starty/2, currentNum, L, H);
		ProcedualGen(currentNum, starty/2, currentNum, L, H);
		ProcedualGen(startx/2, currentNum, currentNum, L, H);
		ProcedualGen(currentNum, currentNum, currentNum, L, H);
	}
}
*/
 //Okay here we are going to work out each corner, and then work out the diamond and square.
void ProcedualGen(int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4, float H,float L,float Checking){
	//int check = X2 - X1;
	//cout << Checking << endl;
	if (Checking < 1){
	}
	else {

		terrain[X1][Y1] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
		terrain[X2][Y2] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
		terrain[X3][Y3] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
		terrain[X4][Y4] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));


		float terrAvg = (terrain[X1][Y1] + terrain[X2][Y2] + terrain[X3][Y3] + terrain[X4][Y4]) / 4;

		terrain[Y4 / 2][Y4 / 2] = terrAvg;


		terrain[X2 / 2][Y1] = (terrain[X1][Y1] + terrain[X4 / 2][Y4 / 2] + terrain[X2][Y2]) / 3;// +(L + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (H - L))));
		terrain[X1][Y3 / 2] = (terrain[X1][Y1] + terrain[X4 / 2][Y4 / 2] + terrain[X3][Y3]) / 3;// +(L + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (H - L))));
		terrain[X2][Y4 / 2] = (terrain[X2][Y2] + terrain[X4 / 2][Y4 / 2] + terrain[X4][Y4]) / 3;// +(L + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (H - L))));
		terrain[X3][Y4 / 2] = (terrain[X3][Y3] + terrain[X4 / 2][Y4 / 2] + terrain[X4][Y4]) / 3;// +(L + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (H - L))));
		/*cout << X2/2 << ", " << Y2 << endl;
		cout << X2 << ", " << Y2 << endl;
		cout << X4/2 << ", " << Y4/2 << endl;
		cout << X2 << ", " << Y4 / 2 << endl;*/

		int midX12 = (X1 + X2) / 2;
		int midX13 = (X1 + X3) / 2;
		int midX42 = (X4 + X2) / 2;
		int midX43 = (X4 + X3) / 2;
		int midX41 = (X4 + X1) / 2;

		int midY12 = (Y1 + Y2) / 2;
		int midY13 = (Y1 + Y3) / 2;
		int midY42 = (Y4 + Y2) / 2;
		int midY43 = (Y4 + Y3) / 2;
		int midY41 = (Y4 + Y1) / 2;
		
		/*cout << midX41 << ", " << midY41 << endl;
		cout << X4 << ", " << midY42 << endl;
		cout << midX43 << ", " << Y4 << endl;
		cout << X4 << ", " << Y4 << endl;
		*/

		ProcedualGen(X1, Y1, X2 / 2, Y2, X3, Y3 / 2, X4 / 2, Y4 / 2, H / 2,L/2,Checking/2);
		ProcedualGen(midX12, Y2, X2, Y2, midX41, midY41, X2, midY42, H / 2,L/2,Checking/2);
		ProcedualGen(X1, midY13, midX41, midY41, X3, Y3, midX43, Y4, H / 2,L/2, Checking / 2);
		ProcedualGen(midX41, midY41, X4, midY42, midX43, Y4, X4, Y4, H / 2,L/2, Checking / 2);
	}
}

void shaderCompileTest(GLuint shader)
{
	GLint result = GL_FALSE;
	int logLength;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]);
	std::cout << &vertShaderError[0] << std::endl;
}


// Initialization routine.
void setup(void)
{
	// Initialise terrain - set values in the height map to 0
	for (int x = 0; x < MAP_SIZE; x++)
	{
		for (int z = 0; z < MAP_SIZE; z++)
		{
			terrain[x][z] = 0;
		}
	}

	srand(10);
	
	ProcedualGen(0, 0, MAP_SIZE-1,0,0,MAP_SIZE-1,MAP_SIZE-1,MAP_SIZE-1,High,Low,MAP_SIZE);

	for (int x = 0; x < MAP_SIZE; x++)
	{
		for (int z = 0; z < MAP_SIZE; z++)
		{
			//cout << "terrain: " << terrain[x][z] << endl;
			// Two triangles
			vec3 v1(x, terrain[x][z], z);
			vec3 v2(x, terrain[x][z+1], z + 1);
			vec3 v3(x + 1, terrain[x+1][z], z);

			vec3 e1 = v2 - v1;
			vec3 e2 = v3 - v1;

			Normal1[x][z] = glm::normalize(glm::cross(e1, e2));
			terrainVertices[(x*MAP_SIZE) + z].normals = Normal1[x][z];
			//terrain[x][z] = 0;
		}
	}
		/*
	//terrain[0][0] = rand() % 2 + -1;
	terrain[0][0] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
	terrain[MAP_SIZE - 1][0] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
	terrain[0][MAP_SIZE - 1] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
	terrain[MAP_SIZE - 1][MAP_SIZE - 1] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));

	float terrAvg = terrain[0][0] + terrain[MAP_SIZE - 1][0] + terrain[0][MAP_SIZE - 1] + terrain[MAP_SIZE - 1][MAP_SIZE - 1];
	terrAvg = (terrAvg / 4) + Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
	
	cout << terrAvg << endl;
	terrain[(MAP_SIZE - 1) /2][(MAP_SIZE - 1) /2] = terrAvg;

	terrain[(MAP_SIZE - 1) / 2][0] = (terrain[(MAP_SIZE - 1) / 2][(MAP_SIZE - 1) / 2] + terrain[0][0] + terrain[MAP_SIZE - 1][0]) / 3;
	terrain[0][(MAP_SIZE - 1) / 2] = (terrain[(MAP_SIZE - 1) / 2][(MAP_SIZE - 1) / 2] + terrain[0][0] + terrain[0][MAP_SIZE - 1]) / 3;
	terrain[MAP_SIZE - 1][(MAP_SIZE - 1) / 2] = (terrain[(MAP_SIZE - 1) / 2][(MAP_SIZE - 1) / 2] + terrain[MAP_SIZE - 1][0] + terrain[MAP_SIZE - 1][MAP_SIZE - 1]) / 3;
	terrain[(MAP_SIZE - 1) / 2][MAP_SIZE - 1] = (terrain[(MAP_SIZE - 1) / 2][(MAP_SIZE - 1) / 2] + terrain[0][MAP_SIZE - 1] + terrain[MAP_SIZE - 1][MAP_SIZE - 1]) / 3;

	ProcedualGen(0,0,MAP_SIZE / 2, Low / 2, High / 2);
	ProcedualGen((MAP_SIZE - 1) / 2, 0, MAP_SIZE / 2, Low / 2, High / 2);
	ProcedualGen(0, (MAP_SIZE - 1) / 2, MAP_SIZE / 2, Low / 2, High / 2);
	ProcedualGen((MAP_SIZE - 1) / 2, (MAP_SIZE - 1) / 2, MAP_SIZE / 2, Low / 2, High / 2);


	//terrain[MAP_SIZE - 1][(MAP_SIZE - 1) / 2] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
	//terrain[(MAP_SIZE - 1) / 2][MAP_SIZE - 1] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
	//terrain[(MAP_SIZE - 1) / 2][0] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
	//terrain[0][(MAP_SIZE - 1) / 2] = Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
	*/
	int MapCurrent = MAP_SIZE;
	


	// http://stackoverflow.com/questions/686353/c-random-float-number-generation REFRENCE



	// TODO: Add your code here to calculate the height values of the terrain using the Diamond square algorithm
	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Intialise vertex array
	int i = 0;

	float fTextureS = float(MAP_SIZE)*0.1f;
	float fTextureT = float(MAP_SIZE)*0.1f;

	for (int z = 0; z < MAP_SIZE; z++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			// Set the coords (1st 4 elements) and a default colour of black (2nd 4 elements) 
			terrainVertices[i] = { { (float)x, terrain[x][z], (float)z, 1.0 }, Normal1[x][z] };
			float fScaleC = float(x) / float(MAP_SIZE - 1);
			float fScaleR = float(z) / float(MAP_SIZE - 1);
			terrainVertices[i].texcoords = vec2(fTextureS*fScaleC, fTextureT*fScaleR);
			i++;
			//cout << "norms: " << terrainVertices[i].normals.x << ", " << terrainVertices[i].normals.y << ", " << terrainVertices[i].normals.z << ", " << endl;
		}
	}

	// Now build the index data 
	i = 0;
	for (int z = 0; z < MAP_SIZE - 1; z++)
	{
		i = z * MAP_SIZE;
		for (int x = 0; x < MAP_SIZE * 2; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
		for (int x = 1; x < MAP_SIZE * 2 + 1; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
	}

	///////////////////////////////////////////////////////////////528 Superbible
	glClearColor(153.0 / 255, 220.0 / 255, 242.0 / 255, 0.0);

	// Create shader program executable - read, compile and link shaders
	char* vertexShader = readTextFile("vertexShader.glsl");
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, (const char**)&vertexShader, NULL);
	glCompileShader(vertexShaderId);
	std::cout << "VERTEX::" << std::endl;
	shaderCompileTest(vertexShaderId);

	char* fragmentShader = readTextFile("fragmentShader.glsl");
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, (const char**)&fragmentShader, NULL);
	glCompileShader(fragmentShaderId);
	std::cout << "FRAGMENT::" << std::endl;
	shaderCompileTest(fragmentShaderId);

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glUseProgram(programId);
	///////////////////////////////////////

	// Create vertex array object (VAO) and vertex buffer object (VBO) and associate data with vertex shader.
	glGenVertexArrays(2, vao);
	glGenBuffers(2, buffer);
	glBindVertexArray(vao[SQUARE]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[SQUARE_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), (GLvoid*)sizeof(terrainVertices[0].coords));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]),	(GLvoid*)(sizeof(terrainVertices[0].coords) + sizeof(terrainVertices[0].normals)));
	glEnableVertexAttribArray(2);
	///////////////////////////////////////

	// Drawing trees
	glBindVertexArray(vao[TREE]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[TREE_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(squareVertices[0]), 0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(squareVertices[0]), (GLvoid*)sizeof(squareVertices[0].coords));
	glEnableVertexAttribArray(4);


	// Obtain projection matrix uniform location and set value.
	projMatLoc = glGetUniformLocation(programId, "projMat");
	perspectiveGL(60, (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1, 100);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));


	///////////////////////////////////////
	//////////////////////////////////////////Worksheet 10
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.ambRefl"), 1, &terrainFandB.ambRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.difRefl"), 1, &terrainFandB.difRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.specRefl"), 1, &terrainFandB.specRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.emitCols"), 1, &terrainFandB.emitCols[0]);
	glUniform1f(glGetUniformLocation(programId, "terrainFandB.shininess"), terrainFandB.shininess);

	//////////////////////////////////////////Light from worksheet 10
	glUniform4fv(glGetUniformLocation(programId, "light0.ambCols"), 1, &light0.ambCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.difCols"), 1, &light0.difCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.specCols"), 1, &light0.specCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.coords"), 1, &light0.coords[0]);

	glUniform4fv(glGetUniformLocation(programId, "globAmb"), 1, &globAmb[0]);

	// Obtain modelview matrix uniform location and set value.
	mat4 modelViewMat = mat4(1.0);
	// Move terrain into view - glm::translate replaces glTranslatef
	modelViewMat = translate(modelViewMat, vec3(-8.5f, -2.5f, -25.0f)); // 5x5 grid
	modelViewMatLoc = glGetUniformLocation(programId, "modelViewMat");
	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

	normalMatLoc = glGetUniformLocation(programId, "normalMat");
	normalMat = transpose(inverse(mat3(modelViewMat)));
	glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));

	upDirection = glm::normalize(upDirection);
	//Getting the images
	image[0] = getbmp("C:/OpenGL/ExperimenterSource/Textures/grass.bmp");
	// Create the texture ID
	glGenTextures(1, texture);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	grassTexLoc = glGetUniformLocation(programId, "grassTex");
	glUniform1i(grassTexLoc, 0);
	//glGenerateMipmap(GL_TEXTURE_2D);
	




	///////////////////////////////////////
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Comment of some form
	glBindVertexArray(vao[TREE]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[TREE_VERTICES]);
	glUniform1i(glGetUniformLocation(programId, "ObjectTree"), 0);
	//glUniform1i(glGetUniformLocation(programId, "Transform"), );
	int z = 0;
	for (int x = 0; x != 10; x++){
		z++;
		if (terrain[x][z] + 0.5 >= 0.6){
			cout << terrain[x][z] + 0.5 << endl;
			break;
		}
		Translation = translate(mat4(1.0), vec3(RandList[x], terrain[x][z] + 0.5, RandList1[x]));
		transMatLoc = glGetUniformLocation(programId, "Translation");
		glUniformMatrix4fv(transMatLoc, 1, GL_FALSE, value_ptr(Translation));
		glDrawElements(GL_LINES, 400, GL_UNSIGNED_INT, indexBuffer);
	}


	// For each row - draw the triangle strip
	glBindVertexArray(vao[SQUARE]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[SQUARE_VERTICES]);
	glUniform1i(glGetUniformLocation(programId, "ObjectTree"), 1);
	for (int i = 0; i < MAP_SIZE - 1; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, verticesPerStrip, GL_UNSIGNED_INT, terrainIndexData[i]);
	}

	glFlush();
	glutPostRedisplay();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 119:
		cameraPosition.x = cameraPosition.x + losUnitVector.x * 0.1;
		cameraPosition.z = cameraPosition.z + losUnitVector.z * 0.1;
		cameraPosition.y = cameraPosition.y + losUnitVector.y * 0.1;
		//cout << "W" << endl;
		break;
	case 115:
		cameraPosition.x = cameraPosition.x - losUnitVector.x * 0.1;
		cameraPosition.z = cameraPosition.z - losUnitVector.z * 0.1;
		cameraPosition.y = cameraPosition.y - losUnitVector.y * 0.1;
		//cout << "S" << endl;
		break;
	case 100:
		cameraTheta = cameraTheta + 0.1;
		//losUnitVector.x = sin(cameraTheta);
		//losUnitVector.z = -cos(cameraTheta);
		losUnitVector.x = cos(cameraPhi) * sin(cameraTheta);
		losUnitVector.z = cos(cameraPhi) * -cos(cameraTheta);
		losUnitVector.y = sin(cameraPhi);
		break;
	case 97:
		cameraTheta = cameraTheta - 0.1;
		//losUnitVector.x = sin(cameraTheta);
		//losUnitVector.z = -cos(cameraTheta);
		losUnitVector.x = cos(cameraPhi) * sin(cameraTheta);
		losUnitVector.z = cos(cameraPhi) * -cos(cameraTheta);
		losUnitVector.y = sin(cameraPhi);
		break;
	case 'q':
		cameraPhi = cameraPhi + 0.1;
		losUnitVector.x = cos(cameraPhi) * sin(cameraTheta);
		losUnitVector.z = cos(cameraPhi) * -cos(cameraTheta);
		losUnitVector.y = sin(cameraPhi);
		break;
	case 'e':
		cameraPhi = cameraPhi - 0.1;
		losUnitVector.x = cos(cameraPhi) * sin(cameraTheta);
		losUnitVector.z = cos(cameraPhi) * -cos(cameraTheta);
		losUnitVector.y = sin(cameraPhi);
	default:
		break;
	}
	mat4 modelViewMat = mat4(1.0);
	modelViewMat = lookAt(cameraPosition, cameraPosition + losUnitVector, upDirection);
	modelViewMat = translate(modelViewMat, vec3(-8.5f, -2.5f, -25.0f)); // 5x5 grid
	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));
}

// Main routine.
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	BasicTree.MakeBranch(0, 1, 2, 1);

	for (int j = 0; j != 10000; j++){
		indexBuffer[j] = BasicTree.passBackIndexBuffer()[j];
	}
	for (int j = 0; j != 1000; j++){
		squareVertices[j] = BasicTree.passBackVertex()[j];
	}

	for (int j = 0; j != 10; j++){
		RandList[j] = 0 + static_cast <int> (rand()) / (static_cast <int> (RAND_MAX / (MAP_SIZE - 0)));
		RandList1[j] = (rand() % (int)(MAP_SIZE - 0 + 1) + 0);
	}


	// Set the version of OpenGL (4.2)
	glutInitContextVersion(4, 2);
	// The core profile excludes all discarded features
	glutInitContextProfile(GLUT_CORE_PROFILE);
	// Forward compatibility excludes features marked for deprecation ensuring compatability with future versions
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("TerrainGeneration");

	// Set OpenGL to render in wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
