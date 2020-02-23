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

#include "TestNavMesh.h"
#include "SimpleAudioEngine.h"
#include "3rdparty/clipper/clipper.hpp"
#include "convex.h"
#include "utils.h"
#include "ui/UIRadioButton.h"
#include "ui/UILayout.h"

#include <Eigen/Core>
#include <igl/triangle/triangulate.h>

USING_NS_CC;

Scene* TestNavMesh::createScene()
{
    return TestNavMesh::create();
}

// on "init" you need to initialize your instance
bool TestNavMesh::init()
{
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
    draw_med_ = DrawNode::create();
    act_layer_->addChild(draw_med_);

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
				bool ret = isPointInPoly(curr_touch, it->shape_); 
                ret = isPointInConvex<cocos2d::Vec2>(curr_touch, it->shape_);
                if(ret)
                {
                    /// remove
                    polygons_.erase(std::next(it).base());
                    draw_tris_->clear();
                    for(Polygon2D &poly : polygons_) poly.drawTris(draw_tris_);
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
                draw_convex_->clear();
                for(auto &poly : polygons_)
                    draw_convex_->drawPolygon(poly.shape_.data(), poly.shape_.size(), cocos2d::Color4F(), 2, cocos2d::Color4F::WHITE);
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
                    draw_convex_->drawPolygon(hull_.data(), hull_.size(), cocos2d::Color4F(), 2, cocos2d::Color4F::WHITE);

                    polygons_.push_back(Polygon2D(std::move(hull_)));
                    polygons_.back().drawTris(draw_tris_);
                    // draw_med_->drawDot(polygons_.back().medPoint(0), 3, cocos2d::Color4F::RED);
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
    /// obstacles
    ClipperLib::ClipperOffset co;
    for(auto &poly : polygons_)
    {
        ClipperLib::Path path;
        for(auto &p : poly.shape_)
        {
            path << ClipperLib::IntPoint(p.x* kPrecision, p.y * kPrecision);
        }
        co.AddPath(path, ClipperLib::JoinType::jtSquare, ClipperLib::EndType::etClosedPolygon);
    }
    float const kDelta = 300.f;
    /// clp = obstacles + offset
    co.Execute(clp, kDelta);

    /// sub - clp
    clipper.AddPaths(sub, ClipperLib::PolyType::ptSubject, true);
    clipper.AddPaths(clp, ClipperLib::PolyType::ptClip, true);
    clipper.Execute(ClipperLib::ClipType::ctDifference, sol_tree, pft, pft);
    

    // std::vector<float> verts;
    // std::vector<int> indices;
    // verts.reserve(0x1000);
    // indices.reserve(0x1000);
    // ClipperLib::PolyNode* pn = sol_tree.GetFirst();
    // int index=0;
    // int start_index = index;
    // for(ClipperLib::IntPoint &p : pn->Contour)
    // {
    //     verts.push_back(p.X/kPrecision);
    //     verts.push_back(p.Y/kPrecision);
    //     indices.push_back(index);
    //     indices.push_back(index+1);
    // }
    // indices.back() = start_index;

    draw_tris_->clear();
    // for(ClipperLib::Path &path : sol)
    for(ClipperLib::PolyNode *pln=sol_tree.GetFirst();pln!=nullptr;pln=pln->GetNext())
    {
        if(!pln->IsHole()) continue;
        // start_index = index;
        std::vector<cocos2d::Vec2> shape;
        // for(ClipperLib::IntPoint &p : pln->Contour)
        for(int i=pln->Contour.size()-1; i>=0; i--)
        {
            auto &p = pln->Contour[i];
            shape.push_back({p.X/kPrecision, p.Y/kPrecision});
            // verts.push_back(p.X/kPrecision);
            // verts.push_back(p.Y/kPrecision);
            // indices.push_back(index);
            // indices.push_back(index+1);
        }
        // indices.back() = start_index;
        Polygon2D poly(std::move(shape));
        poly.drawTris(draw_tris_);
    }


    /// draw
}