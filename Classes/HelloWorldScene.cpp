#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"


#include <boost/bind.hpp>

#include <iostream>
#include <Windows.h>

#include "XiangGuHuaJi/military_kernel.h"

USING_NS_CC;

using namespace std;

const string map_filename = "Tsinghua.map.txt"; 
const string kernel_filename = "kernel.txt"; 
const string web_filename = "web_config.txt";
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

// ��ʼ�� on "init" you need to initialize your instance
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
        background->setAnchorPoint(Vec2(0, 0));  // anchor point ����Ϊ���½�
        background->setPosition(Vec2(0, 0)); //  
        addChild(background, 0); // z-order
    }

    // menu
    {
        // �˵���ť closeItem
        /*auto closeItem = MenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
        closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));*/
        // ��ť Load
        auto loadLabel = Label::createWithTTF("Load", font_filename, 48);
        loadItem = MenuItemLabel::create(loadLabel, CC_CALLBACK_1(HelloWorld::menuLoadFile, this));
        loadItem->setColor(Color3B::BLACK);
        loadItem->setDisabledColor(Color3B::GRAY);
        loadItem->setAnchorPoint(Vec2(0.5, 0.5));
        loadItem->setPosition(origin + Vec2(visibleSize) - Vec2(140, 100));
        // ��ť Next Round
        auto nextRoundLabel = Label::createWithTTF("Next", font_filename, 48);
        nextRoundItem = MenuItemLabel::create(nextRoundLabel, CC_CALLBACK_1(HelloWorld::menuNextRound, this));
        nextRoundItem->setColor(Color3B::BLACK);
        nextRoundItem->setDisabledColor(Color3B::GRAY);
        nextRoundItem->setAnchorPoint(Vec2(0.5, 0.5));
        nextRoundItem->setPosition(origin + Vec2(visibleSize) - Vec2(140, 150));
        nextRoundItem->setEnabled(false);

        // ���� Connect
        auto connectLabel = Label::createWithTTF("Connect", font_filename, 36);
        connectItem = MenuItemLabel::create(connectLabel, CC_CALLBACK_1(HelloWorld::menuConnect, this));
        connectItem->setColor(Color3B::BLACK);
        connectItem->setDisabledColor(Color3B::GRAY);
        connectItem->setAnchorPoint(Vec2(0.5, 0.5));
        connectItem->setPosition(origin + Vec2(visibleSize) - Vec2(140, 200));

        // ��ť Test
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
        // ���� ����ͼ
        auto background = Sprite::create("map.png");
        background->setAnchorPoint(Vec2(0, 0));  // anchor point ����Ϊ���½�
        background->setPosition(Vec2(20, 20)); //  
        addChild(background, 1); // z-order

        // ���� _tileMap 
        _tileMap = CCTMXTiledMap::create("map.tmx");
        _tileMap->setAnchorPoint(Vec2(0, 0));  // anchor point ����Ϊ���½�
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

    // touch event �����ʾ��Ϣ
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
        // ���� _tileDipMap
        _tileDipMap = CCTMXTiledMap::create("dip.tmx");
        _tileDipMap->setAnchorPoint(Vec2(0, 0));  // anchor point ����Ϊ���½�
        _tileDipMap->setPosition(Vec2(530, 20)); // 
        addChild(_tileDipMap, 99); // z-order
        // label for tileDipMap
        for (int id = 0; id < 8; ++id) {
            Label* label = Label::createWithTTF("0/0", font_filename, 18);
            label->setColor(Color3B::BLACK);
            label->setAnchorPoint(Vec2(0.5, 0)); // �±��е�
            label->setPosition(Vec2(480, 20 + (7 - id) * 16));
            score_list.push_back(label);
            addChild(label, 110);
        }
        {
            int id = -1;
            Label* label = Label::createWithTTF("SAV/INC", font_filename, 18);
            label->setColor(Color3B::BLACK);
            label->setAnchorPoint(Vec2(0.5, 0)); // �±��е�
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

    portrait_enabled = false;

    return true;
}


// Close����
void HelloWorld::menuCloseCallback(Ref* sender)
{
    // stop it 
    Director::getInstance()->end();
}

// Load����
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
        //szFileNameΪ��ȡ���ļ���  

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

// NextRound����
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

            if (portrait_enabled) {
                vector<int> rank = game->getPlayerRanking();
                vector<TMoney> inc = game->getPlayerIncome();
                for (int i = 0; i < rank.size(); ++i) {
                    TId a = rank[i];
                    if (inc[a] > 0) {
                        Sprite* s0 = Sprite::create("portrait/" + to_string(a) + ".png");
                        Sprite* ss = Sprite::create("portrait/allied.png");
                        s0->setAnchorPoint(Vec2(0.5, 0.5)); s0->setPosition(Vec2(660 - 250, 60 + (rank.size() - i - 1) * 48));
                        ss->setAnchorPoint(Vec2(0.5, 0.5)); ss->setPosition(Vec2(660 - 220, 60 + (rank.size() - i - 1) * 48));
                        addChild(s0, 200); 
                        addChild(ss, 201);
                        s0->runAction(Sequence::create(DelayTime::create(4.0), FadeOut::create(2.0), Hide::create(), nullptr));
                        ss->runAction(Sequence::create(DelayTime::create(4.0), FadeOut::create(2.0), Hide::create(), nullptr));
                    }
                }
            }

            nextRoundItem->setEnabled(false);
        }
    
}

