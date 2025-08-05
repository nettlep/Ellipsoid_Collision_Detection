// ---------------------------------------------------------------------------------------------------------------------------------
//  ______             _       _     
// |  ____|           | |     | |    
// | |__    ___  _ __ | |_    | |__  
// |  __|  / _ \| '_ \| __|   | '_ \ 
// | |    | (_) | | | | |_  _ | | | |
// |_|     \___/|_| |_|\__|(_)|_| |_|
//                                   
//                                   
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

#ifndef	_H_FONT
#define _H_FONT

// ---------------------------------------------------------------------------------------------------------------------------------

class	FontChar
{
public:
	// Construction/Destruction

				FontChar()
				: _data(NULL)
				{
				}

virtual				~FontChar()
				{
					delete[] _data;
					_data = NULL;
				}

	// Implementation

virtual		bool		read(FILE *fp);
virtual		bool		write(FILE *fp);
virtual		void		draw(unsigned int *frameBuffer, const unsigned int w, const unsigned int h, const int x, const int y, const unsigned int color) const;

	// Accessors

inline	const	unsigned int	&byteWidth() const	{return _byteWidth;}
inline		unsigned int	&byteWidth()		{return _byteWidth;}
inline	const	unsigned int	&byteHeight() const	{return _byteHeight;}
inline		unsigned int	&byteHeight() 		{return _byteHeight;}
inline	const	unsigned int	&screenWidth() const	{return _screenWidth;}
inline		unsigned int	&screenWidth() 		{return _screenWidth;}
inline	const	unsigned int	&screenHeight() const	{return _screenHeight;}
inline		unsigned int	&screenHeight() 	{return _screenHeight;}
inline	const	int		&xOffset() const	{return _xOffset;}
inline		int		&xOffset() 		{return _xOffset;}
inline	const	int		&yOffset() const	{return _yOffset;}
inline		int		&yOffset() 		{return _yOffset;}
inline	const	unsigned char	*data() const		{return _data;}

private:
	// Data members

		unsigned int	_byteWidth;
		unsigned int	_byteHeight;
		unsigned int	_screenWidth;
		unsigned int	_screenHeight;
		int		_xOffset;
		int		_yOffset;
		unsigned char	*_data;
};

// ---------------------------------------------------------------------------------------------------------------------------------

class	Font
{
public:
	// Construction/Destruction

				Font(const string &filename = "");
virtual				~Font();
		void		deallocate();

	// Operators

inline	const	FontChar	&operator[](const unsigned int index) const	{return *_characters[index];}
inline		FontChar	&operator[](const unsigned int index)		{return *_characters[index];}

	// Implementation

virtual		bool		read(const string &filename);
virtual		bool		write(const string &filename);
virtual		void		draw(unsigned int *frameBuffer, const unsigned int w, const unsigned int h, const int x, const int y, const string &str, const unsigned int color) const;
virtual		void		getStringDimensions(const string &str, unsigned int &w, unsigned int &h, int &ox, int &oy);

	// Accessors

inline	const	vector<FontChar *> &characters() const	{return _characters;}
inline		vector<FontChar *> &characters()	{return _characters;}

private:
	// Data members

		vector<FontChar *> _characters;
};

#endif // _H_FONT
// ---------------------------------------------------------------------------------------------------------------------------------
// Font.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

