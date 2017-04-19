// HelloWorldScene.cpp
//   主窗体UI

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#include <boost/bind.hpp>

#include <iostream>
#include <Windows.h>

#include <cstdlib>
#include <ctime>

#include "XiangGuHuaJi/military_kernel.h"

USING_NS_CC;

using namespace std;

const string map_filename = "Tsinghua.map.txt"; 
const string kernel_filename = "kernel.txt"; 

const string font_filename = "fonts/font.ttf";

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// 初始化 on "init" you need to initialize your instance
bool HelloWorld::init()
{
    // super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    ui_status = UiStatusReady;

    // background
    {
        auto background = Sprite::create("background.png");
        background->setAnchorPoint(Vec2(0, 0));  // anchor point 设置为左下角
        background->setPosition(Vec2(0, 0)); //  
        addChild(background, 0); // z-order
    }

    // menu
    {
        // 菜单按钮 closeItem
        /*auto closeItem = MenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
        closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));*/
        // 按钮 Load
        auto loadLabel = Label::createWithTTF("Load", font_filename, 48);
        loadItem = MenuItemLabel::create(loadLabel, CC_CALLBACK_1(HelloWorld::menuLoadFile, this));
        loadItem->setColor(Color3B::BLACK);
        loadItem->setDisabledColor(Color3B::GRAY);
        loadItem->setAnchorPoint(Vec2(0.5, 0.5));
        loadItem->setPosition(origin + Vec2(visibleSize) - Vec2(140, 100));
        // 按钮 Next Round
        auto nextRoundLabel = Label::createWithTTF("Next", font_filename, 48);
        nextRoundItem = MenuItemLabel::create(nextRoundLabel, CC_CALLBACK_1(HelloWorld::menuNextRound, this));
        nextRoundItem->setColor(Color3B::BLACK);
        nextRoundItem->setDisabledColor(Color3B::GRAY);
        nextRoundItem->setAnchorPoint(Vec2(0.5, 0.5));
        nextRoundItem->setPosition(origin + Vec2(visibleSize) - Vec2(140, 150));
        nextRoundItem->setEnabled(false);

        // 联网 Connect
        auto connectLabel = Label::createWithTTF("Connect", font_filename, 36);
        connectItem = MenuItemLabel::create(connectLabel, CC_CALLBACK_1(HelloWorld::menuConnect, this));
        connectItem->setColor(Color3B::BLACK);
        connectItem->setDisabledColor(Color3B::GRAY);
        connectItem->setAnchorPoint(Vec2(0.5, 0.5));
        connectItem->setPosition(origin + Vec2(visibleSize) - Vec2(140, 200));

        // 按钮 Test
        /*auto testLabel = Label::createWithTTF("Test", font_filename, 36);
        testLabel->setTextColor(Color4B::BLACK);
        testItem = MenuItemLabel::create(testLabel, CC_CALLBACK_1(HelloWorld::menuTest, this));
        testItem->setAnchorPoint(Vec2(0.5, 0.5));
        testItem->setPosition(origin + Vec2(visibleSize) - Vec2(140, 250));*/

        // menu 
        auto menu = Menu::create(/*closeItem,*/ loadItem, nextRoundItem, connectItem, /*testItem,*/ nullptr);
        menu->setPosition(Vec2::ZERO);
        this->addChild(menu, 1);
    }

    // main map
    {
        // 加载 背景图
        auto background = Sprite::create("map.png");
        background->setAnchorPoint(Vec2(0, 0));  // anchor point 设置为左下角
        background->setPosition(Vec2(20, 20)); //  
        addChild(background, 1); // z-order

        // 加载 _tileMap 
        _tileMap = CCTMXTiledMap::create("map.tmx");
        _tileMap->setAnchorPoint(Vec2(0, 0));  // anchor point 设置为左下角
        _tileMap->setPosition(Vec2(20, 20)); // 
        addChild(_tileMap, 100); // z-order
        // label for tileMap
        /*for (int x = 0; x < _tileMap->getMapSize().width; ++x) {
            auto label = Label::createWithTTF(to_string(x), font_filename, 16);
            label->setAnchorPoint(Vec2(0, 0.5));
            label->setRotation(-90);
            label->setPosition(Vec2(16 * x + 28, 580));
            addChild(label);
        }
        for (int y = 0; y < _tileMap->getMapSize().height; ++y) {
            auto label = Label::createWithTTF(to_string(y), font_filename, 16);
            label->setAnchorPoint(Vec2(1, 0.5));
            label->setPosition(Vec2(14, 28 + (_tileMap->getMapSize().height - 1 - y) * 16));
            addChild(label);
        }*/
    }

    // touch event 鼠标提示信息
    {
        // status_label
        status_label = Label::createWithTTF("filename", font_filename, 18);
        status_label->setColor(Color3B::BLACK);
        status_label->setAnchorPoint(Vec2(0.5, 0.5));
        status_label->setPosition(Vec2(origin + Vec2(visibleSize) - Vec2(140, 50)));
        this->addChild(status_label, 200);

        // position_label
        position_label = Label::createWithTTF("x=0 y=0", font_filename, 36);
        position_label->setColor(Color3B::BLACK);
        position_label->setAnchorPoint(Vec2(0.5, 0.5));
        position_label->setPosition(Vec2(origin + Vec2(visibleSize) - Vec2(140, 300)));
        this->addChild(position_label, 200);

        // player_label
        player_name_label = Label::createWithTTF("READY", font_filename, 36);
        player_name_label->setColor(Color3B::BLACK);
        player_name_label->setAnchorPoint(Vec2(0.5, 0.5));
        player_name_label->setPosition(Vec2(origin + Vec2(visibleSize) - Vec2(140, 330)));
        this->addChild(player_name_label, 200);

        auto dispatcher = Director::getInstance()->getEventDispatcher();
        auto myListener = EventListenerTouchOneByOne::create();
        myListener->setSwallowTouches(true);
        myListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::touchPosition, this);
        myListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::touchPosition, this);
        myListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::touchPosition, this);
        dispatcher->addEventListenerWithSceneGraphPriority(myListener, this);
    }

    // dip map
    {
        // 加载 _tileDipMap
        _tileDipMap = CCTMXTiledMap::create("dip.tmx");
        _tileDipMap->setAnchorPoint(Vec2(0, 0));  // anchor point 设置为左下角
        _tileDipMap->setPosition(Vec2(530, 20)); // 
        addChild(_tileDipMap, 99); // z-order
        // label for tileDipMap
        for (int id = 0; id < 8; ++id) {
            Label* label = Label::createWithTTF("0/0", font_filename, 18);
            label->setColor(Color3B::BLACK);
            label->setAnchorPoint(Vec2(0.5, 0)); // 下边中点
            label->setPosition(Vec2(480, 20 + (7 - id) * 16));
            score_list.push_back(label);
            addChild(label, 110);
        }
        {
            int id = -1;
            Label* label = Label::createWithTTF("SAV/INC", font_filename, 18);
            label->setColor(Color3B::BLACK);
            label->setAnchorPoint(Vec2(0.5, 0)); // 下边中点
            label->setPosition(Vec2(480, 20 + (7 - id) * 16));
            addChild(label, 110);
        }
    }

    // game
    {
        // load map
        game_map = new XGHJ::Map();
        if (!game_map->easy_load(
            FileUtils::getInstance()->fullPathForFilename(map_filename))) {
            CCLOG("Cannot load map");
            return false;
        }
        // load kernel
        loadMilitaryKernel(military_kernel,
            FileUtils::getInstance()->fullPathForFilename((kernel_filename)));
    }

    // diplomatic
    {
        for (TId id = 0; id < 8; ++id) {
            auto sprite = Sprite::create("portrait/" + to_string(id) + ".png");
            portrait_list.push_back(sprite);
            sprite->setVisible(false);
            addChild(sprite, 300);
        }
    }

    this->scheduleUpdate();
    srand(time(NULL));

    return true;
}

