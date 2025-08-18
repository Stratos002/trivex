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

void TVX_GrowingArray_addElement(struct TVX_GrowingArray* pGrowingArray, void* pElement)
{
	if(pGrowingArray->elementCount == pGrowingArray->capacity)
	{
		void* pTmpData = malloc(pGrowingArray->capacity * 2 * pGrowingArray->elementSize);
		memcpy(pTmpData, pGrowingArray->pData, pGrowingArray->elementSize * pGrowingArray->elementCount);
		free(pGrowingArray->pData);
		pGrowingArray->pData = pTmpData;
		pGrowingArray->capacity *= 2;
	}

	memcpy((char*)pGrowingArray->pData + pGrowingArray->elementSize * pGrowingArray->elementCount, pElement, pGrowingArray->elementSize);
	pGrowingArray->elementCount += 1;
}

enum TVX_Result TVX_loadMeshFromOBJ(const char* pPath, struct TVX_Mesh* pMesh)
{
	struct TVX_GrowingArray positions;
	TVX_GrowingArray_initialize(sizeof(struct TVX_Position), &positions);

	struct TVX_GrowingArray normals;
	TVX_GrowingArray_initialize(sizeof(struct TVX_Normal), &normals);

	struct TVX_GrowingArray UVs;
	TVX_GrowingArray_initialize(sizeof(struct TVX_UV), &UVs);

	struct TVX_GrowingArray attributeIndices;
	TVX_GrowingArray_initialize(sizeof(uint32_t), &attributeIndices);

	FILE* pFile = fopen(pPath, "r");
	if(pFile == NULL)
	{
		return TVX_RESULT_COULD_NOT_OPEN_FILE;
	}

	char line[1024];
	while(fgets(line, sizeof(line), pFile))
	{		
		if(strncmp(line, "v ", 2) == 0)
		{
			struct TVX_Position position;
			if(sscanf(line, "v %f %f %f", &position.x, &position.y, &position.z) == 3)
			{
				TVX_GrowingArray_addElement(&positions, &position);
			}
		}
		else if(strncmp(line, "vn", 2) == 0)
		{
			struct TVX_Normal normal;
			if(sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z) == 3)
			{
				TVX_GrowingArray_addElement(&normals, &normal);
			}
		}
		else if(strncmp(line, "vt", 2) == 0)
		{
			struct TVX_UV UV;
			if(sscanf(line, "vt %f %f", &UV.U, &UV.V) == 2)
			{
				TVX_GrowingArray_addElement(&UVs, &UV);
			}
		}
		else if(strncmp(line, "f", 1) == 0)
		{
			uint32_t vertex0Position;
			uint32_t vertex0UV;
			uint32_t vertex0Normal;

			uint32_t vertex1Position;
			uint32_t vertex1UV;
			uint32_t vertex1Normal;

			uint32_t vertex2Position;
			uint32_t vertex2UV;
			uint32_t vertex2Normal;
			
			// case : v/vt/vn
			if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex0Position, &vertex0UV, &vertex0Normal, &vertex1Position, &vertex1UV, &vertex1Normal, &vertex2Position, &vertex2UV, &vertex2Normal) == 9) 
			{
				TVX_GrowingArray_addElement(&attributeIndices, &vertex0Position);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex0Normal);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex0UV);
				
				TVX_GrowingArray_addElement(&attributeIndices, &vertex1Position);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex1Normal);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex1UV);
				
				TVX_GrowingArray_addElement(&attributeIndices, &vertex2Position);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex2Normal);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex2UV);
			}

			// case : v//vn
			if (sscanf(line, "f %d//%d %d//%d %d//%d", &vertex0Position, &vertex0Normal, &vertex1Position, &vertex1Normal, &vertex2Position, &vertex2Normal) == 6) 
			{
				TVX_GrowingArray_addElement(&attributeIndices, &vertex0Position);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex0Normal);
				
				TVX_GrowingArray_addElement(&attributeIndices, &vertex1Position);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex1Normal);
				
				TVX_GrowingArray_addElement(&attributeIndices, &vertex2Position);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex2Normal);
			}

			// case : v/vt
			if (sscanf(line, "f %d/%d %d/%d %d/%d", &vertex0Position, &vertex0UV, &vertex1Position, &vertex1UV, &vertex2Position, &vertex2UV) == 6) 
			{
				TVX_GrowingArray_addElement(&attributeIndices, &vertex0Position);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex0UV);
				
				TVX_GrowingArray_addElement(&attributeIndices, &vertex1Position);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex1UV);
				
				TVX_GrowingArray_addElement(&attributeIndices, &vertex2Position);
				TVX_GrowingArray_addElement(&attributeIndices, &vertex2UV);
			}

			// case : v
			if (sscanf(line, "f %d %d %d", &vertex0Position, &vertex1Position, &vertex2Position) == 3) 
			{
				TVX_GrowingArray_addElement(&attributeIndices, &vertex0Position);
			
				TVX_GrowingArray_addElement(&attributeIndices, &vertex1Position);
				
				TVX_GrowingArray_addElement(&attributeIndices, &vertex2Position);
			}
		}
	}

	pMesh->positionCount = positions.elementCount;
	pMesh->pPositions = positions.pData;

	pMesh->normalCount = normals.elementCount;
	pMesh->pNormals = normals.pData;

	pMesh->UVCount = UVs.elementCount;
	pMesh->pUVs = UVs.pData;

	pMesh->attributIndexCount = attributeIndices.elementCount;
	pMesh->pAttributeIndices = attributeIndices.pData;

	fclose(pFile);

	return TVX_RESULT_SUCCESS;
}

void TVX_freeMesh(struct TVX_Mesh mesh)
{
	free(mesh.pPositions);
	free(mesh.pNormals);
	free(mesh.pUVs);
	free(mesh.pAttributeIndices);
}