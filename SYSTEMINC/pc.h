#ifndef _PC_H_ 
#define _PC_H_

#include"action.h"

#define MAXCHARACTER			2		// サ`バ`に保存する最大キャラ数
#define CHAR_NAME_LEN			16		// キャラの名前のLさ
#define CHAR_FREENAME_LEN		12		// ユ`ザがO定した称号のLさ
#define MAGIC_NAME_LEN			24		// 庑g名のLさ
#define MAGIC_MEMO_LEN			72		// 庑gのh明文のLさ
#define ITEM_NAME_LEN			28		// アイテム名のLさ
#define ITEM_NAME2_LEN			16		// アイテム名２のLさ
#define ITEM_MEMO_LEN			84		// アイテムのh明のLさ
#define PET_NAME_LEN			16		// ペットの名前のLさ


//#define CHARNAMELEN				256		// キャラリスト用

#define MAX_PET					5		// 最大ペット取得数

#define MAX_MAGIC				5		// 最大庑g数

#define MAX_PARTY				5		// 最大仲g数（自分を含む）

#define	MAX_ADR_BOOK_COUNT		4		// アドレスブックの１ペ`ジに表示する人数
#define	MAX_ADR_BOOK_PAGE		10		// アドレスブックの最大ペ`ジ数
#define MAX_ADR_BOOK			(MAX_ADR_BOOK_COUNT*MAX_ADR_BOOK_PAGE)	// アドレスブックの最大数

#define MAX_ITEM				20		// 最大アイテム数

#define RESULT_ITEM_COUNT		3		// 殛LY果取得アイテム数
#define RESULT_ITEM_NAME_LEN	24		// 殛LY果取得アイテム名のLさ
#define RESULT_CHR_EXP			4		// 殛LY果ＥＸＰ取得メッセ`ジの数

#define SKILL_NAME_LEN			24		// スキルの名前のLさ
#define SKILL_MEMO_LEN			72		// スキルのメモのLさ
#define MAX_SKILL				7		// １キャラが保持できる技の数


enum
{
	PC_ETCFLAG_PARTY		= 0x0001,	// 仲gＯＫフラグビット
	PC_ETCFLAG_JOINT_BTL	= 0x0002,	// 参椋希衰榨楗哎鹰氓
	PC_ETCFLAG_DUEL			= 0x0004,	// 椋希衰榨楗哎鹰氓
	PC_ETCFLAG_CHAT_MODE	= 0x0008,	// チャットモ`ドフラグビット
	PC_ETCFLAG_MAIL			= 0x0010	// 名刺交QＯＫフラグビット
};


#define ITEM_FLAG_PET_MAIL		( 1 << 0 ) 	// ペットメ`ルで送信可能アイテム
#define ITEM_FLAG_MIX			( 1 << 1 ) 	// 根本的に合成できるか
#define ITEM_FLAG_COOKING_MIX	( 1 << 2 ) 	// 料理合成可能アイテム

typedef struct
{
	short useFlag;					// 使用フラグ
	char name[ITEM_NAME_LEN+1];		// アイテム名
	char name2[ITEM_NAME2_LEN+1];	// アイテム名２
	char memo[ITEM_MEMO_LEN+1];		// メモ
	int color;						// 名前の色
	int graNo;						// 画像番号
	int level;						// アイテムレベル
	short field;					// 使用できる鏊
	short target;					// 象
	short deadTargetFlag;			// 死んでるやつも象フラグ
	short sendFlag;					// ペットメ`ルで送信可能か
} ITEM;

typedef struct
{
	int graNo;						// グラフィック番号
	int faceGraNo;					// 画像番号
	int id;							// 管理用ID
	int dir;						// 向き
	int hp, maxHp;					// ＨＰ／最大ＨＰ
	int mp, maxMp;					// ＭＰ／最大ＭＰ
	int vital;						// 体力
	int str, tgh, dex;				// 力／打たれさ／素早さ
	int exp, maxExp;				// UY韦违欹佶毪吮匾式UY
	int level;						// レベル
	int atk, def;					// F在攻牧ΓF在防御力
	int quick, charm, luck;			// 俊敏さ／魅力／\
	int earth, water, fire, wind;	// 地水火L
	int gold;						// お金
	int titleNo;					// 称号番号
	int dp;							// デュエルポイント
	char name[CHAR_NAME_LEN+1];		// 名前
	char freeName[CHAR_FREENAME_LEN+1];	// 自分でつける名前
	short nameColor;				// 名前の色
	unsigned short status;			// ステ`タス(ビットフラグ)
	unsigned short etcFlag;			// 仲gOK?殛L途中参加OK?デュエルOK フラグ
	short battlePetNo;				// 殛Lに参加させるペットの入った配列番号（－１は参加させない）
	short selectPetNo[MAX_PET];		// 殛L画面でxkできるペット
	short mailPetNo;				// メ`ルを\ぶペットの番号
	int battleNo;					// 殛L番号
	short sideNo;					// 殛Lrにどちら趣
	short helpMode;					// ヘルプモ`ド
	ITEM item[MAX_ITEM];			// アイテム
	ACTION *ptAct;					// プレイヤ`のアクション
} PC;


