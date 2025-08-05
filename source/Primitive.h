// ---------------------------------------------------------------------------------------------------------------------------------
//  _____       _           _ _   _                _     
// |  __ \     (_)         (_) | (_)              | |    
// | |__) |_ __ _ _ __ ___  _| |_ ___   __ ___    | |__  
// |  ___/| '__| | '_ ` _ \| | __| \ \ / // _ \   | '_ \ 
// | |    | |  | | | | | | | | |_| |\ V /|  __/ _ | | | |
// |_|    |_|  |_|_| |_| |_|_|\__|_| \_/  \___|(_)|_| |_|
//                                                       
//                                                       
//
// Generic N-dimensional primitive class for points, lines, polygons
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

#ifndef	_H_PRIMITIVE
#define _H_PRIMITIVE

// ---------------------------------------------------------------------------------------------------------------------------------
// Required includes
// ---------------------------------------------------------------------------------------------------------------------------------

#include "VMath.h"
#include "Vertex.h"
#include "3DUtils.h"
#include "RayPlaneLine.h"

// ---------------------------------------------------------------------------------------------------------------------------------

template <unsigned int N = 3, class T = FLT>
class	primitive
{
public:
	// Construction/Destruction

				primitive() {}
inline				primitive(const primitive &p)
					: _vertices(p.vertices()), _plane(p.plane()), _userFlags(p.userFlags()) {}

	// Operators

inline		primitive	&operator =(const primitive &p)		// Assignment operator
				{
					if (this == &p) return *this;

					vertices() = p.vertices();
					plane() = p.plane();
					userFlags() = p.userFlags();

					return *this;
				}

inline	const	vert<>		&operator [](const int index) const	// vert indexing (read only)
				{
					return vertices()[index];
				}

inline		vert<>		&operator [](const int index)		// vert indexing (read/write)
				{
					return vertices()[index];
				}

inline		primitive	&operator +=(const vert<> &v)		// Add a vert to the end
				{
					vertices().push_back(v);
					return *this;
				}

inline		primitive	&operator --()				// Remove a vert from the end
				{
					vertices().pop_back();
					return *this;
				}

	// Utilitarian

virtual		void		setWorldTexture(const T uScale = (T) 1, const T vScale = (T) 1)
				{
					// Calculate |normal|

					vector3	absNormal = plane().normal();
					absNormal.abs();

					// Primary axis == X

					if (absNormal.x() >= absNormal.y() && absNormal.x() >= absNormal.z())
					{
						for (vector<vert<T> >::iterator i = vertices().begin(); i != vertices().end(); ++i)
						{
							i->texture().x() =  i->world().z() * uScale;
							i->texture().y() = -i->world().y() * vScale;
						}
					}

					// Primary axis == Y

					else if (absNormal.y() >= absNormal.x() && absNormal.y() >= absNormal.z())
					{
						for (vector<vert<T> >::iterator i = vertices().begin(); i != vertices().end(); ++i)
						{
							i->texture().x() =  i->world().x() * uScale;
							i->texture().y() = -i->world().z() * vScale;
						}
					}

					// Primary axis == Z

					else
					{
						for (vector<vert<T> >::iterator i = vertices().begin(); i != vertices().end(); ++i)
						{
							i->texture().x() =  i->world().x() * uScale;
							i->texture().y() = -i->world().y() * vScale;
						}
					}
				}

inline	const	point3		calcCenterOfMass() const
				{
					point3	center((T) 0, (T) 0, (T) 0);
					if (vertexCount() < 1) return center;

					for (vector<vert<T> >::const_iterator i = vertices().begin(); i != vertices().end(); ++i)
					{
						center += *i;
					}

					return center * ((T) 1 / (T) vertexCount());
				}

inline		void		calcPlane(const bool counterClock = true)
				{
					plane().origin() = vertices()[0];
					vector3	v0 = vertices()[1].world() - vertices()[0].world();
					vector3	v1 = vertices()[2].world() - vertices()[1].world();
					plane().vector() = v1 % v0;
					if (!counterClock) plane().vector() = -plane().vector();
				}

