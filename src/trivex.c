#include "trivex.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct TVX_GrowingArray
{
	uint32_t elementCount;
	uint32_t elementSize;
	uint32_t capacity;
	void* pData;
};

void TVX_GrowingArray_initialize(uint32_t elementSize, struct TVX_GrowingArray* pGrowingArray)
{
	pGrowingArray->elementCount = 0;
	pGrowingArray->elementSize = elementSize;
	pGrowingArray->capacity = 2;
	pGrowingArray->pData = malloc(elementSize * pGrowingArray->capacity);
}

void TVX_GrowingArray_destroy(struct TVX_GrowingArray growingArray)
{
	if(growingArray.capacity != 0)
	{
		free(growingArray.pData);
	}
}

void TVX_GrowingArray_addElement(struct TVX_GrowingArray* pGrowingArray, void* pElement)
{
	if(pGrowingArray->elementCount == pGrowingArray->capacity)
	{
		void* pTmpData = realloc(pGrowingArray->pData, pGrowingArray->capacity * 2 * pGrowingArray->elementSize);
		pGrowingArray->pData = pTmpData;
		pGrowingArray->capacity *= 2;
	}

	memcpy((uint8_t*)pGrowingArray->pData + pGrowingArray->elementSize * pGrowingArray->elementCount, pElement, pGrowingArray->elementSize);
	pGrowingArray->elementCount += 1;
}

enum TVX_Result TVX_loadMeshFromOBJ(const char* pPath, struct TVX_Mesh* pMesh)
{
	struct TVX_VertexSignature
	{
		uint32_t positionIndex;
		uint32_t normalIndex;
		uint32_t UVIndex;
	};

	FILE* pFile = fopen(pPath, "r");
	if(pFile == NULL)
	{
		return TVX_RESULT_COULD_NOT_OPEN_FILE;
	}
	
	memset(pMesh, 0, sizeof(struct TVX_Mesh));
	
	struct TVX_GrowingArray vertices;
	struct TVX_GrowingArray vertexIndices;
	struct TVX_GrowingArray bulkPositions;
	struct TVX_GrowingArray bulkNormals;
	struct TVX_GrowingArray bulkUVs;
	struct TVX_GrowingArray accumulatedVertexSignatures;
	
	TVX_GrowingArray_initialize(sizeof(struct TVX_Vertex), &vertices);
	TVX_GrowingArray_initialize(sizeof(uint32_t), &vertexIndices);
	TVX_GrowingArray_initialize(sizeof(struct TVX_Position), &bulkPositions);
	TVX_GrowingArray_initialize(sizeof(struct TVX_Normal), &bulkNormals);
	TVX_GrowingArray_initialize(sizeof(struct TVX_UV), &bulkUVs);
	TVX_GrowingArray_initialize(sizeof(struct TVX_VertexSignature), &accumulatedVertexSignatures);

