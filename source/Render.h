// ---------------------------------------------------------------------------------------------------------------------------------
//  _____                 _               _     
// |  __ \               | |             | |    
// | |__) | ___ _ __   __| | ___ _ __    | |__  
// |  _  / / _ \ '_ \ / _` |/ _ \ '__|   | '_ \ 
// | | \ \|  __/ | | | (_| |  __/ |    _ | | | |
// |_|  \_\\___|_| |_|\__,_|\___|_|   (_)|_| |_|
//                                              
//                                              
//
// Rendering pipeline and vewport manager
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

#ifndef	_H_RENDER
#define	_H_RENDER

// ---------------------------------------------------------------------------------------------------------------------------------
// Required includes
// ---------------------------------------------------------------------------------------------------------------------------------

#include "Primitive.h"
#include "winDIB.h"
#include "Font.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	Render  
{
public:
	// Construction/Destruction

				Render(CDC &dc, CWnd &window, const vector<primitive<> > &data, const point3 &cameraPosition);
virtual				~Render();

	// Accessors

inline		CWnd		&window()			{return _window;}
inline		winDIB		&dib()				{return _dib;}
inline	const	unsigned int	&width() const			{return _width;}
inline		unsigned int	&width()			{return _width;}
inline	const	unsigned int	&height() const			{return _height;}
inline		unsigned int	&height()			{return _height;}
inline	const	unsigned int	*frameBuffer() const		{return _frameBuffer;}
inline		unsigned int	*frameBuffer()			{return _frameBuffer;}
inline	const	float		*zBuffer() const		{return _zBuffer;}
inline		float		*zBuffer()			{return _zBuffer;}
inline	const	bool		&lButton() const		{return _lButton;}
inline		bool		&lButton()			{return _lButton;}
inline	const	bool		&rButton() const		{return _rButton;}
inline		bool		&rButton()			{return _rButton;}
inline	const	bool		&antialias() const		{return _antialias;}
inline		bool		&antialias()			{return _antialias;}
inline	const	bool		&cull() const			{return _cull;}
inline		bool		&cull()				{return _cull;}
inline	const	bool		&polygons()	const		{return _polygons;}
inline		bool		&polygons()			{return _polygons;}
inline	const	bool		&wireframe() const		{return _wireframe;}
inline		bool		&wireframe() 			{return _wireframe;}
inline	const	bool		&blinn() const			{return _blinn;}
inline		bool		&blinn() 			{return _blinn;}
inline	const	bool		&widescreen() const		{return _widescreen;}
inline		bool		&widescreen() 			{return _widescreen;}
inline	const	unsigned int	&perspectiveCorrect() const	{return _perspectiveCorrect;}
inline		unsigned int	&perspectiveCorrect() 		{return _perspectiveCorrect;}
inline	const	bool		&help() const			{return _help;}
inline		bool		&help() 			{return _help;}
inline	const	bool		&gravity() const		{return _gravity;}
inline		bool		&gravity() 			{return _gravity;}
inline	const	unsigned int	&collisionType() const		{return _collisionType;}
inline		unsigned int	&collisionType() 		{return _collisionType;}
inline	const	bool		&fly() const			{return _fly;}
inline		bool		&fly() 				{return _fly;}
inline	const	bool		&manual() const			{return _manual;}
inline		bool		&manual() 			{return _manual;}
inline	const	bool		&jumble() const			{return _jumble;}
inline		bool		&jumble() 			{return _jumble;}
inline	const	bool		&filterPulseJumps() const	{return _filterPulseJumps;}
inline		bool		&filterPulseJumps() 		{return _filterPulseJumps;}
inline	const	point2		&screenCenter() const		{return _screenCenter;}
inline		point2		&screenCenter() 		{return _screenCenter;}
inline	const	point3		&cameraPosition() const		{return _cameraPosition;}
inline		point3		&cameraPosition() 		{return _cameraPosition;}
inline	const	vector3		&velocityVector() const		{return _velocityVector;}
inline		vector3		&velocityVector() 		{return _velocityVector;}
inline	const	vector3		&gravityVector() const		{return _gravityVector;}
inline		vector3		&gravityVector() 		{return _gravityVector;}
inline	const	vector3		&radiusVector() const		{return _radiusVector;}
inline		vector3		&radiusVector() 		{return _radiusVector;}
inline	const	FLT		&xrot() const			{return _xrot;}
inline		FLT		&xrot() 			{return _xrot;}
inline	const	FLT		&yrot() const			{return _yrot;}
inline		FLT		&yrot() 			{return _yrot;}
inline	const	FLT		&jumbleScale() const		{return _jumbleScale;}
inline		FLT		&jumbleScale() 			{return _jumbleScale;}

	// Utilitarian

inline		void		flip() {dib().copyToDisplay();}
virtual		void		clear(unsigned int color = 0);
virtual		bool		updateWindowPosition();
virtual		void		animate();
virtual		bool		renderFrame();
virtual		void		drawHelp();
virtual		void		mouseMoved(const int relX, const int relY);
virtual		void		mousePressed(const bool left, const bool right);

		vector<primitive<> >	polygonList;
		vector<primitive<> >	polygonListBackup;

private:
		CWnd		&_window;
		winDIB		_dib;
		unsigned int	_width, _height;
		unsigned int	*_frameBuffer;
		float		*_zBuffer;
		bool		_antialias;
		bool		_cull;
		bool		_polygons;
		bool		_wireframe;
		bool		_blinn;
		bool		_widescreen;
		unsigned int	_perspectiveCorrect;
		bool		_help;
		bool		_gravity;
		unsigned int	_collisionType;
		bool		_fly;
		bool		_manual;
		bool		_jumble;
		bool		_filterPulseJumps;

		point3		_cameraPosition;
		vector3		_velocityVector;
		vector3		_gravityVector;
		vector3		_radiusVector;
		point2		_screenCenter;
		FLT		_xrot;
		FLT		_yrot;
		FLT		_velocity;
		FLT		_jumbleScale;
		bool		_lButton;
		bool		_rButton;

		Font		smallFont;
		Font		mediumFont;
		Font		largeFont;
};

#endif // _H_RENDER
// ---------------------------------------------------------------------------------------------------------------------------------
// Render.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