				// -------------------------------------------------------------------------------------------------
				// This calcArea() routine works for convex & concave polygons. It was adapted from a 2D algorithm
				// presented in Computer Graphics Principles & Practice 2ed (Foley/vanDam/Feiner/Hughes) p. 477
				// -------------------------------------------------------------------------------------------------

inline		T		calcArea() const
				{
					T	xyArea = (T) 0;
					T	yzArea = (T) 0;
					T	zxArea = (T) 0;

					point3	p0 = vertices().back().world();

					for (vector<vert<T> >::const_iterator i = vertices().begin(); i != vertices().end(); ++i)
					{
						point3	p1 = i->world();
						xyArea += (p0.y() + p1.y()) * (p1.x() - p0.x()) / 2.0;
						yzArea += (p0.z() + p1.z()) * (p1.y() - p0.y()) / 2.0;
						zxArea += (p0.x() + p1.x()) * (p1.z() - p0.z()) / 2.0;
						p0 = p1;
					}

					return sqrt(xyArea * xyArea + yzArea * yzArea + zxArea * zxArea);
				}

inline		bool		inside(const point3 &p, const T epsilon = (T) 0) const
				{
					int	pos = 0;
					int	neg = 0;
					point3	center = calcCenterOfMass();

					vector<vert<T> >::const_iterator j = vertices().begin();
					++j;
					if (j == vertices().end()) j = vertices().begin();
					for (vector<vert<T> >::const_iterator i = vertices().begin(); i != vertices().end(); ++i, ++j)
					{
						if (j == vertices().end()) j = vertices().begin();

						// The current edge

						const point3	&p0 = i->world();
						const point3	&p1 = j->world();

						// Generate a normal for this edge

						vector3	n = (p1 - p0) % plane().normal();

						// Which side of this edge-plane is the point?

						T	halfPlane = (p ^ n) - (p0 ^ n);

						// Keep track of positives & negatives (but not zeros -- which means it's on the edge)

						if (halfPlane > epsilon) pos++;
						else if (halfPlane < -epsilon) neg++;
					}

					// If they're ALL positive, or ALL negative, then it's inside

					if (!pos || !neg) return true;

					// Must not be inside, because some were pos and some were neg

					return false;

				}

				// This one is less accurate and slower, but considered "standard"

inline		bool		inside2(const point3 &p, const T epsilon = (T) 0) const
				{
					T	total = (T) (-2.0 * 3.141592654);

					point3	p0 = p - vertices().back().world();
					p0.normalize();

					for (vector<vert<T> >::const_iterator i = vertices().begin(); i != vertices().end(); ++i)
					{
						point3	p1 = p - i->world();
						p1.normalize();
						T	t = p0 ^ p1;
						// Protect acos() input
						if (t < -1) t = -1;
						if (t > 1) t = 1;
						total += acos(t);
						p0 = p1;
					}

					if (fabs(total) > epsilon) return false;
					return true;
				}

inline		point3		closestPointOnPerimeter(const point3 &p, point3 &e0, point3 &e1, bool &edgeFlag) const
				{
					bool	found = false;
					T	closestDistance = (T) 0;
					point3	closestPoint = point3::zero();
					point3	closestP0, closestP1;
					int	closestIndex;

					point3	p0 = vertices().back().world();

					int	index = 0;
					for (vector<vert<T> >::const_iterator i = vertices().begin(); i != vertices().end(); ++i, ++index)
					{
						point3	p1 = i->world();
						bool	edge;
						point3	cp = closestPointOnLineSegment(p0, p1, p, edge);
						T	d = cp.distance(p);

						if (!found || d < closestDistance)
						{
							closestDistance = d;
							closestPoint = cp;
							closestP0 = p0;
							closestP1 = p1;
							edgeFlag = edge;
							closestIndex = index;
							found = true;
						}

						p0 = p1;
					}

					if (!edgeFlag)
					{
						int	a = closestIndex - 1; if (a < 0) a = vertexCount() - 1;
						int	b = closestIndex + 1; if (b >= vertexCount()) b = 0;
						e0 = vertices()[a].world();
						e1 = vertices()[b].world();
					}
					else
					{
						e0 = closestP0;
						e1 = closestP1;
					}

					return closestPoint;
				}

	// Accessors

inline	const	unsigned int	vertexCount() const	{return vertices().size();}
inline	const	vector<vert<T> >&vertices() const	{return _vertices;}
inline		vector<vert<T> >&vertices()		{return _vertices;}
inline	const	plane3		&plane() const		{return _plane;}
inline		plane3		&plane()		{return _plane;}
inline	const	unsigned int	&userFlags() const	{return _userFlags;}
inline		unsigned int	&userFlags() 		{return _userFlags;}

protected:

	// Data

		vector<vert<T> >_vertices;
		ray<N, T>	_plane;
		unsigned int	_userFlags;
};

#endif // _H_PRIMITIVE
// ---------------------------------------------------------------------------------------------------------------------------------
// Primitive.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

