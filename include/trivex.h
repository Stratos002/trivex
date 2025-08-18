#ifndef TRIVEX_H
#define TRIVEX_H

#include <stdbool.h>
#include <stdint.h>

enum TVX_Result
{
	TVX_RESULT_SUCCESS,
	TVX_RESULT_COULD_NOT_OPEN_FILE
};

struct TVX_Position
{
	float x;
	float y;
	float z;
};

struct TVX_Normal
{
	float x;
	float y;
	float z;
};

struct TVX_UV
{
	float U;
	float V;
};

struct TVX_Mesh
{
	uint32_t positionCount;
	uint32_t normalCount;
	uint32_t UVCount;
	uint32_t attributIndexCount;
	struct TVX_Position* pPositions;
	struct TVX_Normal* pNormals;
	struct TVX_UV* pUVs;
	uint32_t* pAttributeIndices; // [position / normal / UV] * vertex count
};

enum TVX_Result TVX_loadMeshFromOBJ(const char* pPath, struct TVX_Mesh* pMesh);
void TVX_freeMesh(struct TVX_Mesh mesh);

#endif