Vec2 HelloWorld::MapToUi(TPosition p)
{
    Vec2 v;

    if (p.x < 0 || p.x >= _tileMap->getMapSize().width
        || p.y < 0 || p.y >= _tileMap->getMapSize().height) return Vec2(0, 0);

    v.x = _tileMap->getPosition().x + p.x * 16;
    v.y = _tileMap->getPosition().y + _tileMap->getContentSize().height - p.y * 16;
    return v;
}

TPosition HelloWorld::UiToMap(Vec2 v)
{
    TPosition p;
    p.x = (v.x - _tileMap->getPosition().x) / 16;
    p.y = (_tileMap->getContentSize().height - (v.y - _tileMap->getPosition().y)) / 16;
    if (p.x < 0 || p.x >= _tileMap->getMapSize().width
        || p.y < 0 || p.y >= _tileMap->getMapSize().height) return INVALID_POSITION;
    return p;
}



// Close动作
void HelloWorld::menuCloseCallback(Ref* sender)
{
    // stop it 
    Director::getInstance()->end();
}

// Load动作
void HelloWorld::menuLoadFile(Ref * sender)
{
    // load file

    OPENFILENAME ofn;
    TCHAR szFileName[MAX_PATH] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn))
    {
        //szFileName为获取的文件名  

        // load log file
        log_reader = new XGHJ::LogReader();
        char buffer[1024];
        int len = WideCharToMultiByte(0, 0, szFileName, wcslen(szFileName), buffer, 1024, NULL, NULL);
        buffer[len] = '\0';
        if (log_reader->load(buffer)<=0) {
            CCLOG("Cannot load log file ");
            return;
        }

        // set game
        game = new XGHJ::Game(*game_map, military_kernel, log_reader->getPlayerSize()); 
        // run first
        vector<int> bidPrice; vector<TPosition> bidPos;
        log_reader->getStart(bidPrice, bidPos);
        game->Start(bidPrice, bidPos);

        // enable
        nextRoundItem->setEnabled(true);
        ui_status = UiStatusLoad;

        // status label
        string name = buffer;
        
        if (name.rfind('/') != string::npos) {
            name = name.substr(name.rfind('/') + 1);
        }
        if (name.rfind('\\') != string::npos) {
            name = name.substr(name.rfind('\\') + 1);
        }
        for (char& c : name) if (!isdigit((unsigned char)c) && !isalpha((unsigned char)c) && c!='.' && c!='_') c = '*';
        status_label->setString(name);
        nextRoundItem->setString("START");

        RefreshMap();
        
    }

}

