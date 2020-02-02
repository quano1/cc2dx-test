#pragma once

#include "cocos2d.h"

template<class T>
constexpr const T& wrap( const T& v, const T& lo, const T& hi )
{
    return lo + (v%hi);
}

class BaseScene : public cocos2d::Scene
{
public:
    BaseScene() : cocos2d::Scene(), home_scene_(nullptr), back_scene_(nullptr)
    {
        setUpKeyBoard();
    }

    inline void setHomeScene(cocos2d::Scene *scene)
    {
        home_scene_ = scene;
    }

    inline void setBackScene(cocos2d::Scene *scene)
    {
        if(scene == nullptr) return;

        scene->retain();
        if(back_scene_) back_scene_->release();

        back_scene_ = scene;
    }

    inline void setUpKeyBoard()
    {
        auto keyboardListener = cocos2d::EventListenerKeyboard::create();
        keyboardListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode code, cocos2d::Event *event)
        {

            std::string keyCode = std::to_string(static_cast<int>(code));
            switch(code)
            {
                case cocos2d::EventKeyboard::KeyCode::KEY_BACKSPACE:
                {
                    // if(back_scene_ == nullptr) return;

                    // cocos2d::Director::getInstance()->replaceScene(back_scene_);
                    // back_scene_->release();
                    // back_scene_ = nullptr;
                    cocos2d::Director::getInstance()->popScene();
                }
                break;

                case cocos2d::EventKeyboard::KeyCode::KEY_HOME:
                {
                    if(home_scene_ == nullptr) return;

                    cocos2d::Director::getInstance()->replaceScene(home_scene_);
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

    virtual void onEnter() override
    {
        USING_NS_CC;

        Scene::onEnter();

        auto const kVisible = Director::getInstance()->getVisibleSize();

        TTFConfig ttfConfig("fonts/arial.ttf", 64);
        auto label = Label::createWithTTF(ttfConfig, getName(), TextHAlignment::CENTER, kVisible.width);
        label->setTextColor(Color4B::YELLOW);
        addChild(label);
        label->setPosition(Vec2(kVisible.width/2, kVisible.height - label->getContentSize().height));
    }
    
private:
    // Behavior m_steeringBehavior;
    cocos2d::Scene *back_scene_;
    cocos2d::Scene *home_scene_;

};