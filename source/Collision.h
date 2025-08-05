// ---------------------------------------------------------------------------------------------------------------------------------
//   _____       _ _ _     _                 _     
//  / ____|     | | (_)   (_)               | |    
// | |      ___ | | |_ ___ _  ___  _ __     | |__  
// | |     / _ \| | | / __| |/ _ \| '_ \    | '_ \ 
// | |____| (_) | | | \__ \ | (_) | | | | _ | | | |
//  \_____|\___/|_|_|_|___/_|\___/|_| |_|(_)|_| |_|
//                                                 
//                                                 
//
// Collision detection tutorial
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
// Originally created on 12/11/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_H_COLLISION
#define _H_COLLISION

// ---------------------------------------------------------------------------------------------------------------------------------
// Required includes
// ---------------------------------------------------------------------------------------------------------------------------------

#include "Primitive.h"

// ---------------------------------------------------------------------------------------------------------------------------------

enum	CollisionType	{CT_NONE = 0, CT_SURFACE, CT_EDGE, CT_POINT, CT_EMBEDDED};

// ---------------------------------------------------------------------------------------------------------------------------------

typedef	struct
{
	primitive<>	*collider;
	plane3		collisionPlane;
	FLT		collisionDistance;
	CollisionType	collisionType;
} sCollision;

// ---------------------------------------------------------------------------------------------------------------------------------

typedef	vector<sCollision> CollisionList;

// ---------------------------------------------------------------------------------------------------------------------------------

class	collision
{
public:

static		point3		collideAndStop(const vector3 &radiusVector, vector<primitive<> > &polygonList, const ray3 &r);
static		point3		collideAndSlide(const vector3 &radiusVector, vector<primitive<> > &polygonList, const ray3 &r, vector3 &lastDir, const bool filterPuseJumps = false);

private:

static		void		traceCollision(CollisionList &cl);
static		bool		unitSphereIntersection(const point3 &center, const ray3 &r, FLT &time);
static		bool		isEmbedded(const primitive<> &p, const point3 &sphereCenter, point3 &innerMostPoint);
static		CollisionList	calcColliders(vector<primitive<> > &polygonList, const ray3 &r);

};

#endif // _H_COLLISION
// ---------------------------------------------------------------------------------------------------------------------------------
// Collision.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

