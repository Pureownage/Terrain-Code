#include "Tree.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

Tree::Tree(){
	squareVertices[0] = { { 0.0, -0.5, 0.0, 1.0 }, { 0.55, 0.27, 0.075, 1.0 } };
	squareVertices[1] = { { 0.0, -0.0, 0.0, 1.0 }, { 0.55, 0.27, 0.075, 1.0 } };
	indexBuffer[0] = 0;
	indexBuffer[1] = 1;

}

Vertex1 *Tree::passBackVertex(){
	return squareVertices;
}

int * Tree::passBackIndexBuffer(){
	return indexBuffer;
}
Vertex1 Tree::passBackVertexFirst(){
	return squareVertices[0];



}

int Tree::MakeBranch(int PreVert, int CurVert, int IndexPos, int count){
	if (count == 5){
		return 1;
	}
	else {
		float x0 = squareVertices[PreVert].coords[0];
		float y0 = squareVertices[PreVert].coords[1];
		float x1 = squareVertices[CurVert].coords[0];
		float y1 = squareVertices[CurVert].coords[1];

		// Lets work out the left branch first

		float xs = (x1 - x0) * Ratio;
		float ys = (y1 - y0) * Ratio;
		float xl = xs * cos(Angle / 2.0) - ys * sin(Angle / 2.0);
		//float yl = xs * cos(Angle / 2.0) + ys * cos(Angle / 2.0);
		float yl = xs * sin(Angle / 2.0) + ys * cos(Angle / 2.0);
		float x2 = x1 + xl;
		float y2 = y1 + yl;

		vec4 PrevBranch(xs, ys, 0.0f, 1.0f);

		mat4 Rotation = mat4(1);

		/*	Rotation = glm::rotate(Rotation, Angle, vec3(1, 0, 0));
		Rotation = glm::rotate(Rotation, Angle, vec3(0, 1, 0));
		Rotation = glm::rotate(Rotation, Angle, vec3(0, 0, 1));

		PrevBranch = PrevBranch * Rotation;
		squareVertices[CurVert * 2] = { { PrevBranch.x, PrevBranch.y, PrevBranch.z , 1.0 }, { 0.55, 0.27, 0.075, 1.0 } };*/
		vec2 nextCoord(x2, y2);
		// Now we work out the right branch
		xs = (x1 - x0) * Ratio;
		ys = (y1 - y0) * Ratio;
		xl = xs * cos(-Angle / 2.0) - ys * sin(-Angle / 2.0);
		//yl = xs * cos(-Angle / 2.0) + ys * cos(-Angle / 2.0);
		yl = xs * sin(-Angle / 2.0) + ys * cos(-Angle / 2.0);
		x2 = x1 + xl;
		y2 = y1 + yl;



		vec2 nextCoord1(x2, y2);
		// Now we add the current branches we have made into the aquare vertices
		squareVertices[CurVert * 2] = { { nextCoord.x, nextCoord.y, 0.0, 1.0 }, { 0.55, 0.27, 0.075, 1.0 } };
		squareVertices[CurVert * 2 + 1] = { { nextCoord1.x, nextCoord1.y, 0.0, 1.0 }, { 0.55, 0.27, 0.075, 1.0 } };
		// Then we add the draw position into the index buffer
		indexBuffer[IndexPos * 2] = CurVert;
		indexBuffer[IndexPos * 2 + 1] = CurVert * 2;
		indexBuffer[IndexPos * 2 + 2] = CurVert;
		indexBuffer[IndexPos * 2 + 3] = CurVert * 2 + 1;
		// We recall Make Branch, with the current branch so we can work out the length, and change the index position. We add up count, once it hits 5 it stops.
		MakeBranch(CurVert, CurVert * 2, IndexPos * 2 + 1, count + 1);
		MakeBranch(CurVert, CurVert * 2 + 1, IndexPos * 2 + 3, count + 1);


	}
}


