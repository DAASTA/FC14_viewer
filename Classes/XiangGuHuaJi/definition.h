/* XiangGuHuaJi 2016, definition.h
 * 
 */

#ifndef _XIANGGUHUAJI_DEFINITION_H__
#define _XIANGGUHUAJI_DEFINITION_H__

#include "debug.hpp"


typedef int TId;                // player id ��ұ��
typedef unsigned int TMap;      // map size ��ͼ�ߴ�
typedef unsigned int TRound;    // round �غ���
typedef int TMoney;         // money Ǯ(SAV/INC)
typedef int TMilitary;      // military ���¼���
typedef bool TMask;         // mask �ɰ�

struct TPosition { TMap x, y; }; // ��άλ��
const static TPosition INVALID_POSITION = {255, 255};

enum TDiplomaticStatus {
    Undiscovered,   // have never appeared in your sight ��һ�û���������Ұ�г��ֹ�
    Neutral,        // default diplomatic  Ĭ���⽻��ϵ
    Allied,          
    AtWar
};
enum TDiplomaticCommand {
    KeepNeutral, 
    FormAlliance, 
    JustifyWar,
	Backstab
};

struct TMilitaryCommand {
	TPosition place;        // ��ص�
	TMilitary bomb_size;    // ���ģ
};

class BaseMap
{
public:
	inline TMap getRows() const {return rows;}  // rows=25
	inline TMap getCols() const {return cols;}  // cols=35
	
    inline bool isPosValid(int x, int y) const { return x>=0&&x<cols&&y>=0&&y<rows; }
    inline bool isPosValid(TPosition pos) const  { return isPosValid(pos.x, pos.y); }

    inline vector<vector<TMoney> >    getMapRes() const {return MapResource_;}      // ��Դ�ֲ�ͼ
	inline vector<vector<TMilitary> > getMapAtk() const {return MapAttackRatio_;}   // ����ϵ��ͼ
	inline vector<vector<TMilitary> > getMapDef() const {return MapDefenseRatio_;}  // ����ϵ��ͼ
    
    inline TMoney getMapRes(int x, int y) const    { if (isPosValid(x,y)) return MapResource_[x][y]; else return 0;}        // res
    inline TMilitary getMapAtk(int x, int y) const { if (isPosValid(x,y)) return MapAttackRatio_[x][y]; else return 0;}     // atk
    inline TMilitary getMapDef(int x, int y) const { if (isPosValid(x,y)) return MapDefenseRatio_[x][y]; else return 0;}    // def


protected:
	TMap	rows, cols;
	vector<vector<TMoney> >  MapResource_;
	vector<vector<TMilitary> > MapDefenseRatio_, MapAttackRatio_;
};


// ��Ϸ����
const static TRound     MAX_ROUND = 50;             // �����޶�
const static TRound     STRICT_ROUND_START = 10;    // �Զ���̭��ʼ��Ч�Ļغ�
// �⽻����
const static TRound     WAR_JUSTIFY_TIME = 3;           // ȷ���жԹ�ϵ����Ҫ��JustifyWar����
const static TMoney     WAR_JUSTIFY_PRICE = 5;          // JustifyWar����Ҫ�ķ���
const static float      UNIT_AREA_ALLY_COST = 0.05f;    // FormAlliance����Ҫ֧���ķ���(����)
// ���³���
const static int        MILITARY_COUNT_LIMIT = 32;      // һ�غ������е���Ч���Ŀ
const static int		MILITARY_KERNEL_SIZE = 5;       // Ӱ�������ĵİ뾶 (�ߴ�Ϊ9*9)
const static TMilitary	SUPPESS_LIMIT = 400;            // �ο������ж� atk*atk_flu - def*def_flu >= SUPPRESS_LIMIT
const static float      CAPITAL_INFLUENCE = 0.3f;       // �׶����� (����)
// ���ó���
const static TMoney     INITIAL_PLAYER_MONEY = 50;      // ��ʼ���
const static float		UNIT_CITY_INCOME = 1.0f;        // �ͱ�����ϵ��
const static float		CORRUPTION_COEF = 0.0003f;      // ����ϵ��
const static TMoney     UNIT_BOMB_COST = 1;             // ��λ�����
// ��ͼ����
const static TMap		MIN_ABS_DIST_BETWEEN_CAP = 3;   // �׶�֮�����С�ֵ��������� �����پ��� >= 3 
const static int		FIELD_BOUNDARY = 2;             // һ����������Ұ�������پ��� = 2
// ���ID����
const static TId		NEUTRAL_PLAYER_ID = 233;
const static TId		UNKNOWN_PLAYER_ID = 213;

struct PlayerInfo {    
	TDiplomaticStatus dipStatus;    // ������⽻״̬
	bool isVisible;     // �Ƿ�ɼ��������Ƿ��֪ basic info
	bool isUnion;       // �Ƿ�ͬ�ˣ������Ƿ��֪ all info
	
    // basic info
    TMoney income;          // ���룬��������������Ա��
	int mapArea;            // ���������
	vector<TId> warList;    // ����ս�������б�
	
    //all info
	TMoney saving;      // �Է��Ĵ��
	TPosition capital;  // �Է����׶�λ��
};

struct MapPointInfo {
    TMoney res;
    TMilitary atk, def;

	TMask isVisible;    // �Ƿ�ɼ�
	TId owner;          // ����
	TMask isSieged;
};

struct Info
{
	TId id;             // �ҷ�id
	TId playerSize;     // ��Ϸ������=8
	TMap rows;          // rows=25
	TMap cols;          // cols=35
	TRound round;       // ��ǰ�غ���

    bool backstabUsed;  // ���Ƿ��Ѿ����˱���

	BaseMap* map;       // ��ͼ

    vector<PlayerInfo> playerInfo;              // �����Ϣ
	vector<vector<MapPointInfo> > mapPointInfo; // ��ͼ��Ϣ

    vector<vector<float> > militaryKernel;  // ����kernel

	vector<TDiplomaticCommand> DiplomaticCommandList;   // �⽻ָ��
    vector<TMilitaryCommand> MilitaryCommandList;       // ����ָ��
	TPosition newCapital;                               // �µ��׶�λ��
};

#endif
