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

#include "TestClipper.h"
#include "SimpleAudioEngine.h"
#include "3rdparty/clipper/clipper.hpp"
#include "convex.h"
#include "ui/UIRadioButton.h"
#include "ui/UILayout.h"

USING_NS_CC;

static void drawConvexs(cocos2d::DrawNode *draw, std::list<std::vector<cocos2d::Vec2>> polys, cocos2d::Color4F const &color, float rate=0.1f)
{
    auto color_fill = color;
    color_fill.a = rate;
    for(auto &poly : polys)
        draw->drawPolygon(poly.data(), poly.size(), color_fill, 2, color);
}

Scene* TestClipper::createScene()
{
    return TestClipper::create();
}

// on "init" you need to initialize your instance
bool TestClipper::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    cocos2d::Size const kVisible = cocos2d::Director::getInstance()->getVisibleSize();
    Vec2 const kOrigin = Director::getInstance()->getVisibleOrigin();

    for(int i=2; i>=0; i--)
    {
        draw_layers[i] = cocos2d::Layer::create();
        addChild(draw_layers[i]);
    }
    debug_layer_ = cocos2d::Layer::create();
    addChild(debug_layer_);


    draw_debug_ = DrawNode::create();
    debug_layer_->addChild(draw_debug_, 3);
    // draw_convex_ = DrawNode::create();
    draw_convex_s_[0] = DrawNode::create();
    draw_convex_s_[1] = DrawNode::create();
    draw_convex_s_[2] = DrawNode::create();
    // draw_delau_tri_ = DrawNode::create();
    // addChild(draw_convex_, 1);
    draw_layers[0]->addChild(draw_convex_s_[0], 1);
    draw_layers[1]->addChild(draw_convex_s_[1], 1);
    draw_layers[2]->addChild(draw_convex_s_[2], 1);
    // addChild(draw_delau_tri_, 1);

    colors_[0] = cocos2d::Color4F::WHITE;
    colors_[1] = cocos2d::Color4F::RED;
    colors_[2] = cocos2d::Color4F::GREEN;

    poly_type_ = 0;
    poly_fill_type_ = 0;
    clip_type_ = 0;
    ctrl_ = false;

    // /// help layer
    // help_layer_ = cocos2d::Layer::create();
    // // help_layer_->setContentSize({kVisible.width, kVisible.height / 4});

    // ui::Layout* layout = ui::Layout::create();
    // help_layer_->addChild(layout);
    // layout->setBackGroundImage("ui/scrollviewbg.png");
    // layout->setContentSize({kVisible.width, kVisible.height});
    // layout->setBackGroundImageScale9Enabled(true);

    // // auto bg_layer = cocos2d::LayerColor::create(Color4B::WHITE);
    // // bg_layer->setContentSize(help_layer_->getContentSize());
    // // bg_layer->setOpacity(125);
    // // help_layer_->addChild(bg_layer);

    // // auto gr1_callback = [=](ui::RadioButton* radioButton, ui::RadioButton::EventType type){

    // // };

    // /// remove
    // /// add sub
    // /// add clp
    // auto rad_btn_grp = ui::RadioButtonGroup::create();
    // layout->addChild(rad_btn_grp);
    // for(int i=0; i<2; i++)
    // {
    //     auto rad_btn = ui::RadioButton::create("ui/radbtnOff.png", "ui/radbtnOn.png");
    //     rad_btn->setScale(2.f);
    //     rad_btn_grp->addRadioButton(rad_btn);
    //     layout->addChild(rad_btn);
    //     rad_btn->setPosition({i*100.f + 100, 200});
    //     rad_btn->addEventListener([=](ui::RadioButton* radioButton, ui::RadioButton::EventType type)
    //     {

    //     });
    // }

    // /// ClipperLib::ClipType { ctIntersection, ctUnion, ctDifference, ctXor };
    // rad_btn_grp = ui::RadioButtonGroup::create();
    // layout->addChild(rad_btn_grp);
    // for(int i=0; i<4; i++)
    // {
    //     auto rad_btn = ui::RadioButton::create("ui/radbtnOff.png", "ui/radbtnOn.png");
    //     rad_btn->setScale(2.f);
    //     rad_btn_grp->addRadioButton(rad_btn);
    //     layout->addChild(rad_btn);
    //     rad_btn->setPosition({i*100.f + 400, 200});
    //     rad_btn->addEventListener([=](ui::RadioButton* radioButton, ui::RadioButton::EventType type)
    //     {

    //     });
    // }

    // /// ClipperLib::PolyFillType { ptSubject, ptClip };
    // rad_btn_grp = ui::RadioButtonGroup::create();
    // layout->addChild(rad_btn_grp);
    // for(int i=0; i<2; i++)
    // {
    //     auto rad_btn = ui::RadioButton::create("ui/radbtnOff.png", "ui/radbtnOn.png");
    //     rad_btn->setScale(2.f);
    //     rad_btn_grp->addRadioButton(rad_btn);
    //     layout->addChild(rad_btn);
    //     rad_btn->setPosition({i*100.f + 1000, 200});
    //     rad_btn->addEventListener([=](ui::RadioButton* radioButton, ui::RadioButton::EventType type)
    //     {

    //     });
    // }

    // addChild(help_layer_, 100);

    debug_label_ = Label::createWithTTF(cocos2d::TTFConfig("fonts/arial.ttf", 28), "", TextHAlignment::LEFT, kVisible.width);
    debug_layer_->addChild(debug_label_);
    updateDebugText();
    debug_label_->setTextColor(Color4B::YELLOW);
    debug_label_->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    debug_label_->setPosition(Vec2(0, kVisible.height));

    initEvents();
    scheduleUpdate();
    return true;
}

