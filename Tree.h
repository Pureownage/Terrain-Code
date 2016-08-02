#pragma once

struct Vertex1
{
	float coords[4];
	float colors[4];
};

class Tree
{
public:
	Tree();

	int MakeBranch(int, int, int, int);


	Vertex1 *passBackVertex();
	int * passBackIndexBuffer();

	Vertex1 passBackVertexFirst();

	Vertex1 squareVertices[1000];
	int indexBuffer[10000];
	float Ratio = 0.85;
	float Angle = 40.0 / 180.0 * 3.15192;

};

