#ifndef TRIVEX_H
#define TRIVEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

enum TVX_Result
{
	TVX_RESULT_SUCCESS,
	TVX_RESULT_COULD_NOT_OPEN_FILE
};

enum TVX_VertexAttributeFlagBit
{
	TVX_VERTEX_ATTRIBUTE_FLAG_BIT_POSITION = 0,
	TVX_VERTEX_ATTRIBUTE_FLAG_BIT_NORMAL = 1,
	TVX_VERTEX_ATTRIBUTE_FLAG_BIT_UV = 2
};

typedef uint8_t TVX_VertexAttributeFlags;

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
	TVX_VertexAttributeFlags vertexAttributeFlags;
	uint32_t vertexCount;
	struct TVX_Position* pPositions;
	struct TVX_Normal* pNormals;
	struct TVX_UV* pUVs;
	uint32_t vertexIndexCount;
	uint32_t* pVertexIndices;
};

enum TVX_Result TVX_loadMeshFromOBJ(const char* pPath, struct TVX_Mesh* pMesh);
void TVX_freeMesh(struct TVX_Mesh mesh);

#ifdef __cplusplus
}
#endif

#endif