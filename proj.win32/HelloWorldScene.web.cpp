// HelloWorldScene.cpp
//   主窗体，网络部分

#include "HelloWorldScene.h"

#include <boost/bind.hpp>

USING_NS_CC;

using namespace std;

const string web_filename = "web_config.txt";

// 握手
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

// 联网
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

// 连接失败
void HelloWorld::fail_to_connect()
{
    connectItem->setEnabled(true);
    status_label->setString("FAILED TO CONNEDT");
}