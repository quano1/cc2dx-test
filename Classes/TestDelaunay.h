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
#include "recast/Recast/Recast.h"

inline void truncate(cocos2d::Vec2 *vec, double max)
{
    if ((*vec).length() > max)
    {
        (*vec).normalize();
        (*vec) *= max;
    }
}

class Polygon2D
{
public:
    Polygon2D();
    Polygon2D(std::vector<cocos2d::Vec2> &shape);
    ~Polygon2D() = default;
    std::tuple<cocos2d::Vec2&, cocos2d::Vec2&, cocos2d::Vec2&> triangle(size_t idx);

    std::vector<cocos2d::Vec2> shape_;              //< points
    cocos2d::Vec2 bmin_, bmax_; //< bounding box
    std::vector<int> tris_; //< triangles
};

class BuildContex : public rcContext
{
public:
    void doLog(const rcLogCategory /*category*/, const char* msg, const int /*len*/) 
    {
        CCLOG("dolog: %s\n", msg);
    }

    /// Clears all timers. (Resets all to unused.)
    void doResetTimers() 
    {

    }

    /// Starts the specified performance timer.
    ///  @param[in]     label   The category of timer.
    void doStartTimer(const rcTimerLabel /*label*/) 
    {

    }

    /// Stops the specified performance timer.
    ///  @param[in]     label   The category of the timer.
    void doStopTimer(const rcTimerLabel /*label*/) 
    {

    }

    /// Returns the total accumulated time of the specified performance timer.
    ///  @param[in]     label   The category of the timer.
    ///  @return The accumulated time of the timer, or -1 if timers are disabled or the timer has never been started.
    int doGetAccumulatedTime(const rcTimerLabel /*label*/) const 
    { 
        return -1; 
    }
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
    void initTouchEvent();
    void drawPolygons();
    bool doRecast();

    CREATE_FUNC(TestDelaunay);

    std::vector<cocos2d::Vec2> touch_points_;
    std::vector<cocos2d::Vec2> hull_;
    std::list<Polygon2D> polygons_;
    cocos2d::Vec2 prev_touch_;

    cocos2d::DrawNode *draw_debug_, 
                      *draw_convex_, 
                      *draw_delau_tri_;

    BuildContex m_ctx;
    rcHeightfield* m_solid;
    rcCompactHeightfield* m_chf;
    rcContourSet* m_cset;
    rcPolyMesh* m_pmesh;
    rcPolyMeshDetail* m_dmesh;
    rcConfig m_cfg;

private:
};

