#ifndef _PC_H_ 
#define _PC_H_

#include"action.h"

#define MAXCHARACTER			2		// T[o[L
#define CHAR_NAME_LEN			16		// LO
#define CHAR_FREENAME_LEN		12		// [U
#define MAGIC_NAME_LEN			24		// p
#define MAGIC_MEMO_LEN			72		// p
#define ITEM_NAME_LEN			28		// ACe
#define ITEM_NAME2_LEN			16		// ACeQ
#define ITEM_MEMO_LEN			84		// ACe
#define PET_NAME_LEN			16		// ybgO


//#define CHARNAMELEN				256		// LXgp

#define MAX_PET					5		// ybg

#define MAX_MAGIC				5		// p

#define MAX_PARTY				5		// ij

#define	MAX_ADR_BOOK_COUNT		4		// AhXubNPy[W\l
#define	MAX_ADR_BOOK_PAGE		10		// AhXubNy[W
#define MAX_ADR_BOOK			(MAX_ADR_BOOK_COUNT*MAX_ADR_BOOK_PAGE)	// AhXubN

#define MAX_ITEM				20		// ACe

#define RESULT_ITEM_COUNT		3		// ACe
#define RESULT_ITEM_NAME_LEN	24		// ACe
#define RESULT_CHR_EXP			4		// dwobZ[W

#define SKILL_NAME_LEN			24		// XLO
#define SKILL_MEMO_LEN			72		// XL
#define MAX_SKILL				7		// PLZ


enum
{
	PC_ETCFLAG_PARTY		= 0x0001,	// njtOrbg
	PC_ETCFLAG_JOINT_BTL	= 0x0002,	// QnjtOrbg
	PC_ETCFLAG_DUEL			= 0x0004,	// njtOrbg
	PC_ETCFLAG_CHAT_MODE	= 0x0008,	// `bg[htOrbg
	PC_ETCFLAG_MAIL			= 0x0010	// hnjtOrbg
};


#define ITEM_FLAG_PET_MAIL		( 1 << 0 ) 	// ybg[M\ACe
#define ITEM_FLAG_MIX			( 1 << 1 ) 	// {I
#define ITEM_FLAG_COOKING_MIX	( 1 << 2 ) 	// \ACe

typedef struct
{
	short useFlag;					// gptO
	char name[ITEM_NAME_LEN+1];		// ACe
	char name2[ITEM_NAME2_LEN+1];	// ACeQ
	char memo[ITEM_MEMO_LEN+1];		// 
	int color;						// OF
	int graNo;						// 
	int level;						// ACex
	short field;					// gp
	short target;					// 
	short deadTargetFlag;			// tO
	short sendFlag;					// ybg[M\
} ITEM;

typedef struct
{
	int graNo;						// OtBbN
	int faceGraNo;					// 
	int id;							// pID
	int dir;						// 
	int hp, maxHp;					// go^go
	int mp, maxMp;					// lo^lo
	int vital;						// 
	int str, tgh, dex;				// ^^f
	int exp, maxExp;				// ol^xKvol
	int level;						// x
	int atk, def;					// U^h
	int quick, charm, luck;			// rq^^^
	int earth, water, fire, wind;	// n
	int gold;						// 
	int titleNo;					// 
	int dp;							// fG|Cg
	char name[CHAR_NAME_LEN+1];		// O
	char freeName[CHAR_FREENAME_LEN+1];	// O
	short nameColor;				// OF
	unsigned short status;			// Xe[^X(rbgtO)
	unsigned short etcFlag;			// OKErQOKEfGOK tO
	short battlePetNo;				// Qybgzi|PQj
	short selectPetNo[MAX_PET];		// Iybg
	short mailPetNo;				// [^ybg
	int battleNo;					// 
	short sideNo;					// 
	short helpMode;					// wv[h
	ITEM item[MAX_ITEM];			// ACe
	ACTION *ptAct;					// vC[ANV
} PC;


enum
{
	MAGIC_FIELD_ALL,		// g
	MAGIC_FIELD_BATTLE,		// 
	MAGIC_FIELD_MAP			// }bv
};