// NextRound动作
void HelloWorld::menuNextRound(Ref * sender)
{
    if (game == nullptr) {
        nextRoundItem->setString("NEXT");
        return;
    }

    TRound round = game->getRound();
    int player_size = game->getPlayerSize();

    vector<vector<TMilitaryCommand>> MilitaryCommandMap;
    vector<vector<TDiplomaticCommand>> DiplomaticCommandMap;
    vector<TPosition> NewCapitalList;

    nextRoundItem->setString("NEXT(" + to_string(round) + ")");

    if (loadItem->isEnabled()) {
        if (log_reader->get(round,
            MilitaryCommandMap,
            DiplomaticCommandMap,
            NewCapitalList))
            game->Run(
                MilitaryCommandMap,
                DiplomaticCommandMap,
                NewCapitalList);
    }
    else {


    }

    RefreshMap();

    if (log_reader!=nullptr)
        if (game->getRound() >= log_reader->getRound()) {

            //if (portrait_enabled) {
            //    vector<int> rank = game->getPlayerRanking();
            //    vector<TMoney> inc = game->getPlayerIncome();
            //    for (int i = 0; i < rank.size(); ++i) {
            //        TId a = rank[i];
            //        if (inc[a] > 0) {
            //            Sprite* s0 = Sprite::create("portrait/" + to_string(a) + ".png");
            //            Sprite* ss = Sprite::create("portrait/allied.png");
            //            s0->setAnchorPoint(Vec2(0.5, 0.5)); s0->setPosition(Vec2(660 - 250, 60 + (rank.size() - i - 1) * 48));
            //            ss->setAnchorPoint(Vec2(0.5, 0.5)); ss->setPosition(Vec2(660 - 220, 60 + (rank.size() - i - 1) * 48));
            //            addChild(s0, 200); 
            //            addChild(ss, 201);
            //            s0->runAction(Sequence::create(DelayTime::create(4.0), FadeOut::create(2.0), Hide::create(), nullptr));
            //            ss->runAction(Sequence::create(DelayTime::create(4.0), FadeOut::create(2.0), Hide::create(), nullptr));
            //        }
            //    }
            //}

            nextRoundItem->setEnabled(false);
        }
    
}

