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

#include "TestDelaunay.h"
#include "SimpleAudioEngine.h"
#include "GameEntity.h"
#include "3rdparty/delaunator.hpp"
#include "Graph.h"
#include "convex.h"
// #include "3rdparty/clipper/clipper.hpp"

USING_NS_CC;

Scene* TestDelaunay::createScene()
{
    return TestDelaunay::create();
}

// on "init" you need to initialize your instance
bool TestDelaunay::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    draw_debug_ = DrawNode::create();
    draw_convex_ = DrawNode::create();
    draw_delau_tri_ = DrawNode::create();
    addChild(draw_debug_, 3);
    addChild(draw_convex_, 1);
    addChild(draw_delau_tri_, 1);

    // /////////////////////////////
    // // 2. add a menu item with "X" image, which is clicked to quit the program
    // //    you may modify it.

    // // add a "close" icon to exit the progress. it's an autorelease object
    // auto closeItem = MenuItemImage::create(
    //                                        "CloseNormal.png",
    //                                        "CloseSelected.png",
    //                                        CC_CALLBACK_1(TestDelaunay::menuCloseCallback, this));

    // if (closeItem == nullptr ||
    //     closeItem->getContentSize().width <= 0 ||
    //     closeItem->getContentSize().height <= 0)
    // {
    //     problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    // }
    // else
    // {
    //     float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
    //     float y = origin.y + closeItem->getContentSize().height/2;
    //     closeItem->setPosition(Vec2(x,y));
    // }

    // // create menu, it's an autorelease object
    // auto menu = Menu::create(closeItem, NULL);
    // menu->setPosition(Vec2::ZERO);
    // this->addChild(menu, 1);

    // /////////////////////////////
    // // 3. add your codes below...

    // // add a label shokwin_size "Hello World"
    // // create and initialize a label

    // auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    // if (label == nullptr)
    // {
    //     problemLoading("'fonts/Marker Felt.ttf'");
    // }
    // else
    // {
    //     // position the label on the center of the screen
    //     label->setPosition(Vec2(origin.x + visibleSize.width/2,
    //                             origin.y + visibleSize.height - label->getContentSize().height));

    //     // add the label as a child to this layer
    //     this->addChild(label, 1);
    // }

    // // add "TestDelaunay" splash screen"
    // auto sprite = Sprite::create("TestDelaunay.png");
    // if (sprite == nullptr)
    // {
    //     problemLoading("'TestDelaunay.png'");
    // }
    // else
    // {
    //     // position the sprite on the center of the screen
    //     sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    //     // add the sprite as a child to this layer
    //     this->addChild(sprite, 0);
    // }

    // Behavior *steering = Behavior::create();
    // this->addChild(steering, 0, "behavior/steering");

    // MovingEntity *movEntity;
    // movEntity = MovingEntity::create("res/sharp_navigation_white_18dp.png");
    // movEntity->connectSignalSteering(steering/*, &Behavior::onMessage*/);
    // movEntity->setMovEntity(1, 500, 500, 10);
    // this->addChild(movEntity, 0, "movE");
    // movEntity->setPosition(static_cast<Vec2>(visibleSize/2) + origin);

    // movEntity = MovingEntity::create("res/sharp_navigation_white_18dp.png");
    // movEntity->connectSignalSteering(steering/*, &Behavior::onMessage*/);
    // movEntity->setMovEntity(1, 250, 1000, 10);
    // this->addChild(movEntity, 0, "movE");
    // movEntity->setPosition(static_cast<Vec2>(visibleSize/4) + origin);



    // auto const kwin_size = Director::getInstance()->getWinSize();

    // // obstacle
    // std::vector<Vec2> obstacle = {
    //     Vec2(kwin_size.width*5/10, kwin_size.height*2/8 /*+ 0.01f*/),
    //     Vec2(kwin_size.width*6/10, kwin_size.height*3/8 /*+ 0.01f*/),
    //     Vec2(kwin_size.width*6/10, kwin_size.height*5/8 /*+ 0.01f*/),
    //     Vec2(kwin_size.width*5/10, kwin_size.height*6/8 /*- 0.01f*/),
    //     Vec2(kwin_size.width*4/10, kwin_size.height*5/8 /*- 0.01f*/),
    //     Vec2(kwin_size.width*4/10, kwin_size.height*3/8 /*- 0.01f*/),
    // };


    // std::vector<Vec2> triangles;

    /// delaunator
    // std::vector<double> coords;

    // int const ktiles = 1;
    // int const ktile_points = ktiles + 1;

    // double const ktile_height = kwin_size.height / (ktiles);
    // double const ktile_width = kwin_size.width / (ktiles);

    // coords.reserve(ktile_points * ktile_points);

    // for(int r=0; r<ktile_points; r++)
    // {
    //     for(int c=0; c<ktile_points; c++)
    //     {
    //         if(!isInsideConvex(cocos2d::Vec2(c*ktile_width, r*ktile_height), obstacle))
    //         {
    //             coords.push_back(c*ktile_width);
    //             coords.push_back(r*ktile_height);
    //         }
    //         else
    //         {
    //             continue;
    //         }
    //     }
    // }

    // for( auto &p : obstacle)
    // {
    //     coords.push_back(p.x);
    //     coords.push_back(p.y);
    // }

    coords_.push_back(0), coords_.push_back(0);
    coords_.push_back(visibleSize.width), coords_.push_back(0);
    coords_.push_back(0), coords_.push_back(visibleSize.height);
    coords_.push_back(visibleSize.width), coords_.push_back(visibleSize.height);
    delaunator::Delaunator delaunay(coords_);
    triangles_ = std::move(delaunay.triangles);
    for(std::size_t i = 0; i < triangles_.size(); i+=3) 
    {
        Vec2 x = Vec2(delaunay.coords[2 * triangles_[i]],
                      delaunay.coords[2 * triangles_[i] + 1]);
        Vec2 y = Vec2(delaunay.coords[2 * triangles_[i + 1]],
                      delaunay.coords[2 * triangles_[i + 1] + 1]);
        Vec2 z = Vec2(delaunay.coords[2 * triangles_[i + 2]],
                      delaunay.coords[2 * triangles_[i + 2] + 1]);
        draw_delau_tri_->drawTriangle(x,y,z,Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0.5));
    }

    // draw->drawTriangle(Vec2(0,0),Vec2(kwin_size.width, kwin_size.height),Vec2(0,kwin_size.height),
                           // Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0.5));
    // 2048, 1536

    // float S, T;
    // const cocos2d::Vec2 A(kwin_size/2), B(FLT_MAX,A.y), C(kwin_size.width*6/10, kwin_size.height*3/8), D(kwin_size.width*6/10, kwin_size.height*5/8);
    // int rs = cocos2d::Vec2::isLineIntersect(A, B, C, D, &S, &T);
    // const float denom = crossProduct2Vector(A, B, C, D);
    // const float f1 = crossProduct2Vector(C, D, C, A);
    // const float f2 = crossProduct2Vector(A, B, C, A);

    // CCLOG("(%.2f:%.2f %.2f:%.2f) x (%.2f:%.2f %.2f:%.2f) = %d %.2f %.2f", 
    //       A.x,A.y, B.x,B.y, 
    //       C.x,C.y, D.x,D.y, 
    //       rs, S, T);
    // CCLOG("%.2f %.2f %.2f %.2f %.2f", denom, f1, f2, f1/denom, f2/denom);

    initTouchEvent();
    scheduleUpdate();
    return true;
}

