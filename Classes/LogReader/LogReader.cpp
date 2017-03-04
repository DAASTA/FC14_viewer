#include "LogReader.h"

#include <fstream>
#include <iostream>
#include <exception>

namespace XGHJ {

    using namespace std;

    LogReader::LogReader()
        : _round(0)
        , _player_size(0)
    {
    }

    int LogReader::load(std::string filename)
    {
        ifstream ifs;


        try {
            ifs.open(filename);
            if (!ifs.is_open()) throw exception("File not open / existed.");

            int t;

            // _player_size
            ifs >> t; _player_size = t;
            if (ifs.eof()) return -1;

            // Bid phase
            // _bid_price
            for (int i = 0; i < _player_size; ++i) {
                ifs >> t; _bid_price.push_back((TMoney)t); if (ifs.eof()) return -1;
            }
            // _bid_pos
            for (int i = 0; i < _player_size; ++i) {
                TPosition p;
                ifs >> t; p.x = t; if (ifs.eof()) return -1;
                ifs >> t; p.y = t; if (ifs.eof()) return -1;
                _bid_pos.push_back(p);
            }

            // Main phase
            while (!ifs.eof()) {
                vector<vector<TDiplomaticCommand> > diplomatic_command;
                vector<vector<TMilitaryCommand> > military_command;
                vector<TPosition> new_capital_list;
                for (int id = 0; id < _player_size; ++id) {
                    vector<TDiplomaticCommand> diplomatic;
                    vector<TMilitaryCommand> military;
                    TPosition new_capital;
                    // diplomatic
                    for (int id2 = 0; id2 < _player_size; ++id2) {
                        ifs >> t; diplomatic.push_back((TDiplomaticCommand)t);
                        if (ifs.eof()) break;
                    }
                    if (ifs.eof()) break;

                    // military
                    ifs >> t;
                    while (t != -1 && !ifs.eof()) {
                        TMilitaryCommand tmc;
                        tmc.place.x = t;
                        ifs >> t; tmc.place.y = t;
                        ifs >> t; tmc.bomb_size = (TMilitary)t;
                        ifs >> t;
                        military.push_back(tmc);
                        if (ifs.eof()) break;
                    }
                    if (ifs.eof()) break;

                    // new capital
                    ifs >> t; new_capital.x = t;
                    ifs >> t; new_capital.y = t;

                    // push_back
                    diplomatic_command.push_back(diplomatic);
                    military_command.push_back(military);
                    new_capital_list.push_back(new_capital);
                }
                if (ifs.eof()) break;

                // push_back
                _diplomatic_command_list.push_back(diplomatic_command);
                _military_command_list.push_back(military_command);
                _new_capital_list_list.push_back(new_capital_list);
                ++_round;
            }

        }
        catch (exception e) {
            cerr << "Error occurred when loading log file" << endl;
            cerr << e.what() << endl;
          //return -1; // File Error
        }

        return _round;
    }

    bool LogReader::get(TRound round, 
        vector<vector<TMilitaryCommand>>& MilitaryCommandMap, 
        vector<vector<TDiplomaticCommand>>& DiplomaticCommandMap,
        vector<TPosition>& NewCapitalList)
    {
        // CAUTIOUS: i = round - 1
        int i = round - 1;
        if (i < 0 || i >= _round) return false;

        MilitaryCommandMap = _military_command_list[i];
        DiplomaticCommandMap = _diplomatic_command_list[i];
        NewCapitalList = _new_capital_list_list[i];
        return true;
    }

    bool LogReader::getStart(vector<TMoney>& bidPrice,
        vector<TPosition>& bidPos)
    {
        if (_player_size == 0) return false;

        bidPrice = _bid_price;
        bidPos = _bid_pos;
        return true;
    }
}