// Connect����
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

// Test����
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

// �����������λ��
bool HelloWorld::touchPosition(cocos2d::Touch* touch, cocos2d::Event* event)
{
    Vec2 p = touch->getLocation();

    if (p.x > 434 && p.y < 200) portrait_enabled = !portrait_enabled;
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
                        if (portrait_enabled) {
                            Sprite* s0 = Sprite::create("portrait/" + to_string(owner) + ".png");
                            s0->setAnchorPoint(Vec2(0.5, 0.5)); s0->setPosition(Vec2(460, 285));
                            addChild(s0, 300);
                            s0->runAction(Sequence::create(DelayTime::create(3.0), FadeOut::create(1.0), Hide::create(), nullptr));
                        }
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

// ˢ�µ�ͼ
void HelloWorld::RefreshMap()
{

    if (game == nullptr) return;

    TMap cols = game->getCols();
    TMap rows = game->getRows();
    TId player_size = game->getPlayerSize();
    
    const int GID_COLOR_ICON = 1;       // ɫ��
    const int GID_MILITARY_ICON = 11;   // С����
    const int GID_SHADOW = 21;          // ��Ӱ
    const int GID_BORDER = 23;          // �߽�
    const int GID_LINK_BORDER = 151;    // �߽�������

    const int DIP_ICON = 1;         // �⽻ͼ���⽻
    const int TARGET_LEFT = 11;     // �⽻ͼ������ͷ
    const int TARGET_UP = 21;       // �⽻ͼ���ϲ��ͷ

    CCLOG("refresh map");

    // ��ͼ��Ϣ
    /*vector<vector<MapPointInfo> >*/ map = game->getGlobalMap();
    static vector<vector<TId> > tmc_map(cols, vector<TId>(rows, UNKNOWN_PLAYER_ID));
    static vector<vector<int> > mil_map(cols, vector<int>(rows, 0));
    static vector<vector<TDiplomaticStatus> > dip;
    static vector<TMoney> inc;
    vector<vector<TMilitaryCommand>> MilitaryCommandMap;
    vector<vector<TDiplomaticCommand>> DiplomaticCommandMap;
    vector<TPosition> NewCapitalList;

    // ���µ�С����
    if (ui_status == UiStatusLoad)
    {
        // ��ȡ MilitaryCommand����ʾС����

        auto layer_icon = _tileMap->getLayer("icon");
        
        if (log_reader->get(game->getRound(), 
            MilitaryCommandMap, 
            DiplomaticCommandMap, 
            NewCapitalList)) {

            // Ȼ������
            for (TMap x = 0; x < cols; ++x)
                for (TMap y = 0; y < rows; ++y)
                    tmc_map[x][y] = UNKNOWN_PLAYER_ID;

            // 
            for (TId id = 0; id < player_size; ++id)
                for (size_t i = 0; i < MilitaryCommandMap[id].size(); ++i) {
                    TMilitaryCommand& tmc = MilitaryCommandMap[id][i];
                    if (tmc.bomb_size > 0 && tmc.place.x < cols && tmc.place.y < rows) {
                        tmc_map[tmc.place.x][tmc.place.y] = id;
                        mil_map[tmc.place.x][tmc.place.y] = tmc.bomb_size;
                    }

                }
            for (TId id = 0; id < player_size; ++id)
                if (NewCapitalList[id].x < cols && NewCapitalList[id].y < rows) {
                    tmc_map[NewCapitalList[id].x][NewCapitalList[id].y] = id;
                    mil_map[NewCapitalList[id].x][NewCapitalList[id].y] = 60;
                }

            for (TMap x = 0; x < cols; ++x)
                for (TMap y = 0; y < rows; ++y) {
                    if (game->isPlayer(tmc_map[x][y])) {
                        layer_icon->setTileGID(GID_MILITARY_ICON + tmc_map[x][y], Vec2(x, y));

                        // ����С���ӵĶ���

                        if (mil_map[x][y] >= 40) {
                            float scale_size = (float)mil_map[x][y] / 20.0;
                            if (scale_size < 1.2) scale_size = 1.2;
                            if (scale_size > 7) scale_size = 7;

                            Sprite* sprite = Sprite::create("icon/" + std::to_string(tmc_map[x][y]) + ".png");
                            ScaleTo* scale_to = ScaleTo::create(0.3, scale_size);
                            FadeOut* fade_out = FadeOut::create(0.5);
                            Sequence* sequence = Sequence::create(scale_to, fade_out, Hide::create(), nullptr);
                            sprite->setAnchorPoint(Vec2(0.5, 0.5));
                            sprite->setPosition(Vec2(_tileMap->getPosition().x + 16 * x + 8, _tileMap->getPosition().y + 16 * (35 - y) - 8));
                            addChild(sprite, 200);
                            sprite->runAction(sequence);
                        }
                    }
                    else layer_icon->setTileGID(0, Vec2(x, y));
                }

        }
        else {
            for (TMap x = 0; x < cols; ++x)
                for (TMap y = 0; y < rows; ++y)
                    layer_icon->setTileGID(0, Vec2(x, y));
        }
    }
    else {
        // ���������һ��

        auto layer_icon = _tileMap->getLayer("icon");
        for (TMap x = 0; x < cols; ++x)
            for (TMap y = 0; y < rows; ++y)
                layer_icon->setTileGID(0, Vec2(x, y));

    }

    // ��ʾ��ͼ��ɫ���Ͷϲ���Ӱ
    {
        auto layer_color = _tileMap->getLayer("color");
        auto layer_shadow = _tileMap->getLayer("shadow");
        for (int x = 0; x < cols; ++x)
            for (int y = 0; y < rows; ++y) {
                MapPointInfo& mpi = map[x][y];
                int gid_color = -1;
                int gid_shadow = 0;

                if (game->isPlayer(mpi.owner)) {
                    gid_color = mpi.owner;
                    if (mpi.isSieged) gid_shadow = GID_SHADOW;
                }

                layer_color->setTileGID(GID_COLOR_ICON + gid_color, Vec2(x, y));
                layer_shadow->setTileGID(gid_shadow, Vec2(x, y));
            }
    }

    // ��ʾ�⽻Сͼ
    {
        // �⽻�;������
        vector<TMoney> sav = game->getPlayerSaving();
        vector<int> rank = game->getPlayerRanking();
        vector<bool> backstab_enabled = game->getBackstabEnabled();

        auto layer_dip = _tileDipMap->getLayer("dip");
        auto layer_icon = _tileDipMap->getLayer("icon");
        vector<int> order(player_size);
        
        vector<vector<TDiplomaticStatus> > new_dip = game->getDiplomacy();
        vector<TMoney> new_inc = game->getPlayerIncome();

        if (portrait_enabled && dip.size() == player_size && DiplomaticCommandMap.size() == player_size) {

            int dip_display_count = 0;
            int round = log_reader->getRound();
            int failed_players = 0;
            int backstab_players = 0;

            for (TId id = 0; id < player_size; ++id) {
                if (!game->isAlive(id)) ++failed_players;
            }

            for (TId a = 0; a < player_size; ++a) {
                vector<TId> new_war;
                vector<TId> new_ally;

                for (TId b = 0; b < player_size; ++b) {
                    if (new_dip[a][b] == AtWar && dip[a][b] != AtWar && (DiplomaticCommandMap[a][b] == JustifyWar || DiplomaticCommandMap[a][b] == Backstab))
                        new_war.push_back(b);
                    if (new_dip[a][b] == Allied && dip[a][b] != Allied)
                        new_ally.push_back(b);
                }

                // ����
                if (new_inc[a] == 0 && inc[a] != 0) {
                    --failed_players;
                    Sprite* s0 = Sprite::create("portrait/" + to_string(a) + ".png");
                    Sprite* ss = Sprite::create("portrait/atwar.png");
                    s0->setAnchorPoint(Vec2(0.5, 0.5)); s0->setPosition(Vec2(660 - 250, 60 + failed_players * 48));
                    ss->setAnchorPoint(Vec2(0.5, 0.5)); ss->setPosition(Vec2(660 - 220, 60 + failed_players * 48));
                    addChild(s0, 200 + round * 2);
                    addChild(ss, 201 + round * 2);
                    s0->runAction(Sequence::create(DelayTime::create(3.0), FadeOut::create(1.0), Hide::create(), nullptr));
                    ss->runAction(Sequence::create(DelayTime::create(3.0), FadeOut::create(1.0), Hide::create(), nullptr));
                }

                // ����
                if (backstab_enabled[a]) {
                    ++backstab_players;
                    Sprite* s0 = Sprite::create("portrait/" + to_string(a) + ".png");
                    Sprite* ss = Sprite::create("portrait/backstab.png");
                    s0->setAnchorPoint(Vec2(0.5, 0.5)); s0->setPosition(Vec2(660 - 250, 396 + backstab_players * 48));
                    ss->setAnchorPoint(Vec2(0.5, 0.5)); ss->setPosition(Vec2(660 - 220, 396 + backstab_players * 48));
                    addChild(s0, 200 + round * 2);
                    addChild(ss, 201 + round * 2);
                    s0->runAction(Sequence::create(DelayTime::create(5.0), FadeOut::create(2.0), Hide::create(), nullptr));
                    ss->runAction(Sequence::create(DelayTime::create(5.0), FadeOut::create(2.0), Hide::create(), nullptr));
                }

                // ��ս
                if (new_war.size() != 0) {
                    Sprite* s0 = Sprite::create("portrait/" + to_string(a) + ".png");
                    Sprite* ss = Sprite::create("portrait/atwar.png");
                    s0->setAnchorPoint(Vec2(0.5, 0.5)); s0->setPosition(Vec2(660, 200 + dip_display_count * 48));
                    ss->setAnchorPoint(Vec2(0.5, 0.5)); ss->setPosition(Vec2(660 - 30, 200 + dip_display_count * 48));
                    addChild(s0, round * 2);
                    addChild(ss, 1 + round * 2);
                    s0->runAction(Sequence::create(DelayTime::create(3.0), MoveTo::create(0.2, Vec2(730, 200 + dip_display_count * 48)), Hide::create(), nullptr));
                    ss->runAction(Sequence::create(DelayTime::create(3.0), MoveTo::create(0.2, Vec2(730, 200 + dip_display_count * 48)), Hide::create(), nullptr));
                    for (int i = 0; i < new_war.size(); ++i) {
                        Sprite* s = Sprite::create("portrait/" + to_string(new_war[i]) + ".png");
                        s->setAnchorPoint(Vec2(0.5, 0.5)); s->setPosition(Vec2(660 - 70 - 48 * i, 200 + dip_display_count * 48));
                        addChild(s, round * 2);
                        s->runAction(Sequence::create(DelayTime::create(3.0), MoveTo::create(0.2, Vec2(730, 200 + dip_display_count * 48)), Hide::create(), nullptr));
                        dip[new_war[i]][a] = dip[a][new_war[i]] = AtWar; // clear the record
                    }
                    ++dip_display_count;
                }

                // ��ƽ
                if (new_ally.size() != 0) {
                    Sprite* s0 = Sprite::create("portrait/" + to_string(a) + ".png");
                    Sprite* ss = Sprite::create("portrait/allied.png");
                    s0->setAnchorPoint(Vec2(0.5, 0.5)); s0->setPosition(Vec2(660, 200 + dip_display_count * 48));
                    ss->setAnchorPoint(Vec2(0.5, 0.5)); ss->setPosition(Vec2(660 - 30, 200 + dip_display_count * 48));
                    addChild(s0, round * 2);
                    addChild(ss, 1 + round * 2);
                    s0->runAction(Sequence::create(DelayTime::create(3.0), MoveTo::create(0.2, Vec2(730, 200 + dip_display_count * 48)), Hide::create(), nullptr));
                    ss->runAction(Sequence::create(DelayTime::create(3.0), MoveTo::create(0.2, Vec2(730, 200 + dip_display_count * 48)), Hide::create(), nullptr));
                    for (int i = 0; i < new_ally.size(); ++i) {
                        Sprite* s = Sprite::create("portrait/" + to_string(new_ally[i]) + ".png");
                        s->setAnchorPoint(Vec2(0.5, 0.5)); s->setPosition(Vec2(660 - 70 - 48 * i, 200 + dip_display_count * 48));
                        addChild(s, round * 2);
                        s->runAction(Sequence::create(DelayTime::create(3.0), MoveTo::create(0.2, Vec2(730, 200 + dip_display_count * 48)), Hide::create(), nullptr));
                    }
                    ++dip_display_count;
                }
            }

        }

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

    // �ڵ�ͼ����ʾ�߽�����ߣ�չ�ֹ����⽻��ϵ
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

// ����
void HelloWorld::web_hello()
{
    // init
    try
    {
        std::ifstream web_ifs(FileUtils::getInstance()->fullPathForFilename(web_filename));
        std::string server_ip;
        int server_port;
        if (!web_ifs.is_open()) {
            fail_to_connect();
            return;
        }
        web_ifs >> server_ip;
        web_ifs >> server_port;

        xs = new XGHJ_Client::XghjProtocolSocket(*io_service, server_ip, server_port);
        if (!xs->isValid()) {
            fail_to_connect();
            return;
        }
        io_service->run();

    }
    catch (exception e) {
        CCLOG(e.what());
        fail_to_connect();
        return;
    }

    // first hello
    try
    {
        using namespace XGHJ_Client;
        XghjObject obj(XghjObject::Viewer, XghjObject::NewGame, "UI Log Viewer");
        xs->send(obj);
        obj = xs->getObj(); CCLOG(obj.content.c_str());
        if (obj.action != XghjObject::OK) {
            fail_to_connect();
            return;
        }
    }
    catch (exception e) {
        CCLOG(e.what());
        fail_to_connect();
        return;
    }

    loadItem->setEnabled(false);
    nextRoundItem->setEnabled(false);
    connectItem->setEnabled(false);

    status_label->setString("CONNECTED");

    ui_status = UiStatusConnect;

    // new thread
    {
        boost::function0<void> f = boost::bind(&HelloWorld::web_logic, this);
        thread = new boost::thread(f);
        thread->timed_join(boost::posix_time::milliseconds(1));
    }
}

// ����
void HelloWorld::web_logic()
{
    int player_size = 0;

    // wait for the start
    try
    {
        using namespace XGHJ_Client;
        XghjObject obj;
        obj = xs->getObj(); CCLOG(obj.content.c_str());
        if (obj.action != XghjObject::NewGame) return;
        player_size = atoi(obj.content.c_str());
    }
    catch (exception e) {
        CCLOG(e.what());
        return;
    }

    // game
    game = new XGHJ::Game(*game_map, military_kernel, player_size);


    while (true) {
        if (game->getRound() == 0)
        { // bid phrase
            using namespace XGHJ_Client;
            vector<TMoney> bidPrice;
            vector<TPosition> bidPosition;
            XghjObject obj;

            obj = xs->getObj(); CCLOG(obj.content.c_str());
            if (!obj.getBidPrice(bidPrice, player_size)) {
                CCLOG("[Error] get bid price list");
                return;
            }

            obj = xs->getObj(); CCLOG(obj.content.c_str());
            if (!obj.getBidPosition(bidPosition, player_size)) {
                CCLOG("[Error] get bid position list");
                return;
            }

            game->Start(bidPrice, bidPosition);
        }
        else if (game->getRound() > 0) {

            using namespace XGHJ_Client;
            XghjObject obj;

            vector<vector<TMilitaryCommand> > MilitaryCommandMap;
            vector<vector<TDiplomaticCommand> > DiplomaticCommandMap;
            vector<TPosition > NewCapitalList;

            do {
                obj = xs->getObj(); CCLOG(obj.content.c_str());
                if (obj.action == XghjObject::GameOver || obj.action == XghjObject::Invalid) {
                    return;
                }
            } while (obj.action != XghjObject::NextRound);


            if (!obj.getMilitaryCommand(MilitaryCommandMap, DiplomaticCommandMap, NewCapitalList, player_size)) {
                CCLOG("[Error] get military command");
                return;
            }

            if (!game->Run(MilitaryCommandMap, DiplomaticCommandMap, NewCapitalList))
            {
                CCLOG("game over");
                return;
            }

            
        }

        RefreshMap();
    }
}

void HelloWorld::fail_to_connect()
{
    connectItem->setEnabled(true);
    status_label->setString("FAILED TO CONNEDT");
}
