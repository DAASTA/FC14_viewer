#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "XiangGuHuaJi\game.h"
#include "LogReader\LogReader.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:

    // 响应函数
    void menuCloseCallback(Ref* sender);
    void menuLoadFile(Ref* sender);
    void menuNextRound(Ref* sender);
    void menuTest(Ref* sender);

    // 私有函数
    void RefreshMap();

    // ui 指针
    cocos2d::CCTMXTiledMap *_tileMap;
    cocos2d::CCTMXTiledMap *_tileDipMap;
    cocos2d::MenuItemLabel *loadItem, *nextRoundItem, *testItem;
    std::vector<cocos2d::Label*>  score_list;

    // 游戏
    XGHJ::Map *game_map;
    XGHJ::Game *game;
    XGHJ::LogReader *log_reader;
};

#endif // __HELLOWORLD_SCENE_H__
