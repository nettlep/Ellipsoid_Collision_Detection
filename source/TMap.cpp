// ---------------------------------------------------------------------------------------------------------------------------------
//  _______ __  __                                  
// |__   __|  \/  |                                 
//    | |  | \  / | __ _ _ __       ___ _ __  _ __  
//    | |  | |\/| |/ _` | '_ \     / __| '_ \| '_ \ 
//    | |  | |  | | (_| | |_) | _ | (__| |_) | |_) |
//    |_|  |_|  |_|\__,_| .__/ (_) \___| .__/| .__/ 
//                      | |            | |   | |    
//                      |_|            |_|   |_|    
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
//
// NOTES ABOUT THE ROUTINES BELOW:
//
// When drawing, there are four polygons plotted (in perspective) that combine to create a single continuous surface.  This is done
// to test for overlaps in adjacent polygons.
//
// Each polygon routine acheives results as accurate as the algorithm will allow.  See the comments above each routine for details
// of accuracy issues.
//
// Each polygon routine performs no wrapping (and the wrapping error should be visible if there is overflow error.)  They also ADD
// each pixel to the screen, rather than simply plotting them, to show any overlapping of adjacent polygons.
//
// Each routine is hard-coded for a 64x64 texture.  If you change the texture size defines (i.e. TEX_X & TEX_Y) you need to change
// the inner-loops of the texture mappers.
//
// Vertices must be in clock-wise order.
//
// The routines SHOULD not crash with concave polygons, but may only render a portion of the given polygon (and incorrectly at
// that.)
//
// Any other notes about a specific algorithm are given above each polgyon routine.
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// OTHER NOTES:
//
// The routines may not properly handle polygons that have three vertices in a straight line across the top of the polygon (causing
// it to bail too early)
//
// The routines SHOULD not crash with concave polygons, but may only render a portion of the given polygon (and incorrectly at
// that.)
//
// ---------------------------------------------------------------------------------------------------------------------------------

#include <stdafx.h> // If this produces an error, feel free to remove it
#include <math.h>
#include "tmap.h"

// ---------------------------------------------------------------------------------------------------------------------------------
// This is handy
// ---------------------------------------------------------------------------------------------------------------------------------