enum
{
	MAGIC_FIELD_ALL,		// すべての鏊鞘工à
	MAGIC_FIELD_BATTLE,		// 殛L中のみ
	MAGIC_FIELD_MAP			// 通常マップ上のみ
};

enum
{
	MAGIC_TARGET_MYSELF,				// 自分のみ
	MAGIC_TARGET_OTHER,					// 他の人（自分含む)
	MAGIC_TARGET_ALLMYSIDE,				// 味方全体
	MAGIC_TARGET_ALLOTHERSIDE,			// 相手热
	MAGIC_TARGET_ALL,					// 全て
	MAGIC_TARGET_NONE,					// タ`ゲットoし
	MAGIC_TARGET_OTHERWITHOUTMYSELF,	// 自分以外
	MAGIC_TARGET_WITHOUTMYSELFANDPET,	// 自分とペット以外
	MAGIC_TARGET_WHOLEOTHERSIDE,    	// どちらかのサイド全体
};

enum
{
	PETSKILL_FIELD_ALL,			// すべての鏊鞘工à
	PETSKILL_FIELD_BATTLE,		// 殛L中のみ
	PETSKILL_FIELD_MAP			// 通常マップ上のみ
};

enum
{
	PETSKILL_TARGET_MYSELF,				// 自分のみ
	PETSKILL_TARGET_OTHER,				// 他の人（自分含む)
	PETSKILL_TARGET_ALLMYSIDE,			// 味方全体
	PETSKILL_TARGET_ALLOTHERSIDE,		// 相手热
	PETSKILL_TARGET_ALL,				// 全て
	PETSKILL_TARGET_NONE,				// タ`ゲットoし
	PETSKILL_TARGET_OTHERWITHOUTMYSELF,	// 自分以外
	PETSKILL_TARGET_WITHOUTMYSELFANDPET	// 自分とペット以外
};

enum
{
	ITEM_FIELD_ALL,				// すべての鏊鞘工à
	ITEM_FIELD_BATTLE,			// 殛L中のみ
	ITEM_FIELD_MAP,				// 通常マップ上のみ
};

enum
{
	ITEM_TARGET_MYSELF,			// 自分のみ
	ITEM_TARGET_OTHER,			// 他の人（自分含む)
	ITEM_TARGET_ALLMYSIDE,		// 味方全体
	ITEM_TARGET_ALLOTHERSIDE,	// 相手热
	ITEM_TARGET_ALL,			// 全て
	ITEM_TARGET_NONE,				// タ`ゲットoし
	ITEM_TARGET_OTHERWITHOUTMYSELF,	// 自分以外
	ITEM_TARGET_WITHOUTMYSELFANDPET	// 自分とペット以外
};

typedef struct
{
	int graNo;						// 画像番号
	int hp, maxHp;					// ＨＰ／最大ＨＰ
	int mp, maxMp;					// ＭＰ／最大ＭＰ
	int exp, maxExp;				// UY韦违欹佶毪吮匾式UY
	int level;						// レベル
	int atk, def;					// F在攻牧ΓF在防御力
	int quick;						// 俊敏さ
	int ai;							// 忠\度
	int earth, water, fire, wind;	// 地水火L
	int maxSkill;					// 最大スキル数
	unsigned short status;			// ステ`タス(ビットフラグ)
	char name[CHAR_NAME_LEN+1];		// N名
	char freeName[PET_NAME_LEN+1];	// ユ`ザがつける名前
	short useFlag;					// この冥衰讠氓趣い毪榨楗
	short changeNameFlag;			// 名前をつけれるかフラグ
} PET;