// Connect动作
void HelloWorld::menuConnect(Ref * sender)
{
    int player_size = 0;

    if (io_service == nullptr) io_service = new boost::asio::io_service;

    status_label->setString("CONNECTING... PLEASE WAIT");
    connectItem->setEnabled(false);

    // new thread
    {
        boost::function0<void> f = boost::bind(&HelloWorld::web_hello, this);
        thread_hello = new boost::thread(f);
        thread_hello->timed_join(boost::posix_time::milliseconds(1));
    }

}

// Test动作
void HelloWorld::menuTest(Ref * sender)
{

    /*static int a = 0;

    auto layer = _tileMap->getLayer("owner");
    Size s = layer->getLayerSize();

    for (int x = 0; x < s.width; ++x) {
        for (int y = 0; y < s.height; ++y) {
            layer->setTileGID(11 + (a + x + y) % 8, Vec2(x, y));
        }
    }
    ++a;*/
    RefreshMap();
}

// 点击窗口任意位置
bool HelloWorld::touchPosition(cocos2d::Touch* touch, cocos2d::Event* event)
{
    Vec2 p = touch->getLocation();

    CCLOG((to_string(p.x) + " " + to_string(p.y)).c_str());

    int x = (p.x - _tileMap->getPosition().x) / 16;
    int y = (_tileMap->getContentSize().height - (p.y - _tileMap->getPosition().y)) / 16;
    
    if (x < 0 || x >= _tileMap->getMapSize().width
        || y < 0 || y >= _tileMap->getMapSize().height) return false;

    position_label->setString("x=" + to_string(x) + " y=" + to_string(y));
    

    if (map.size() > x) 
        if (map[x].size() > y) {
            TId owner = map[x][y].owner;
            string s;
            if (owner == UNKNOWN_PLAYER_ID)
                s = "UNKOWN";
            else if (owner == NEUTRAL_PLAYER_ID)
                s = "NEUTRAL";
            else {
                switch (ui_status)
                {
                case HelloWorld::UiStatusReady:
                    s = "Ready";
                    break;
                case HelloWorld::UiStatusLoad:
                    if (log_reader != NULL) {
                        s = log_reader->getUserName(owner);
                    }
                    else s = "log_reader???";
                    break;
                case HelloWorld::UiStatusConnect:
                    s = "...";
                    break;
                default:
                    s = "???";
                    break;
                }
            }
            player_name_label->setString(s);
        }
                


    return true;
}

