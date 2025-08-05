// ---------------------------------------------------------------------------------------------------------------------------------
//  _____                 _                                
// |  __ \               | |                               
// | |__) | ___ _ __   __| | ___ _ __      ___ _ __  _ __  
// |  _  / / _ \ '_ \ / _` |/ _ \ '__|    / __| '_ \| '_ \ 
// | | \ \|  __/ | | | (_| |  __/ |    _ | (__| |_) | |_) |
// |_|  \_\\___|_| |_|\__,_|\___|_|   (_) \___| .__/| .__/ 
//                                            | |   | |    
//                                            |_|   |_|    
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

#include "stdafx.h"
#include "Common.h"
#include "Render.h"
#include "TMap.h"
#include "Collision.h"
#include "Clip.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ---------------------------------------------------------------------------------------------------------------------------------
// Rendering constants
// ---------------------------------------------------------------------------------------------------------------------------------

const	FLT	FOV = (FLT) 60;
const	FLT	nearZ = (FLT) 1;
const	FLT	farZ =  (FLT) 10000;

// ---------------------------------------------------------------------------------------------------------------------------------

		Render::Render(CDC &dc, CWnd &window, const vector<primitive<> > &data, const point3 &cameraPosition)
		:_window(window), _dib(dc), polygonList(data), _cameraPosition(cameraPosition)
{
	// Setup the window stuff

	_velocityVector = _velocityVector.zero();
	_gravityVector = vector3(0, -1, 0);
	_width = 0;
	_height = 0;
	_frameBuffer = NULL;
	_zBuffer = NULL;
	_xrot = 0;
	_yrot = 0;
	_velocity = 0;
	_lButton = _rButton = false;
	_antialias = true;
	_cull = true;
	_polygons = true;
	_wireframe = false;
	_blinn = true;
	_widescreen = true;
	_perspectiveCorrect = 1;
	_help = false;
	_gravity = true;
	_collisionType = 2;
	_fly = false;
	_manual = true;
	_jumble = false;
	_filterPulseJumps = false;
	_jumbleScale = 5;
	_radiusVector = vector3(15, 30, 15);

	// Load up the fonts

	if (!smallFont.read("Small.f"))
	{
		AfxMessageBox("Unable to load the font file: Small.f");
		exit(0);
	}

	if (!mediumFont.read("Medium.f"))
	{
		AfxMessageBox("Unable to load the font file: Medium.f");
		exit(0);
	}

	// Our entropy test is going to screw stuff up, so save a copy of the world data

	polygonListBackup = data;

	// Setup the window's DIB

	updateWindowPosition();

	// Init the texture mapper

	drawTexture();
}

