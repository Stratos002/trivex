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

struct TVX_Vertex
{
	struct TVX_Position position;
	struct TVX_Normal normal;
	struct TVX_UV UV;
};

enum TVX_VertexAttributeFlagBit
{
	TVX_VERTEX_ATTRIBUTE_FLAG_BIT_POSITION = (1 << 0),
	TVX_VERTEX_ATTRIBUTE_FLAG_BIT_NORMAL = (1 << 1),
	TVX_VERTEX_ATTRIBUTE_FLAG_BIT_UV = (1 << 2)
};

typedef uint8_t TVX_VertexAttributeFlags;

struct TVX_Mesh
{
	TVX_VertexAttributeFlags vertexAttributeFlags;
	uint32_t vertexCount;
	struct TVX_Vertex* pVertices;
	uint32_t vertexIndexCount;
	uint32_t* pVertexIndices;
};

enum TVX_Result TVX_loadMeshFromOBJ(const char* pPath, struct TVX_Mesh* pMesh);
void TVX_freeMesh(struct TVX_Mesh mesh);

#ifdef __cplusplus
}
#endif

#endif
