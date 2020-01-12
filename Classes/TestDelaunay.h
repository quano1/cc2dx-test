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

#pragma once

#include "cocos2d.h"
#include "BaseScene.h"

inline void truncate(cocos2d::Vec2 *vec, double max)
{
    if ((*vec).length() > max)
    {
        (*vec).normalize();
        (*vec) *= max;
    }
}

class Behavior : public cocos2d::Node
{
public:
    cocos2d::Vec2 onMessage(std::string const &, 
                                  float,
                                  cocos2d::Vec2 const &, 
                                  cocos2d::Vec2 const &, 
                                  cocos2d::Vec2 const &);
    virtual bool init();

    CREATE_FUNC(Behavior);

private:
};


class TestDelaunay : public BaseScene
{
public:
    TestDelaunay() : BaseScene()
    {}

    static cocos2d::Scene* createScene();

    virtual bool init() override;
    virtual void update(float) override;
    
    /// a selector callback
    void menuCloseCallback(cocos2d::Ref* sender);
    void initTouchEvent();
    void doDelaunay();
    void doJob();

    // void setHomeScene(cocos2d::Scene *scene);
    // void setBackScene(cocos2d::Scene *scene);

    // implement the "static create()" method manually
    CREATE_FUNC(TestDelaunay);

    std::vector<std::size_t> triangles_;
    std::vector<double> coords_;
    std::vector<cocos2d::Vec2> pts_;
    std::vector<cocos2d::Vec2> hull_;
    std::list<std::vector<cocos2d::Vec2>> poly_list_;
    cocos2d::Vec2 prev_touch_;

    cocos2d::DrawNode *draw_debug_, 
                      *draw_convex_, 
                      *draw_delau_tri_;

private:
    // Behavior m_steeringBehavior;
    // cocos2d::Scene *back_scene_;
    // cocos2d::Scene *home_scene_;

};