// void TestDelaunay::setHomeScene(cocos2d::Scene *scene)
// {
//     home_scene_ = scene;
// }

// void TestDelaunay::setBackScene(cocos2d::Scene *scene)
// {
//     if(scene == nullptr) return;

//     if(back_scene_)
//         back_scene_->release();
    
//     back_scene_ = scene;
//     back_scene_->retain();
// }

void TestDelaunay::doJob()
{
    draw_delau_tri_->clear();
    coords_.clear();
    auto const kVisible = Director::getInstance()->getVisibleSize();
    coords_.push_back(0), coords_.push_back(0);
    coords_.push_back(kVisible.width), coords_.push_back(0);
    coords_.push_back(0), coords_.push_back(kVisible.height);
    coords_.push_back(kVisible.width), coords_.push_back(kVisible.height);

    for(std::vector<cocos2d::Vec2> &poly : poly_list_)
    {
        for(cocos2d::Vec2 &points : poly)
        {
            coords_.push_back(points.x);
            coords_.push_back(points.y);
        }
    }

    doDelaunay();
}

void TestDelaunay::doDelaunay()
{
    delaunator::Delaunator delaunay(coords_);
    triangles_ = std::move(delaunay.triangles);
    for(std::size_t i = 0; i < triangles_.size(); i+=3) 
    {
        Vec2 x = Vec2(delaunay.coords[2 * triangles_[i]],
                      delaunay.coords[2 * triangles_[i] + 1]);
        Vec2 y = Vec2(delaunay.coords[2 * triangles_[i + 1]],
                      delaunay.coords[2 * triangles_[i + 1] + 1]);
        Vec2 z = Vec2(delaunay.coords[2 * triangles_[i + 2]],
                      delaunay.coords[2 * triangles_[i + 2] + 1]);
        draw_delau_tri_->drawTriangle(x,y,z,
                                     Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 0.5));
    }
}

