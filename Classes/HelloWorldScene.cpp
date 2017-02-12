#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#include <iostream>
#include <Windows.h>

#include "MilitaryKernelReader\military_kernel_reader.h"



USING_NS_CC;

using namespace std;

const string map_filename = "Tsinghua.map.txt"; 
const string kernel_filename = "kernel.txt"; 

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
    //////////////////////////////
    // super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

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
    loadItem->setPosition(origin + Vec2(visibleSize) - Vec2(100, 100));
    // 按钮 Next Round
    auto nextRoundLabel = Label::createWithTTF("Next Round", "fonts/Ubuntu-RI.ttf", 30);
    nextRoundLabel->setTextColor(Color4B::WHITE);
    nextRoundItem = MenuItemLabel::create(nextRoundLabel, CC_CALLBACK_1(HelloWorld::menuNextRound, this));
    nextRoundItem->setAnchorPoint(Vec2(0.5, 0.5));
    nextRoundItem->setPosition(origin + Vec2(visibleSize) - Vec2(100, 150));
    nextRoundItem->setEnabled(false);
    // 按钮 Test
    auto testLabel = Label::createWithTTF("Test", "fonts/Ubuntu-RI.ttf", 30);
    testLabel->setTextColor(Color4B::WHITE);
    testItem = MenuItemLabel::create(testLabel, CC_CALLBACK_1(HelloWorld::menuTest, this));
    testItem->setAnchorPoint(Vec2(0.5, 0.5));
    testItem->setPosition(origin + Vec2(visibleSize) - Vec2(100, 200));

    // menu 
    auto menu = Menu::create(closeItem, loadItem, nextRoundItem, testItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 加载 背景图
    auto background = Sprite::create("THU_map_color_s.png");
    background->setAnchorPoint(Vec2(0, 0));  // anchor point 设置为左下角
    background->setPosition(Vec2(0, 0)); //  
    addChild(background, 1); // z-order

    

    // 加载 tile map
    _tileMap = CCTMXTiledMap::create("map.tmx");
    _tileMap->setAnchorPoint(Vec2(0, 0));  // anchor point 设置为左下角
    _tileMap->setPosition(Vec2(0, 0)); // 
    addChild(_tileMap, 100); // z-order
    
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

        // load map
        game_map = new XGHJ::Map();
        if (!game_map->easy_load(
                FileUtils::getInstance()->fullPathForFilename(map_filename))) {
            cerr << "Cannot load map" << endl;
            return;
        }
        // load kernel
        vector<vector<float> > military_kernel;
        XGHJ::loadMilitaryKernel(military_kernel, 
            FileUtils::getInstance()->fullPathForFilename((kernel_filename)));

        // load log file
        log_reader = new XGHJ::LogReader();
        char buffer[1024];
        int len = WideCharToMultiByte(0, 0, szFileName, wcslen(szFileName), buffer, 1024, NULL, NULL);
        buffer[len] = '\0';
        if (log_reader->load(buffer)<=0) {
            cerr << "Cannot load log file " << buffer << endl;
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
    
    vector<vector<TMilitaryCommand>> MilitaryCommandMap;
    vector<vector<TDiplomaticCommand>> DiplomaticCommandMap;
    vector<TPosition> NewCapitalList;
    if (log_reader->get(round,
            MilitaryCommandMap,
            DiplomaticCommandMap,
            NewCapitalList))
        game->Run(
            MilitaryCommandMap,
            DiplomaticCommandMap,
            NewCapitalList);

    RefreshMap();

    if (game->getRound() >= log_reader->getRound())
        nextRoundItem->setEnabled(false);
    
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
    vector<vector<MapPointInfo> > map = game->getGlobalMap();

    auto layer = _tileMap->getLayer("owner");
    for (int x = 0; x < game->getCols(); ++x) {
        for (int y = 0; y < game->getRows(); ++y) {
            MapPointInfo& mpi = map[x][y];
            if (game->isPlayer(mpi.owner)) 
                layer->setTileGID(11 + mpi.owner, Vec2(x, y));
            else layer->setTileGID(0, Vec2(x, y));
        }
    }

}
