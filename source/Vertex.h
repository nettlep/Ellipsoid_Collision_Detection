// ---------------------------------------------------------------------------------------------------------------------------------
//  _     _           _                 _     
// | |   | |         | |               | |    
// | |   | | ___ _ __| |_  _____  __   | |__  
//  \ \ / / / _ \ '__| __|/ _ \ \/ /   | '_ \ 
//   \ V / |  __/ |  | |_|  __/>  <  _ | | | |
//    \_/   \___|_|   \__|\___/_/\_\(_)|_| |_|
//                                            
//                                            
//
// Simplistic vertex management class
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

#ifndef	_H_VERTEX
#define	_H_VERTEX

// ---------------------------------------------------------------------------------------------------------------------------------

template<class T = FLT>
class	vert
{
public:
	// Construction/Destruction

inline				vert() {}
inline				vert(const vert &v) : _texture(v.texture()), _world(v.world()), _screen(v.screen()) {}
inline				vert(const point3 &p)	: _world(p) {}

	// Operators

inline		vert		&operator =(const vert &v)
				{
					texture() = v.texture();
					world() = v.world();
					screen() = v.screen();
					return *this;
				}

	// Cast world-space vertex to a point

inline				operator point2() const	{return texture();}
inline				operator point2()	{return texture();}
inline				operator point3() const	{return world();}
inline				operator point3()	{return world();}
inline				operator point4() const	{return screen();}
inline				operator point4()	{return screen();}

	// Accessors

inline	const	point2		&texture() const{return _texture;}
inline		point2		&texture()	{return _texture;}
inline	const	point3		&world() const	{return _world;}
inline		point3		&world()	{return _world;}
inline	const	point4		&screen() const	{return _screen;}
inline		point4		&screen()	{return _screen;}

protected:
	// Data

		matrix<2, 1, T>	_texture;
		matrix<3, 1, T>	_world;
		matrix<4, 1, T>	_screen;
};

#endif // _H_VERTEX
// ---------------------------------------------------------------------------------------------------------------------------------
// Vertex.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

