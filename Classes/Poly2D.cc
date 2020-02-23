#include "Poly2D.h"

Polygon2D::Polygon2D()
{

}

Polygon2D::Polygon2D(std::vector<cocos2d::Vec2> &&shape)
{
    shape_ = shape;
    bmin_ = shape_[0];
    bmax_ = shape_[0];
    for(size_t i=1; i<shape_.size(); i++)
    {
        if(bmin_.x > shape_[i].x) bmin_.x = shape_[i].x;
        if(bmin_.y > shape_[i].y) bmin_.y = shape_[i].y;
        if(bmax_.x < shape_[i].x) bmax_.x = shape_[i].x;
        if(bmax_.y < shape_[i].y) bmax_.y = shape_[i].y;
    }

    tris_ = ::triangulate<cocos2d::Vec2>(shape_);
    med_ = this->medPoint(0);
    // size_t ntris = this->tris_.size()/3;
    // for(size_t i = 0; i < ntris; i++)
    // {
    //     med_ = med_ + this->medPoint(i);
    // }
    // med_ = med_/ntris;
}

std::tuple<cocos2d::Vec2&, cocos2d::Vec2&, cocos2d::Vec2&> Polygon2D::triangle(size_t idx)
{
    size_t i = idx * 3;
    return std::tie(shape_[tris_[i]], shape_[tris_[i+1]], shape_[tris_[i+2]]);
}

void Polygon2D::drawTris(cocos2d::DrawNode *dnode)
{
    std::vector<cocos2d::Vec2> &shape = this->shape_;
    for(std::size_t i = 0; i < this->tris_.size()/3; i++) 
    {
        cocos2d::Vec2 t1,t2,t3;
        std::tie(t1,t2,t3) = this->triangle(i);
        dnode->drawTriangle(t1,t2,t3,
                                     cocos2d::Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0.5));
    }

    dnode->drawRect(this->bmin_, this->bmax_, cocos2d::Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0.5));
    dnode->drawDot(med_, 5, cocos2d::Color4F::RED);
}

cocos2d::Vec2 Polygon2D::ranPoint(size_t idx)
{
    cocos2d::Vec2 ret;
    if(idx < tris_.size()/3)
    {
        cocos2d::Vec2 t1,t2,t3;
        std::tie(t1,t2,t3) = this->triangle(idx);
        ret = ::ranPoint<cocos2d::Vec2>(t1, t2, t3);
    }
    return ret;
}

cocos2d::Vec2 Polygon2D::medPoint(size_t idx)
{
    cocos2d::Vec2 ret;
    if(idx < tris_.size()/3)
    {
        cocos2d::Vec2 t1,t2,t3;
        std::tie(t1,t2,t3) = this->triangle(idx);
        ret = ::medPoint<cocos2d::Vec2>(t1, t2, t3);
    }
    return ret;
}

void Polygon2D::getEigen(Eigen::MatrixXf &v, Eigen::MatrixXi &e) const
{
    v.resize(shape_.size(),2);
    e.resize(shape_.size(),2);
    int i=0;
    for(; i<shape_.size(); i++)
    {
        v.block<1,2>(i,0) = Eigen::Vector2f{shape_[i].x, shape_[i].y};
        e.block<1,2>(i,0) = Eigen::Vector2i{i, i+1};
    }

    e(i-1,1) = 0;
}

void Polygon2D::fromEigen(Eigen::MatrixXf const &v)
{
    shape_.resize(v.rows());
    for(int i=0; i<v.rows(); i++)
    {
        shape_[i] = {v(i,0), v(i,1)};
    }
}