#ifndef _PC_H_ 
#define _PC_H_

#include"action.h"

#define MAXCHARACTER			2		// �T�[�o�[�ɕۑ�����ő�L������
#define CHAR_NAME_LEN			16		// �L�����̖��O�̒���
#define CHAR_FREENAME_LEN		12		// ���[�U���ݒ肵���̍��̒���
#define MAGIC_NAME_LEN			24		// ��p���̒���
#define MAGIC_MEMO_LEN			72		// ��p�̐������̒���
#define ITEM_NAME_LEN			28		// �A�C�e�����̒���
#define ITEM_NAME2_LEN			16		// �A�C�e�����Q�̒���
#define ITEM_MEMO_LEN			84		// �A�C�e���̐����̒���
#define PET_NAME_LEN			16		// �y�b�g�̖��O�̒���


//#define CHARNAMELEN				256		// �L�������X�g�p

#define MAX_PET					5		// �ő�y�b�g�擾��

#define MAX_MAGIC				5		// �ő��p��

#define MAX_PARTY				5		// �ő咇�Ԑ��i�������܂ށj

#define	MAX_ADR_BOOK_COUNT		4		// �A�h���X�u�b�N�̂P�y�[�W�ɕ\������l��
#define	MAX_ADR_BOOK_PAGE		10		// �A�h���X�u�b�N�̍ő�y�[�W��
#define MAX_ADR_BOOK			(MAX_ADR_BOOK_COUNT*MAX_ADR_BOOK_PAGE)	// �A�h���X�u�b�N�̍ő吔

#define MAX_ITEM				20		// �ő�A�C�e����

#define RESULT_ITEM_COUNT		3		// �퓬���ʎ擾�A�C�e����
#define RESULT_ITEM_NAME_LEN	24		// �퓬���ʎ擾�A�C�e�����̒���
#define RESULT_CHR_EXP			4		// �퓬���ʂd�w�o�擾���b�Z�[�W�̐�

#define SKILL_NAME_LEN			24		// �X�L���̖��O�̒���
#define SKILL_MEMO_LEN			72		// �X�L���̃����̒���
#define MAX_SKILL				7		// �P�L�������ێ��ł���Z�̐�


enum
{
	PC_ETCFLAG_PARTY		= 0x0001,	// ���Ԃn�j�t���O�r�b�g
	PC_ETCFLAG_JOINT_BTL	= 0x0002,	// �Q��n�j�t���O�r�b�g
	PC_ETCFLAG_DUEL			= 0x0004,	// �ΐ�n�j�t���O�r�b�g
	PC_ETCFLAG_CHAT_MODE	= 0x0008,	// �`���b�g���[�h�t���O�r�b�g
	PC_ETCFLAG_MAIL			= 0x0010	// ���h�����n�j�t���O�r�b�g
};


#define ITEM_FLAG_PET_MAIL		( 1 << 0 ) 	// �y�b�g���[���ő��M�\�A�C�e��
#define ITEM_FLAG_MIX			( 1 << 1 ) 	// ���{�I�ɍ����ł��邩
#define ITEM_FLAG_COOKING_MIX	( 1 << 2 ) 	// ���������\�A�C�e��

typedef struct
{
	short useFlag;					// �g�p�t���O
	char name[ITEM_NAME_LEN+1];		// �A�C�e����
	char name2[ITEM_NAME2_LEN+1];	// �A�C�e�����Q
	char memo[ITEM_MEMO_LEN+1];		// ����
	int color;						// ���O�̐F
	int graNo;						// �摜�ԍ�
	int level;						// �A�C�e�����x��
	short field;					// �g�p�ł���ꏊ
	short target;					// �Ώ�
	short deadTargetFlag;			// ����ł����Ώۃt���O
	short sendFlag;					// �y�b�g���[���ő��M�\��
} ITEM;

