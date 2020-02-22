#pragma once
#include <vector>
#include <cocos2d.h>

#include "convex.h"

class Polygon2D
{
public:
    Polygon2D();
    Polygon2D(std::vector<cocos2d::Vec2> &shape);
    ~Polygon2D() = default;

    void drawTris(cocos2d::DrawNode *dnode);
    std::tuple<cocos2d::Vec2&, cocos2d::Vec2&, cocos2d::Vec2&> triangle(size_t idx);

    std::vector<cocos2d::Vec2> shape_;              //< points
    cocos2d::Vec2 bmin_, bmax_; //< bounding box
    std::vector<int> tris_; //< triangles
};

