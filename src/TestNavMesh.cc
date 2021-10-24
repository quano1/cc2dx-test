/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include <iostream>
#include <algorithm>
#include <sstream>

#include "TestNavMesh.h"
#include "thirdparty/clipper/cpp/clipper.hpp"
#include "utils/convex.h"
#include "ui/UIRadioButton.h"
#include "ui/UILayout.h"

#include <eigen3/Eigen/Core>
#include <igl/triangle/triangulate.h>
#include <igl/AABB.h>

USING_NS_CC;

static void drawEigen(cocos2d::DrawNode *dnode, Eigen::Matrix<float,Eigen::Dynamic,2> const &verts, Eigen::Matrix<int, Eigen::Dynamic, 3> const &indices)
{
    for(int i=0; i<indices.rows(); i++)
    {
        cocos2d::Vec2 a(verts(indices(i,0),0), verts(indices(i,0),1)), 
                        b(verts(indices(i,1),0), verts(indices(i,1),1)),
                        c(verts(indices(i,2),0), verts(indices(i,2),1));
        dnode->drawTriangle(a,b,c,
            cocos2d::Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0.5));
    }
}

Scene* TestNavMesh::createScene()
{
    // TLL_GLOGTF();
    return TestNavMesh::create();
}

// on "init" you need to initialize your instance
bool TestNavMesh::init()
{
    // TLL_GLOGTF();
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    cocos2d::Size const kVisible = cocos2d::Director::getInstance()->getVisibleSize();
    Vec2 const kOrigin = Director::getInstance()->getVisibleOrigin();

    debug_layer_ = cocos2d::Layer::create();
    addChild(debug_layer_);

    act_layer_ = cocos2d::Layer::create();
    addChild(act_layer_);

    draw_debug_ = DrawNode::create();
    debug_layer_->addChild(draw_debug_, 3);

    draw_convex_ = DrawNode::create();
    act_layer_->addChild(draw_convex_);
    draw_tris_ = DrawNode::create();
    act_layer_->addChild(draw_tris_);
    draw_igl_ = DrawNode::create();
    act_layer_->addChild(draw_igl_);

    debug_text_ = ui::Text::create("",
                                      "fonts/arial.ttf",32);
    debug_text_->setTextHorizontalAlignment(TextHAlignment::LEFT);
    debug_text_->setPosition(Vec2(0, kVisible.height));
    debug_text_->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    debug_layer_->addChild(debug_text_);

    initEvents();
    scheduleUpdate();
    return true;
}