enum
{
	MAGIC_TARGET_MYSELF,				// 
	MAGIC_TARGET_OTHER,					// li)
	MAGIC_TARGET_ALLMYSIDE,				// S
	MAGIC_TARGET_ALLOTHERSIDE,			// S
	MAGIC_TARGET_ALL,					// S
	MAGIC_TARGET_NONE,					// ^[Qbg
	MAGIC_TARGET_OTHERWITHOUTMYSELF,	// O
	MAGIC_TARGET_WITHOUTMYSELFANDPET,	// ybgO
	MAGIC_TARGET_WHOLEOTHERSIDE,    	// TChS
};

enum
{
	PETSKILL_FIELD_ALL,			// g
	PETSKILL_FIELD_BATTLE,		// 
	PETSKILL_FIELD_MAP			// }bv
};

enum
{
	PETSKILL_TARGET_MYSELF,				// 
	PETSKILL_TARGET_OTHER,				// li)
	PETSKILL_TARGET_ALLMYSIDE,			// S
	PETSKILL_TARGET_ALLOTHERSIDE,		// S
	PETSKILL_TARGET_ALL,				// S
	PETSKILL_TARGET_NONE,				// ^[Qbg
	PETSKILL_TARGET_OTHERWITHOUTMYSELF,	// O
	PETSKILL_TARGET_WITHOUTMYSELFANDPET	// ybgO
};

enum
{
	ITEM_FIELD_ALL,				// g
	ITEM_FIELD_BATTLE,			// 
	ITEM_FIELD_MAP,				// }bv
};

enum
{
	ITEM_TARGET_MYSELF,			// 
	ITEM_TARGET_OTHER,			// li)
	ITEM_TARGET_ALLMYSIDE,		// S
	ITEM_TARGET_ALLOTHERSIDE,	// S
	ITEM_TARGET_ALL,			// S
	ITEM_TARGET_NONE,				// ^[Qbg
	ITEM_TARGET_OTHERWITHOUTMYSELF,	// O
	ITEM_TARGET_WITHOUTMYSELFANDPET	// ybgO
};

typedef struct
{
	int graNo;						// 
	int hp, maxHp;					// go^go
	int mp, maxMp;					// lo^lo
	int exp, maxExp;				// ol^xKvol
	int level;						// x
	int atk, def;					// U^h
	int quick;						// rq
	int ai;							// x
	int earth, water, fire, wind;	// n
	int maxSkill;					// XL
	unsigned short status;			// Xe[^X(rbgtO)
	char name[CHAR_NAME_LEN+1];		// 
	char freeName[PET_NAME_LEN+1];	// [UO
	short useFlag;					// ybgtO
	short changeNameFlag;			// OtO
} PET;


typedef struct
{
	short useFlag;					// gptO
	int mp;							// Kvlo
	short field;					// gp
	short target;					// 
	short deadTargetFlag;			// tO
	char name[MAGIC_NAME_LEN+1];	// p
	char memo[MAGIC_MEMO_LEN+1];	// 
} MAGIC;


typedef struct
{
	short useFlag;					// gptO
	int id;							// hc
	int level;						// x
	int maxHp;						// go
	int hp;							// go
	int mp;							// lo
	char name[CHAR_NAME_LEN+1];		// O
	ACTION *ptAct;					// ANV
} PARTY;


typedef struct
{
	short useFlag;					// gptO
	short onlineFlag;				// IC
	int level;						// x
	int dp;							// fG|Cg
	int graNo;						// 
	char name[CHAR_NAME_LEN+1];		// O
} ADDRESS_BOOK;


typedef struct
{
	short petNo;					// ybg
	short levelUp;					// xAbvtO
	int exp;						// ol
} BATTLE_RESULT_CHR;

typedef struct
{
	short useFlag;					// gptO
	BATTLE_RESULT_CHR resChr[RESULT_CHR_EXP];	// olbZ[W
	char item[RESULT_ITEM_COUNT][RESULT_ITEM_NAME_LEN+1];		// ACe
} BATTLE_RESULT_MSG;


typedef struct
{
	short useFlag;						// gptO
	short skillId;						// XLhc
	short field;						// gp
	short target;						// Z
	char name[SKILL_NAME_LEN+1];		// ZO
	char memo[SKILL_MEMO_LEN+1];		// 
} PET_SKILL;


typedef struct
{
	char name[CHAR_NAME_LEN+1];		// SJISO
	short level;					// x
	int login;						// OC

	int faceGraNo;					// 
	int hp;							// go
	int str;						// U
	int def;						// h
	int agi;						// q
	int app;						// 
	int attr[4];					// inEEEj
	int dp;							// fG|Cg
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
