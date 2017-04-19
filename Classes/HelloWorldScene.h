#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include <boost/thread.hpp>

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

    enum UiStatus {
        UiStatusReady,
        UiStatusLoad,
        UiStatusConnect
    };
    UiStatus ui_status;

    // menu ��Ӧ����
    void menuCloseCallback(cocos2d::Ref* sender);
    void menuLoadFile(cocos2d::Ref* sender);
    void menuNextRound(cocos2d::Ref* sender);
    void menuConnect(cocos2d::Ref* sender);
    void menuTest(cocos2d::Ref* sender);
    // touch ��Ӧ����
    bool touchPosition(cocos2d::Touch* touch, cocos2d::Event* event);

    // ˽�к���
    void RefreshMap();
    void ShowDiplomacy();

    // ui ָ��
    cocos2d::CCTMXTiledMap *_tileMap;
    cocos2d::CCTMXTiledMap *_tileDipMap;
    cocos2d::MenuItemLabel *loadItem, *nextRoundItem, *connectItem, *testItem;
    cocos2d::Label *position_label, *player_name_label, *status_label;
    std::vector<cocos2d::Label*>  score_list;
    std::vector<cocos2d::Sprite*> portrait_list;

    // �������
    cocos2d::Vec2 MapToUi(TPosition p);
    TPosition UiToMap(cocos2d::Vec2 v);
    TPosition p;

    // ��Ϸ
    XGHJ::Map *game_map;
    vector<vector<float> > military_kernel;
    vector<vector<MapPointInfo> > map;
    XGHJ::Game *game;
    XGHJ::LogReader *log_reader;

    // ����
    void web_hello(); // ����
    void web_logic(); // ����
    void fail_to_connect();

    boost::thread *thread_hello, *thread;
    boost::asio::io_service* io_service;
    XGHJ_Client::XghjProtocolSocket* xs;

    // �Զ�ˢ��
    void update(float dt);

};

#endif // __HELLOWORLD_SCENE_H__
