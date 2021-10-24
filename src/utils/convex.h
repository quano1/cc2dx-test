#pragma once

#include <vector>
#include <cocos2d.h>
#include <random>

/// cocos2d
inline float operator* (const cocos2d::Vec2 &v1, const cocos2d::Vec2 &v2)
{
    return v1.dot(v2);
}

////////////////////////////////////////////////////////////////////////////////


/// https://blackpawn.com/texts/pointinpoly/default.html
template <typename V>
inline bool isPointInTri(const V &p, const V &a, const V &b, const V &c)
{
    // Compute vectors
    V v0 = c - a;
    V v1 = b - a;
    V v2 = p - a;

    // Compute dot products
    float dot00 = v0 * v0;
    float dot01 = v0 * v1;
    float dot02 = v0 * v2;
    float dot11 = v1 * v1;
    float dot12 = v1 * v2;

    // Compute barycentric coordinates
    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v < 1);
}

template <typename V>
inline bool isPointInConvex(const V &p, const std::vector<V> &poly)
{
    for(size_t i=0; i<poly.size()-2; i++)
    {
        if(isPointInTri(p, poly[0], poly[i+1], poly[i+2]))
            return true;
    }

    return false;
}

// typedef std::pair<float, float> cocos2d::Vec2;

// template <typename T>
// inline T wrap(T val, T lo, T hi)
// {
//     return val > hi ? lo + (val - hi) : val < lo ? hi - (lo - val) : val;
// }
// Last time I checked the if version got compiled using cmov, which was a lot faster than module (with idiv).
inline int prev(int i, int n) { return i == 0 ? n-1 : i-1; }
inline int next(int i, int n) { return i+1 == n ? 0 : i+1; }

template <typename T>
inline int area2(const T& a, const T& b, const T& c)
{
    /// cross product
    T ab = b - a;
    T ac = c - a;
    return (ab.x) * (ac.y) - (ab.y) * (ac.x);
    // return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    // return (b[0] - a[0]) * (c[2] - a[2]) - (c[0] - a[0]) * (b[2] - a[2]);
}

//  Exclusive or: true iff exactly one argument is true.
//  The arguments are negated to ensure that they are 0/1
//  values.  Then the bitwise Xor operator may apply.
//  (This idea is due to Michael Baldwin.)
inline bool xorb(bool x, bool y)
{
    return !x ^ !y;
}

// Returns true iff c is strictly to the left of the directed
// line through a to b.
template <typename T>
inline bool isClockWise2D(const T& a, const T& b, const T& c)
{
    return area2(a, b, c) > 0;
}

template <typename T>
inline bool isClockWise2DOn(const T& a, const T& b, const T& c)
{
    return area2(a, b, c) >= 0;
}

template <typename T>
inline bool collinear(const T& a, const T& b, const T& c)
{
    return area2(a, b, c) == 0;
}

//  Returns true iff ab properly intersects cd: they share
//  a point interior to both segments.  The properness of the
//  intersection is ensured by using strict isClockWise2Dness.
template <typename T>
static bool intersectProp(const T& a, const T& b, const T& c, const T& d)
{
    // Eliminate improper cases.
    if (collinear(a,b,c) || collinear(a,b,d) ||
        collinear(c,d,a) || collinear(c,d,b))
        return false;
    
    return xorb(isClockWise2D(a,b,c), isClockWise2D(a,b,d)) && xorb(isClockWise2D(c,d,a), isClockWise2D(c,d,b));
}

// Returns T iff (a,b,c) are collinear and point c lies 
// on the closed segement ab.
template <typename T>
static bool between(const T& a, const T& b, const T& c)
{
    if (!collinear(a, b, c))
        return false;
    // If ab not vertical, check betweenness on x; else on y.
    if (a.x != b.x)
        return  ((a.x <= c.x) && (c.x <= b.x)) || ((a.x >= c.x) && (c.x >= b.x));
    else
        return  ((a.y <= c.y) && (c.y <= b.y)) || ((a.y >= c.y) && (c.y >= b.y));
}

