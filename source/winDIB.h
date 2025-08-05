// ---------------------------------------------------------------------------------------------------------------------------------
//  _   _   _ _       _____  _____ ____      _     
// | | | | | (_)     |  __ \|_   _|  _ \    | |    
// | | | | | |_ _ __ | |  | | | | | |_) |   | |__  
// | | | | | | | '_ \| |  | | | | |  _ <    | '_ \ 
// |  V _ V  | | | | | |__| |_| |_| |_) | _ | | | |
//  \__/ \__/|_|_| |_|_____/|_____|____/ (_)|_| |_|
//                                                 
//                                                 
//
// Generic windows DIBsection class for quick and painless output to a windows window
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

#ifndef	_H_WINDIB
#define _H_WINDIB

// ---------------------------------------------------------------------------------------------------------------------------------

class	winDIB
{
public:
				winDIB(CDC &dc);
inline				~winDIB() {}

	// Accessors

inline	const	CRect		&srcRect() const	{return _srcRect;}
inline		CRect		&srcRect()		{return _srcRect;}
inline	const	CRect		&dstRect() const	{return _dstRect;}
inline		CRect		&dstRect()		{return _dstRect;}
inline	const	WORD		depth() const		{return _bmi[0].bmiHeader.biBitCount;}
virtual		void		depth(const WORD depth);
inline		CDC		&dc()			{return _dc;}
inline		unsigned char	*frameBuffer()		{return _frameBuffer;}
inline		float		*zBuffer()		{return _zBuffer;}
virtual		unsigned char	*frameBuffer(unsigned char *fb, float *zb, const LONG width, const LONG height, const WORD depth = 0);
inline	const	bool		isInitialized() const	{return (_dc && _frameBuffer && _bmi[0].bmiHeader.biSizeImage && _bmi[0].bmiHeader.biBitCount);}

	// Utilitarian

virtual		unsigned int	stretchToDisplay();
virtual		unsigned int	copyToDisplay();
virtual	const	bool		clipLine(float &x0, float &y0, float &x1, float &y1) const;
virtual		void		line(const float x0, const float y0, const float x1, const float y1, const unsigned int color);
virtual		void		aaLine(const float x0, const float y0, const float x1, const float y1, const unsigned int color);
virtual		void		fillRect(const int left, const int top, const int right, const int bottom, const unsigned int color);

inline	const	unsigned int	width() const		{return _bmi[0].bmiHeader.biWidth;}
inline	const	unsigned int	height() const		{return -_bmi[0].bmiHeader.biHeight;}

private:
	// Data

		CRect		_srcRect;
		CRect		_dstRect;
		CDC		&_dc;
		unsigned char	*_frameBuffer;
		float		*_zBuffer;
		BITMAPINFO	_bmi[2];
};

#endif // _H_WINDIB
// ---------------------------------------------------------------------------------------------------------------------------------
// WinDIB.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

