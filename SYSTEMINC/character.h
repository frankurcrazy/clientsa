#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include"action.h"
#include"pc.h"

enum
{
	CHR_STATUS_P 		= 0x0001,			// 毒
	CHR_STATUS_N 		= 0x0002,			// 麻w
	CHR_STATUS_Q 		= 0x0004,			// 沈a
	CHR_STATUS_S 		= 0x0008,			// 石化
	CHR_STATUS_D 		= 0x0010,			// 暗
	CHR_STATUS_C 		= 0x0020,			// 混乱
	CHR_STATUS_W 		= 0x0040,			// この上をiけるか
	CHR_STATUS_H 		= 0x0080,			// 高さがあるか
	CHR_STATUS_LEADER	= 0x0100,			// リ`ダ`
	CHR_STATUS_PARTY	= 0x0200,			// パ`ティの一T
	CHR_STATUS_BATTLE	= 0x0400,			// 殛L中
	CHR_STATUS_USE_MAGIC= 0x0800,			// 庑g使用
	CHR_STATUS_HELP		= 0x1000,			// ヘルプ
	CHR_STATUS_FUKIDASHI= 0x2000,			// 吹き出し
	CHR_STATUS_WATCH	= 0x4000			// Q橹
};


enum
{											// キャラ管理rのN
	CHAROBJ_TYPE_NPC		= 0x0001,		// NPC
	CHAROBJ_TYPE_ITEM		= 0x0002,		// アイテム
	CHAROBJ_TYPE_MONEY		= 0x0004,		// お金
	CHAROBJ_TYPE_USER_NPC	= 0x0008,		// 他人のキャラクタ
	CHAROBJ_TYPE_LOOKAT		= 0x0010,		// れる
	CHAROBJ_TYPE_ALL		= 0x00FF		// 全て
};


// サ`バ上での管理番号
enum
{
	CHAR_TYPENONE,			// 何でもない
	CHAR_TYPEPLAYER,		// プレイヤ`
	CHAR_TYPEENEMY,			// 
	CHAR_TYPEPET,			// ペット
	CHAR_TYPEDOOR,			// ドア
	CHAR_TYPEBOX,			// 宝箱
	CHAR_TYPEMSG,			// 看板
	CHAR_TYPEWARP,			// ワ`プゾ`ン
	CHAR_TYPESHOP,			// 店
	CHAR_TYPEHEALER,		// ヒ`ラ`
	CHAR_TYPEOLDMAN,		// L老
	CHAR_TYPEROOMADMIN,		// 不赢b屋
	CHAR_TYPETOWNPEOPLE,	// まちのひと
	CHAR_TYPEDENGON,		// 谎园
	CHAR_TYPEADM,			// 谎园
	CHAR_TYPETEMPLE,		// Temple master
	CHAR_TYPESTORYTELLER,	// Zり部
	CHAR_TYPERANKING,		// 不赢bランキング表示ＮＰＣ
	CHAR_TYPEOTHERNPC,		// その他の仕象にならないNPC
	CHAR_TYPEPRINTPASSMAN,	// ドアのパスワ`ド表示するNPC
	CHAR_TYPENPCENEMY,		// 固定
	CHAR_TYPEACTION,		// アクションに反するNPC
	CHAR_TYPEWINDOWMAN,		// ウィンドウ表示するNPC（テストかも)
	CHAR_TYPESAVEPOINT,		// セ`ブポイント
	CHAR_TYPEWINDOWHEALER,	// ウインドウタイプのヒ`ラ`
	CHAR_TYPEITEMSHOP,		// お店
	CHAR_TYPESTONESHOP,		// 石P屋（ペットの技屋）
	CHAR_TYPEDUELRANKING,	// DUELランキングNPC
	CHAR_TYPENUM
};