	char line[1024];
	while(fgets(line, sizeof(line), pFile))
	{		
		if(strncmp(line, "v ", 2) == 0)
		{
			struct TVX_Position position;
			if(sscanf(line, "v %f %f %f", &position.x, &position.y, &position.z) == 3)
			{
				TVX_GrowingArray_addElement(&bulkPositions, &position);
				pMesh->vertexAttributeFlags |= TVX_VERTEX_ATTRIBUTE_FLAG_BIT_POSITION;
			}
		}
		else if(strncmp(line, "vn", 2) == 0)
		{
			struct TVX_Normal normal;
			if(sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z) == 3)
			{
				TVX_GrowingArray_addElement(&bulkNormals, &normal);
				pMesh->vertexAttributeFlags |= TVX_VERTEX_ATTRIBUTE_FLAG_BIT_NORMAL;
			}
		}
		else if(strncmp(line, "vt", 2) == 0)
		{
			struct TVX_UV UV;
			if(sscanf(line, "vt %f %f", &UV.U, &UV.V) == 2)
			{
				TVX_GrowingArray_addElement(&bulkUVs, &UV);
				pMesh->vertexAttributeFlags |= TVX_VERTEX_ATTRIBUTE_FLAG_BIT_UV;
			}
		}
		else if(strncmp(line, "f", 1) == 0)
		{
			struct TVX_VertexSignature currentVertexSignatures[3];
			memset(currentVertexSignatures, -1, sizeof(currentVertexSignatures));
			
			// case : v/vt/vn
			if(pMesh->vertexAttributeFlags == (TVX_VERTEX_ATTRIBUTE_FLAG_BIT_POSITION | TVX_VERTEX_ATTRIBUTE_FLAG_BIT_NORMAL | TVX_VERTEX_ATTRIBUTE_FLAG_BIT_UV))
			{
				sscanf(line, "f %u/%u/%u %u/%u/%u %u/%u/%u", 
					&currentVertexSignatures[0].positionIndex, &currentVertexSignatures[0].UVIndex, &currentVertexSignatures[0].normalIndex, 
					&currentVertexSignatures[1].positionIndex, &currentVertexSignatures[1].UVIndex, &currentVertexSignatures[1].normalIndex, 
					&currentVertexSignatures[2].positionIndex, &currentVertexSignatures[2].UVIndex, &currentVertexSignatures[2].normalIndex);
			}
			// case : v//vn
			else if(pMesh->vertexAttributeFlags == (TVX_VERTEX_ATTRIBUTE_FLAG_BIT_POSITION | TVX_VERTEX_ATTRIBUTE_FLAG_BIT_NORMAL))
			{
				sscanf(line, "f %u//%u %u//%u %u//%u", 
					&currentVertexSignatures[0].positionIndex, &currentVertexSignatures[0].normalIndex, 
					&currentVertexSignatures[1].positionIndex, &currentVertexSignatures[1].normalIndex, 
					&currentVertexSignatures[2].positionIndex, &currentVertexSignatures[2].normalIndex);
			}
			// case : v/vt
			else if(pMesh->vertexAttributeFlags == (TVX_VERTEX_ATTRIBUTE_FLAG_BIT_POSITION | TVX_VERTEX_ATTRIBUTE_FLAG_BIT_UV))
			{
				sscanf(line, "f %u/%u %u/%u %u/%u", 
					&currentVertexSignatures[0].positionIndex, &currentVertexSignatures[0].UVIndex, 
					&currentVertexSignatures[1].positionIndex, &currentVertexSignatures[1].UVIndex, 
					&currentVertexSignatures[2].positionIndex, &currentVertexSignatures[2].UVIndex);
			}
			// case : v
			else if(pMesh->vertexAttributeFlags == (TVX_VERTEX_ATTRIBUTE_FLAG_BIT_POSITION))
			{
				sscanf(line, "f %u %u %u", 
					&currentVertexSignatures[0].positionIndex, 
					&currentVertexSignatures[1].positionIndex, 
					&currentVertexSignatures[2].positionIndex);
			}
			
			for(uint32_t currentVertexSignatureIndex = 0; currentVertexSignatureIndex < 3; ++currentVertexSignatureIndex)
			{
				struct TVX_VertexSignature currentVertexSignature = currentVertexSignatures[currentVertexSignatureIndex];
				
				bool vertexSignatureAlreadyEncountered = false;
				for(uint32_t accumulatedVertexSignatureIndex = 0; accumulatedVertexSignatureIndex < accumulatedVertexSignatures.elementCount; ++accumulatedVertexSignatureIndex)
				{
					struct TVX_VertexSignature accumulatedVertexSignature = ((struct TVX_VertexSignature*)accumulatedVertexSignatures.pData)[accumulatedVertexSignatureIndex];

					if(memcmp(&currentVertexSignature, &accumulatedVertexSignature, sizeof(struct TVX_VertexSignature)) == 0)
					{
						vertexSignatureAlreadyEncountered = true;
						TVX_GrowingArray_addElement(&vertexIndices, &accumulatedVertexSignatureIndex);
						break;
					}
				}

				if(!vertexSignatureAlreadyEncountered)
				{
					struct TVX_Vertex vertex;
					memset(&vertex, 0, sizeof(struct TVX_Vertex));

					if((pMesh->vertexAttributeFlags & TVX_VERTEX_ATTRIBUTE_FLAG_BIT_POSITION) != 0)
					{
						vertex.position = ((struct TVX_Position*)bulkPositions.pData)[currentVertexSignature.positionIndex - 1];
					}
					
					if((pMesh->vertexAttributeFlags & TVX_VERTEX_ATTRIBUTE_FLAG_BIT_NORMAL) != 0)
					{
						vertex.normal = ((struct TVX_Normal*)bulkNormals.pData)[currentVertexSignature.normalIndex - 1];
					}

					if((pMesh->vertexAttributeFlags & TVX_VERTEX_ATTRIBUTE_FLAG_BIT_UV) != 0)
					{
						vertex.UV = ((struct TVX_UV*)bulkUVs.pData)[currentVertexSignature.UVIndex - 1];
					}

					TVX_GrowingArray_addElement(&vertexIndices, &vertices.elementCount);
					TVX_GrowingArray_addElement(&vertices, &vertex);
					TVX_GrowingArray_addElement(&accumulatedVertexSignatures, &currentVertexSignature);
				}
			}
		}
	}

	fclose(pFile);

	pMesh->vertexCount = vertices.elementCount;
	pMesh->pVertices = vertices.pData;
	pMesh->vertexIndexCount = vertexIndices.elementCount;
	pMesh->pVertexIndices = vertexIndices.pData;

	TVX_GrowingArray_destroy(bulkPositions);
	TVX_GrowingArray_destroy(bulkNormals);
	TVX_GrowingArray_destroy(bulkUVs);
	TVX_GrowingArray_destroy(accumulatedVertexSignatures);

	return TVX_RESULT_SUCCESS;
}

void TVX_destroyMesh(struct TVX_Mesh mesh)
{
	free(mesh.pVertices);
	free(mesh.pVertexIndices);
}
