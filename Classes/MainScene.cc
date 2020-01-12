#include "MainScene.h"
#include "extensions/cocos-ext.h"
#include "TestDelaunay.h"
#include "TestClipper.h"
#include "TestPoly2tri.h"

USING_NS_CC;

static const float kBtnWidth = 360.f;
static const float kBtnHeight = 60.f;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainScene.cpp\n");
}

/// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    const auto kVisible = cocos2d::Director::getInstance()->getVisibleSize();

    ui::Layout* layout = ui::Layout::create();
    this->addChild(layout);
    layout->setBackGroundImage("ui/background.png");
    layout->setContentSize(kVisible);
    layout->setBackGroundImageScale9Enabled(true);

    // Create the list view
    list_view_ = cocos2d::ui::ListView::create();
    layout->addChild(list_view_);
    list_view_->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
    list_view_->setBounceEnabled(true);
    list_view_->setBackGroundImage("ui/scrollviewbg.png");
    list_view_->setBackGroundImageScale9Enabled(true);
    list_view_->setScrollBarPositionFromCorner(Vec2(7, 7));
    list_view_->setContentSize({kVisible.width *3/4, kVisible.height});
    list_view_->setPosition({kVisible.width/8, 0});

    addAllScene();

    return true;
}

void MainScene::addAllScene()
{
    // for(int i=0; i<50; i++)
    {
        addScene<TestDelaunay>("Test Delaunay");
        addScene<TestClipper>("Test Clipper");
        addScene<TestPoly2tri>("Test Poly2tri");
    }

}
