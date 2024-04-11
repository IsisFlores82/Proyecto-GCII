
#ifndef _OBJPARSER_H_
#define _OBJPARSER_H_

#include <xnamath.h>


// Vertex struct
struct VertexObj
{
	XMFLOAT3 pos;
	XMFLOAT2 tex0;
	XMFLOAT3 norm;
	XMFLOAT3 campos;
};


// Main class
class CObjParser
{
	// Constructors
public:
	CObjParser();
	virtual ~CObjParser();

	// Methods
public:
	bool LoadFile(char* szFileName);
	void Clear();

	// Attributes
public:
	VertexObj* m_pVertex;
	unsigned long m_nVertexCount;
};


#endif // _OBJPARSER_H_