#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"


#include <boost/bind.hpp>

#include <iostream>
#include <Windows.h>

#include "MilitaryKernelReader\military_kernel_reader.h"

USING_NS_CC;

using namespace std;

const string map_filename = "Tsinghua.map.txt"; 
const string kernel_filename = "kernel.txt"; 
const string web_filename = "web.ini";

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

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    // super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // menu
    {
        // 菜单按钮 closeItem
        auto closeItem = MenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
        closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));
        // 按钮 Load
        auto loadLabel = Label::createWithTTF("Load", "fonts/Ubuntu-RI.ttf", 30);
        loadLabel->setTextColor(Color4B::WHITE);
        loadItem = MenuItemLabel::create(loadLabel, CC_CALLBACK_1(HelloWorld::menuLoadFile, this));
        loadItem->setAnchorPoint(Vec2(0.5, 0.5));
        loadItem->setPosition(origin + Vec2(visibleSize) - Vec2(120, 100));
        // 按钮 Next Round
        auto nextRoundLabel = Label::createWithTTF("Next Round", "fonts/Ubuntu-RI.ttf", 30);
        nextRoundLabel->setTextColor(Color4B::WHITE);
        nextRoundItem = MenuItemLabel::create(nextRoundLabel, CC_CALLBACK_1(HelloWorld::menuNextRound, this));
        nextRoundItem->setAnchorPoint(Vec2(0.5, 0.5));
        nextRoundItem->setPosition(origin + Vec2(visibleSize) - Vec2(120, 150));
        nextRoundItem->setEnabled(false);

        // 联网 Connect
        auto connectLabel = Label::createWithTTF("Connect", "fonts/Ubuntu-RI.ttf", 30);
        connectLabel->setTextColor(Color4B::WHITE);
        connectItem = MenuItemLabel::create(connectLabel, CC_CALLBACK_1(HelloWorld::menuConnect, this));
        connectItem->setAnchorPoint(Vec2(0.5, 0.5));
        connectItem->setPosition(origin + Vec2(visibleSize) - Vec2(120, 200));

        // 按钮 Test
        auto testLabel = Label::createWithTTF("Test", "fonts/Ubuntu-RI.ttf", 30);
        testLabel->setTextColor(Color4B::WHITE);
        testItem = MenuItemLabel::create(testLabel, CC_CALLBACK_1(HelloWorld::menuTest, this));
        testItem->setAnchorPoint(Vec2(0.5, 0.5));
        testItem->setPosition(origin + Vec2(visibleSize) - Vec2(120, 250));

        // menu 
        auto menu = Menu::create(closeItem, loadItem, nextRoundItem, connectItem, testItem, nullptr);
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
        for (int x = 0; x < _tileMap->getMapSize().width; ++x) {
            auto label = Label::createWithTTF(to_string(x), "fonts/Ubuntu-RI.ttf", 10);
            label->setAnchorPoint(Vec2(0, 0.5));
            label->setRotation(-90);
            label->setPosition(Vec2(16 * x + 28, 580));
            addChild(label);
        }
        for (int y = 0; y < _tileMap->getMapSize().height; ++y) {
            auto label = Label::createWithTTF(to_string(y), "fonts/Ubuntu-RI.ttf", 10);
            label->setAnchorPoint(Vec2(1, 0.5));
            label->setPosition(Vec2(14, 28 + (_tileMap->getMapSize().height - 1 - y) * 16));
            addChild(label);
        }
    }

    // dip map
    {
        // 加载 _tileDipMap
        _tileDipMap = CCTMXTiledMap::create("dip.tmx");
        _tileDipMap->setAnchorPoint(Vec2(0, 0));  // anchor point 设置为左下角
        _tileDipMap->setPosition(Vec2(500, 20)); // 
        addChild(_tileDipMap, 99); // z-order
        // label for tileDipMap
        for (int id = 0; id < 8; ++id) {
            Label* label = Label::createWithTTF("0/0", "fonts/Ubuntu-RI.ttf", 12);
            label->setAnchorPoint(Vec2(0.5, 0)); // 下边中点
            label->setPosition(Vec2(460, 20 + (7 - id) * 16));
            score_list.push_back(label);
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
        XGHJ::loadMilitaryKernel(military_kernel,
            FileUtils::getInstance()->fullPathForFilename((kernel_filename)));
    }

    return true;
}


void HelloWorld::menuCloseCallback(Ref* sender)
{
    // stop it 
    Director::getInstance()->end();
}

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
        RefreshMap();

        // enable
        nextRoundItem->setEnabled(true);
    }

}

void HelloWorld::menuNextRound(Ref * sender)
{
    if (game == nullptr) return;

    TRound round = game->getRound();
    int player_size = game->getPlayerSize();

    vector<vector<TMilitaryCommand>> MilitaryCommandMap;
    vector<vector<TDiplomaticCommand>> DiplomaticCommandMap;
    vector<TPosition> NewCapitalList;

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
        if (game->getRound() >= log_reader->getRound())
            nextRoundItem->setEnabled(false);
    
}

