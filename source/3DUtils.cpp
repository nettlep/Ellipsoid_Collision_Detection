// ---------------------------------------------------------------------------------------------------------------------------------
//  ____  _____  _    _ _   _ _
// |___ \|  __ \| |  | | | (_) |
//   __) | |  | | |  | | |_ _| |___      ___ _ __  _ __
//  |__ <| |  | | |  | | __| | / __|    / __| '_ \| '_ \
//  ___) | |__| | |__| | |_| | \__ \ _ | (__| |_) | |_) |
// |____/|_____/ \____/ \__|_|_|___/(_) \___| .__/| .__/
//                                          | |   | |
//                                          |_|   |_|
//
// Utilitarian funcitons useful for 3D graphics
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

#include "stdafx.h"
#include "Common.h"
#include "VMath.h"
#include "3DUtils.h"

// ---------------------------------------------------------------------------------------------------------------------------------
// Returns a random number between 0...1
// ---------------------------------------------------------------------------------------------------------------------------------

FLT	frand()
{
	return (FLT) rand() / (FLT) RAND_MAX;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Returns the closest point to 'p' on the line-segment defined by a & b. Sets 'edge' to true if result is one of the endpoints
// ---------------------------------------------------------------------------------------------------------------------------------

point3	closestPointOnLineSegment(const point3 &a, const point3 &b, const point3 &p, bool &edge)
{
	vector3 c = p - a;
	vector3 v = b - a;
	v.normalize();

	edge = false;
	FLT	t = v ^ c;
	if (t < 0) return a;

	FLT	d = b.distance(a);
	if (t > d) return b;

	edge = true;
	return a + v * t;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Returns the closest point to 'p' on the line defined by 'a' and 'b'
// ---------------------------------------------------------------------------------------------------------------------------------

point3	closestPointOnLine(const point3 &a, const point3 &b, const point3 &p)
{
	vector3 c = p - a;
	vector3 v = b - a;
	v.normalize();

	FLT	t = v ^ c;
	FLT	d = b.distance(a);
	return a + v * t;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Returns true/false if point 'p' is between 'a' and 'b'. This routine assumes that all three points are linear.
// ---------------------------------------------------------------------------------------------------------------------------------

bool	isPointInsideEdge(const point3 &a, const point3 &b, const point3 &p)
{
	return (vector3(a-p) ^ vector3(b-p)) <= 0;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// 3DUtils.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