typedef struct
{
	int graNo;						// �O���t�B�b�N�ԍ�
	int faceGraNo;					// ��摜�ԍ�
	int id;							// �Ǘ��pID
	int dir;						// ����
	int hp, maxHp;					// �g�o�^�ő�g�o
	int mp, maxMp;					// �l�o�^�ő�l�o
	int vital;						// �̗�
	int str, tgh, dex;				// �́^�ł��ꋭ���^�f����
	int exp, maxExp;				// �o���l�^���̃��x���ɕK�v�Ȍo���l
	int level;						// ���x��
	int atk, def;					// ���ݍU���́^���ݖh���
	int quick, charm, luck;			// �r�q���^���́^�^
	int earth, water, fire, wind;	// �n���Ε�
	int gold;						// ����
	int titleNo;					// �̍��ԍ�
	int dp;							// �f���G���|�C���g
	char name[CHAR_NAME_LEN+1];		// ���O
	char freeName[CHAR_FREENAME_LEN+1];	// �����ł��閼�O
	short nameColor;				// ���O�̐F
	unsigned short status;			// �X�e�[�^�X(�r�b�g�t���O)
	unsigned short etcFlag;			// ����OK�E�퓬�r���Q��OK�E�f���G��OK �t���O
	short battlePetNo;				// �퓬�ɎQ��������y�b�g�̓������z��ԍ��i�|�P�͎Q�������Ȃ��j
	short selectPetNo[MAX_PET];		// �퓬��ʂőI���ł���y�b�g
	short mailPetNo;				// ���[�����^�ԃy�b�g�̔ԍ�
	int battleNo;					// �퓬�ԍ�
	short sideNo;					// �퓬���ɂǂ��瑤��
	short helpMode;					// �w���v���[�h
	ITEM item[MAX_ITEM];			// �A�C�e��
	ACTION *ptAct;					// �v���C���[�̃A�N�V����
} PC;


enum
{
	MAGIC_FIELD_ALL,		// ���ׂĂ̏ꏊ�Ŏg����
	MAGIC_FIELD_BATTLE,		// �퓬���̂�
	MAGIC_FIELD_MAP			// �ʏ�}�b�v��̂�
};

enum
{
	MAGIC_TARGET_MYSELF,				// �����̂�
	MAGIC_TARGET_OTHER,					// ���̐l�i�����܂�)
	MAGIC_TARGET_ALLMYSIDE,				// �����S��
	MAGIC_TARGET_ALLOTHERSIDE,			// ���葤�S��
	MAGIC_TARGET_ALL,					// �S��
	MAGIC_TARGET_NONE,					// �^�[�Q�b�g����
	MAGIC_TARGET_OTHERWITHOUTMYSELF,	// �����ȊO
	MAGIC_TARGET_WITHOUTMYSELFANDPET,	// �����ƃy�b�g�ȊO
	MAGIC_TARGET_WHOLEOTHERSIDE,    	// �ǂ��炩�̃T�C�h�S��
};

enum
{
	PETSKILL_FIELD_ALL,			// ���ׂĂ̏ꏊ�Ŏg����
	PETSKILL_FIELD_BATTLE,		// �퓬���̂�
	PETSKILL_FIELD_MAP			// �ʏ�}�b�v��̂�
};

enum
{
	PETSKILL_TARGET_MYSELF,				// �����̂�
	PETSKILL_TARGET_OTHER,				// ���̐l�i�����܂�)
	PETSKILL_TARGET_ALLMYSIDE,			// �����S��
	PETSKILL_TARGET_ALLOTHERSIDE,		// ���葤�S��
	PETSKILL_TARGET_ALL,				// �S��
	PETSKILL_TARGET_NONE,				// �^�[�Q�b�g����
	PETSKILL_TARGET_OTHERWITHOUTMYSELF,	// �����ȊO
	PETSKILL_TARGET_WITHOUTMYSELFANDPET	// �����ƃy�b�g�ȊO
};

enum
{
	ITEM_FIELD_ALL,				// ���ׂĂ̏ꏊ�Ŏg����
	ITEM_FIELD_BATTLE,			// �퓬���̂�
	ITEM_FIELD_MAP,				// �ʏ�}�b�v��̂�
};

