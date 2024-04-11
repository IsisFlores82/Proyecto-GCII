// ObjParser.cpp : Implements the CObjParser class.
//
//
//

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <d3d11.h>
#include "loadModel.h"

// Specifies the buffer size
#define BUFSIZE 65536


//////////////////////////////////////////////////////////////////////
// Constructors

CObjParser::CObjParser()
{
	m_pVertex = NULL;
	m_nVertexCount = 0;
}

CObjParser::~CObjParser()
{
	if (m_pVertex != NULL)
		free(m_pVertex);
	m_pVertex = NULL;
	m_nVertexCount = 0;
}


//////////////////////////////////////////////////////////////////////
// Utility functions

// Reads a token from the file and places it in buf
bool GetToken(FILE* fp, char* buf)
{
	buf[0] = 0;
	return (fscanf_s(fp, "%s", buf, BUFSIZE) != EOF);
}

// Skips to the end of the line in the file
bool SkipLine(FILE* fp)
{
	char ch;
	do {
		ch = fgetc(fp);
	} while ((ch != 10) && (ch != 13) && (ch != EOF));
	return (ch != EOF);
}

// Adds a float value to the ppBuf buffer
bool AddFloatBuffer(float value, float** ppBuf, unsigned long* pBufSize, unsigned long* pBufPos)
{
	if ((*ppBuf) == NULL)
		return false;
	(*ppBuf)[*pBufPos] = value;
	(*pBufPos)++;
	if ((*pBufPos) >= (*pBufSize)) {
		(*ppBuf) = (float*)realloc((void*)(*ppBuf), ((*pBufSize) + BUFSIZE) * sizeof(float));
		if ((*ppBuf) == NULL)
			return false;
		(*pBufSize) += BUFSIZE;
	}
	return true;
}