void TestNavMesh::initEvents()
{
    // TLL_GLOGTF();
    EventListenerTouchAllAtOnce *listener = EventListenerTouchAllAtOnce::create();
    prev_touch_ = cocos2d::Vec2::ZERO;
    /// pressed event
    listener->onTouchesBegan = [=] (const std::vector<Touch*>& touches, Event *pEvent) mutable
    {
        cocos2d::Vec2 curr_touch = touches[0]->getLocation();
        /// remove or start a convex
        if (touch_points_.empty())
        {
            bool is_removing = false;
            for(decltype(polygons_.rbegin()) it = polygons_.rbegin(); it != polygons_.rend(); it++)
            {
                bool ret = isPointInPoly(curr_touch, it->verts_); 
                ret = isPointInConvex<cocos2d::Vec2>(curr_touch, it->verts_);
                if(ret)
                {
                    /// remove
                    polygons_.erase(std::next(it).base());
                    draw_convex_->clear();
                    for(Polygon2D &poly : polygons_) poly.drawTris(draw_convex_);
                    is_removing = true;
                    break;
                }
            }
            if(!is_removing)
            {
                touch_points_.push_back(curr_touch);
                draw_debug_->drawDot(curr_touch, 3, cocos2d::Color4F::YELLOW);
            }
            else
            {
                // draw_convex_->clear();
                // for(auto &poly : polygons_)
                //     draw_convex_->drawPolygon(poly.verts_.data(), poly.verts_.size(), cocos2d::Color4F(), 2, cocos2d::Color4F::WHITE);
            }
        }
        else /// add point or complete convex
        {
            /// add new point or complete
            if (prev_touch_ != curr_touch) /// add new point
            {
                touch_points_.push_back(curr_touch);
                hull_.reserve(4);
                convexhull2D(touch_points_, hull_);

                draw_debug_->drawDot(curr_touch, 3, cocos2d::Color4F::YELLOW);

                draw_debug_->drawPolygon(hull_.data(), hull_.size(), cocos2d::Color4F(), 3, cocos2d::Color4F::YELLOW);
            }
            else
            {
                draw_debug_->clear();
                touch_points_.clear();
                /// complete convex
                if(hull_.size() > 2)
                {
                    // draw_convex_->drawPolygon(hull_.data(), hull_.size(), cocos2d::Color4F(), 2, cocos2d::Color4F::WHITE);

                    polygons_.push_back(Polygon2D(std::move(hull_)));
                    polygons_.back().drawTris(draw_convex_);
                    // draw_igl_->drawDot(polygons_.back().medPoint(0), 3, cocos2d::Color4F::RED);
                    // draw_tris_->clear();
                    // for(Polygon2D &poly : polygons_) poly.drawTris(draw_tris_);
                }
            }
        }
        
        prev_touch_ = curr_touch;
    };
    /// add event
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    /// Keyboard
    auto keyboardListener = cocos2d::EventListenerKeyboard::create();
    using KeyCode = cocos2d::EventKeyboard::KeyCode;
    keyboardListener->onKeyPressed = [=](KeyCode code, cocos2d::Event *event)
    {
        switch(code)
        {
            case KeyCode::KEY_SPACE:
            {
                // add_mode_ ^= 1;
                execute();
            }
            break;
        }
    };

    keyboardListener->onKeyReleased = [=](KeyCode code, cocos2d::Event *event)
    {};

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void TestNavMesh::update(float delta)
{
    Scene::update(delta);
}

void TestNavMesh::execute()
{
    // TLL_GLOGTF();
    ClipperLib::Clipper clipper;
    ClipperLib::Paths sub, clp, sol;
    ClipperLib::PolyTree sol_tree;
    const float kPrecision = 10.f;
    cocos2d::Size const kVisible = cocos2d::Director::getInstance()->getVisibleSize()* kPrecision;
    ClipperLib::PolyFillType pft = ClipperLib::PolyFillType::pftEvenOdd;

    ClipperLib::Path path;
    path << ClipperLib::IntPoint(0,0);
    path << ClipperLib::IntPoint(kVisible.width,0);
    path << ClipperLib::IntPoint(kVisible.width,kVisible.height);
    path << ClipperLib::IntPoint(0,kVisible.height);
    sub.push_back(path);

    /// Eigen
    std::vector<float> verts_data;
    std::vector<int> elems_data;
    std::vector<float> obs_data;
    verts_data.reserve(0x1000);
    elems_data.reserve(0x1000);
    obs_data.reserve(0x100);

    /// obstacles - holes
    ClipperLib::ClipperOffset co;
    // size_t nvert=0;
    for(auto &poly : polygons_)
    {
        ClipperLib::Path path;
        obs_data.push_back(poly.med_.x);
        obs_data.push_back(poly.med_.y);
        for(auto &vert : poly.verts_)
        {
            path << ClipperLib::IntPoint(vert.x* kPrecision, vert.y * kPrecision);
        }
        co.AddPath(path, ClipperLib::JoinType::jtMiter, ClipperLib::EndType::etClosedPolygon);
    }
    float const kDelta = 150.f;
    /// clp = obstacles + offset
    co.Execute(clp, kDelta);

    /// sub - clp
    clipper.AddPaths(sub, ClipperLib::PolyType::ptSubject, true);
    clipper.AddPaths(clp, ClipperLib::PolyType::ptClip, true);
    clipper.Execute(ClipperLib::ClipType::ctDifference, sol_tree, pft, pft);
    
    int index=0;
    for(ClipperLib::PolyNode *pln=sol_tree.GetFirst();pln!=nullptr;pln=pln->GetNext())
    {
        // for(ClipperLib::IntPoint &p : pln->Contour)
        int start_index = index;
        if(pln->IsHole())
        {
            /// revert order
            for(int i=pln->Contour.size()-1; i>=0; i--)
            {
                verts_data.push_back(pln->Contour[i].X/kPrecision);
                verts_data.push_back(pln->Contour[i].Y/kPrecision);
                elems_data.push_back(index);
                elems_data.push_back(index+1);
                index++;
            }
        }
        else
        {
            /// cc wise
            for(size_t i=0; i<pln->Contour.size(); i++)
            {
                verts_data.push_back(pln->Contour[i].X/kPrecision);
                verts_data.push_back(pln->Contour[i].Y/kPrecision);
                elems_data.push_back(index);
                elems_data.push_back(index+1);
                index++;
            }
        }
        elems_data.back() = start_index;
    }

    /// construct eigen
    Eigen::MatrixXf verts_mat = Eigen::Map<Eigen::Matrix<float,Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> >(verts_data.data(), verts_data.size()/2, 2);
    Eigen::MatrixXi elems_mat = Eigen::Map<Eigen::Matrix<int,Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> >(elems_data.data(), elems_data.size()/2, 2);
    Eigen::MatrixXf obs_mat = Eigen::Map<Eigen::Matrix<float,Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> >(obs_data.data(), obs_data.size()/2, 2);
    
    Eigen::MatrixXf V2;
    Eigen::MatrixXi F2;
    std::string flag = std::string("a") + std::to_string((kVisible.width/kPrecision * kVisible.height/kPrecision) / 4) + "q";
    igl::triangle::triangulate(verts_mat,elems_mat,obs_mat,flag.data(),V2,F2);

    draw_tris_->clear();
    /// draw triangulate
    drawEigen(draw_tris_, V2, F2);

    igl::AABB<Eigen::MatrixXf, 2> tree;
    tree.init(V2, F2);
    // Eigen::Matrix<float,1,2> qp((kVisible.width / kPrecision) / 2, 10);
    // std::vector<int> found = tree.find(V2,F2,qp);
    // LOGD("%d", found.back());
    // std::stringstream ss;
    // ss << F2 << std::endl;
    // LOGD("%s", ss.str().data());
    // // ss.str(""); 
    // ss.clear();
    // ss << V2 << std::endl;
    // LOGD("%s", ss.str().data());
}