// Returns true iff segments ab and cd intersect, properly or improperly.
template <typename T>
static bool intersect(const T& a, const T& b, const T& c, const T& d)
{
    if (intersectProp(a, b, c, d))
        return true;
    else if (between(a, b, c) || between(a, b, d) ||
             between(c, d, a) || between(c, d, b))
        return true;
    else
        return false;
}

template <typename T>
static bool vequal(const T& a, const T& b)
{
    return a.x == b.x && a.y == b.y;
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
            if (hull == endpt || area2(pts[hull], pts[endpt], pts[j]) < 0)
                endpt = j;
        hull = endpt;
    }
    while (endpt != kStartPoint);
    
    return out.size();
}

/// raycast
template <typename T>
inline bool isPointInPoly(T const &p, std::vector<T> const &verts)
{
    size_t i, j;
    bool c = false;
    size_t nvert = verts.size();
    for (i = 0, j = nvert-1; i < nvert; j = i, i++)
    {
        const auto &v1 = verts[i];
        const auto &v2 = verts[j];
        if ((v1.y > p.y) != (v2.y > p.y))
        {
            float const kRaycastIntersecX = (v2.x-v1.x) * (v1.y-p.y) / (v1.y-v2.y) + v1.x;
            /// p lies on the edge
            if(p.x == kRaycastIntersecX) return true;
            /// p lies on the left of the edge
            if(p.x < kRaycastIntersecX) c = !c;
        }
    }
    return c;
}

// Returns T iff (v_i, v_j) is a proper internal *or* external
// diagonal of P, *ignoring edges incident to v_i and v_j*.
template <typename T>
static bool diagonalie(int i, int j, int n, const T* verts, const int* indices)
{
    // const int* d0 = &verts[(indices[i] & 0x0fffffff) * 4];
    // const int* d1 = &verts[(indices[j] & 0x0fffffff) * 4];
    const T d0 = verts[indices[i] & 0x0fffffff];
    const T d1 = verts[indices[j] & 0x0fffffff];

    // For each edge (k,k+1) of P
    for (int k = 0; k < n; k++)
    {
        int k1 = next(k, n);
        // Skip edges incident to i or j
        if (!((k == i) || (k1 == i) || (k == j) || (k1 == j)))
        {
            // const int* p0 = &verts[(indices[k] & 0x0fffffff) * 4];
            // const int* p1 = &verts[(indices[k1] & 0x0fffffff) * 4];
            const T p0 = verts[indices[k] & 0x0fffffff];
            const T p1 = verts[indices[k1] & 0x0fffffff];

            if (vequal(d0, p0) || vequal(d1, p0) || vequal(d0, p1) || vequal(d1, p1))
                continue;
            
            if (intersect(d0, d1, p0, p1))
                return false;
        }
    }
    return true;
}

// Returns true iff the diagonal (i,j) is strictly internal to the 
// polygon P in the neighborhood of the i endpoint.
template <typename T>
static bool inCone(int i, int j, int n, const T* verts, const int* indices)
{
    // const int* pi = &verts[(indices[i] & 0x0fffffff) * 4];
    // const int* pj = &verts[(indices[j] & 0x0fffffff) * 4];
    // const int* pi1 = &verts[(indices[next(i, n)] & 0x0fffffff) * 4];
    // const int* pin1 = &verts[(indices[prev(i, n)] & 0x0fffffff) * 4];
    const T pi = verts[indices[i] & 0x0fffffff];
    const T pj = verts[indices[j] & 0x0fffffff];
    const T pi1 = verts[indices[next(i, n)] & 0x0fffffff];
    const T pin1 = verts[indices[prev(i, n)] & 0x0fffffff];

    // If P[i] is a convex vertex [ i+1 isClockWise2D or on (i-1,i) ].
    if (isClockWise2DOn(pin1, pi, pi1))
        return isClockWise2D(pi, pj, pin1) && isClockWise2D(pj, pi, pi1);
    // Assume (i-1,i,i+1) not collinear.
    // else P[i] is reflex.
    return !(isClockWise2DOn(pi, pj, pi1) && isClockWise2DOn(pj, pi, pin1));
}