template<class T>
static	inline	T &_min(T &a, T &b)
{
	return a < b ? a : b;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------------------------------------------------------------

const	unsigned int	textureWidth = 64;		// Texture resolution
const	unsigned int	textureHeight = 64;		//

const	unsigned int	subShift = 4;			// Sub-affine span size
const	unsigned int	subSpan = 1 << subShift;	//

// ---------------------------------------------------------------------------------------------------------------------------------
// Our texture
// ---------------------------------------------------------------------------------------------------------------------------------

static	unsigned int	textureBuffer[textureWidth * textureHeight];

// ---------------------------------------------------------------------------------------------------------------------------------
// Draws a checkerboard texture into textureBuffer
// ---------------------------------------------------------------------------------------------------------------------------------

void	drawTexture()
{
	// Frequency: the lower the number, the higher the frequency

	const int	freq = 2;
	const int	fAnd = 1 << freq;

	for (int y = 0; y < textureHeight; y++)
	{
		int	yIndex = y * textureWidth;

		for (int x = 0; x < textureWidth; x++)
		{
			int		x1 = x - textureWidth / 2;
			int		y1 = y - textureHeight / 2;
			unsigned int	c1 = (unsigned int) sqrt(x1*x1 + y1*y1) * 5 + 16; // circle of light
			unsigned int	c2 = (unsigned int) sqrt(x1*x1 + y1*y1) * 4 + 32; // circle of light
			textureBuffer[yIndex+x] = (y&fAnd) == (x&fAnd) ? (c1 << 16):((c2 << 16) | ((c2<< 7) & 0x7f00) | c2);
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Calculate the deltas along an edge.  This routine is called once per edge per polygon.  Notice how the affine does not require
// the calculation of the homogenous coordinate (w)
// ---------------------------------------------------------------------------------------------------------------------------------

static	inline	void	calcEdgeDeltas(sEDGE &edge, sVERT *top, sVERT *bot)
{
	// Edge deltas

	float	overHeight = 1.0f / (bot->y - top->y);
	edge.du = (bot->u - top->u) * overHeight;
	edge.dv = (bot->v - top->v) * overHeight;
	edge.dw = (bot->w - top->w) * overHeight;
	edge.dx = (bot->x - top->x) * overHeight;

	// Screen pixel Adjustments (some call this "sub-pixel accuracy")

	float	subPix = (float) top->iy - top->y;
	edge.u  = top->u + edge.du * subPix;
	edge.v  = top->v + edge.dv * subPix;
	edge.w  = top->w + edge.dw * subPix;
	edge.x  = top->x + edge.dx * subPix;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Draw an affine texture-mapped polygon.
//
// With a simple affine texture mapper (and without the use of sub-texel accuracy) the final pixel on each scanline of the polygon
// references the texel along that edge.  If the polygon uses the entire texture, then that last pixel will be out of bounds of the
// texture.  For example, a 4-sided polygon might reference these UV values:
//
//    [0,0] [1,0]                             [0,0] [64,0]
//      +-----+                                 +-----+
//      |     |                                 |     |
//      |     |    given a 64x64 texture:       |     |  
//      |     |                                 |     |
//      |     |                                 |     |
//      +-----+                                 +-----+
//    [0,1] [1,1]                            [0,64] [64,64]
//
// Note that the edges on the right reference the texel that is just beyond the range of the texture map (0-63 does not include
// 64).  This is safe, since these polygons are rendered top/left, which means that the far right edge and the last scanline of the
// polygon is not drawn (to avoid overlapping of adjacent polygons.)
//
// Normal affine texture mapping has only one form of accuracy loss which can not be avoided.  The texture mapper interpolates
// (i.e. adds a delta to each U/V value per pixel) which accumulates error, since the deltas calculated are only stored at the
// resolution that the floating point unit will provide. Very few numbers can be represented exactly in IEEE floating point, so the
// closest representative is stored instead.  This inaccuracy is accumulated as values are interpolated from pixel to pixel,
// accumulating the error in the deltas.  Add this to the error of the original U/V value where the interpolation began (also due
// to the inability to store an exact value) and the error is still small, but not negligable.
//
// This can be reduced by using higher precision floating point values (i.e. using doubles rather than floats.)  But this sill only
// reduces the problem, and does not solve the problem entirely.
//
// This problem can manifest itself in a few ways.  First, it can cause inaccurate texel selections, and cause slight jitters in
// the texture.  It can cause overflows in the texture (i.e. the right edge of the span may reference a texel beyond the range of
// the texture).  It may also cause slight inaccuracies along each edge of the polygon, choosing to render to the wrong pixel.
//
// These problems are very rare, indeed, and may never be visually noticed. Especially the last error (choosing the wrong pixel
// along the edges of the polygon) since the adjacent polygons will most likely make the same wrong choice, if the two adjacent
// polygons share their edge vertices.
//
// There is no cure for this inaccuracy, given the algorithms used.  However, there is an acceptable work-around.  Simply choosing
// UV values that are just INSIDE the bounds of the texture can solve this problem.  Also, allowing wrapping textures can also
// solve this problem, provided the overflow wraps to a texel that "looks right."
// ---------------------------------------------------------------------------------------------------------------------------------

void	drawAffineTexturedPolygon(sVERT *verts, unsigned int *frameBuffer, float *zBuffer, const unsigned int pitch)
{
	// Find the top-most vertex

	sVERT		*v = verts, *lastVert = verts, *lTop = verts, *rTop;

	while(v)
	{
		if (v->y < lTop->y) lTop = v;
		lastVert = v;
		v->iy = (int) ceil(v->y);
		v = v->next;
	}

	// Make sure we have the top-most vertex that is earliest in the winding order

	if (lastVert->y == lTop->y && verts->y == lTop->y) lTop = lastVert;

	rTop = lTop;

	// Top scanline of the polygon in the frame buffer

	unsigned int	*fb = &frameBuffer[lTop->iy * pitch];
	float		*zb = &zBuffer[lTop->iy * pitch];

	// Left & Right edges (primed with 0 to force edge calcs first-time through)

	sEDGE		le, re;
	le.height = 0;
	re.height = 0;

	// Render the polygon

	bool	done = false;
	while(!done)
	{
		if (!le.height)
		{
			sVERT	*lBot = lTop - 1; if (lBot < verts) lBot = lastVert;
			le.height = lBot->iy - lTop->iy;
			if (le.height < 0) return;
			calcEdgeDeltas(le, lTop, lBot);
			lTop = lBot;
			if (lTop == rTop) done = true;
			if (lTop != rTop && done) return;
		}

		if (!re.height)
		{
			sVERT	*rBot = rTop + 1; if (rBot > lastVert) rBot = verts;
			re.height = rBot->iy - rTop->iy;
			if (re.height < 0) return;
			calcEdgeDeltas(re, rTop, rBot);
			rTop = rBot;
			if (lTop == rTop) done = true;
			if (lTop != rTop && done) return;
		}

		// Get the height

		int	height = _min(le.height, re.height);

		// Subtract the height from each edge

		le.height -= height;
		re.height -= height;

		// Render the current trapezoid defined by left & right edges

		while(height-- > 0)
		{
			// Texture coordinates

			float		overWidth = 1.0f / (re.x - le.x);
			float		du  = (re.u - le.u) * overWidth;
			float		dv  = (re.v - le.v) * overWidth;
			float		dw  = (re.w - le.w) * overWidth;
			int		idu = (int) (du * 65536.0f);
			int		idv = (int) (dv * 65536.0f);

			// Find the end-points

			int		start = (int) ceil(le.x);
			int		end   = (int) ceil(re.x);

			// Texture adjustment (some call this "sub-texel accuracy")

			float		subTex = (float) start - le.x;
			int		iu = (int)((le.u + du * subTex) * 65536.0f);
			int		iv = (int)((le.v + dv * subTex) * 65536.0f);
			float		w  =       (le.w + dw * subTex);

			// Fill the entire span

			unsigned int	*span = fb + start;
			float		*zspan = zb + start;

			for (; start < end; start++)
			{
				if (w > *zspan)
				{
					*span = textureBuffer[((iv>>10) & 0xFC0) + ((iu>>16) & 0x3F)];
					*zspan = w;
				}
				span++;
				zspan++;
				iu += idu;
				iv += idv;
				w += dw;
			}

			// Step

			le.u += le.du;
			le.v += le.dv;
			le.w += le.dw;
			le.x += le.dx;
			re.u += re.du;
			re.v += re.dv;
			re.w += re.dw;
			re.x += re.dx;
			fb += pitch;
			zb += pitch;
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Draw a perspective-correct texture-mapped polygon.  The following routine performs perspective correction on ALL pixels.  This
// produces a much slower routine, but at the same time, much more accurate results.
//
// Given the inaccuracies I've already explained for the affine texture mapper, this routine suffers from one more accumulation of
// error.  The fact that the values interpolated are not their original values, rather they are divided by Z.
//
// Interpolating these u/z and v/z values accumulates the error in an amplified form, so that when the values are then divided by W
// for each pixel, the amplified error from accumulation is added to the accuracy lost from the two divisions (first division by Z,
// then the division by W).
// 
// This error can manifest itself in the same ways that the affine version can, with the exception that the error produced by the
// following routine is amplified.
// ---------------------------------------------------------------------------------------------------------------------------------

void	drawPerspectiveTexturedPolygon(sVERT *verts, unsigned int *frameBuffer, float *zBuffer, const unsigned int pitch)
{
	// Find the top-most vertex

	sVERT		*v = verts, *lastVert = verts, *lTop = verts, *rTop;

	while(v)
	{
		if (v->y < lTop->y) lTop = v;
		lastVert = v;
		v->iy = (int) ceil(v->y);
		v = v->next;
	}

	// Make sure we have the top-most vertex that is earliest in the winding order

	if (lastVert->y == lTop->y && verts->y == lTop->y) lTop = lastVert;

	rTop = lTop;

	// Top scanline of the polygon in the frame buffer

	unsigned int	*fb = &frameBuffer[lTop->iy * pitch];
	float		*zb = &zBuffer[lTop->iy * pitch];

	// Left & Right edges (primed with 0)

	sEDGE		le, re;
	le.height = 0;
	re.height = 0;

	// Render the polygon

	bool	done = false;
	while(!done)
	{
		if (!le.height)
		{
			sVERT	*lBot = lTop - 1; if (lBot < verts) lBot = lastVert;
			le.height = lBot->iy - lTop->iy;
			if (le.height < 0) return;
			calcEdgeDeltas(le, lTop, lBot);
			lTop = lBot;
			if (lTop == rTop) done = true;
			if (lTop != rTop && done) return;
		}

		if (!re.height)
		{
			sVERT	*rBot = rTop + 1; if (rBot > lastVert) rBot = verts;
			re.height = rBot->iy - rTop->iy;
			if (re.height < 0) return;
			calcEdgeDeltas(re, rTop, rBot);
			rTop = rBot;
			if (lTop == rTop) done = true;
			if (lTop != rTop && done) return;
		}

		// Get the height

		int	height = _min(le.height, re.height);

		// Subtract the height from each edge

		le.height -= height;
		re.height -= height;

		// Render the current trapezoid defined by left & right edges

		while(height-- > 0)
		{
			// Texture coordinates

			float		overWidth = 1.0f / (re.x - le.x);
			float		du = (re.u - le.u) * overWidth;
			float		dv = (re.v - le.v) * overWidth;
			float		dw = (re.w - le.w) * overWidth;

			// Find the end-points

			int		start = (int) ceil(le.x);
			int		end   = (int) ceil(re.x);

			// Texture adjustment (some call this "sub-texel accuracy")

			float		subTex = (float) start - le.x;
			float		u = le.u + du * subTex;
			float		v = le.v + dv * subTex;
			float		w = le.w + dw * subTex;

			// Fill the entire span

			unsigned int	*span = fb + start;
			float		*zspan = zb + start;

			for (; start < end; start++)
			{
				if (w > *zspan)
				{
					float	z = 1.0f / w;
					int	s = (int) (u * z);
					int	t = (int) (v * z);

					*span = textureBuffer[((t<<6) & 0xFC0) + (s & 0x3F)];
					*zspan = w;
				}
				u += du;
				v += dv;
				w += dw;
				span++;
				zspan++;
			}

			// Step

			le.u += le.du;
			le.v += le.dv;
			le.w += le.dw;
			le.x += le.dx;
			re.u += re.du;
			re.v += re.dv;
			re.w += re.dw;
			re.x += re.dx;
			fb += pitch;
			zb += pitch;
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Draw a "sub-affine" perspective-correct texture-mapped polygon.  This routine uses affine texture-mapping between sub-spans of
// subSpan length while only performing perspective correction every subSpan pixels.  This produces a much faster routine that
// the one above, but suffers from accuracy loss.
//
// This routine also suffers from other aliasing problems of the first two, however, since these polygons are an estimated
// perspective-correct, they choose texels in a non-perfect fasion. Remember that the perspective curve (explained in the comments
// above the previous example) is being estimated with linear interpolation (i.e. straight lines.)  This can cause the error
// (already present in a non-linear estimation) to be amplified even more.
//
// The greater the subSpan length, the less "perspective correction" is performed AND the less accurately texels will be chosen.
//
// This routine also uses a fixed-point representation of the UV values as it interpolates each sub-span.  This should not cause
// any problems since the fixed-point representation is 8.24 (24 bits used to represent the fractional component) which is a higher
// degree of resolution than a 32-bit floating-point variable offers.  However, if the delta from texel to texel goes beyond
// 255.999... texels from texel to texel, the value will overflow and results may be unpredictable.
// ---------------------------------------------------------------------------------------------------------------------------------

void	drawSubPerspectiveTexturedPolygon(sVERT *verts, unsigned int *frameBuffer, float *zBuffer, const unsigned int pitch)
{
	// Find the top-most vertex

	sVERT		*v = verts, *lastVert = verts, *lTop = verts, *rTop;

	while(v)
	{
		if (v->y < lTop->y) lTop = v;
		lastVert = v;
		v->iy = (int) ceil(v->y);
		v = v->next;
	}

	// Make sure we have the top-most vertex that is earliest in the winding order

	if (lastVert->y == lTop->y && verts->y == lTop->y) lTop = lastVert;

	rTop = lTop;

	// Top scanline of the polygon in the frame buffer

	unsigned int	*fb = &frameBuffer[lTop->iy * pitch];
	float		*zb = &zBuffer[lTop->iy * pitch];

	// Left & Right edges (primed with 0)

	sEDGE		le, re;
	le.height = 0;
	re.height = 0;

	// Render the polygon

	bool	done = false;
	while(!done)
	{
		if (!le.height)
		{
			sVERT	*lBot = lTop - 1; if (lBot < verts) lBot = lastVert;
			le.height = lBot->iy - lTop->iy;
			if (le.height < 0) return;
			calcEdgeDeltas(le, lTop, lBot);
			lTop = lBot;
			if (lTop == rTop) done = true;
			if (lTop != rTop && done) return;
		}

		if (!re.height)
		{
			sVERT	*rBot = rTop + 1; if (rBot > lastVert) rBot = verts;
			re.height = rBot->iy - rTop->iy;
			if (re.height < 0) return;
			calcEdgeDeltas(re, rTop, rBot);
			rTop = rBot;
			if (lTop == rTop) done = true;
			if (lTop != rTop && done) return;
		}

		// Get the height

		int	height = _min(le.height, re.height);

		// Subtract the height from each edge

		le.height -= height;
		re.height -= height;

		// Render the current trapezoid defined by left & right edges

		while(height-- > 0)
		{
			// Texture coordinates

			float		overWidth = 1.0f / (re.x - le.x);
			float		du = (re.u - le.u) * overWidth;
			float		dv = (re.v - le.v) * overWidth;
			float		dw = (re.w - le.w) * overWidth;

			// Find the end-points

			int		start = (int) ceil(le.x);
			int		end   = (int) ceil(re.x);

			// Texture adjustment (some call this "sub-texel accuracy")

			float		subTex = (float) start - le.x;
			float		u = le.u + du * subTex;
			float		v = le.v + dv * subTex;
			float		w = le.w + dw * subTex;

			// Start of the first span

			float		z  = 1.0f / w;
			float		s1 = u * z;
			float		t1 = v * z;
			float		w1 = w;

			// Fill the entire span

			unsigned int	*span = fb + start;
			float		*zspan = zb + start;
			int		pixelsDrawn = 0;

			for(; start < end; start += subSpan)
			{
				// Start of the current span

				float		s0 = s1;
				float		t0 = t1;
				float		w0 = w1;

				unsigned int	l = end-start;
				int		len = _min(subSpan, l);
				pixelsDrawn += len;

				// End of the current span

				w1 = (w + dw * pixelsDrawn);
				float	z = 1.0f / w1;
				s1 = (u + du * pixelsDrawn) * z;
				t1 = (v + dv * pixelsDrawn) * z;

				float		dw = (w1 - w0) / len;

				// The span (8.24 fixed-point)

				float		divisor = 1.0f / len * 0x1000000;
				unsigned int	ds = (unsigned int) ((s1 - s0) * divisor);
				unsigned int	dt = (unsigned int) ((t1 - t0) * divisor);
				unsigned int	s  = (unsigned int) (s0 * 0x1000000);
				unsigned int	t  = (unsigned int) (t0 * 0x1000000);

				// Draw the sub-span

				for (int j = 0; j < len; j++)
				{
					if (w0 > *zspan)
					{
						*span = textureBuffer[((t>>18)&0xFC0)+((s>>24) & 0x3f)];
						*zspan = w0;
					}
					s += ds;
					t += dt;
					w0 += dw;
					span++;
					zspan++;
				}
			}

			// Scanline step

			le.u += le.du;
			le.v += le.dv;
			le.w += le.dw;
			le.x += le.dx;
			re.u += re.du;
			re.v += re.dv;
			re.w += re.dw;
			re.x += re.dx;
			fb += pitch;
			zb += pitch;
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// TMap.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