typedef struct
{
	short useFlag;					// 使用フラグ
	int mp;							// 必要ＭＰ
	short field;					// 使用できる鏊
	short target;					// 象
	short deadTargetFlag;			// 死んでるやつも象フラグ
	char name[MAGIC_NAME_LEN+1];	// 庑g名
	char memo[MAGIC_MEMO_LEN+1];	// h明
} MAGIC;


typedef struct
{
	short useFlag;					// 使用フラグ
	int id;							// 管理ＩＤ
	int level;						// レベル
	int maxHp;						// 最大ＨＰ
	int hp;							// ＨＰ
	int mp;							// ＭＰ
	char name[CHAR_NAME_LEN+1];		// 名前
	ACTION *ptAct;					// アクション
} PARTY;


typedef struct
{
	short useFlag;					// 使用フラグ
	short onlineFlag;				// オンラインどうか
	int level;						// レベル
	int dp;							// デュエルポイント
	int graNo;						// 画像番号
	char name[CHAR_NAME_LEN+1];		// 名前
} ADDRESS_BOOK;


typedef struct
{
	short petNo;					// ペット番号
	short levelUp;					// レベルアップフラグ
	int exp;						// 取得UY
} BATTLE_RESULT_CHR;

typedef struct
{
	short useFlag;					// 使用フラグ
	BATTLE_RESULT_CHR resChr[RESULT_CHR_EXP];	// 殛LY果UY幞氓哗`ジ
	char item[RESULT_ITEM_COUNT][RESULT_ITEM_NAME_LEN+1];		// 取得アイテム名
} BATTLE_RESULT_MSG;


typedef struct
{
	short useFlag;						// 使用フラグ
	short skillId;						// スキルＩＤ
	short field;						// 使用できる鏊
	short target;						// 技の象
	char name[SKILL_NAME_LEN+1];		// 技の名前
	char memo[SKILL_MEMO_LEN+1];		// メモ
} PET_SKILL;


typedef struct
{
	char name[CHAR_NAME_LEN+1];		// SJISで名前をいれる
	short level;					// レベル
	int login;						// ログイン回数

	int faceGraNo;					// 画像番号
	int hp;							// ＨＰ
	int str;						// 攻牧
	int def;						// 防御力
	int agi;						// 敏捷力
	int app;						// 魅力
	int attr[4];					// 属性（地?水?火?L）
	int dp;							// デュエルポイント
} CHARLISTTABLE;


extern PC pc;
extern short maxPcNo;
extern short selectPcNo;
extern short prSendMode;
extern short prSendFlag;
extern short jbSendFlag;
extern short duelSendFlag;

extern int loginDp;

extern short helpFlag;


extern MAGIC magic[];

extern PET pet[];

extern PARTY party[];
extern short partyModeFlag;

extern ADDRESS_BOOK addressBook[];
extern BOOL addressBookFlag;

extern BATTLE_RESULT_MSG battleResultMsg;

extern PET_SKILL petSkill[][MAX_SKILL];

extern CHARLISTTABLE chartable[];

extern char newCharacterName[];
extern int newCharacterGraNo;
extern int newCharacterFaceGraNo;
extern int newCharacterVit;
extern int newCharacterStr;
extern int newCharacterTgh;
extern int newCharacterDex;
extern int newCharacterEarth;
extern int newCharacterWater;
extern int newCharacterFire;
extern int newCharacterWind;
extern int newCharacterHomeTown;

void initPcAll( void );
void initPc( void );
void createPc( int, int, int, int );
void resetPc( void );
void setPcMovePoint( int, int );
void setPcWarpPoint( int, int );
void setPcPoint( void );
void setPcGraNo( int, int );
void setPcId( int );
void setPcDir( int );
void setPcAction( int );
int getPcAction( void );
void setPcParam( char *, char *, int, int, int, int );
void updataPcAct( void );
void setPcLeader( void );
void delPcLeader( void );
void setPcParty( void );
void delPcParty( void );
void setPcWatch( void );
void delPcWatch( void );
void changePcAct( int, int, int, int, int, int, int );
void setPcWalkFlag( void );
void delPcWalkFlag( void );
int checkPcWalkFlag( void );
void setPcUseMagic( void );
void delPcUseMagic( void );
void setPcFukidashi( unsigned int );

void clearPartyParam( void );
void clearPtActPartyParam( void );

int existCharacterListEntry( int );
int cmpNameCharacterList( char * );
int setCharacterList( char *, char * );
int resetCharacterList( int );

void getItem( void );
void swapItem( int, int );

BOOL lookAtAround( void );


void initAddressBook( void );
void addressBookProc( void );

#endif

