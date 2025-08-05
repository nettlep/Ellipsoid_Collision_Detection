// OctMap.h: interface for the OctMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCTMAP_H__A470042D_D891_4D88_9D2C_10441601F802__INCLUDED_)
#define AFX_OCTMAP_H__A470042D_D891_4D88_9D2C_10441601F802__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// simple math types just to store the raw data
typedef float octVect2[2];
typedef float octVect3[3];
typedef float octPlane[4];				// 4th float is distance

struct octVert
{
   octVect2 tv;										// texture coordinates
   octVect2 lv;										// lightmap coordinates
   octVect3 pos;									// vertex position
};

struct octFace
{
   int start;											// first face vert in vertex array
   int num;												// number of verts in the face
   int id;												// texture index into the texture array
   int lid;												// lightmap index into the lightmap array
   octPlane p;
};

#define MAXTEXTURENAMELEN 64

struct octTexture
{
   unsigned int id;							// texture id
   char name[MAXTEXTURENAMELEN];// texture name
};

struct octLightmap
{
   unsigned int id;							// lightmaps id
   unsigned char map[49152];		// 128 x 128 raw RGB data
};

class OctMap  
{
public:
	OctMap();
	virtual ~OctMap();

	void SetNumVerts(int n)           { numVerts = n; };
	void SetNumFaces(int n)           { numFaces = n; };
	void SetNumTextures(int n)        { numTextures = n; };
	void SetNumLightmaps(int n)       { numLightmaps = n; };
	void SetVerts(octVert *v)         { verts = v; };
	void SetFaces(octFace *f)         { faces = f; };
	void SetTextures(octTexture *t)   { textures = t; };
	void SetLightmaps(octLightmap *l) { lightmaps = l; };

	int GetNumVerts(void)							{ return numVerts; };
	int GetNumFaces(void)							{ return numFaces; };
	int GetNumTextures(void)					{ return numTextures; };
	int GetNumLightmaps(void)					{ return numLightmaps; };

	octVert *GetVerts(void)           { return verts; };
	octFace *GetFaces(void)           { return faces; };
	octTexture *GetTextures(void)     { return textures; };
	octLightmap *GetLightmaps(void)   { return lightmaps; };
	octVect3 *GetPlayerStartPos(void)	{ return &playerStartPos; };
	float GetPlayerStartRot(void)			{ return playerStartRot; };
	
	void Reset(void);
	bool Load(char *name, bool text = 0);
	bool Write(char *dest, bool text = 0);	
protected:
	int numVerts;
	int numFaces;
	int numTextures;
	int numLightmaps;
	octVert *verts;
	octFace *faces;
	octTexture *textures;
	octLightmap *lightmaps;
	octVect3 playerStartPos;
	float playerStartRot;
};

#endif // !defined(AFX_OCTMAP_H__A470042D_D891_4D88_9D2C_10441601F802__INCLUDED_)
