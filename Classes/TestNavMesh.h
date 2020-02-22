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

#include <cocos2d.h>
#include <ui/CocosGUI.h>

#include "BaseScene.h"
#include "Poly2D.h"

class TestNavMesh : public BaseScene
{
public:
    TestNavMesh() : BaseScene()
    {}

    static cocos2d::Scene* createScene();

    virtual bool init() override;
    virtual void update(float) override;
    
    void initEvents();
    void doJob();

    // implement the "static create()" method manually
    CREATE_FUNC(TestNavMesh);

private:
    std::vector<std::size_t> triangles_;
    std::vector<double> coords_;

    cocos2d::Color4F colors_[3];

    std::vector<cocos2d::Vec2> touch_points_;
    std::vector<cocos2d::Vec2> hull_;
    std::list<Polygon2D> polygons_;
    cocos2d::Vec2 prev_touch_;

    cocos2d::DrawNode *draw_debug_, 
                      *draw_convex_, 
                      *draw_tris_;
                      
    cocos2d::Layer *debug_layer_,
                    *act_layer_;

    bool ctrl_;

    cocos2d::ui::Text *debug_text_;
};

