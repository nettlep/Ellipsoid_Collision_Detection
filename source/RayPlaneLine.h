// ---------------------------------------------------------------------------------------------------------------------------------
//  _____              _____  _                  _      _                _     
// |  __ \            |  __ \| |                | |    (_)              | |    
// | |__) | __ _ _   _| |__) | | __ _ _ __   ___| |     _ _ __   ___    | |__  
// |  _  / / _` | | | |  ___/| |/ _` | '_ \ / _ \ |    | | '_ \ / _ \   | '_ \ 
// | | \ \| (_| | |_| | |    | | (_| | | | |  __/ |____| | | | |  __/ _ | | | |
// |_|  \_\\__,_|\__, |_|    |_|\__,_|_| |_|\___|______|_|_| |_|\___|(_)|_| |_|
//                __/ |                                                        
//               |___/                                                         
//
// Generic class to represent rays, planes and lines of N dimensions
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
// Originally created on 12/20/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Note that necessary values (the 'D' value from the plane equation) and a normalized version of the direction vector are
// calculated on-demand and cached to improve performance.
//
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_H_RAYPLANELINE
#define _H_RAYPLANELINE

// ---------------------------------------------------------------------------------------------------------------------------------
// Required includes
// ---------------------------------------------------------------------------------------------------------------------------------

#include "VMath.h"

// ---------------------------------------------------------------------------------------------------------------------------------

template <unsigned int N = 3, class T = FLT>
class	ray
{
public:
	// Construction/Destruction

				ray() {}
				ray(const matrix<N, 1, T> &origin, const matrix<N, 1, T> &vector)
					:_origin(origin), _vector(vector), _nCalculated(false), _lCalculated(false), _dCalculated(false) {}
virtual				~ray() {}

	// Operators

inline		ray		&operator=(const ray &r)
				{
					_origin = r._origin;
					_vector = r._vector;
					_normal = r._normal;
					_length = r._length;
					_D = r._D;
					_nCalculated = r._nCalculated;
					_lCalculated = r._lCalculated;
					_dCalculated = r._dCalculated;
					return *this;
				}

	// Implementation

inline	const	T		distance(const matrix<N, 1, T> &point) const
				{
					T	t;
					intersect(ray(point, -normal()), t);
					return t;
				}

inline	const	bool		intersect(ray &r, T &time) const
				{
					time = 0;
					T	denom = normal() ^ r.normal();

					if (denom == 0) return false;

					T	numer = normal() ^ r.origin();

					time = -((numer + D()) / denom);

					return true;
				}

inline	const	matrix<N, 1, T>	closest(const matrix<N, 1, T> &point) const
				{
					T	t = distance(point);
					return point - normal() * t;
				}

inline	const	T		length() const
				{
					if (!_lCalculated)
					{
						_length = vector().length();
						_lCalculated = true;
					}

					return _length;
				}

inline		void		setLength(const T len)
				{
					// If it's already normalized, just multiply

					if (_nCalculated)	_vector = _normal * len;
					else			_vector.setLength(len);

					_length = len;
					_lCalculated = true;
				}

inline	const	T		D() const
				{
					if (!_dCalculated)
					{
						_D = -(origin() ^ normal());
						_dCalculated = true;
					}

					return _D;
				}

inline	const	matrix<N, 1, T>	&normal() const
				{
					if (!_nCalculated)
					{
						_normal = _vector;
						_normal.normalize();
						_nCalculated = true;
					}

					return _normal;
				}

inline	const	matrix<N, 1, T>	end() const
				{
					return _origin + _vector;
				}

inline	const	matrix<N, 1, T>	end(const T len) const
				{
					return _origin + normal() * len;
				}

inline		void		flush() const
				{
					_nCalculated = false;
					_lCalculated = false;
					_dCalculated = false;
				}

	// Accessors

inline	const	matrix<N, 1, T>	&origin() const		{                                                                  return _origin;}
inline		matrix<N, 1, T>	&origin()		{                                            _dCalculated = false; return _origin;}
inline	const	matrix<N, 1, T>	&vector() const		{                                                                  return _vector;}
inline		matrix<N, 1, T>	&vector()		{_nCalculated = false; _lCalculated = false; _dCalculated = false; return _vector;}

private:
	// Data members

		matrix<N, 1, T>	_origin;		// Origin of Ray/Plane/Line
		matrix<N, 1, T>	_vector;		// Direction of Ray/Plane
	mutable	matrix<N, 1, T>	_normal;		// Direction of Ray/Plane (normalized)
	mutable	T		_length;		// Length of the ray
	mutable	T		_D;			// 'D' of the plane equation

	mutable	bool		_nCalculated;
	mutable	bool		_lCalculated;
	mutable	bool		_dCalculated;
};

// ---------------------------------------------------------------------------------------------------------------------------------
// For convenience
// ---------------------------------------------------------------------------------------------------------------------------------

typedef	ray<2>		ray2;
typedef	ray<3>		ray3;
typedef	ray<4>		ray4;
typedef	ray<2>		plane2;
typedef	ray<3>		plane3;
typedef	ray<4>		plane4;

#endif // _H_RAYPLANELINE
// ---------------------------------------------------------------------------------------------------------------------------------
// RayPlaneLine.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

