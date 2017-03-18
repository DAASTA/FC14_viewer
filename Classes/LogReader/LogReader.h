
#ifndef _XGHJ_LOGREADER_H_
#define _XGHJ_LOGREADER_H_

#include <string>
#include <vector>

#include "XiangGuHuaJi\definition.h"

namespace XGHJ {

    class LogReader {

    public:
        LogReader();

        int load(std::string filename);

        bool get(TRound round,
            std::vector<std::vector<TMilitaryCommand> > & MilitaryCommandMap,
            std::vector<std::vector<TDiplomaticCommand> > & DiplomaticCommandMap,
            std::vector<TPosition > &NewCapitalList);

        bool getStart(
            std::vector<TMoney>& bidPrice,
            std::vector<TPosition>& posChoosed);

        std::string getUserName(int i) { 
            if (i<_user_name_list.size()) return _user_name_list[i];
            else return "P" + std::to_string(i);
        }

        inline int getPlayerSize() { return _player_size; }
        inline TRound getRound() { return _round+1; }

    private:

        TRound _round;
        TId _player_size;

        std::vector<TMoney> _bid_price;
        std::vector<TPosition> _bid_pos;

        std::vector<std::vector<std::vector<TMilitaryCommand> > > _military_command_list;
        std::vector<std::vector<std::vector<TDiplomaticCommand> > > _diplomatic_command_list;
        std::vector<std::vector<TPosition> > _new_capital_list_list;

        std::vector<std::string> _user_name_list;
    };
}

#endif //_XGHJ_LOGREADER_H_