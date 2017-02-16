#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "XiangGuHuaJi\game.h"
#include "LogReader\LogReader.h"
#include "HuaJiClient\xghj_protocol.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:

    // ��Ӧ����
    void menuCloseCallback(Ref* sender);
    void menuLoadFile(Ref* sender);
    void menuNextRound(Ref* sender);
    void menuConnect(Ref* sender);
    void menuTest(Ref* sender);

    // ˽�к���
    void RefreshMap();

    // ui ָ��
    cocos2d::CCTMXTiledMap *_tileMap;
    cocos2d::CCTMXTiledMap *_tileDipMap;
    cocos2d::MenuItemLabel *loadItem, *nextRoundItem, *connectItem, *testItem;
    std::vector<cocos2d::Label*>  score_list;

    // ��Ϸ
    XGHJ::Map *game_map;
    vector<vector<float> > military_kernel;
    XGHJ::Game *game;
    XGHJ::LogReader *log_reader;

    // ����
    boost::asio::io_service* io_service;
    XGHJ_Client::XghjProtocolSocket* xs;
};

#endif // __HELLOWORLD_SCENE_H__