// Adds an in value to the ppBuf buffer
bool AddIntBuffer(int value, int** ppBuf, unsigned long* pBufSize, unsigned long* pBufPos)
{
	if ((*ppBuf) == NULL)
		return false;
	(*ppBuf)[*pBufPos] = value;
	(*pBufPos)++;
	if ((*pBufPos) >= (*pBufSize)) {
		(*ppBuf) = (int*)realloc((void*)(*ppBuf), ((*pBufSize) + BUFSIZE) * sizeof(int));
		if ((*ppBuf) == NULL)
			return false;
		(*pBufSize) += BUFSIZE;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////
// Methods

// Loads an OBJ-file
bool CObjParser::LoadFile(char* szFileName)
{
	// Open file
	FILE* fp = NULL;
	fopen_s(&fp, szFileName, "rb");
	if (fp == NULL)
		return false; // file error

	// Variables
	int r, a, b, c, i;
	char buf[BUFSIZE];

	// File vertexes
	float* vbuf = (float*)malloc(BUFSIZE * sizeof(float));
	unsigned long vbufsize = BUFSIZE;
	unsigned long vbufpos = 0;

	// File vertex normals
	float* vnbuf = (float*)malloc(BUFSIZE * sizeof(float));
	unsigned long vnbufsize = BUFSIZE;
	unsigned long vnbufpos = 0;

	// File vertex texture coordinates
	float* vtbuf = (float*)malloc(BUFSIZE * sizeof(float));
	unsigned long vtbufsize = BUFSIZE;
	unsigned long vtbufpos = 0;

	// File faces
	int* fbuf = (int*)malloc(BUFSIZE * sizeof(int));
	unsigned long fbufsize = BUFSIZE;
	unsigned long fbufpos = 0;

	// Check buffers
	if ((vbuf == NULL) || (vnbuf == NULL) || (vtbuf == NULL) || (fbuf == NULL))
		return false; // out of memory

	while (!feof(fp))
	{
		// Get token
		r = GetToken(fp, buf);
		if (!r) break;

		// Vertexes
		if (strcmp(buf, "v") == 0) {
			GetToken(fp, buf);
			AddFloatBuffer((float)atof(buf), &vbuf, &vbufsize, &vbufpos);
			GetToken(fp, buf);
			AddFloatBuffer((float)atof(buf), &vbuf, &vbufsize, &vbufpos);
			GetToken(fp, buf);
			AddFloatBuffer((float)atof(buf), &vbuf, &vbufsize, &vbufpos);
			continue;
		}

		// Vertex normals
		if (strcmp(buf, "vn") == 0) {
			GetToken(fp, buf);
			AddFloatBuffer((float)atof(buf), &vnbuf, &vnbufsize, &vnbufpos);
			GetToken(fp, buf);
			AddFloatBuffer((float)atof(buf), &vnbuf, &vnbufsize, &vnbufpos);
			GetToken(fp, buf);
			AddFloatBuffer((float)atof(buf), &vnbuf, &vnbufsize, &vnbufpos);
			continue;
		}

		// Vertex texture coordinates
		if (strcmp(buf, "vt") == 0) {
			GetToken(fp, buf);
			AddFloatBuffer((float)atof(buf), &vtbuf, &vtbufsize, &vtbufpos);
			GetToken(fp, buf);
			AddFloatBuffer((float)atof(buf), &vtbuf, &vtbufsize, &vtbufpos);
			continue;
		}

		// Vertex faces
		if (strcmp(buf, "f") == 0) {
			for (i = 0; i < 3; i++) {
				GetToken(fp, buf);
				a = b = c = 0;
				if (strstr(buf, "//") == 0)
					sscanf_s(buf, "%d/%d/%d", &a, &b, &c, BUFSIZE);
				else
					sscanf_s(buf, "%d//%d", &a, &c, BUFSIZE);
				AddIntBuffer(a, &fbuf, &fbufsize, &fbufpos);
				AddIntBuffer(b, &fbuf, &fbufsize, &fbufpos);
				AddIntBuffer(c, &fbuf, &fbufsize, &fbufpos);
			}
			continue;
		}

		// Skip comments and unknown items
		SkipLine(fp);
	}


	// Convert (non-indexed).
	// Note: It is possible to convert the model to use an indexed vertex buffer.
	//       That would reduce the memory size of the model by a factor of two.
	m_nVertexCount = fbufpos / 3;
	m_pVertex = (VertexObj*)malloc(m_nVertexCount * sizeof(VertexObj));
	if (m_pVertex == NULL)
		return false;
	::memset((void*)m_pVertex, 0, m_nVertexCount * sizeof(VertexObj));
	unsigned long vi, vti, vni, f;
	for (i = 0; i < (int)m_nVertexCount; i++) {
		f = i * 3;
		vi = fbuf[f + 0] - 1;
		vti = fbuf[f + 1] - 1;
		vni = fbuf[f + 2] - 1;

		// vertex
		if (vi < vbufpos) {
			m_pVertex[i].pos.x = vbuf[vi * 3 + 0];
			m_pVertex[i].pos.y = vbuf[vi * 3 + 1];
			m_pVertex[i].pos.z = vbuf[vi * 3 + 2];
		}

		// texture coordinate
		if (vti < vtbufpos) {
			m_pVertex[i].tex0.x = vtbuf[vti * 2 + 0];
			m_pVertex[i].tex0.y = 1 - vtbuf[vti * 2 + 1];
		}

		// normal
		if (vni < vnbufpos) {
			m_pVertex[i].norm.x = vnbuf[vni * 3 + 0];
			m_pVertex[i].norm.y = vnbuf[vni * 3 + 1];
			m_pVertex[i].norm.z = vnbuf[vni * 3 + 2];
		}
		m_pVertex[i].campos.x = 0;
		m_pVertex[i].campos.y = 0;
		m_pVertex[i].campos.z = 0;
	}


	// Cleanup
	free(vbuf);
	free(vnbuf);
	free(vtbuf);
	free(fbuf);
	fclose(fp);
	return true;
}

// Clears the loaded data
void CObjParser::Clear()
{
	if (m_pVertex != NULL)
		free(m_pVertex);
	m_pVertex = NULL;
	m_nVertexCount = 0;
}
