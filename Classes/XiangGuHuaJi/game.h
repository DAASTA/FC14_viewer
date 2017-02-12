/* XiangGuHuaJi 2016, game.h
 * 
 */

#ifndef _XIANGGUHUAJI_GAME_H__
#define _XIANGGUHUAJI_GAME_H__


#include <string>
#include <cmath>
#include <vector>

#include "definition.h"
#include "map.h"


using std::vector;

namespace XGHJ {
    class Game
    {
    public:
        Game(Map& map, vector<vector<float> > militaryKernel, int playersize);
        ~Game();

        bool Start(vector<TMoney> bidPrice, vector<TPosition> posChoosed);
        bool Run(vector<vector<TMilitaryCommand> > & MilitaryCommandMap,
            vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap,
            vector<TPosition > &NewCapitalList);

        bool isPlayer(TId id) const;
        vector<vector<MapPointInfo> > getGlobalMap(TId id = UNKNOWN_PLAYER_ID) const;
        PlayerInfo getPlayerInfo(TId id, TId playerId) const;
        MapPointInfo getMapPointInfo(TMap x, TMap y, TId playerId) const;
        Info generateInfo(TId playerid) const;

        inline TMap getCols() const { return cols; }
        inline TMap getRows() const { return rows; }
    inline int getMapChecksum() const { return map_checksum; }
	inline TRound getRound() const {return round ;}
	inline TId getPlayerSize() const {return playerSize ;}
    
    Map& map; 

protected:
    vector<bool> isPlayerAlive;
    bool                isValid;
    int                 map_checksum;
	TMap				rows,cols;
	TId                 playerSize;
	//��Ҫ���浽�»غϵ��м����
	TRound								round;
	vector<vector<TId> >				globalMap;//ownership of the lands
	vector<vector<TMask> >				isSieged;
	vector<TPosition>					playerCapital;
	vector<TMoney>						playerSaving;
	vector<int>							playerArea;
	vector<vector<TDiplomaticStatus> >	diplomacy;
	vector<vector<int>>					roundToJusifyWar;
	vector<bool>						backstabUsed;
    vector<TMoney>                      playerIncome;

	//һЩ��Ҫ����
	vector<TId> getWarList(TId id) const;
    TMask isPointVisible(TMap x, TMap y, TId playerId) const;
    void DiscoverCountry() ;
    TDiplomaticCommand getDefaultCommand(TDiplomaticStatus ds) const;
private:
	//�����²���ʹ�õĸ�˹��
	vector<vector<float> > MilitaryKernel;
	//��Ϸ������
    bool DiplomacyPhase(vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap);
    bool MilitaryPhase(vector<vector<TMilitaryCommand> > & MilitaryCommandList, vector<TPosition > &NewCapitalList);
    bool ProducingPhase();
    bool CheckWinner();
    void UpdateMapChecksum();
	//�����Լ���ӵ�С������д������
	TMap inf(TMap pos);
	TMap sup(TMap pos, TMap max);
    bool canSetGlobalMapPos(TPosition pos, TId id);
    
};

}


#endif