// 刷新地图
void HelloWorld::RefreshMap()
{

    if (game == nullptr) return;

    TMap cols = game->getCols();
    TMap rows = game->getRows();
    TId player_size = game->getPlayerSize();
    
    const int GID_COLOR_ICON = 1;       // 色块
    const int GID_MILITARY_ICON = 11;   // 小房子
    const int GID_SHADOW = 21;          // 阴影
    const int GID_BORDER = 23;          // 边界
    const int GID_LINK_BORDER = 151;    // 边界连接线

    const int DIP_ICON = 1;         // 外交图：外交
    const int TARGET_LEFT = 11;     // 外交图：左侧箭头
    const int TARGET_UP = 21;       // 外交图：上侧箭头

    CCLOG("refresh map");

    // 地图信息
    /*vector<vector<MapPointInfo> >*/ map = game->getGlobalMap();
    static vector<vector<TId> > tmc_map(cols, vector<TId>(rows, UNKNOWN_PLAYER_ID));
    static vector<vector<int> > mil_map(cols, vector<int>(rows, 0));
    static vector<vector<TDiplomaticStatus> > dip;
    static vector<TMoney> inc;
    vector<vector<TMilitaryCommand>> MilitaryCommandMap;
    vector<vector<TDiplomaticCommand>> DiplomaticCommandMap;
    vector<TPosition> NewCapitalList;

    // 清空小房子
    {
        auto layer_icon = _tileMap->getLayer("icon");
        for (TMap x = 0; x < cols; ++x)
            for (TMap y = 0; y < rows; ++y) {
                layer_icon->setTileGID(0, Vec2(x, y));
            }
    }


    // 显示地图颜色，和断补阴影
    {
        struct position_gid
        {
            Vec2 v;
            int gid_color;
            int gid_shadow;
        };
        vector<position_gid> waiting_list;

        auto layer_color = _tileMap->getLayer("color");
        auto layer_shadow = _tileMap->getLayer("shadow");

        for (int x = 0; x < cols; ++x)
            for (int y = 0; y < rows; ++y) {

                MapPointInfo& mpi = map[x][y];
                position_gid pg;
                pg.v = Vec2(x, y);
                pg.gid_color = -1;
                pg.gid_shadow = 0;

                if (game->isPlayer(mpi.owner)) {
                    pg.gid_color = mpi.owner;
                    if (mpi.isSieged) pg.gid_shadow = GID_SHADOW;
                }
                pg.gid_color = GID_COLOR_ICON + pg.gid_color;

                if (layer_color->getTileGIDAt(Vec2(x, y)) != pg.gid_color || 
                        layer_shadow->getTileGIDAt(Vec2(x, y)) != pg.gid_shadow) {
                    waiting_list.push_back(pg);
                }

                //layer_color->setTileGID(GID_COLOR_ICON + gid_color, Vec2(x, y));
                //layer_shadow->setTileGID(gid_shadow, Vec2(x, y));
            }
        
        if (waiting_list.size() > 0) {
            int cnt = waiting_list.size() / 20;
            if (cnt < 8) cnt = 8;
            for (int _ = 0; _ < cnt; ++_) {
                int n = rand() % waiting_list.size();
                layer_color->setTileGID(waiting_list[n].gid_color, waiting_list[n].v);
                layer_shadow->setTileGID(waiting_list[n].gid_shadow, waiting_list[n].v);
            }
        }

    }

    // 显示外交小图
    {
        // 外交和经济情况
        vector<TMoney> sav = game->getPlayerSaving();
        vector<int> rank = game->getPlayerRanking();
        vector<bool> backstab_enabled = game->getBackstabEnabled();

        auto layer_dip = _tileDipMap->getLayer("dip");
        auto layer_icon = _tileDipMap->getLayer("icon");
        vector<int> order(player_size);
        
        vector<vector<TDiplomaticStatus> > new_dip = game->getDiplomacy();
        vector<TMoney> new_inc = game->getPlayerIncome();


        dip = new_dip;
        inc = new_inc;

        for (int i = 0; i < player_size; ++i) {
            layer_icon->setTileGID(TARGET_LEFT + rank[i], Vec2(0, 1 + i));
            order[rank[i]] = i;
        }

        for (TId i1=0; i1<player_size; ++i1)
            for (TId i2 = 0; i2 < player_size; ++i2) {
                if (i1 != i2) layer_dip->setTileGID(DIP_ICON + dip[i1][i2], Vec2(1 + i1, 1 + order[i2]));
                else layer_dip->setTileGID(0, Vec2(1 + i1, 1 + order[i2]));
            }
        for (TId id=0; id<player_size; ++id)
            if (game->isAlive(id)) {
                score_list[order[id]]->setString(to_string(sav[id]) + "/" + to_string(inc[id]));
            }
            else {
                score_list[order[id]]->setString("-");
            }
                
    }

    // 在地图上显示边界和连线，展现国家外交关系
    {
        auto layer_border = _tileMap->getLayer("border");
        auto layer_border_bak = _tileMap->getLayer("border_bak");
        auto layer_allied = _tileMap->getLayer("allied");
        vector<vector<int> > map_border(cols,
            vector<int>(rows, 0));
        vector<vector<int> > map_allied(cols,
            vector<int>(rows, 0));
        int dx[] = { 0,  1,  0, -1 };
        int dy[] = { -1,  0,  1,  0 };
        int bd[] = { 1, 2, 4, 8 };

        for (int x = 0; x < cols; ++x)
            for (int y = 0; y < rows; ++y)
                layer_border_bak->setTileGID(layer_border->getTileGIDAt(Vec2(x, y)), Vec2(x, y));

        for (int x = 0; x < cols; ++x)
            for (int y = 0; y < rows; ++y) {
                if (!game->isPlayer(map[x][y].owner)) {
                    layer_border->setTileGID(0, Vec2(x, y));
                    continue;
                }
                for (int i = 0; i < 4; ++i) {
                    int lx = x + dx[i], ly = y + dy[i];
                    if (lx < 0 || lx >= cols || ly < 0 || ly >= rows) map_border[x][y] ;
                    else if (!game->isPlayer(map[lx][ly].owner)) map_border[x][y] ;
                    else if (dip[map[lx][ly].owner][map[x][y].owner] == AtWar) map_border[x][y] += bd[i];
                }
                /*if (map_border[x][y] == 0)layer_border->setTileGID(0, Vec2(x, y));
                else*/ layer_border->setTileGID(16 * map[x][y].owner + GID_BORDER + map_border[x][y], Vec2(x, y));
            }

        for (int x = 0; x < cols; ++x)
            for (int y = 0; y < rows; ++y) {
                if (!game->isPlayer(map[x][y].owner)) {
                    layer_allied->setTileGID(0, Vec2(x, y));
                    continue;
                }
                for (int i = 0; i < 4; ++i) {
                    int lx = x + dx[i], ly = y + dy[i];
                    if (lx < 0 || lx >= cols || ly < 0 || ly >= rows) map_allied[x][y];
                    else if (!game->isPlayer(map[lx][ly].owner)) map_allied[x][y];
                    else if (map[lx][ly].owner == map[x][y].owner) map_allied[x][y];
                    else if (dip[map[lx][ly].owner][map[x][y].owner] == Allied) map_allied[x][y] += bd[i];
                }
                /*if (map_border[x][y] == 0)layer_border->setTileGID(0, Vec2(x, y));
                else*/ layer_allied->setTileGID(16 * map[x][y].owner + GID_LINK_BORDER + map_allied[x][y], Vec2(x, y));
            }
    }
}

// 更新显示外交关系图
void HelloWorld::ShowDiplomacy()
{
    if (nullptr == game) return;

    // 瞬间显示即可

    // 计算核心位置
    vector<TPosition> show_place;
    vector<vector<MapPointInfo> > map = game->getGlobalMap();
    
    vector<TPosition> cnt_core;
    // 计算地图重心；如果重心不属于自己，则选择一个距离重心最近的点
}


// 自动刷新
void HelloWorld::update(float dt)
{
    RefreshMap();
}