void TestClipper::initEvents()
{
    EventListenerTouchAllAtOnce *listener = EventListenerTouchAllAtOnce::create();
    prev_touch_ = cocos2d::Vec2::ZERO;
    /// pressed event
    listener->onTouchesBegan = [=] (const std::vector<Touch*>& touches, Event *pEvent) mutable
    {
        // if(help_layer_->isVisible()) return;
        cocos2d::Vec2 curr_touch = touches[0]->getLocation();

        /// remove or start a convex
        if (pts_.empty())
        {
            bool is_removing = false;
            for(decltype(clip_paths_[poly_type_].rbegin()) it = clip_paths_[poly_type_].rbegin(); it != clip_paths_[poly_type_].rend(); it++)
            {
                if(isPointInPoly(curr_touch, *it))
                {
                    /// remove
                    clip_paths_[poly_type_].erase((std::next(it)).base());
                    // doJob();
                    is_removing = true;
                    break;
                }
            }

            if(!is_removing)
            {
                pts_.push_back(curr_touch);
                draw_debug_->drawDot(curr_touch, 3, colors_[poly_type_]);
            }
            else
            {
                // draw_delau_tri_->clear();
                // draw_convex_->clear();
                // for(auto &poly : clip_paths_[poly_type_])
                //     draw_convex_->drawPolygon(poly.data(), poly.size(), cocos2d::Color4F(), 2, cocos2d::Color4F::WHITE);
                // drawConvex(draw_convex_s_[poly_type_], clip_paths_[poly_type_], colors_[poly_type_]);
                reDrawConvexs(poly_type_);
            }
        }
        else /// add point or complete convex
        {
            /// add new point or complete
            if (prev_touch_ != curr_touch) /// add new point
            {
                pts_.push_back(curr_touch);
                convexhull2D(pts_, hull_);

                draw_debug_->drawDot(curr_touch, 3, colors_[poly_type_]);

                draw_debug_->drawPolygon(hull_.data(), hull_.size(), cocos2d::Color4F(), 3, colors_[poly_type_]);
            }
            else
            {
                draw_debug_->clear();
                pts_.clear();
                /// complete convex
                if(hull_.size() > 2)
                {
                    clip_paths_[poly_type_].push_back(hull_);
                    // poly_list_.push_back(hull_);
                    // draw_delau_tri_->clear();
                    // draw_convex_s_[poly_type_]->drawPolygon(hull_.data(), hull_.size(), cocos2d::Color4F(), 2, colors_[poly_type_]);
                    drawConvexs(draw_convex_s_[poly_type_], {hull_}, colors_[poly_type_]);
                    // doJob();
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
        std::string keyCode = std::to_string(static_cast<int>(code));
        switch(code)
        {
        case KeyCode::KEY_CTRL:
            {
                ctrl_ = true;
            }
            break;
        }
    };

    keyboardListener->onKeyReleased = [=](KeyCode code, cocos2d::Event *event)
    {
        cocos2d::Size const kVisible = cocos2d::Director::getInstance()->getVisibleSize();
        std::string keyCode = std::to_string(static_cast<int>(code));
        switch(code)
        {
            case KeyCode::KEY_CTRL:
            {
                ctrl_ = false;
            }
            break;
            /// ClipType
            case KeyCode::KEY_Q:
            {
                poly_type_++;
                poly_type_ = wrap(poly_type_, 0, static_cast<int>(ClipperLib::PolyType::ptClip) + 1);
                // poly_type_ %= static_cast<int>(ClipperLib::PolyType::ptClip);
            }
            break;
            /// PolyFillType
            case KeyCode::KEY_W:
            {
                poly_fill_type_++;
                poly_fill_type_ = wrap(poly_fill_type_, 0, static_cast<int>(ClipperLib::PolyFillType::pftNegative) + 1);
                // poly_fill_type_ %= static_cast<int>(ClipperLib::PolyFillType::pftNegative);
            }
            break;
            /// PolyType
            case KeyCode::KEY_E:
            {
                clip_type_++;
                clip_type_ = wrap(clip_type_, 0, static_cast<int>(ClipperLib::ClipType::ctXor) + 1);
                // clip_type_ %= static_cast<int>(ClipperLib::ClipType::ctXor);
            }
            break;

            case KeyCode::KEY_A:
            {
                if(ctrl_)
                {
                    clip_paths_[poly_type_].push_back({{0, 0}, {kVisible.width, 0}, {kVisible.width, kVisible.height}, {0, kVisible.height}});
                    reDrawConvexs(poly_type_);
                }
                else
                {
                    draw_layers[0]->setVisible(draw_layers[0]->isVisible() ? false : true);
                }
            }
            break;
            case KeyCode::KEY_S:
            {
                draw_layers[1]->setVisible(draw_layers[1]->isVisible() ? false : true);
            }
            break;
            case KeyCode::KEY_D:
            {
                draw_layers[2]->setVisible(draw_layers[2]->isVisible() ? false : true);
            }
            break;
            /// Execution
            case KeyCode::KEY_SPACE:
            {
                // add_mode_ ^= 1;
                executeClipper();
            }
            break;

            default:
            {
            }
            break;
        }

        updateDebugText();
    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void TestClipper::update(float delta)
{
    Scene::update(delta);
}

void TestClipper::doJob()
{
    cocos2d::Size const kVisible = cocos2d::Director::getInstance()->getVisibleSize();
    // draw_delau_tri_->clear();
    if(!poly_list_.size()) return;
    // ClipperLib::Clipper c;
    // ClipperLib::Paths sub(1), clp(1), sol;

    // /// bounding box
    // sub[0].push_back({0, 0});
    // sub[0].push_back({kVisible.width, 0});
    // sub[0].push_back({kVisible.width, kVisible.height});
    // sub[0].push_back({0, kVisible.height});

    // for(cocos2d::Vec2 &point : poly_list_.back())
    // {
    //     clp[0].push_back({point.x, point.y});
    // }

    // ClipperLib::ClipType ct = ClipperLib::ClipType::ctXor;
    // ClipperLib::PolyFillType pft = ClipperLib::PolyFillType::pftNonZero;

    // c.AddPaths(sub, ClipperLib::PolyType::ptSubject, true);
    // c.AddPaths(clp, ClipperLib::PolyType::ptClip, true);

    // c.Execute(ct, sol, pft, pft);

    // for(ClipperLib::Path &ps : sol)
    // {
    //     std::vector<cocos2d::Vec2> poly;
    //     for(ClipperLib::IntPoint &p : ps)
    //     {
    //         poly.push_back({(float)p.X, (float)p.Y});
    //     }

    //     draw_delau_tri_->drawPolygon(poly.data(), poly.size(), cocos2d::Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0.5), 2, cocos2d::Color4F::WHITE);
    // }
}


void TestClipper::updateDebugText()
{
    std::string str;
    switch(poly_type_/*%static_cast<int>(ClipperLib::PolyType::ptClip)*/)
    {
        case ClipperLib::PolyType::ptSubject:
        {
            str += "Subject";
        }
        break;

        case ClipperLib::PolyType::ptClip:
        {
            str += "Clip";
        }
        break;

        default:
        {
            str += std::to_string(poly_type_);
        }
        break;
    };

    str += "\n";
    switch(poly_fill_type_/*%static_cast<int>(ClipperLib::PolyFillType::pftNegative)*/)
    {
        case ClipperLib::PolyFillType::pftEvenOdd:
        {
            str += "EvenOdd";
        }
        break;

        case ClipperLib::PolyFillType::pftNonZero:
        {
            str += "NonZero";
        }
        break;

        case ClipperLib::PolyFillType::pftPositive:
        {
            str += "Positive";
        }
        break;

        case ClipperLib::PolyFillType::pftNegative:
        {
            str += "Negative";
        }
        break;

        default:
        {
            str += std::to_string(poly_fill_type_);
        }
        break;
    };
    
    str += "\n";
    switch(clip_type_/*%static_cast<int>(ClipperLib::ClipType::ctXor)*/)
    {
        case ClipperLib::ClipType::ctIntersection:
        {
            str += "Intersection";
        }
        break;

        case ClipperLib::ClipType::ctUnion:
        {
            str += "Union";
        }
        break;

        case ClipperLib::ClipType::ctDifference:
        {
            str += "Difference";
        }
        break;

        case ClipperLib::ClipType::ctXor:
        {
            str += "Xor";
        }
        break;

        default:
        {
            str += std::to_string(clip_type_);
        }
        break;
    };
    
    // str += "\n";
    debug_label_->setString(str);
}


void TestClipper::executeClipper()
{
    ClipperLib::Clipper clipper;
    // ClipperLib::Clipper c;
    ClipperLib::Paths sub, clp, sol;
    ClipperLib::PolyTree sol_tree;
    const float kPrecision = 10.f;
    /// subject
    for(auto &ps : clip_paths_[0])
    {
        ClipperLib::Path path;
        for(auto &p : ps)
        {
            // path.push_back({p.x, p.y});
            path << ClipperLib::IntPoint(p.x* kPrecision, p.y * kPrecision);
        }
        sub.push_back(path);
    }

    /// clip
    for(auto &ps : clip_paths_[1])
    {
        ClipperLib::Path path;
        for(auto &p : ps)
        {
            // path.push_back({p.x, p.y});
            path << ClipperLib::IntPoint(p.x* kPrecision, p.y * kPrecision);
        }
        clp.push_back(path);
    }

    // clipper.Clear();
    clipper.AddPaths(sub, ClipperLib::PolyType::ptSubject, true);
    clipper.AddPaths(clp, ClipperLib::PolyType::ptClip, true);
    clipper.Execute((ClipperLib::ClipType)clip_type_, sol_tree, (ClipperLib::PolyFillType)poly_fill_type_, (ClipperLib::PolyFillType)poly_fill_type_);

    ClipperLib::PolyNode* p2 = sol_tree.GetFirst();

    // while(p2->IsHole())
        // p2 = p2->GetNext();

    draw_convex_s_[2]->clear();
    // for(ClipperLib::Path &ps : sol)
    for(;p2!=nullptr;p2=p2->GetNext())
    {
        if(p2->IsHole())
            continue;

        std::vector<cocos2d::Vec2> poly;
        for(ClipperLib::IntPoint &p : p2->Contour)
        {
            poly.push_back({p.X/kPrecision, p.Y/kPrecision});
        }

        drawConvexs(draw_convex_s_[2], {poly}, cocos2d::Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 1), 1);
    }
}

void TestClipper::reDrawConvexs(int type)
{
    draw_convex_s_[type]->clear();
    drawConvexs(draw_convex_s_[type], clip_paths_[type], colors_[type]);

}