// Returns T iff (v_i, v_j) is a proper internal
// diagonal of P.
template <typename T>
static bool diagonal(int i, int j, int n, const T* verts, const int* indices)
{
    return inCone(i, j, n, verts, indices) && diagonalie(i, j, n, verts, indices);
}

template <typename T>
static bool diagonalieLoose(int i, int j, int n, const T* verts, const int* indices)
{
    // const int* d0 = &verts[(indices[i] & 0x0fffffff) * 4];
    // const int* d1 = &verts[(indices[j] & 0x0fffffff) * 4];
    const T d0 = verts[indices[i] & 0x0fffffff];
    const T d1 = verts[indices[j] & 0x0fffffff];

    // For each edge (k,k+1) of P
    for (int k = 0; k < n; k++)
    {
        int k1 = next(k, n);
        // Skip edges incident to i or j
        if (!((k == i) || (k1 == i) || (k == j) || (k1 == j)))
        {
            // const int* p0 = &verts[(indices[k] & 0x0fffffff) * 4];
            // const int* p1 = &verts[(indices[k1] & 0x0fffffff) * 4];
            const T p0 = verts[indices[k] & 0x0fffffff];
            const T p1 = verts[indices[k1] & 0x0fffffff];
            
            if (vequal(d0, p0) || vequal(d1, p0) || vequal(d0, p1) || vequal(d1, p1))
                continue;
            
            if (intersectProp(d0, d1, p0, p1))
                return false;
        }
    }
    return true;
}

template <typename T>
static bool inConeLoose(int i, int j, int n, const T* verts, const int* indices)
{
    // const int* pi = &verts[(indices[i] & 0x0fffffff) * 4];
    // const int* pj = &verts[(indices[j] & 0x0fffffff) * 4];
    // const int* pi1 = &verts[(indices[next(i, n)] & 0x0fffffff) * 4];
    // const int* pin1 = &verts[(indices[prev(i, n)] & 0x0fffffff) * 4];
    const T pi = verts[indices[i] & 0x0fffffff];
    const T pj = verts[indices[j] & 0x0fffffff];
    const T pi1 = verts[indices[next(i, n)] & 0x0fffffff];
    const T pin1 = verts[indices[prev(i, n)] & 0x0fffffff];
    
    // If P[i] is a convex vertex [ i+1 isClockWise2D or on (i-1,i) ].
    if (isClockWise2DOn(pin1, pi, pi1))
        return isClockWise2DOn(pi, pj, pin1) && isClockWise2DOn(pj, pi, pi1);
    // Assume (i-1,i,i+1) not collinear.
    // else P[i] is reflex.
    return !(isClockWise2DOn(pi, pj, pi1) && isClockWise2DOn(pj, pi, pin1));
}

template <typename T>
static bool diagonalLoose(int i, int j, int n, const T* verts, const int* indices)
{
    return inConeLoose(i, j, n, verts, indices) && diagonalieLoose(i, j, n, verts, indices);
}


