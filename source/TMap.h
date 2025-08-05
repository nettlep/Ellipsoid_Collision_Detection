// ---------------------------------------------------------------------------------------------------------------------------------
//  _______ __  __                 _     
// |__   __|  \/  |               | |    
//    | |  | \  / | __ _ _ __     | |__  
//    | |  | |\/| |/ _` | '_ \    | '_ \ 
//    | |  | |  | | (_| | |_) | _ | | | |
//    |_|  |_|  |_|\__,_| .__/ (_)|_| |_|
//                      | |              
//                      |_|              
//
// Generic texture mapper
//
// Best viewed with 8-character tabs and (at least) 132 columns
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Originally released under a custom license.
// This historical re-release is provided under the MIT License.
// See the LICENSE file in the repo root for details.
//
// https://github.com/nettlep
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/06/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_H_TMAP
#define	_H_TMAP

// ---------------------------------------------------------------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------------------------------------------------------------

extern	const	unsigned int	textureWidth;
extern	const	unsigned int	textureHeight;
extern	const	unsigned int	subShift;
extern	const	unsigned int	subSpan;

// ---------------------------------------------------------------------------------------------------------------------------------
// The vertex structure.  Note that this uses a linked list.  I tend to prefer
// them for ease of managing polygons with large numbers of dynamic vertices,
// though lists will work fine, too.
// ---------------------------------------------------------------------------------------------------------------------------------

typedef	struct	vertex
{
	float	u, v, w;
	float	x, y, z;
	int	iy;
	struct	vertex *next;
} sVERT;

// ---------------------------------------------------------------------------------------------------------------------------------
// The edge structure.  This is used to keep track of each left & right edge
// during scan conversion.  The algorithm does not pre-build an edge list
// prior to rendering, rather it renders edges as it builds them.  This
// structure is used only to keep the variables together.
// ---------------------------------------------------------------------------------------------------------------------------------

typedef	struct	edge
{
	float	u, du;
	float	v, dv;
	float	w, dw;
	float	x, dx;
	int	height;
} sEDGE;

// ---------------------------------------------------------------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------------------------------------------------------------

void	drawTexture();
void	drawAffineTexturedPolygon(sVERT *verts, unsigned int *frameBuffer, float *zBuffer, const unsigned int pitch);
void	drawPerspectiveTexturedPolygon(sVERT *verts, unsigned int *frameBuffer, float *zBuffer, const unsigned int pitch);
void	drawSubPerspectiveTexturedPolygon(sVERT *verts, unsigned int *frameBuffer, float *zBuffer, const unsigned int pitch);

#endif
// ---------------------------------------------------------------------------------------------------------------------------------
// TMap.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
