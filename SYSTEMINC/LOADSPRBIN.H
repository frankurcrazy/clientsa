#ifndef _SPRITE_H_
#define _SPRITE_H_

//スプライトのN数を定xしている。
//?????????E?????
#define mxSPRITE	32768

typedef unsigned char MOJI;
typedef unsigned char U1;
typedef          char S1;
typedef unsigned short U2;
typedef          short S2;
typedef unsigned long U4;
typedef          long S4;
typedef float  F4;
typedef double F8;

typedef enum {
	ANIM_ATTACK,
	ANIM_DAMAGE,
	ANIM_DEAD,
	ANIM_STAND,
	ANIM_WALK,
	ANIM_SIT,
	ANIM_HAND,
	ANIM_HAPPY,
	ANIM_ANGRY,
	ANIM_SAD,
	ANIM_GUARD,
	ANIM_NOD,
	ANIM_THROW,
	ANIM_LIST_
} ANIM_LIST;//???????????????????????


// 1フレ`ムの情
typedef struct
{
	U4 BmpNo;				// bmp番号(real.binでの位置)
	S2 PosX,PosY;			// 表示位置のオフセット
	U2 SoundNo;				// サウンド番号
} FRAMELIST;


// １アニメ`ションの情
typedef struct
{
	U2 dir;					// アニメ`ションの向き
	U2 no;					// 何のアニメ`ションか(0は立ちポ`ズとか)
	U4 dtAnim;				// アニメ`ションのg行rg
	U4 frameCnt;
	FRAMELIST *ptFramelist;	// １アニメ`ションの情
} ANIMLIST;
//???????

// SPRの位置情 ????????
typedef struct
{
	U4 sprNo;				// I理上０から始まる(gHのSPR番号は65536ぐらいから)
	U4 offset;				// SPR.BINのファイルのiみzみ位置
	U2 animSize;			// アニメ`ションの数
} SPRADRN;


typedef struct
{
	U2 animSize;				// アニメ`ションの数
	ANIMLIST	*ptAnimlist;	//アニメ`ションリストここがNULLなら直接BMPを表示
} SPRITEDATA;


typedef struct {
	U2			dtz;			//描画されるときの番をQめる
	U2			noSpritedata;	//スプライト番号
	U2			mdAnim;			//アニメ`ションモ`ド
	U2			mdAnim2;		//次のアニメ`ションモ`ド
	U2			dir;			//方向は？
	U2			ctLoop;			//何回ル`プしたか？
	float		ctAnim;			//アニメ`ションカウンタ` ０～１未氦喜有∈I理、整数はフレ`ム番号I理
	float		dtAnim;			//スピ`ド	これが０であれば、sprファイルから引っる
} SPRITE;

extern SPRITEDATA SpriteData[mxSPRITE];


BOOL InitSprBinFileOpen( char *, char * );


#endif