void HelloWorld::menuConnect(Ref * sender)
{
    int player_size = 0;

    if (thread != nullptr) {
        return;
    }

    if (io_service == nullptr) io_service = new boost::asio::io_service;



    // init
    try
    {
        std::ifstream web_ifs(FileUtils::getInstance()->fullPathForFilename(web_filename));
        std::string server_ip;
        int server_port;
        if (!web_ifs.is_open()) return;
        web_ifs >> server_ip;
        web_ifs >> server_port;

        xs = new XGHJ_Client::XghjProtocolSocket(*io_service, server_ip, server_port);
        if (!xs->isValid()) return;
        io_service->run();

    }
    catch (exception e) {
        CCLOG(e.what());
        return;
    }

    // first hello
    try
    {
        using namespace XGHJ_Client;
        XghjObject obj(XghjObject::Viewer, XghjObject::NewGame, "UI Log Viewer");
        xs->send(obj);
        obj = xs->getObj(); CCLOG(obj.content.c_str());
        if (obj.action != XghjObject::OK) return;
    }
    catch (exception e) {
        CCLOG(e.what());
        return;
    }

    loadItem->setEnabled(false);
    nextRoundItem->setEnabled(false);

    // new thread
    {
        boost::function0<void> f = boost::bind(&HelloWorld::web_logic, this);
        thread = new boost::thread(f);
        thread->timed_join(boost::posix_time::milliseconds(200));
    }

}

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

void HelloWorld::RefreshMap()
{

    if (game == nullptr) return;
    int cols = game->getCols();
    int rows = game->getRows();
    int player_size = game->getPlayerSize();
    vector<vector<MapPointInfo> > map = game->getGlobalMap();
    vector<vector<TDiplomaticStatus> > dip = game->getDiplomacy();
    vector<TMoney> sav = game->getPlayerSaving();
    vector<TMoney> inc = game->getPlayerIncome();
    vector<int> rank = game->getPlayerRanking();

    CCLOG("refresh map");

    // military icon
    if (log_reader != nullptr)
    {
        auto layer_icon = _tileMap->getLayer("icon");
        vector<vector<TId> > tmc_map(cols,
            vector<TId>(rows, UNKNOWN_PLAYER_ID));
        vector<vector<TMilitaryCommand>> MilitaryCommandMap;
        vector<vector<TDiplomaticCommand>> DiplomaticCommandMap;
        vector<TPosition> NewCapitalList;
        if (log_reader->get(game->getRound(),
            MilitaryCommandMap,
            DiplomaticCommandMap,
            NewCapitalList)) {

            for (TId id = 0; id < player_size; ++id)
                for (size_t i = 0; i < MilitaryCommandMap[id].size(); ++i) {
                    TMilitaryCommand& tmc = MilitaryCommandMap[id][i];
                    if (tmc.bomb_size > 0 && tmc.place.x < cols && tmc.place.y < rows)
                        tmc_map[tmc.place.x][tmc.place.y] = id;

                }
            for (TId id = 0; id < player_size; ++id)
                if (NewCapitalList[id].x < cols && NewCapitalList[id].y) {
                    tmc_map[NewCapitalList[id].x][NewCapitalList[id].y] = id;
                }

            for (TMap x = 0; x < cols; ++x)
                for (TMap y = 0; y < rows; ++y) {
                    if (game->isPlayer(tmc_map[x][y])) {
                        layer_icon->setTileGID(11 + tmc_map[x][y], Vec2(x, y));
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
        auto layer_icon = _tileMap->getLayer("icon");
        for (TMap x = 0; x < cols; ++x)
            for (TMap y = 0; y < rows; ++y)
                layer_icon->setTileGID(0, Vec2(x, y));

    }


    // map color & shadow
    {
        auto layer_color = _tileMap->getLayer("color");
        auto layer_shadow = _tileMap->getLayer("shadow");
        for (int x = 0; x < cols; ++x)
            for (int y = 0; y < rows; ++y) {
                MapPointInfo& mpi = map[x][y];
                int gid_color = 0;
                int gid_shadow = 0;

                if (game->isPlayer(mpi.owner)) {
                    gid_color = mpi.owner + 1;
                    if (mpi.isSieged) gid_shadow = 21;
                }

                layer_color->setTileGID(gid_color, Vec2(x, y));
                layer_shadow->setTileGID(gid_shadow, Vec2(x, y));
            }
    }



    // dip
    {
       
        auto layer_dip = _tileDipMap->getLayer("dip");
        auto layer_icon = _tileDipMap->getLayer("icon");
        vector<int> order(player_size);

        for (int i = 0; i < player_size; ++i) {
            layer_icon->setTileGID(11 + rank[i], Vec2(0, 1 + i));
            order[rank[i]] = i;
        }
        for (TId id = 0; id < player_size; ++id) {
            if (game->isAlive(id))
                layer_icon->setTileGID(11 + id, Vec2(1 + id, 0));
            else
                layer_icon->setTileGID(0, Vec2(1 + id, 0));
        }

        for (TId i1=0; i1<player_size; ++i1)
            for (TId i2 = 0; i2 < player_size; ++i2) {
                if (i1 != i2) layer_dip->setTileGID(1 + dip[i1][i2], Vec2(1 + i1, 1 + order[i2]));
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


    //auto layer_border = _tileMap->getLayer("border");
    //vector<vector<int> > map_border(cols,
    //    vector<int>(rows, 0));
    //int dx[] = { 0,  1,  0, -1};
    //int dy[] = {-1,  0,  1,  0};
    //int bd[] = {1, 2, 4, 8};
    //for (int x = 0; x < cols; ++x) 
    //    for (int y = 0; y < rows; ++y) {
    //        if (!game->isPlayer(map[x][y].owner)) {
    //            layer_border->setTileGID(0, Vec2(x, y));
    //            continue;
    //        }
    //        for (int i = 0; i < 4; ++i) {
    //            int lx = x + dx[i], ly = y + dy[i];
    //            if (lx < 0 || lx >= cols || ly < 0 || ly >= rows) map_border[x][y] += bd[i];
    //            else if (map[lx][ly].owner != map[x][y].owner) map_border[x][y] += bd[i];
    //        }
    //        /*if (map_border[x][y] == 0)layer_border->setTileGID(0, Vec2(x, y));
    //        else*/ layer_border->setTileGID(16 * map[x][y].owner + 1 + map_border[x][y], Vec2(x, y));
    //    }
    
}

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