void TestDelaunay::initTouchEvent()
{
    EventListenerTouchAllAtOnce *listener = EventListenerTouchAllAtOnce::create();
    prev_touch_ = cocos2d::Vec2::ZERO;
    /// pressed event
    listener->onTouchesBegan = [=] (const std::vector<Touch*>& touches, Event *pEvent) mutable
    {
        cocos2d::Vec2 curr_touch = touches[0]->getLocation();

        /// remove or start a convex
        if (pts_.empty())
        {
            bool is_removing = false;
            for(decltype(poly_list_.rbegin()) it = poly_list_.rbegin(); it != poly_list_.rend(); it++)
            {
                if(pointInPoly2D(*it, curr_touch))
                {
                    /// remove
                    poly_list_.erase((std::next(it)).base());
                    doJob();
                    is_removing = true;
                    break;
                }
            }

            if(!is_removing)
            {
                pts_.push_back(curr_touch);
                draw_debug_->drawDot(curr_touch, 3, cocos2d::Color4F::YELLOW);
            }
            else
            {
                draw_convex_->clear();
                for(auto &poly : poly_list_)
                    draw_convex_->drawPolygon(poly.data(), poly.size(), cocos2d::Color4F(), 2, cocos2d::Color4F::WHITE);
            }
        }
        else /// add point or complete convex
        {
            /// add new point or complete
            if (prev_touch_ != curr_touch) /// add new point
            {
                pts_.push_back(curr_touch);
                convexhull2D(pts_, hull_);

                draw_debug_->drawDot(curr_touch, 3, cocos2d::Color4F::YELLOW);

                draw_debug_->drawPolygon(hull_.data(), hull_.size(), cocos2d::Color4F(), 3, cocos2d::Color4F::YELLOW);
            }
            else
            {
                draw_debug_->clear();
                pts_.clear();
                /// complete convex
                if(hull_.size() > 2)
                {
                    poly_list_.push_back(hull_);
                    draw_convex_->drawPolygon(hull_.data(), hull_.size(), cocos2d::Color4F(), 2, cocos2d::Color4F::WHITE);

                    doJob();
                }
            }
        }
        
        prev_touch_ = curr_touch;
    };
    /// add event
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
}

void TestDelaunay::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}

void TestDelaunay::update(float delta)
{
    Scene::update(delta);

    enumerateChildren("//movE", [=](Node *node)
    {
        MovingEntity *movEntity = reinterpret_cast<MovingEntity*>(node);
        // if(movEntity)    
            // movEntity->setTarget();

            // movEntity->setPosition(movEntity->getPosition() + cocos2d::Vec2(100 * delta,0));
        return false;
    });
}

bool Behavior::init()
{
    return true;
}

cocos2d::Vec2 Behavior::onMessage(std::string const &msg, 
                                  float maxSpeed,
                                  cocos2d::Vec2 const &velocity, 
                                  cocos2d::Vec2 const &crrPos, 
                                  cocos2d::Vec2 const &target)
{
    cocos2d::Vec2 ret = cocos2d::Vec2::ZERO;

    if(msg == "seek")
    {
        // Vector2D DesiredVelocity = (TargetPos - m_pVehicle->Pos()).getNormalized()
                                    // * m_pVehicle->MaxSpeed();

        // return (DesiredVelocity - m_pVehicle->Velocity());
        ret = (target - crrPos).getNormalized() * maxSpeed;
        return ret - velocity;
    }

    return ret;
}

// void TestDelaunay::onEnter()
// {
//     Scene::onEnter();
// }