template <typename T>
static std::vector<int> triangulate(const std::vector<T> &verts)
{
    std::vector<int> tris;
    int n = verts.size();
    tris.reserve(n * 3);
    // int ntris = 0;
    std::vector<int> indices(n);
    // int* dst = tris;
    for(int i=0; i<n; i++)
    {
        indices[i] = i;
    }
    
    // The last bit of the index is used to indicate if the vertex can be removed.
    for (int i = 0; i < n; i++)
    {
        int i1 = next(i, n);
        int i2 = next(i1, n);
        if (diagonal(i, i2, n, verts.data(), indices.data()))
            indices[i1] |= 0x80000000;
    }
    
    while (n > 3)
    {
        int minLen = -1;
        int mini = -1;
        for (int i = 0; i < n; i++)
        {
            int i1 = next(i, n);
            if (indices[i1] & 0x80000000)
            {
                // const int* p0 = &verts[(indices[i] & 0x0fffffff) * 4];
                // const int* p2 = &verts[(indices[next(i1, n)] & 0x0fffffff) * 4];
                const T p0 = verts[(indices[i] & 0x0fffffff)];
                const T p2 = verts[(indices[next(i1, n)] & 0x0fffffff)];

                int dx = p2.x - p0.x;
                int dy = p2.y - p0.y;
                int len = dx*dx + dy*dy;
                
                if (minLen < 0 || len < minLen)
                {
                    minLen = len;
                    mini = i;
                }
            }
        }
        
        if (mini == -1)
        {
            // We might get here because the contour has overlapping segments, like this:
            //
            //  A o-o=====o---o B
            //   /  |C   D|    \.
            //  o   o     o     o
            //  :   :     :     :
            // We'll try to recover by loosing up the inCone test a bit so that a diagonal
            // like A-B or C-D can be found and we can continue.
            minLen = -1;
            mini = -1;
            for (int i = 0; i < n; i++)
            {
                int i1 = next(i, n);
                int i2 = next(i1, n);
                if (diagonalLoose(i, i2, n, verts.data(), indices.data()))
                {
                    // const int* p0 = &verts[(indices[i] & 0x0fffffff) * 4];
                    // const int* p2 = &verts[(indices[next(i2, n)] & 0x0fffffff) * 4];
                    const T p0 = verts[(indices[i] & 0x0fffffff)];
                    const T p2 = verts[(indices[next(i2, n)] & 0x0fffffff)];
                    int dx = p2.x - p0.x;
                    int dy = p2.y - p0.y;
                    int len = dx*dx + dy*dy;
                    
                    if (minLen < 0 || len < minLen)
                    {
                        minLen = len;
                        mini = i;
                    }
                }
            }
            if (mini == -1)
            {
                // The contour is messed up. This sometimes happens
                // if the contour simplification is too aggressive.
                // return -ntris;
                return std::vector<int>();
            }
        }
        
        int i = mini;
        int i1 = next(i, n);
        int i2 = next(i1, n);
        
        tris.push_back(indices[i] & 0x0fffffff);
        tris.push_back(indices[i1] & 0x0fffffff);
        tris.push_back(indices[i2] & 0x0fffffff);
        // ntris++;
        
        // Removes P[i1] by copying P[i+1]...P[n-1] isClockWise2D one index.
        n--;
        for (int k = i1; k < n; k++)
            indices[k] = indices[k+1];
        
        if (i1 >= n) i1 = 0;
        i = prev(i1,n);
        // Update diagonal flags.
        if (diagonal(prev(i, n), i1, n, verts.data(), indices.data()))
            indices[i] |= 0x80000000;
        else
            indices[i] &= 0x0fffffff;
        
        if (diagonal(i, next(i1, n), n, verts.data(), indices.data()))
            indices[i1] |= 0x80000000;
        else
            indices[i1] &= 0x0fffffff;
    }
    
    // Append the remaining triangle.
    tris.push_back(indices[0] & 0x0fffffff);
    tris.push_back(indices[1] & 0x0fffffff);
    tris.push_back(indices[2] & 0x0fffffff);
    // ntris++;
    
    return tris;
}

/// barycentric
template <typename T>
T ranPoint(T const &a, T const &b, T const &c)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0, 1.f);
    float u = dis(gen);
    float v = dis(gen);
    while(u+v>=1 || u+v==0)
    {
        u = dis(gen);
        v = dis(gen);
    }
    return a + u*(b-a) + v*(c-a);
}

template <typename T>
T medPoint(T const &a, T const &b, T const &c)
{
    float const kMedian = 1.f/3;
    // return a + kMedian*((b-a) + (c-a));
	return kMedian * (a + b + c);
}
