#pragma once
#include <vector>
#include <cocos2d.h>

#include <Eigen/Core>
#include "convex.h"

class Polygon2D
{
public:
    Polygon2D();
    Polygon2D(std::vector<cocos2d::Vec2> &&shape);
    ~Polygon2D() = default;

    void drawTris(cocos2d::DrawNode *dnode);
    cocos2d::Vec2 ranPoint(size_t idx);
    cocos2d::Vec2 medPoint(size_t idx);
    void getEigen(Eigen::MatrixXf &V, Eigen::MatrixXi &E) const;
    void fromEigen(Eigen::MatrixXf const &v);
    std::tuple<cocos2d::Vec2&, cocos2d::Vec2&, cocos2d::Vec2&> triangle(size_t idx);

    std::vector<cocos2d::Vec2> verts_;              //< points
    cocos2d::Vec2 med_; //< median points
    cocos2d::Vec2 bmin_, bmax_; //< bounding box
    std::vector<int> tris_; //< triangles
};