// ---------------------------------------------------------------------------------------------------------------------------------

		Render::~Render()
{
	delete[] _frameBuffer;
	delete[] _zBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void		Render::clear(unsigned int color)
{
	if (!color)
	{
		memset(_frameBuffer, 0, width() * height() *sizeof(unsigned int));
	}
	else
	{
		unsigned int	size = width() * height();
		unsigned int	*ptr = _frameBuffer;

		for (unsigned int i = 0; i < size; i++, ptr++)
		{
			*ptr = color;
		}
	}

	memset(_zBuffer, 0, width() * height() *sizeof(float));
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool		Render::updateWindowPosition()
{
	// The new DIB dimensions

	{
		CRect	clientRect;
		window().GetClientRect(clientRect);
		unsigned int	w = clientRect.Width();
		unsigned int	h = clientRect.Height();

		// Bail if we don't have any dimension

		if (!w || !h) return true;

		// Bail if there's no change

		if (width() == w && height() == h && frameBuffer()) return true;

		// Update

		width() = w;
		height() = h;
		screenCenter() = point2(width() / 2, height() / 2);
	}

	// Reallocate our frame buffer

	delete[] _frameBuffer;
	_frameBuffer = new unsigned int[width()*height()];
	if (!_frameBuffer) return false;

	// Reallocate our Zbuffer

	delete[] _zBuffer;
	_zBuffer = new float[width()*height()];
	if (!_zBuffer) return false;

	// Setup the dib

	dib().dstRect() = CRect(0, 0, width(), height());
	dib().srcRect() = CRect(0, 0, width(), height());
	dib().frameBuffer((unsigned char *) frameBuffer(), zBuffer(), width(), height());
	dib().depth(32);
	
	// Done

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Render::animate()
{
const	FLT	speed = (FLT) 3.0;

	if (_lButton || !manual())
	{
		if (fly() || !manual())
		{
			matrix4	rotation = matrix4::rotation(xrot(), yrot(), 0);
			vector3	v = rotation.extractZVector();
			velocityVector() += v * speed;
		}
		else
		{
			matrix4	rotation = matrix4::rotation(0, yrot(), 0);
			vector3	v = rotation.extractZVector();
			velocityVector() += v * speed;
		}
	}

	// Limit the speed of the velocity vector ("terminal velocity")

	if (velocityVector().length() > 150) velocityVector().setLength(150);

	// Cheezy friction

	velocityVector() *= 0.60f;

	// Add entropy to the scene

	if (jumble() && jumbleScale())
	{
		for (int i = 0; i < polygonList.size(); i++)
		{
			primitive<>	&lp = polygonList[i];
			primitive<>	&bp = polygonListBackup[i];

			for (int j = 0; j < lp.vertexCount(); j++)
			{
				vector3	rot = lp[j].world();
				rot.x() = fmod(rot.x(), 0.54321f);
				rot.y() = fmod(rot.y(), 0.4321f);
				rot.z() = fmod(rot.z(), 0.321f);

				vector3	v = lp[j].world() - bp[j].world();
				v = matrix3::rotation(rot.x(), rot.y(), rot.z()) >> v;

				if (v != v.zero()) v.setLength(jumbleScale());

				lp[j].world() = bp[j].world() + v;
			}

			lp.calcPlane(false);
		}
	}

	// Add gravity

	if (gravity() && manual())
	{
		velocityVector() += gravityVector();
	}

	// Our collision ray

	ray3	collisionRay(cameraPosition(), velocityVector());

	// Calculate the bounding box around our source position
	//
	// Because we might collide and slide, we can really move in any direction. However, since
	// our collision code is somewhat smart, it should never move us further than the distance in
	// our velocity vector. So as long as we can create a box that contains our ellipsoide plus the
	// size of our radius vector (in all directions) plus a little extra for good measure, then it
	// should be fine.
	//
	// Whew! All that text for this little bit of code...

	point3	bbMin = collisionRay.origin() - velocityVector().length() * 2 - radiusVector();
	point3	bbMax = collisionRay.origin() + velocityVector().length() * 2 + radiusVector();

	// Calculate the potential colliders from this bounding box

	vector<primitive<> > potentialColliders;
	for (vector<primitive<> >::const_iterator i = polygonList.begin(); i != polygonList.end(); ++i)
	{
		const primitive<>	&p = *i;
		unsigned int	vc = p.vertexCount();

		point3	pbbMin = p[0].world();
		point3	pbbMax = p[0].world();

		for (unsigned int j = 1; j < vc; j++)
		{
			const point3	&world = p[j].world();

			if (world.x() < pbbMin.x()) pbbMin.x() = world.x();
			if (world.y() < pbbMin.y()) pbbMin.y() = world.y();
			if (world.z() < pbbMin.z()) pbbMin.z() = world.z();

			if (world.x() > pbbMax.x()) pbbMax.x() = world.x();
			if (world.y() > pbbMax.y()) pbbMax.y() = world.y();
			if (world.z() > pbbMax.z()) pbbMax.z() = world.z();
		}

		if (pbbMax.x() < bbMin.x()) continue;
		if (pbbMax.y() < bbMin.y()) continue;
		if (pbbMax.z() < bbMin.z()) continue;
		if (pbbMin.x() > bbMax.x()) continue;
		if (pbbMin.y() > bbMax.y()) continue;
		if (pbbMin.z() > bbMax.z()) continue;

		potentialColliders.push_back(p);
	}

	// Move & collide

	vector3	lastDir;
	if (collisionType() == 2)
	{
		cameraPosition() = collision::collideAndSlide(radiusVector(), potentialColliders, collisionRay, lastDir, filterPulseJumps());
	}
	else if (collisionType() == 1)
	{
		cameraPosition() = collision::collideAndStop(radiusVector(), potentialColliders, collisionRay);
		lastDir = vector3::zero();
	}
	else
	{
		cameraPosition() = collisionRay.end();
	}

	// Retain our last sliding direction so we can keep moving in THAT direction

	velocityVector() = lastDir;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	Render::renderFrame()
{
	// Clear the frame buffer

	clear();

	// "The shape of things to come" (sounds like an advert) -- This is thhe shape of the screen

	FLT 	xScale = (FLT) 0.98;
	FLT 	yScale = (FLT) 0.70;
	if (!widescreen())
	{
		xScale = (FLT) 0.98;
		yScale = (FLT) 0.98;
	}

	// Automated movement

	if (!manual())
	{
		xrot() += (FLT) (sin(0.23) * 0.1);
		yrot() += (FLT) (cos(0.31) * 0.1);
	}

	// Move our player

	animate();

	// Generate a view matrix (in this simple engine, this is just the rotation of the camera

	matrix4	rotation = matrix4::rotation(xrot(), yrot(), 0);

	// We want our camera where the player's head might be, not in the middle of our ellipsoid

	point3	headPosition = cameraPosition();
	headPosition.y() += radiusVector().y() * (FLT) 0.9;

	// Add in the transformation to the view matrix

	matrix4	view = matrix4::translation(-vector4(headPosition.x(), headPosition.y(), headPosition.z(), 0)) >> rotation;

	// Our aspect ratio, and our [F]ield [O]f [V]iew

	FLT	aspect = ((FLT) width() * xScale) / ((FLT) height() * yScale);
	FLT	fov    = FOV * (FLT) 3.14159265359 / (FLT) 180;

	// Generate a perspective projection transformation matrix

	matrix4	perspectiveXform;
	if (blinn()) perspectiveXform = matrix4::projectPerspectiveBlinn(fov, aspect, nearZ, farZ);
	else		perspectiveXform = matrix4::projectPerspectiveD3D(fov, aspect, nearZ, farZ);

	// And combine the two for our final transform matrix

	matrix4	xform = view >> perspectiveXform;

	for (int i = 0; i < polygonList.size(); i++)
	{
		primitive<>	p = polygonList[i];

		// Backface culling (optional)

		if (cull() && p.plane().distance(headPosition) < 0) continue;

		// Transform & code the vertices

		unsigned int	codeOff = -1;
		unsigned int	codeOn = 0;
		for (unsigned int j = 0; j < p.vertexCount(); j++)
		{
			vert<>	&v = p[j];
			point4	&s = v.screen();
			s = xform >> v.world();

			unsigned int	code =	(s.x() >  s.w() ?  1:0) | (s.x() < -s.w() ?  2:0) |
						(s.y() >  s.w() ?  4:0) | (s.y() < -s.w() ?  8:0) |
						(s.z() <    0.0 ? 16:0) | (s.z() >  s.w() ? 32:0);
			codeOff &= code;
			codeOn  |= code;
		}

		// Completely coded off-screen?

		if (codeOff) continue;

		// Only bother trying to clip if it's partially off-screen

		if (codeOn && !clipPrimitive(p)) continue;

		// Project

		sVERT	verts[64];
		for (j = 0; j < p.vertexCount(); j++)
		{
			vert<>	&v = p[j];
			point4	&s = v.screen();
			FLT	ow = 1.0 / s.w();
			s.x() = verts[j].x = screenCenter().x() + s.x() * ow * screenCenter().x() * xScale;
			s.y() = verts[j].y = screenCenter().y() - s.y() * ow * screenCenter().y() * yScale;
			s.z() = verts[j].z = ow;
			s.w() = 1.0;

			verts[j].w = ow;

			if (perspectiveCorrect() == 0) ow = 1.0;

			point2	&t = v.texture();
			verts[j].u = t.x() * ow;
			verts[j].v = t.y() * ow;
			verts[j].next = &verts[j+1];
		}
		verts[j-1].next = NULL;

		// Draw it

		if (polygons())
		{
			     if (perspectiveCorrect() == 0)	drawAffineTexturedPolygon(verts, frameBuffer(), zBuffer(), width());
			else if (perspectiveCorrect() == 1)	drawSubPerspectiveTexturedPolygon(verts, frameBuffer(), zBuffer(), width());
			else if (perspectiveCorrect() == 2)	drawPerspectiveTexturedPolygon(verts, frameBuffer(), zBuffer(), width());
		}

		// Outline it

		if (wireframe())
		{
			int	color = 0xff00ff00;

			point4	p0 = p[p.vertexCount() - 1].screen();
			for (j = 0; j < p.vertexCount(); j++)
			{
				point4	p1 = p[j].screen();
				if (antialias())
				{
					dib().aaLine(p0.x(), p0.y(), p1.x(), p1.y(), color);
				}
				else
				{
					dib().line(p0.x(), p0.y(), p1.x(), p1.y(), color);
				}
				p0 = p1;
			}
		}
	}

	dib().aaLine(screenCenter().x() - 5, screenCenter().y(), screenCenter().x() + 5, screenCenter().y(), 0xffffffff);
	dib().aaLine(screenCenter().x(), screenCenter().y() - 5, screenCenter().x(), screenCenter().y() + 5, 0xffffffff);

	// Draw the position text

	unsigned int	w, h;
	int		ox, oy;
	char		xxx[90];

	sprintf(xxx, "Total polygons: %d", polygonList.size());
	mediumFont.getStringDimensions(xxx, w, h, ox, oy);
	mediumFont.draw((unsigned int *)dib().frameBuffer(), dib().width(), dib().height(), dib().width() / 2 - w / 2, dib().height()-65, xxx, 0xC08080ff);

	sprintf(xxx, "RADIUS: %12.5f, %12.5f, %12.5f", radiusVector().x(), radiusVector().y(), radiusVector().z());
	smallFont.getStringDimensions(xxx, w, h, ox, oy);
	smallFont.draw((unsigned int *) dib().frameBuffer(), dib().width(), dib().height(), dib().width() / 2 - w / 2, dib().height()-45, xxx, 0x8000ff00);

	sprintf(xxx, "POS: %12.5f, %12.5f, %12.5f", headPosition.x(), headPosition.y(), headPosition.z());
	smallFont.getStringDimensions(xxx, w, h, ox, oy);
	smallFont.draw((unsigned int *) dib().frameBuffer(), dib().width(), dib().height(), dib().width() / 2 - w / 2, dib().height()-30, xxx, 0x8000ff00);

	vector4	zdir = rotation.extractZVector();
	sprintf(xxx, "DIR: %12.5f, %12.5f, %12.5f", zdir.x(), zdir.y(), zdir.z());
	smallFont.getStringDimensions(xxx, w, h, ox, oy);
	smallFont.draw((unsigned int *)dib().frameBuffer(), dib().width(), dib().height(), dib().width() / 2 - w / 2, dib().height()-15, xxx, 0x80ffffff);

	// Display the help window

	if (help())
	{
		drawHelp();
	}
	else
	{
		sprintf(xxx, "Press H for help");
		smallFont.getStringDimensions(xxx, w, h, ox, oy);
		smallFont.draw((unsigned int *)dib().frameBuffer(), dib().width(), dib().height(), dib().width() / 2 - w / 2, 15, xxx, 0x80ffffff);
	}

	// Update the screen

	flip();

	// Done

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Render::drawHelp()
{
	struct
	{
		const	char	*key;
		const	char	*desc;
		const	char	*state;
	} sKeys[] =
	{
		{"Action",      "Description",                              "State"}, // header
		{"V",           "Toggle collision type",                    collisionType()==0 ? "None":collisionType()==1 ? "Collide-And-Stop":"Collide-And-Slide"},
		{"P",           "Toggle perspective correction method",     perspectiveCorrect()==0 ? "None":perspectiveCorrect()==1 ? "Sub-affine":"Full"},
		{"B",           "Toggle Blinn/D3D perspective xform",       blinn() ? "Blinn":"D3D"},
		{"A",           "Toggle antialiased outlines",              antialias() ? "On":"Off"},
		{"C",           "Toggle backface culling",                  cull() ? "On":"Off"},
		{"D",           "Toggle draw polygons",                     polygons() ? "On":"Off"},
		{"F",           "Toggle flying",                            fly() ? "On":"Off"},
		{"G",           "Toggle gravity",                           gravity() ? "On":"Off"},
		{"H",           "Toggle help",                              help() ? "On":"Off"},
		{"M",           "Toggle manual control mode",               manual() ? "On":"Off"},
		{"N",           "Toggle filtering of pulse jumps",          filterPulseJumps() ? "On":"Off"},
		{"O",           "Toggle outlined polygons (wireframe)",     wireframe() ? "On":"Off"},
		{"W",           "Toggle widescreen",                        widescreen() ? "On":"Off"},
		{"J",           "Toggle jumble",                            jumble() ? "On":"Off"},
		{"X",           "Increase jumble",                          "--"},
		{"Z",           "Decrease jumble",                          "--"},
		{"E",           "Entropy (add noise to scene)",             "--"},
		{"R",           "Randomize the gravity vector",             "--"},
		{"L",           "Load campos.dat file",                     "--"},
		{"S",           "Save campos.dat file",                     "--"},
		{"F2-F7",       "Change the size of the radius vector",     "--"},
		{"Left-button", "Move forward",                             "--"},
		{"Mouse",       "Change direction",                         "--"},
		{NULL, NULL, NULL}
	};

	// Find the dimensions of the window

	int	widestKey = 0;
	int	widestDesc = 0;
	int	widestState = 0;
	int	tallestLine = 0;
	int	totalRows = 0;

	for(int i = 0; ; i++)
	{
		if (!sKeys[i].key) break;

		unsigned int	w, h;
		int		ox, oy;

		smallFont.getStringDimensions(sKeys[i].key, w, h, ox, oy);
		if (widestKey < w) widestKey = w;
		if (tallestLine < h) tallestLine = h;

		smallFont.getStringDimensions(sKeys[i].desc, w, h, ox, oy);
		if (widestDesc < w) widestDesc = w;
		if (tallestLine < h) tallestLine = h;

		smallFont.getStringDimensions(sKeys[i].state, w, h, ox, oy);
		if (widestState < w) widestState = w;
		if (tallestLine < h) tallestLine = h;

		totalRows++;
	}

const	int	columnGap = 10;
const	int	borderGap = 5;
const	int	rowGap = -5;
const	int	separatorGap = 5;

	// Actual window dimensions

	int	windowWidth = borderGap + widestKey + columnGap + widestDesc + columnGap + widestState + borderGap;
	int	windowHeight = borderGap + (tallestLine + rowGap) * totalRows + separatorGap + borderGap;

	point2	upperLeft = screenCenter();
	upperLeft.x() -= windowWidth / 2;
	upperLeft.y() -= windowHeight / 2;

	point2	lowerRight = screenCenter();
	lowerRight.x() += windowWidth / 2;
	lowerRight.y() += windowHeight / 2;

	dib().fillRect(upperLeft.x(), upperLeft.y(), lowerRight.x(), lowerRight.y(), 0x60000000);
	dib().line(upperLeft.x(), upperLeft.y(), lowerRight.x(), upperLeft.y(), 0x40ffffff);
	dib().line(upperLeft.x(), upperLeft.y(), upperLeft.x(), lowerRight.y(), 0x40ffffff);
	dib().line(upperLeft.x(), lowerRight.y(), lowerRight.x(), lowerRight.y(), 0x40ffffff);
	dib().line(lowerRight.x(), lowerRight.y(), lowerRight.x(), upperLeft.y(), 0x40ffffff);

	int	fx = upperLeft.x() + borderGap;
	int	fy = upperLeft.y() + borderGap;
	for(i = 0; i < totalRows; i++)
	{
		int	color = 0xff00ff00;

		// Is this the header?

		if (!i)
		{
			color = 0xffffffff;
		}

		int	cx = fx;

		smallFont.draw((unsigned int *) dib().frameBuffer(), dib().width(), dib().height(), cx, fy, sKeys[i].key, color);
		cx += widestKey + columnGap;

		smallFont.draw((unsigned int *) dib().frameBuffer(), dib().width(), dib().height(), cx, fy, sKeys[i].desc, color);
		cx += widestDesc + columnGap;

		smallFont.draw((unsigned int *) dib().frameBuffer(), dib().width(), dib().height(), cx, fy, sKeys[i].state, color);
		fy += tallestLine + rowGap;

		if (!i)
		{
			point2	p0 = upperLeft;
			p0.y() += borderGap + tallestLine + rowGap;
			point2	p1 = p0;
			p1.x() = lowerRight.x();
			dib().line(p0.x(), p0.y(), p1.x(), p1.y(), 0xffffffff);
			fy += separatorGap;
		}

	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Render::mouseMoved(const int relX, const int relY)
{
	yrot() += (FLT) relX * (FLT) -0.005;
	xrot() += (FLT) relY * (FLT)  0.005;

	// Clamp X rotation (look up/down)

	const	FLT	pi2 = (FLT) 3.141592654 / (FLT) 2;

	if (xrot() < -pi2) xrot() = -pi2;
	if (xrot() >  pi2) xrot() =  pi2;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Render::mousePressed(const bool left, const bool right)
{
	_lButton = left;
	_rButton = right;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Render.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
