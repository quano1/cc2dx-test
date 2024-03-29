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

#include "AppDelegate.hh"
#include "MainScene.h"
#include "TestDelaunay.h"
#include "TestClipper.h"
// #include "logger.h"

// tll::Logger<0x400, 0x1000, 1000> *plogger;

// #define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
#endif

USING_NS_CC;

static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);
static cocos2d::Size currentReso = cocos2d::Size(3480, 2160);

static cocos2d::Size designResolutionSize = largeResolutionSize * 0.8f;

AppDelegate::AppDelegate()
{
    // plogger = new tll::Logger<0x400, 0x1000, 1000>(tll::LogEntity{tll::mask::all, 
    //             [](){return static_cast<void*>(new std::ofstream("cc2dx.prj.log", std::ios::out | std::ios::binary));}, 
    //             [](void *handle){delete static_cast<std::ofstream*>(handle);},
    //             [&](void *handle, const char *buff, size_t size){
    //                 static_cast<std::ofstream*>(handle)->write((const char *)buff, size);
    //                 static_cast<std::ofstream*>(handle)->flush();
    //             }, 0x10000, nullptr});
}

AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#endif
    if(main_scene_) delete main_scene_;
    // delete plogger;
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    // auto director = Director::getInstance();
    // auto console = director->getConsole();
    // console->listenOnTCP(65500);
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

        
        glview = GLViewImpl::createWithRect("cpp-prj", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
        // glview = GLViewImpl::createWithFullScreen("cpp-prj");
#else
        glview = GLViewImpl::create("cpp-prj");
#endif
        director->setOpenGLView(glview);
    }

    // TLL_GLOGTF();
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

    register_all_packages();

    // create a scene. it's an autorelease object
    // auto scene = MainScene::createScene();
    main_scene_ = new (std::nothrow) MainScene();
    if(main_scene_ == nullptr || main_scene_->init() == false) return false;

    auto keyboardListener = cocos2d::EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode code, cocos2d::Event *event)
    {
        std::string keyCode = std::to_string(static_cast<int>(code));
        switch(code)
        {
            case cocos2d::EventKeyboard::KeyCode::KEY_P:
            {
                static uint8_t pause = 0;
                pause ^= 1;
                pause ? main_scene_->pause() : main_scene_->resume();
            }
            break;

            case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE:
            {
                director->end();
            }
            break;

            default:
            {
            }
            break;
        }

    };

    main_scene_->getEventDispatcher()->addEventListenerWithFixedPriority(keyboardListener, -11);

    /// run
    director->runWithScene(main_scene_);

    // return director->getConsole()->listenOnTCP(1011);
    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}
