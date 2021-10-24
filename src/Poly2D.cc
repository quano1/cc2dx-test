#include "utils/Poly2D.h"

Polygon2D::Polygon2D()
{

}

Polygon2D::Polygon2D(std::vector<cocos2d::Vec2> &&shape)
{
    verts_ = shape;
    bmin_ = verts_[0];
    bmax_ = verts_[0];
    for(size_t i=1; i<verts_.size(); i++)
    {
        if(bmin_.x > verts_[i].x) bmin_.x = verts_[i].x;
        if(bmin_.y > verts_[i].y) bmin_.y = verts_[i].y;
        if(bmax_.x < verts_[i].x) bmax_.x = verts_[i].x;
        if(bmax_.y < verts_[i].y) bmax_.y = verts_[i].y;
    }

    tris_ = ::triangulate<cocos2d::Vec2>(verts_);
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
    return std::tie(verts_[tris_[i]], verts_[tris_[i+1]], verts_[tris_[i+2]]);
}

void Polygon2D::drawTris(cocos2d::DrawNode *dnode)
{
    // std::vector<cocos2d::Vec2> &shape = this->verts_;
    // for(std::size_t i = 0; i < this->tris_.size()/3; i++) 
    // {
    //     cocos2d::Vec2 t1,t2,t3;
    //     std::tie(t1,t2,t3) = this->triangle(i);
    //     dnode->drawTriangle(t1,t2,t3,
    //                                  cocos2d::Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0.5));
    // }

    // dnode->drawRect(this->bmin_, this->bmax_, cocos2d::Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0.5));
    // dnode->drawDot(med_, 5, cocos2d::Color4F::RED);

    dnode->drawPolygon(this->verts_.data(), this->verts_.size(), cocos2d::Color4F(), 2, cocos2d::Color4F::RED);
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
    v.resize(verts_.size(),2);
    e.resize(verts_.size(),2);
    int i=0;
    for(; i<verts_.size(); i++)
    {
        v.block<1,2>(i,0) = Eigen::Vector2f{verts_[i].x, verts_[i].y};
        e.block<1,2>(i,0) = Eigen::Vector2i{i, i+1};
    }

    e(i-1,1) = 0;
}

void Polygon2D::fromEigen(Eigen::MatrixXf const &v)
{
    verts_.resize(v.rows());
    for(int i=0; i<v.rows(); i++)
    {
        verts_[i] = {v(i,0), v(i,1)};
    }
}