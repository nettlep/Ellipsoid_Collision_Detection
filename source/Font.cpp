// ---------------------------------------------------------------------------------------------------------------------------------
//  ______             _                        
// |  ____|           | |                       
// | |__    ___  _ __ | |_      ___ _ __  _ __  
// |  __|  / _ \| '_ \| __|    / __| '_ \| '_ \ 
// | |    | (_) | | | | |_  _ | (__| |_) | |_) |
// |_|     \___/|_| |_|\__|(_) \___| .__/| .__/ 
//                                 | |   | |    
//                                 |_|   |_|    
//
// Font file (*.f) manager -- see the FontGen utility for file generation
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
// Originally created on 12/18/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h" // If this line gives you an error, comment it out
#include <string>
#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <strstream>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

#include "Font.h"

// ---------------------------------------------------------------------------------------------------------------------------------

	Font::Font(const string &filename)
{
	if (filename.length())
	{
		read(filename);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

	Font::~Font()
{
	deallocate();
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Font::deallocate()
{
	for (int i = 0; i < _characters.size(); i++)
	{
		delete _characters[i];
	}

	_characters.clear();
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	Font::read(const string &filename)
{
	// Deallocate any existing fonts

	_characters.clear();

	// Load it up

	bool	rc = true;
	FILE	*fp = fopen(filename.c_str(), "rb");
	if (!fp) return false;

	try
	{
		for (int i = 0; i < 256; i++)
		{
			FontChar	*c = new FontChar;
			if (!c->read(fp)) return false;
			_characters.push_back(c);
		}
	}
	catch(const bool ret)
	{
		rc = ret;
	}

	// Spaces may be empty... protect against that -- we'll use the width of a narrow character. The leter 'i' should work :)

	if (rc)
	{
		if (!(*this)[' '].screenWidth()) (*this)[' '].screenWidth() = (*this)['i'].screenWidth();
	}

	if (fp) fclose(fp);
	return rc;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	Font::write(const string &filename)
{
	// Write it out

	bool	rc = true;
	FILE	*fp = fopen(filename.c_str(), "wb");
	if (!fp) return false;

	try
	{
		for (int i = 0; i < 256; i++)
		{
			if (!(*this)[i].write(fp)) throw false;
		}
	}
	catch(const bool ret)
	{
		rc = ret;
	}

	if (fp) fclose(fp);
	return rc;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Font::draw(unsigned int *frameBuffer, const unsigned int w, const unsigned int h, const int x, const int y, const string &str, const unsigned int color) const
{
	if (!str.length()) return;

	int	curX = x;

	// We'll offset the line by the first char's offset so that our X/Y position is really the upper-left corner of the string

	int	ox = -(*this)[str[0]].xOffset();
	int	oy = -(*this)[str[0]].yOffset();

	for (int i = 0; i < str.length(); i++)
	{
		const FontChar	&c = (*this)[str[i]];
		c.draw(frameBuffer, w, h, curX + ox, y + oy, color);
		curX += c.screenWidth();
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Font::getStringDimensions(const string &str, unsigned int &w, unsigned int &h, int &ox, int &oy)
{
	w = 0;
	h = 0;

	if (!str.length()) return;

	ox = -(*this)[str[0]].xOffset();
	oy = -(*this)[str[0]].yOffset();

	for (int i = 0; i < str.length(); i++)
	{
		FontChar	&fc = (*this)[str[i]];
		w += fc.screenWidth();
		if (h < fc.screenHeight()) h = fc.screenHeight();
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	FontChar::read(FILE *fp)
{
	if (fread(&_byteWidth   , sizeof(_byteWidth   ), 1, fp) != 1) return false;
	if (fread(&_byteHeight  , sizeof(_byteHeight  ), 1, fp) != 1) return false;
	if (fread(&_xOffset     , sizeof(_xOffset     ), 1, fp) != 1) return false;
	if (fread(&_yOffset     , sizeof(_yOffset     ), 1, fp) != 1) return false;
	if (fread(&_screenWidth , sizeof(_screenWidth ), 1, fp) != 1) return false;
	if (fread(&_screenHeight, sizeof(_screenHeight), 1, fp) != 1) return false;

	delete[] _data;
	_data = new unsigned char[byteWidth() * byteHeight()];
	if (!data()) return false;

	if (fread(_data, byteWidth(), byteHeight(), fp) != byteHeight()) return false;

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	FontChar::write(FILE *fp)
{
	if (fwrite(&_byteWidth   , sizeof(_byteWidth   ), 1, fp) != 1) return false;
	if (fwrite(&_byteHeight  , sizeof(_byteHeight  ), 1, fp) != 1) return false;
	if (fwrite(&_xOffset     , sizeof(_xOffset     ), 1, fp) != 1) return false;
	if (fwrite(&_yOffset     , sizeof(_yOffset     ), 1, fp) != 1) return false;
	if (fwrite(&_screenWidth , sizeof(_screenWidth ), 1, fp) != 1) return false;
	if (fwrite(&_screenHeight, sizeof(_screenHeight), 1, fp) != 1) return false;
	if (fwrite(_data, byteWidth(), byteHeight(), fp) != byteHeight()) return false;

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FontChar::draw(unsigned int *frameBuffer, const unsigned int w, const unsigned int h, const int x, const int y, const unsigned int color) const
{
	if (!data()) return;

	int		cx = x + xOffset();
	int		cy = y + yOffset();
	unsigned char	*iPtr = _data;
	unsigned int	*fPtr = frameBuffer + cy * w + cx;
	unsigned int	alpha = color >> 24;

	for (int iy = cy; iy < cy + (int) byteHeight(); iy++, fPtr += w)
	{
	 	if (iy >= 0 && iy < h)
		{
			unsigned int	*oPtr = fPtr;
			for (int ix = cx; ix < cx + (int) byteWidth(); ix++, iPtr++, oPtr++)
			{
				if (ix >= 0 && ix < w)
				{
					unsigned int	a = (alpha * (*iPtr)) >> 8;
					unsigned int	dst = *oPtr;
					unsigned int	rb = (((color & 0x00ff00ff) * a) + ((dst & 0x00ff00ff) * (0xff - a))) & 0xff00ff00;
					unsigned int	g  = (((color & 0x0000ff00) * a) + ((dst & 0x0000ff00) * (0xff - a))) & 0x00ff0000;
					*oPtr = (rb | g) >> 8;
				}
			}
		}
		else
  		{
    			iPtr += byteWidth();
      		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Font.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

