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
#include "recast/Recast/Recast.h"
// #include "3rdparty/clipper/clipper.hpp"
#include "Poly2D.h"
#include "logger.h"

USING_NS_CC;

Scene* TestDelaunay::createScene()
{
    TLL_GLOGTF();
    return TestDelaunay::create();
}

// on "init" you need to initialize your instance
bool TestDelaunay::init()
{
    TLL_GLOGTF();
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    m_solid = nullptr;
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    draw_debug_ = DrawNode::create();
    draw_convex_ = DrawNode::create();
    draw_delau_tri_ = DrawNode::create();
    addChild(draw_debug_, 3);
    addChild(draw_convex_, 1);
    addChild(draw_delau_tri_, 1);

    initTouchEvent();
    scheduleUpdate();
    return true;
}

void TestDelaunay::drawPolygons()
{
    TLL_GLOGTF();
    draw_delau_tri_->clear();
    for(Polygon2D &poly : polygons_)
    {
        poly.drawTris(draw_delau_tri_);
    }
}

bool TestDelaunay::doRecast()
{
    TLL_GLOGTF();
    static cocos2d::Size const kVisible = cocos2d::Director::getInstance()->getVisibleSize();

    m_cfg.cs = 10;
    m_cfg.ch = 1;
    memset(m_cfg.bmin, 0, sizeof(m_cfg.bmin) * 3);
    m_cfg.bmax[0] = kVisible.width;
    m_cfg.bmax[1] = 0;
    m_cfg.bmax[2] = kVisible.height;
    m_cfg.walkableSlopeAngle = 0;
    m_cfg.walkableHeight = (int)0;
    m_cfg.walkableClimb = (int)0;
    m_cfg.walkableRadius = (int)0.5;
    // m_cfg.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
    // m_cfg.maxSimplificationError = m_edgeMaxError;
    // m_cfg.minRegionArea = (int)rcSqr(m_regionMinSize);      // Note: area = size*size
    // m_cfg.mergeRegionArea = (int)rcSqr(m_regionMergeSize);  // Note: area = size*size
    m_cfg.maxVertsPerPoly = 0x1000;
    // m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
    // m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;
    
    // if(m_solid) return false;
    // m_ctx.enableLog(false);
    m_ctx.enableTimer(true);

    m_ctx.resetTimers();
    m_ctx.startTimer(RC_TIMER_TOTAL);

    rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);
    m_solid = ::rcAllocHeightfield();
    rcCreateHeightfield(&m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch);
    /// allocate & init RC_WALKABLE_AREA spans
    for(int x=0; x<m_solid->width; x++) 
    {
        for(int y=0; y<m_solid->height; y++) 
        {
            // rcAddSpan(&m_ctx, *m_solid, x, y, (uint16_t)0, (uint16_t)0, RC_WALKABLE_AREA, m_cfg.walkableClimb);
            rcAddSpan(&m_ctx, *m_solid, x, y, (uint16_t)0, (uint16_t)0, RC_WALKABLE_AREA, m_cfg.walkableClimb);
            /// update RC_NULL_AREA spans
            for(auto &poly : polygons_)
            {
                if(isPointInPoly({(float)x,(float)y}, poly.verts_))
                {
                    m_solid->spans[x + y*m_cfg.width]->area = RC_NULL_AREA;
                }
            }
        }
    }

    m_chf = rcAllocCompactHeightfield();
    if (!m_chf)
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
        return false;
    }
    if (!rcBuildCompactHeightfield(&m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
        return false;
    }
    
    if (!false)
    {
        rcFreeHeightField(m_solid);
        m_solid = 0;
    }
        
    // Erode the walkable area by agent radius.
    if (!rcErodeWalkableArea(&m_ctx, m_cfg.walkableRadius, *m_chf))
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
        return false;
    }

    // // (Optional) Mark areas.
    // const ConvexVolume* vols = m_geom->getConvexVolumes();
    // for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
    //     rcMarkConvexPolyArea(&m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);

    if (!rcBuildDistanceField(&m_ctx, *m_chf))
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
        return false;
    }
    
    // Partition the walkable surface into simple regions without holes.
    if (!rcBuildRegions(&m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
        return false;
    }

    //
    // Step 5. Trace and simplify region contours.
    //
    
    // Create contours.
    m_cset = rcAllocContourSet();
    if (!m_cset)
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
        return false;
    }
    if (!rcBuildContours(&m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
        return false;
    }

    //
    // Step 6. Build polygons mesh from contours.
    //
    
    // Build polygon navmesh from the contours.
    m_pmesh = rcAllocPolyMesh();
    if (!m_pmesh)
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
        return false;
    }
    if (!rcBuildPolyMesh(&m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
        return false;
    }
    
    //
    // Step 7. Create detail mesh which allows to access approximate height on each polygon.
    //
    
    m_dmesh = rcAllocPolyMeshDetail();
    if (!m_dmesh)
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
        return false;
    }

    if (!rcBuildPolyMeshDetail(&m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
    {
        m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
        return false;
    }

    if (!false)
    {
        rcFreeCompactHeightfield(m_chf);
        m_chf = 0;
        rcFreeContourSet(m_cset);
        m_cset = 0;
    }
    m_ctx.stopTimer(RC_TIMER_TOTAL);
    // Show performance stats.
    // duLogBuildTimes(*m_ctx, m_ctx.getAccumulatedTime(RC_TIMER_TOTAL));
    // m_ctx.log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons", m_pmesh->nverts, m_pmesh->npolys);
    
    CCLOG("total: %f",m_ctx.getAccumulatedTime(RC_TIMER_TOTAL)/1000.0f);

	return true;
}

void TestDelaunay::initTouchEvent()
{
    TLL_GLOGTF();
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
            tll::utils::Timer pip1, pip2;
            float t1=0, t2=0;
            for(decltype(polygons_.rbegin()) it = polygons_.rbegin(); it != polygons_.rend(); it++)
            {
                // if(pointInPoly2D(it->verts_, curr_touch))
                pip1.reset();isPointInPoly(curr_touch, it->verts_); t1+=pip1.elapse();
                pip2.reset();
                bool ret = isPointInConvex<cocos2d::Vec2>(curr_touch, it->verts_); t2+=pip2.elapse();
                if(ret)
                {
                    /// remove
                    polygons_.erase((std::next(it)).base());
                    drawPolygons();
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
                    draw_convex_->drawPolygon(poly.verts_.data(), poly.verts_.size(), cocos2d::Color4F(), 2, cocos2d::Color4F::WHITE);
            }
        }
        else /// add point or complete convex
        {
            /// add new point or complete
            if (prev_touch_ != curr_touch) /// add new point
            {
                TIMER(convexhull2D);
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
                    // polygons_.back().tris_ = triangulate<cocos2d::Vec2>(polygons_.back().verts_);
                    drawPolygons();
                }
            }
        }
        
        prev_touch_ = curr_touch;
    };
    /// add event
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    auto keyboardListener = cocos2d::EventListenerKeyboard::create();
    using KeyCode = cocos2d::EventKeyboard::KeyCode;
    keyboardListener->onKeyReleased = [=](KeyCode code, cocos2d::Event *event)
    {
        std::string keyCode = std::to_string(static_cast<int>(code));
        switch(code)
        {
            case KeyCode::KEY_Q:
            {
                
            }
            break;
            /// PolyFillType
            case KeyCode::KEY_W:
            {
                
            }
            break;
            /// PolyType
            case KeyCode::KEY_E:
            {
                
            }
            break;

            case KeyCode::KEY_R:
            {
            }
            break;
            
            /// Execution
            case KeyCode::KEY_SPACE:
            {
                // add_mode_ ^= 1;
                bool ret = doRecast();
                CCLOG("[!] %d\n", ret);
            }
            break;

            default:
            {
            }
            break;
        }

    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void TestDelaunay::update(float delta)
{
    TLL_GLOGTF();
    Scene::update(delta);
}

