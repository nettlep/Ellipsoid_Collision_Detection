// ---------------------------------------------------------------------------------------------------------------------------------
//   _____       _ _ _     _
//  / ____|     | | (_)   (_)
// | |      ___ | | |_ ___ _  ___  _ __       ___ _ __  _ __
// | |     / _ \| | | / __| |/ _ \| '_ \     / __| '_ \| '_ \
// | |____| (_) | | | \__ \ | (_) | | | | _ | (__| |_) | |_) |
//  \_____|\___/|_|_|_|___/_|\___/|_| |_|(_) \___| .__/| .__/
//                                               | |   | |
//                                               |_|   |_|
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

#include "stdafx.h"
#include "Common.h"
#include "Collision.h"

// ---------------------------------------------------------------------------------------------------------------------------------

const	FLT	EPSILON = (FLT) 1.0e-5;

// ---------------------------------------------------------------------------------------------------------------------------------

static	vector<primitive<> *>	skipList;

// ---------------------------------------------------------------------------------------------------------------------------------

void	collision::traceCollision(CollisionList &cl)
{
	if (!cl.size()) return;

	static	int	traceCount;

	TRACE("[%4d] %d colliding surface(s):", traceCount++, cl.size());

	for (int i = 0; i < cl.size(); i++)
	{
		const char	*typeString = cl[i].collisionType == CT_SURFACE ? "surface" : cl[i].collisionType == CT_EDGE ? "edge" : cl[i].collisionType == CT_POINT ? "point" : "embedded";
		if (!i) TRACE("  ");
		else	TRACE("                                ");
		TRACE("ID[%3d] ", (int) cl[i].collider & 0x1FF);
		TRACE("dist[%+12.7f] ", cl[i].collisionDistance);
		if (fmod(cl[i].collisionDistance, 1) == 0) TRACE("* "); else TRACE("  ");
		TRACE("type[%8s] ", typeString);
		TRACE("nx[%+12.7f] ", cl[i].collisionPlane.normal().x());
		if (fmod(cl[i].collisionPlane.normal().x(), 1) == 0) TRACE("* "); else TRACE("  ");
		TRACE("ny[%+12.7f] ", cl[i].collisionPlane.normal().y());
		if (fmod(cl[i].collisionPlane.normal().y(), 1) == 0) TRACE("* "); else TRACE("  ");
		TRACE("nz[%+12.7f] ", cl[i].collisionPlane.normal().z());
		if (fmod(cl[i].collisionPlane.normal().z(), 1) == 0) TRACE("* "); else TRACE("  ");
		TRACE("\n");
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	collision::unitSphereIntersection(const point3 &center, const ray3 &r, FLT &time)
{
	vector3	q = center - r.origin();
	FLT	c = q.length();
	FLT	v = q ^ r.normal();
	FLT	d = 1 - (c*c - v*v);

	// Was there an intersection?

	if (d < 0)
	{
		time = 0;
		return false;
	}

	// Return the distance to the [first] intersecting point

	time = v - sqrt(d);
	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	collision::isEmbedded(const primitive<> &p, const point3 &sphereCenter, point3 &innerMostPoint)
{
	// How far is the sphere from the plane?

	FLT	t = p.plane().distance(sphereCenter);

	// If the plane is farther than the radius of the sphere, it's not embedded

	if (t > 1 - EPSILON) return false;

	// Find the closest point on the polygon to the center of the sphere

	innerMostPoint = sphereCenter - p.plane().normal() * t;

	// If the closest point on the plane is within the polygon, the polygon is embedded

	if (!p.inside(innerMostPoint))
	{
		point3	e0, e1;
		bool	ef;
		innerMostPoint = p.closestPointOnPerimeter(innerMostPoint, e0, e1, ef);
		if (innerMostPoint.distance(sphereCenter) > 1 - EPSILON) return false;
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

CollisionList collision::calcColliders(vector<primitive<> > &polygonList, const ray3 &r)
{
	CollisionList	result;

	// Go through our list of potential colliders

	for (int i = 0; i < polygonList.size(); i++)
	{
		// The goal through this loop is to calculate the collision plane

		FLT		collisionDistance;
		plane3		collisionPlane;
		CollisionType	collisionType = CT_NONE;

		// Our current polygon

		primitive<>	&p = polygonList[i];

		// Ignore polygons in the skip list

		bool	skipMe = false;
		for (int j = 0; j < skipList.size(); j++)
		{
			if (skipList[j] == &p) skipMe = true;
		}
		if (skipMe) continue;

		// Ignore back-facing polygons

		if (p.plane().distance(r.origin()) <= 0) continue;

		// Find the point on the sphere, that will eventually collide with the polygon's plane

		point3	sphereIntersectionPoint = r.origin() - p.plane().normal();

		// At this point, we know we're in front of the current plane, and we're heading toward it
		//
		// Trace a ray to the plane

		ray3	sphereIntersectionRay(sphereIntersectionPoint, r.normal());
		if (!p.plane().intersect(sphereIntersectionRay, collisionDistance)) continue;

		// If the polygon is embedded, set the collision type, and define a collision plane that is perpendicular
		// to the direction that the sphere must travel to be un-embedded

		point3	innerMostPoint;
		if (isEmbedded(p, r.origin(), innerMostPoint))
		{
			collisionDistance = innerMostPoint.distance(r.origin()) - 1;
			collisionPlane.origin() = innerMostPoint;
			collisionPlane.vector() = r.origin() - innerMostPoint;
			collisionType = CT_EMBEDDED;
		}

		// Surface/Edge/Point collision

		else

		{
			// The plane intersection point

			point3	planeIntersectionPoint = sphereIntersectionPoint + r.normal() * collisionDistance;

			// Surface collision?

			if (p.inside(planeIntersectionPoint, EPSILON))
			{
				// Is this a valid collision?

				if (collisionDistance < -EPSILON * (FLT) 2) continue;

				// Our collision plane is our polygon's plane

				collisionPlane = p.plane();
				collisionType = CT_SURFACE;
			}

			// Edge/Point collision

			else
			{
				// New collision plane origin

				point3	e0, e1;
				bool	edgeFlag;
				collisionPlane.origin() = p.closestPointOnPerimeter(planeIntersectionPoint, e0, e1, edgeFlag);

				// Point collision?

				if (!edgeFlag)
				{
					ray3	toSphere(collisionPlane.origin(), -r.normal());
					if (!unitSphereIntersection(r.origin(), toSphere, collisionDistance)) continue;

					// The collision plane

					sphereIntersectionPoint = collisionPlane.origin() - r.normal() * collisionDistance;
					collisionPlane.vector() = r.origin() - sphereIntersectionPoint;

					// Generate a vector that points from the vertex at the sphere

					vector3	atSphere = r.origin() - collisionPlane.origin();

					// We must be headed at the collision plane
					
					if ((r.vector() ^ atSphere) > 0) continue;

					// The plane is not allowed to face away from the normal of the polygon

					if ((collisionPlane.vector() ^ p.plane().vector()) < 0) continue;

					// The plane is not allowed to face the interior of the polygon

					if (collisionPlane.distance(e0) > 0) continue;
					if (collisionPlane.distance(e1) > 0) continue;

					collisionType = CT_POINT;
				}

				// Edge collision

				else
				{
					// Find the plane defined by the edge and the velocity vector (cross product)
					//
					// This plane will be used to bisect the sphere

					vector3	edgeNormal = e1 - e0;
					edgeNormal.normalize();

					float	edgeDot = fabs(r.normal() ^ edgeNormal);
					if (edgeDot > 1-EPSILON) continue;
					plane3	bisectionPlane(e1, r.normal() % edgeNormal);

					// The intersection of a plane and a sphere is a disc. We want to find the center
					// of that disc. The center of that disc is the closest point on the bisection plane
					// to the center of the sphere.

					point3	discCenter = bisectionPlane.closest(r.origin());

					// If the center of the disc is outside the sphere, then the sphere does not intersect
					// the bisection plane and therefore, will never collide with the edge

					if (discCenter.distance(r.origin()) > 1-EPSILON) continue;

					// Find the closest point on the edge to the center of the disc

					point3	edgePoint = closestPointOnLine(e0, e1, discCenter);

					// Generate a ray that traces back toward the sphere

					ray3	toSphere(edgePoint, discCenter - edgePoint);

					// Trace from the edgePoint back to the sphere.
					//
					// This will be the sphereIntersectionPoint
					
					FLT	t;
					if (!unitSphereIntersection(r.origin(), toSphere, t)) continue;
					sphereIntersectionPoint = toSphere.end(t);

					// Will the sphere interesction point, when travelling along 'r' ever intersect the actual
					// edge?
					//
					// In order to determine this, we'll start by defining a plane that lies on the edge, but is
					// perpendicular to 'r'.

					vector3	edgeVect = (e1 - e0) % r.normal() % (e1 - e0);
					edgeVect.normalize();
					plane3	edgePlane(e0, edgeVect);

					// Next, we'll intersect the edgeVect ray with the plane

					ray3 toLine(sphereIntersectionPoint, r.normal());
					edgePlane.intersect(toLine, t);

					// Finally, does the result end up with a point inside the line segment defined by the
					// two edge points? If not, then the sphere won't intersect the edge.

					if (!isPointInsideEdge(e0, e1, toLine.end(t))) continue;

					// Our collision plane is the tangent plane at the sphereIntersectionPoint

					collisionPlane.vector() = r.origin() - sphereIntersectionPoint;

					// The plane is not allowed to face the interior of the polygon...

					if (collisionPlane.distance(p.calcCenterOfMass()) > -0) continue;

					// The plane is not allowed to face away from the normal of the polygon

					if ((collisionPlane.normal() ^ p.plane().normal()) < 0) continue;

					// Trace from the sphereIntersectionPoint to the plane to find the collisionDistance

					ray3	fromSphere(sphereIntersectionPoint, r.normal());
					VERIFY(collisionPlane.intersect(fromSphere, collisionDistance));

					// Don't allow collisions behind us

					if (collisionDistance < -1) continue;

					collisionType = CT_EDGE;
				}
			}
		}

		// Was it an actual collision?

		if (collisionType != CT_EMBEDDED && collisionDistance > r.length()) continue;

		// Ignore collision planes that we're traveling away from

		if (collisionType != CT_EMBEDDED && (collisionPlane.normal() ^ r.normal()) >= 0) continue;

		// Is it closer, farther away, or the same distance as what we've found so far?

		if (result.size())
		{
			FLT	d = fabs(collisionDistance - result[0].collisionDistance);
			if (d > EPSILON)
			{
				if (collisionDistance < result[0].collisionDistance) result.clear();
				else if (collisionDistance > result[0].collisionDistance) continue;
			}

			// Make sure it's not already in the list (this plane's normal)

			else
			{
				bool	skipMe = false;
				for (int i = 0; i < result.size(); i++)
				{
					if ((result[i].collisionPlane.normal() ^ collisionPlane.normal()) > 1 - EPSILON)
					{
						skipMe = true;
						break;
					}
				}
				if (skipMe) continue;
			}
		}

		// Add this collider to the list

		sCollision	nearest;
		nearest.collider = &p;
		nearest.collisionDistance = collisionDistance;
		nearest.collisionPlane = collisionPlane;
		nearest.collisionType = collisionType;
		result.push_back(nearest);
	}

	return result;
}

// ---------------------------------------------------------------------------------------------------------------------------------

point3	collision::collideAndStop(const vector3 &radiusVector, vector<primitive<> > &polygonList, const ray3 &r)
{
	// Scale our world according to the radius vector

	matrix3	scaleMatrix = matrix3::scale((FLT) 1 / radiusVector);
	vector<primitive<> > scaledPolygons = polygonList;

	for (vector<primitive<> >::iterator i = scaledPolygons.begin(); i != scaledPolygons.end(); ++i)
	{
		primitive<>	&p = *i;
		for (unsigned int j = 0; j < p.vertexCount(); j++)
		{
			p[j].world() = scaleMatrix >> p[j].world();
		}

		p.calcPlane(false);
	}

	// Scale our input & output rays

	ray3	outputRay(r);
	outputRay.vector() = scaleMatrix >> outputRay.vector();
	outputRay.origin() = scaleMatrix >> outputRay.origin();

	// Get the colliders

	CollisionList	cl = calcColliders(scaledPolygons, outputRay);

	point3	dest = outputRay.end();

	// If we didn't collide, just go all the way

	if (cl.size() && cl[0].collisionDistance < outputRay.length())
	{
		dest = outputRay.end(cl[0].collisionDistance);
	}

	// Return a point at the destination where the collision took place

	scaleMatrix = matrix3::scale(radiusVector);
	return scaleMatrix >> dest;
}

// ---------------------------------------------------------------------------------------------------------------------------------

point3	collision::collideAndSlide(const vector3 &radiusVector, vector<primitive<> > &polygonList, const ray3 &r, vector3 &lastDir, const bool filterPulseJumps)
{
	unsigned int	maxLoops = 30;
	lastDir = vector3::zero();

	// Scale our world according to the radius vector

	matrix3	scaleMatrix = matrix3::scale((FLT) 1 / radiusVector);
	vector<primitive<> > scaledPolygons = polygonList;

	for (vector<primitive<> >::iterator i = scaledPolygons.begin(); i != scaledPolygons.end(); ++i)
	{
		primitive<>	&p = *i;
		for (unsigned int j = 0; j < p.vertexCount(); j++)
		{
			p[j].world() = scaleMatrix >> p[j].world();
		}

		p.calcPlane(false);
	}

	// Scale our input & output rays

	ray3	outputRay(r);
	outputRay.vector() = scaleMatrix >> outputRay.vector();
	outputRay.origin() = scaleMatrix >> outputRay.origin();

	ray3	inputRay(r);
	inputRay.vector() = scaleMatrix >> inputRay.vector();
	inputRay.origin() = scaleMatrix >> inputRay.origin();

	// Clear the skip list

	skipList.clear();

	// Skiding is an iterative process

	while(maxLoops-- && outputRay.vector() != vector3::zero() && outputRay.length() > EPSILON)
	{
		// This is handy

		lastDir = outputRay.vector();

		// Get the colliders

		CollisionList	cl = calcColliders(scaledPolygons, outputRay);

		// We can consider these polygons again

		skipList.clear();

		// If we didn't hit anything just return the destination

		if (!cl.size())
		{
			outputRay.origin() = outputRay.end();
			break;
		}

		// Keep track of what's going on

		traceCollision(cl);

		// If it was embedded, push away

		if (cl[0].collisionType == CT_EMBEDDED)
		{
			// If it's embedded, back up along the collision plane normal

			outputRay.origin() -= cl[0].collisionPlane.normal() * cl[0].collisionDistance;
		}
		else
		{
			// We hit something -- move as far as we can

			outputRay.origin() += outputRay.normal() * cl[0].collisionDistance;

			// If we hit two or more, find the two with the most "pressure"

			if (cl.size() >= 2)
			{
				int	c0;
				FLT	d0 = 2;

				for (int i = 0; i < cl.size(); i++)
				{
					FLT	dot = cl[i].collisionPlane.normal() ^ outputRay.normal();
					if (dot < d0)
					{
						d0 = dot;
						c0 = i;
					}
				}

				int	c1;
				FLT	d1 = 2;

				for (int j = 0; j < cl.size(); j++)
				{
					if (j == c0) continue;
					FLT	dot = cl[j].collisionPlane.normal() ^ outputRay.normal();
					if (dot < d1)
					{
						d1 = dot;
						c1 = j;
					}
				}


				// Temp vector

				vector3	tempV = outputRay.vector();

				// Point along the crease

				vector3	perp(cl[c1].collisionPlane.normal() % cl[c0].collisionPlane.normal());
				perp.abs();
				tempV *= perp;

				if (tempV.length() < EPSILON) continue;

				tempV.setLength(outputRay.vector().length());

				// Find out how much to scale our vector that points along the crease
				
				vector3	n1 = tempV;			n1.normalize();
				vector3	n2 = outputRay.vector();	n2.normalize();
				FLT	scalar = n1 ^ n2;

				// New vector is new direction vector, scaled

				outputRay.vector() = tempV * scalar;

				if (filterPulseJumps)
				{
					if ((outputRay.vector() ^ inputRay.vector()) < 0) outputRay.vector() = vector3::zero();
				}

				lastDir = outputRay.vector();

				// Don't collide with these next time

				skipList.push_back(cl[c0].collider);
				skipList.push_back(cl[c1].collider);

				// Iterate

				continue;
			}

			// Otherwise we slide

			point3	slideSrc = cl[0].collisionPlane.closest(outputRay.origin());
			point3	slideDst = cl[0].collisionPlane.closest(outputRay.end());

			outputRay.vector() = slideDst - slideSrc;

			if (filterPulseJumps)
			{
				if ((outputRay.vector() ^ inputRay.vector()) < 0) outputRay.vector() = vector3::zero();
			}

			lastDir = outputRay.vector();

			// Don't collide with this one next time

			skipList.push_back(cl[0].collider);
		}
	}

	// Un-scale our result

	scaleMatrix = matrix3::scale(radiusVector);
	lastDir = scaleMatrix >> lastDir;
	return scaleMatrix >> outputRay.origin();
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Collision.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

