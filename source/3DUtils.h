// ---------------------------------------------------------------------------------------------------------------------------------
//  ____  _____  _    _ _   _ _         _
// |___ \|  __ \| |  | | | (_) |       | |
//   __) | |  | | |  | | |_ _| |___    | |__
//  |__ <| |  | | |  | | __| | / __|   | '_ \
//  ___) | |__| | |__| | |_| | \__ \ _ | | | |
// |____/|_____/ \____/ \__|_|_|___/(_)|_| |_|
//
//
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

#ifndef	_H_3DUTILS
#define _H_3DUTILS

// ---------------------------------------------------------------------------------------------------------------------------------

FLT	frand();
point3	closestPointOnLineSegment(const point3 &a, const point3 &b, const point3 &p, bool &edge);
point3	closestPointOnLine(const point3 &a, const point3 &b, const point3 &p);
bool	isPointInsideEdge(const point3 &a, const point3 &b, const point3 &p);

#endif // _H_3DUTILS
// ---------------------------------------------------------------------------------------------------------------------------------
// 3DUtils.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

