// ---------------------------------------------------------------------------------------------------------------------------------
//   _____ _ _                            
//  / ____| (_)                           
// | |    | |_ _ __       ___ _ __  _ __  
// | |    | | | '_ \     / __| '_ \| '_ \ 
// | |____| | | |_) | _ | (__| |_) | |_) |
//  \_____|_|_| .__/ (_) \___| .__/| .__/ 
//            | |            | |   | |    
//            |_|            |_|   |_|    
//
// 3D Homogenous clipper
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
#include "Primitive.h"
#include "Clip.h"

// ---------------------------------------------------------------------------------------------------------------------------------

static	inline	void	nClip(point4 &dst, const point4 &on, const point4 &off, point2 &tdst, const point2 &ton, const point2 &toff)
{
	FLT	delta = -on.z() / (off.z() - on.z());
	dst = on + (off - on) * delta;
	tdst = ton + (toff - ton) * delta;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	inline	void	fClip(point4 &dst, const point4 &on, const point4 &off, point2 &tdst, const point2 &ton, const point2 &toff)
{
	FLT	delta = (on.w() - on.z()) / ((off.z() + on.w()) - (on.z() + off.w()));
	dst = on + (off - on) * delta;
	tdst = ton + (toff - ton) * delta;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	inline	void	lClip(point4 &dst, const point4 &on, const point4 &off, point2 &tdst, const point2 &ton, const point2 &toff)
{
	FLT	delta = (-on.x() - on.w()) / (off.w() - on.w() + off.x() - on.x());
	dst = on + (off - on) * delta;
	tdst = ton + (toff - ton) * delta;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	inline	void	rClip(point4 &dst, const point4 &on, const point4 &off, point2 &tdst, const point2 &ton, const point2 &toff)
{
	FLT	delta = (on.x() - on.w()) / (off.w() - on.w() - off.x() + on.x());
	dst = on + (off - on) * delta;
	tdst = ton + (toff - ton) * delta;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	inline	void	tClip(point4 &dst, const point4 &on, const point4 &off, point2 &tdst, const point2 &ton, const point2 &toff)
{
	FLT	delta = (on.y() - on.w()) / (off.w() - on.w() - off.y() + on.y());
	dst = on + (off - on) * delta;
	tdst = ton + (toff - ton) * delta;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	inline	void	bClip(point4 &dst, const point4 &on, const point4 &off, point2 &tdst, const point2 &ton, const point2 &toff)
{
	FLT	delta = (-on.y() - on.w()) / (off.w() - on.w() + off.y() - on.y());
	dst = on + (off - on) * delta;
	tdst = ton + (toff - ton) * delta;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	unsigned int	neClip(primitive<> &src)
{
	primitive<>	dst;
	vert<>		tmp;

	for (unsigned int i = 0; i < src.vertexCount(); i++)
	{
		point4		&cur = src[i].screen();
		point2		&tcur = src[i].texture();
		point4		&nex = (i == src.vertexCount()-1) ? src[0].screen():src[i+1].screen();
		point2		&tnex = (i == src.vertexCount()-1) ? src[0].texture():src[i+1].texture();

		switch((cur.z() < 0.0 ? 1:0)|(nex.z() < 0.0 ? 2:0))
		{
			case 0:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp;						break;
			case 1:                                                       nClip(tmp.screen(), nex, cur, tmp.texture(), tnex, tcur); dst += tmp; break;
			case 2:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp; nClip(tmp.screen(), cur, nex, tmp.texture(), tcur, tnex); dst += tmp; break;
		}
	}
	src = dst;
	return src.vertexCount();
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	unsigned int	feClip(primitive<> &src)
{
	primitive<>	dst;
	vert<>		tmp;

	for (unsigned int i = 0; i < src.vertexCount(); i++)
	{
		point4		&cur = src[i].screen();
		point2		&tcur = src[i].texture();
		point4		&nex = (i == src.vertexCount()-1) ? src[0].screen():src[i+1].screen();
		point2		&tnex = (i == src.vertexCount()-1) ? src[0].texture():src[i+1].texture();

		switch((cur.z() > cur.w() ? 1:0)|(nex.z() > nex.w() ? 2:0))
		{
			case 0:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp;						break;
			case 1:                                                       fClip(tmp.screen(), nex, cur, tmp.texture(), tnex, tcur); dst += tmp; break;
			case 2:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp; fClip(tmp.screen(), cur, nex, tmp.texture(), tcur, tnex); dst += tmp; break;
		}
	}
	src = dst;
	return src.vertexCount();
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	unsigned int	leClip(primitive<> &src)
{
	primitive<>	dst;
	vert<>		tmp;

	for (unsigned int i = 0; i < src.vertexCount(); i++)
	{
		point4		&cur = src[i].screen();
		point2		&tcur = src[i].texture();
		point4		&nex = (i == src.vertexCount()-1) ? src[0].screen():src[i+1].screen();
		point2		&tnex = (i == src.vertexCount()-1) ? src[0].texture():src[i+1].texture();

		switch((cur.x() < -cur.w() ? 1:0)|(nex.x() < -nex.w() ? 2:0))
		{
			case 0:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp;						break;
			case 1:                                                       lClip(tmp.screen(), nex, cur, tmp.texture(), tnex, tcur); dst += tmp; break;
			case 2:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp; lClip(tmp.screen(), cur, nex, tmp.texture(), tcur, tnex); dst += tmp; break;
		}
	}
	src = dst;
	return src.vertexCount();
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	unsigned int	reClip(primitive<> &src)
{
	primitive<>	dst;
	vert<>		tmp;

	for (unsigned int i = 0; i < src.vertexCount(); i++)
	{
		point4		&cur = src[i].screen();
		point2		&tcur = src[i].texture();
		point4		&nex = (i == src.vertexCount()-1) ? src[0].screen():src[i+1].screen();
		point2		&tnex = (i == src.vertexCount()-1) ? src[0].texture():src[i+1].texture();

		switch((cur.x() > cur.w() ? 1:0)|(nex.x() > nex.w() ? 2:0))
		{
			case 0:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp;                                            break;
			case 1:	                                                      rClip(tmp.screen(), nex, cur, tmp.texture(), tnex, tcur); dst += tmp; break;
			case 2:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp; rClip(tmp.screen(), cur, nex, tmp.texture(), tcur, tnex); dst += tmp; break;
		}
	}
	src = dst;
	return src.vertexCount();
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	unsigned int	teClip(primitive<> &src)
{
	primitive<>	dst;
	vert<>		tmp;

	for (unsigned int i = 0; i < src.vertexCount(); i++)
	{
		point4		&cur = src[i].screen();
		point2		&tcur = src[i].texture();
		point4		&nex = (i == src.vertexCount()-1) ? src[0].screen():src[i+1].screen();
		point2		&tnex = (i == src.vertexCount()-1) ? src[0].texture():src[i+1].texture();

		switch((cur.y() > cur.w() ? 1:0)|(nex.y() > nex.w() ? 2:0))
		{
			case 0:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp;                                            break;
			case 1:	                                                      tClip(tmp.screen(), nex, cur, tmp.texture(), tnex, tcur); dst += tmp; break;
			case 2:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp; tClip(tmp.screen(), cur, nex, tmp.texture(), tcur, tnex); dst += tmp; break;
		}
	}
	src = dst;
	return src.vertexCount();
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	unsigned int	beClip(primitive<> &src)
{
	primitive<>	dst;
	vert<>		tmp;

	for (unsigned int i = 0; i < src.vertexCount(); i++)
	{
		point4		&cur = src[i].screen();
		point2		&tcur = src[i].texture();
		point4		&nex = (i == src.vertexCount()-1) ? src[0].screen():src[i+1].screen();
		point2		&tnex = (i == src.vertexCount()-1) ? src[0].texture():src[i+1].texture();

		switch((cur.y() < -cur.w() ? 1:0)|(nex.y() < -nex.w() ? 2:0))
		{
			case 0:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp;                                            break;
			case 1:	                                                      bClip(tmp.screen(), nex, cur, tmp.texture(), tnex, tcur); dst += tmp; break;
			case 2:	tmp.screen() = cur; tmp.texture() = tcur; dst += tmp; bClip(tmp.screen(), cur, nex, tmp.texture(), tcur, tnex); dst += tmp; break;
		}
	}
	src = dst;
	return src.vertexCount();
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	clipPrimitive(primitive<> &p)
{
	if (neClip(p) < 3) return false;
	if (feClip(p) < 3) return false;
	if (leClip(p) < 3) return false;
	if (reClip(p) < 3) return false;
	if (teClip(p) < 3) return false;
	if (beClip(p) < 3) return false;
	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Clip.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
