#pragma once

#include <vector>
#include <cocos2d.h>

// typedef std::pair<float, float> cocos2d::Vec2;

// Returns true if 'c' is left of line 'a'-'b'.
inline bool left2D(cocos2d::Vec2 const &a, cocos2d::Vec2 const &b, cocos2d::Vec2 const &c)
{ 
	const float u1 = b.x - a.x;
	const float v1 = b.y - a.y;
	const float u2 = c.x - a.x;
	const float v2 = c.y - a.y;
	return u1 * v2 - v1 * u2 < 0;
}

// Returns true if 'a' is more lower-left than 'b'.
// inline bool cmppt(const float* a, const float* b)
inline bool cmppt2D(cocos2d::Vec2 const &a, cocos2d::Vec2 const &b)
{
	if (a.x < b.x) return true;
	if (a.x > b.x) return false;
	if (a.y < b.y) return true;
	if (a.y > b.y) return false;
	return false;
}
// Calculates convex hull on xz-plane of points on 'pts',
// stores the indices of the resulting hull in 'out' and
// returns number of points on hull.
// inline int convexhull2D(const float* pts, int npts, int* out)
inline size_t convexhull2D(std::vector<cocos2d::Vec2> const &pts, std::vector<cocos2d::Vec2> &out)
{
	// Find lower-leftmost point.
	size_t npts = pts.size();
	size_t hull = 0;
	out.clear();
	for (size_t i = 1; i < npts; ++i)
		if (cmppt2D(pts[i], pts[hull]))
			hull = i;
	// Gift wrap hull.
	size_t endpt = 0;
	size_t const kStartPoint = hull;
	do
	{
		out.push_back(pts[hull]);
		endpt = 0;
		for (size_t j = 1; j < npts; ++j)
			if (hull == endpt || left2D(pts[hull], pts[endpt], pts[j]))
				endpt = j;
		hull = endpt;
	}
	while (endpt != kStartPoint);
	
	return out.size();
}

inline size_t pointInPoly2D(std::vector<cocos2d::Vec2> const &v, cocos2d::Vec2 const &p)
{
	size_t i, j, c = 0;
	size_t nvert = v.size();
	for (i = 0, j = nvert-1; i < nvert; j = i++)
	{
		if (((v[i].y > p.y) != (v[j].y > p.y)) &&
			(p.x < (v[j].x-v[i].x) * (p.y-v[i].y) / (v[j].y-v[i].y) + v[i].x) )
			c = !c;
	}
	return c;
}