enum
{
	ITEM_TARGET_MYSELF,			// �����̂�
	ITEM_TARGET_OTHER,			// ���̐l�i�����܂�)
	ITEM_TARGET_ALLMYSIDE,		// �����S��
	ITEM_TARGET_ALLOTHERSIDE,	// ���葤�S��
	ITEM_TARGET_ALL,			// �S��
	ITEM_TARGET_NONE,				// �^�[�Q�b�g����
	ITEM_TARGET_OTHERWITHOUTMYSELF,	// �����ȊO
	ITEM_TARGET_WITHOUTMYSELFANDPET	// �����ƃy�b�g�ȊO
};

typedef struct
{
	int graNo;						// �摜�ԍ�
	int hp, maxHp;					// �g�o�^�ő�g�o
	int mp, maxMp;					// �l�o�^�ő�l�o
	int exp, maxExp;				// �o���l�^���̃��x���ɕK�v�Ȍo���l
	int level;						// ���x��
	int atk, def;					// ���ݍU���́^���ݖh���
	int quick;						// �r�q��
	int ai;							// �����x
	int earth, water, fire, wind;	// �n���Ε�
	int maxSkill;					// �ő�X�L����
	unsigned short status;			// �X�e�[�^�X(�r�b�g�t���O)
	char name[CHAR_NAME_LEN+1];		// ��ޖ�
	char freeName[PET_NAME_LEN+1];	// ���[�U�����閼�O
	short useFlag;					// ���̗��Ƀy�b�g�����邩�t���O
	short changeNameFlag;			// ���O������邩�t���O
} PET;


typedef struct
{
	short useFlag;					// �g�p�t���O
	int mp;							// �K�v�l�o
	short field;					// �g�p�ł���ꏊ
	short target;					// �Ώ�
	short deadTargetFlag;			// ����ł����Ώۃt���O
	char name[MAGIC_NAME_LEN+1];	// ��p��
	char memo[MAGIC_MEMO_LEN+1];	// ����
} MAGIC;


typedef struct
{
	short useFlag;					// �g�p�t���O
	int id;							// �Ǘ��h�c
	int level;						// ���x��
	int maxHp;						// �ő�g�o
	int hp;							// �g�o
	int mp;							// �l�o
	char name[CHAR_NAME_LEN+1];		// ���O
	ACTION *ptAct;					// �A�N�V����
} PARTY;


typedef struct
{
	short useFlag;					// �g�p�t���O
	short onlineFlag;				// �I�����C���ǂ���
	int level;						// ���x��
	int dp;							// �f���G���|�C���g
	int graNo;						// �摜�ԍ�
	char name[CHAR_NAME_LEN+1];		// ���O
} ADDRESS_BOOK;


typedef struct
{
	short petNo;					// �y�b�g�ԍ�
	short levelUp;					// ���x���A�b�v�t���O
	int exp;						// �擾�o���l
} BATTLE_RESULT_CHR;

typedef struct
{
	short useFlag;					// �g�p�t���O
	BATTLE_RESULT_CHR resChr[RESULT_CHR_EXP];	// �퓬���ʌo���l���b�Z�[�W
	char item[RESULT_ITEM_COUNT][RESULT_ITEM_NAME_LEN+1];		// �擾�A�C�e����
} BATTLE_RESULT_MSG;


typedef struct
{
	short useFlag;						// �g�p�t���O
	short skillId;						// �X�L���h�c
	short field;						// �g�p�ł���ꏊ
	short target;						// �Z�̑Ώ�
	char name[SKILL_NAME_LEN+1];		// �Z�̖��O
	char memo[SKILL_MEMO_LEN+1];		// ����
} PET_SKILL;


typedef struct
{
	char name[CHAR_NAME_LEN+1];		// SJIS�Ŗ��O�������
	short level;					// ���x��
	int login;						// ���O�C����

	int faceGraNo;					// ��摜�ԍ�
	int hp;							// �g�o
	int str;						// �U����
	int def;						// �h���
	int agi;						// �q����
	int app;						// ����
	int attr[4];					// �����i�n�E���E�΁E���j
	int dp;							// �f���G���|�C���g
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