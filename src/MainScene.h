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
#include <ui/CocosGUI.h>

// #include "logger.h"
// extern tll::Logger<0x400, 0x1000, 1000> *plogger;

class MainScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // virtual void update(float) override;
    
    /// a selector callback
    // void menuCloseCallback(cocos2d::Ref* sender);
    // void initTouchEvent();

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);
    void addAllScene();

    // void addScene(std::string const &scene_name, std::function<cocos2d::Scene *(void *p)> next_scene);
    template <class S>
    void addScene(const std::string &scene_name);

private:
	// Behavior m_steeringBehavior;
	cocos2d::ui::ScrollView *scroll_view_;
    cocos2d::Scene *back_scene_;
    cocos2d::ui::ListView *list_view_;
	// cocos2d::Vector<cocos2d::Scene *> m_scene_list;

};

template <class S>
void MainScene::addScene(const std::string &scene_name)
{
    // TLL_GLOGTF();
    using namespace cocos2d;
    using namespace cocos2d::ui;
    const auto kVisible = cocos2d::Director::getInstance()->getVisibleSize();
    const auto kListViewSize = list_view_->getContentSize();
    const auto kGridSize = cocos2d::Size{kListViewSize.width, kListViewSize.height/10};
    const auto kButtonSize = cocos2d::Size{kListViewSize.width / 2, kListViewSize.height/10};
    const float kMargin = kListViewSize.height / 200;

    auto layout = Layout::create();
    layout->setLayoutType(Layout::Type::VERTICAL);
    layout->setContentSize(kGridSize);
    auto button = Button::create("ui/button.png", "ui/buttonHighlighted.png");
    // layout->addChild(button);
    button->setScale9Enabled(true);
    button->setContentSize(kButtonSize);
    button->setTitleText(scene_name);
    button->setTitleFontSize(12.5f * kVisible.height / (15 * 15));
    /// touch event
    button->addTouchEventListener([=](cocos2d::Ref *pSender, ui::Widget::TouchEventType type)
    {
        if(type != ui::Widget::TouchEventType::ENDED) return;
        auto scene = S::createScene();
        scene->setName(scene_name);
        cocos2d::Director::getInstance()->pushScene(this);
        cocos2d::Director::getInstance()->pushScene(scene);
    });

    LinearLayoutParameter* llp = LinearLayoutParameter::create();
    button->setLayoutParameter(llp);
    llp->setGravity(LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL);
    llp->setMargin({0.0f, kMargin, 0.0f, kMargin});

    layout->addChild(button);
    list_view_->pushBackCustomItem(layout);
}