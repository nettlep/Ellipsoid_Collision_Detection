// OctMap.cpp: implementation of the OctMap class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OctMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

struct TgaHeader {
	unsigned char identsize;
	unsigned char colormaptype;             // color map type
	unsigned char imagetype;                // image type
	unsigned short colormapstart;            // color map origin
	unsigned char  colormaplength;           // color map length
	unsigned char colormapbits;             // color map entry size
	unsigned short  xstart;                   // x-coordinate origin
	unsigned short  ystart;                   // y-coordinate origin
	unsigned short  width;
  unsigned short  height;            // image width,depth
	unsigned char bits;                     // image depth (bits per pixel)
	unsigned char descriptor;               // image descriptor flag
};

bool LoadTGA(char *, unsigned char **, TgaHeader *);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OctMap::OctMap()
{
	numVerts = numFaces = numTextures = numLightmaps = 0;
	verts = 0;
	faces = 0;
	textures = 0;
	lightmaps = 0;
	playerStartPos[0] = playerStartPos[1] = playerStartPos[2] = 0;
	playerStartRot = 0;
}

OctMap::~OctMap()
{
	Reset();
}

void OctMap::Reset(void)
{
	delete [] verts;
	delete [] faces;
	delete [] textures;
	delete [] lightmaps;
	
	numVerts = numFaces = numTextures = numLightmaps = 0;
	verts = 0;
	faces = 0;
	textures = 0;
	lightmaps = 0;
}

unsigned char *FileReadAll(FILE *in)
{
   unsigned char *b = 0;
   long size = 0;
   
   long curpos;
   curpos = ftell(in);
   fseek(in, 0L, SEEK_END);
   size = ftell(in);
   fseek(in, curpos, SEEK_SET);

   if (!size)
      return 0;
   
   b = new unsigned char[size + 1];
   memset(b, '\0', size + 1);

   if (!b)
      return 0;
   
   fread(b, 1, size, in); // check return with size?

   return b;
}

bool OctMap::Load(char *name, bool text)
{  
	FILE *in = 0;
	
	if (!text)
	{
		if (!(in = fopen(name, "rb"))) return 0;
		
		unsigned char *buffer = FileReadAll(in);
		unsigned char *where = buffer;
		
		fclose(in);
		
		if (!buffer) return 0;
		
		numVerts = *(int *) where; where += sizeof(int);
		numFaces = *(int *) where; where += sizeof(int);
		numTextures = *(int *) where; where += sizeof(int);
		numLightmaps = *(int *) where; where += sizeof(int);
		
		verts = new octVert[numVerts];
		faces = new octFace[numFaces];
		textures = new octTexture[numTextures];
		lightmaps = new octLightmap[numLightmaps];
		
		int size = 0;
		
		size = sizeof(octVert) * numVerts;
		memcpy(verts, where, size);
		where += size;
		
		size = sizeof(octFace) * numFaces;
		memcpy(faces, where, size);
		where += size;
		
		size = sizeof(octTexture) * numTextures;
		memcpy(textures, where, size);
		where += size;
		
		size = sizeof(octLightmap) * numLightmaps;
		memcpy(lightmaps, where, size);
		where += size;
		
		// read in player start pos and rot
		size = sizeof(octVect3);
		memcpy(playerStartPos,where,size);
		where += size;
		size = sizeof(float);
		memcpy(&playerStartRot,where,size);
		where += size;

		delete [] buffer;
	}
	else
	{
		int i;
		
		if (!(in = fopen(name, "r"))) return 0;
		
		fscanf(in, "%d\n", &numVerts);
		fscanf(in, "%d\n", &numFaces);
		fscanf(in, "%d\n", &numTextures);
		fscanf(in, "%d\n", &numLightmaps);
		
		verts = new octVert[numVerts];
		faces = new octFace[numFaces];
		textures = new octTexture[numTextures];
		lightmaps = new octLightmap[numLightmaps];
		
		for (i = 0; i < numVerts; i++)
		{
			fscanf(	in, "(%f %f) (%f %f) (%f %f %f)\n",
							&verts[i].tv[0], &verts[i].tv[1], 
							&verts[i].lv[0], &verts[i].lv[1], 
							&verts[i].pos[0], &verts[i].pos[1], &verts[i].pos[2]);
		}
		
		for (i = 0; i < numFaces; i++)
		{
			fscanf(	in, "%d %d %d %d (%f %f %f %f)\n",
							&faces[i].start, &faces[i].num, &faces[i].id, &faces[i].lid, 
							&faces[i].p[0], &faces[i].p[1], &faces[i].p[2], &faces[i].p[3]);
		}
		
		for (i = 0; i < numTextures; i++) fscanf(in, "%d %s\n", &textures[i].id, textures[i].name);
		
		for (i = 0; i < numLightmaps; i++)
		{
			char buff[1024];
			fscanf(in, "%d %s\n", &lightmaps[i].id, buff);
			// we assume lightmaps are in the same directory as text .oct file
			char lmapName[1024];
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			_splitpath(name,drive,dir,NULL,NULL);
			sprintf(lmapName,"%s%s%s",drive,dir,buff);
			TgaHeader head;
			unsigned char *tbuf = NULL;
			if(!LoadTGA(lmapName,&tbuf,&head)) return false;
			memcpy(lightmaps[i].map,tbuf,49152);
			delete[] tbuf;
		}

		// read in player start
		fscanf(in,"%f %f %f %f\n",playerStartPos[0],playerStartPos[1],playerStartPos[2],playerStartRot);

		fclose(in);
	}
	
	return 1;
}

bool LoadTGA(char *filename, unsigned char **data, TgaHeader *header)
{
  char 						buffer[256];
  unsigned char		*tdata = NULL;

	// open file
  FILE *fp = fopen(filename,"rb");
  if(!fp) return false;

	// read header
	fread(header,sizeof(TgaHeader),1,fp);
  if(header->identsize) fread(&buffer,header->identsize,1,fp);

	// determine image type
	switch((int)header->imagetype)
  {
	case 0:		// no image data
  case 1:		// uncompressed color map *unsupported*
  case 3:		// monochrome *unsupported*
		break;
	case 2:		// uncompressed RGB
	{
    // actually create the surface		
		int depth = header->bits/8;
		*data = new unsigned char[header->height*header->width*depth];
		tdata = *data;
		// start at bottom of data because TGAs are bottom up, but make sure tga is in upper left of image
    //tdata += (header->height*header->width*depth);
   	// read bitmap in line by line
		for(unsigned long y=0;y<header->height;y++)
    {
      //tdata -= header->width*depth;
    	fread(tdata,header->width*depth,1,fp);
			tdata += header->width*depth;
		}
  	break;
  }
  case 10:	// RLE RGB *unsupported*
  case 9:		// RLE color-mapped *unsupported*
	case 11:	// RLE monochrome *unsupported*
	case 32:	// Huffmann, Delta, and RLE color-map *unsupported*
	case 33:	// Huffmann, Delta, and RLE volor-map (4-pass quadtree) *unsupported*
		break;
  }

	// close file, we are done
  fclose(fp);

	return true;
}