typedef struct
{
	short use;				// 使用中か
	short type;				// N
	int id;					// サ`バの管理番号
	int graNo;				// グラフィック番号
	int gx, gy;				// マップの座
	int level;				// レベル
	int dir;				// 向き
	int stockDir;			// 移俞幛讼颏涓欷涓工
	int stockDirX, stockDirY;// stockDirをm用する座
	short nameColor;		// 名前表示rの色
	unsigned short status;	// ステ`タス
	int classNo;			// アイテム用（？）
	int money;				// お金用
	char name[CHAR_NAME_LEN+1];			// 名前
	char freeName[CHAR_FREENAME_LEN+1];	// ユ`ザが付けた称号
	char info[64];			// １行インフォメ`ション（ACTIONに入れたらいらないかも）
	int battleNo;			// 殛L番号
	short sideNo;			// 殛Lrにどちらサイドに属しているか
	short helpMode;			// ヘルプモ`ド
	int charType;			// アクションにO定する属性
	short newFoundFlag;		// 新kの怪かをチェックするかのフラグ
	ACTION *ptAct;
} CHAROBJ;


typedef struct
{
	int charObjTblId;			// 管理テ`ブルの何番目に入っているか
	ACTION *ptActLeaderMark;	// リ`ダ`マ`ク表示用
	ACTION *ptActMagicEffect;	// 庑gエフェクト
	unsigned int drawFukidashiTime;	// 吹き出しを出すrg
} CHAREXTRA;


typedef struct
{
	short mode;					// ペットの幼鳐猢`ド
	short moveDir;				// 移rに向かう方向
	short preDir;				// ひとつ前に曲がった向き
	short dirCnt;				// BAで曲がった回数
	int preGx, preGy;			// ひとつ前のグリッド
	int walkCnt;				// 移i数
	unsigned int createTime;	// 作成rg
	ACTION *ptAct;
	ACTION *ptAct2;
	float angle;
	int r;
	int id;						// サ`バの管理番号
} PETEXTRA;


void charMove( ACTION * );
void charMove2( ACTION * );
void _charMove( ACTION * );
void shiftBufCount( ACTION * );

ACTION *createCharAction( int, int, int, int );
void stockCharMovePoint( ACTION *, int, int );
void correctCharMovePoint( ACTION *, int, int );
void setCharMovePoint( ACTION *, int, int );
void _setCharMovePoint( ACTION *, int, int );
void setCharWarpPoint( ACTION *, int, int );
void setCharStatus( unsigned short *, char * );

ACTION *createCommmonEffectAction( int, int, int, int, int, int );
ACTION *createCommmonEffectNoLoop( int, int, int, int, int, int );
void drawCharStatus( ACTION * );

void changeCharAct( ACTION *, int, int, int, int, int, int, int );

void setCharLeader( ACTION * );
void delCharLeader( ACTION * );
void setCharParty( ACTION * );
void delCharParty( ACTION * );
void setCharWatch( ACTION * );
void delCharWatch( ACTION * );
void setCharBattle( ACTION *, int, short, short );
void delCharBattle( ACTION * );
void setCharUseMagic( ACTION * );
void delCharUseMagic( ACTION * );

void setCharFukidashi( ACTION *, unsigned int );


void charObjProc( void );
BOOL checkCharObjPoint( int, int, short );
int checkCharObjPointStatus( int, int, short, unsigned short );
int checkCharObjPointNotStatus( int, int, short, unsigned short );
void setNpcCharObj( int, int, int, int, int, char *, char *, int, int, int, int, int );
BOOL setReturnPetObj( int, int, int, int, int, char *, char *, int, int, int, int, int );
void setItemCharObj( int, int, int, int, int, int, char * );
void setMoneyCharObj( int, int, int, int, int, int, char * );
int getAtrCharType( int );
int getAtrCharObjType( int );
void delCharObj( int );
void initCharObj( void );
void resetCharObj( void );
void clearPtActCharObj( void );
void restorePtActCharObjAll( void );
void restorePtActCharObj( int );
void setMovePointCharObj( int, int, int );
ACTION *getCharObjAct( int );

int getCharObjBuf( void );

void initItemOverlapCheck( void );
BOOL itemOverlapCheck( unsigned int, int, int );

ACTION *createPetAction( int, int, int, int, int, int, int );
void ajustClientDir( short * );

#endif

