/************************/
/*	action.h			*/
/************************/
#ifndef _ACTION_H_
#define _ACTION_H_

/* I理先位  prio の ****************************************************/
enum{
	PRIO_TOP,			/* 最先 	*/

	PRIO_CHR,			/* キャラクタ等 */

	PRIO_BG,			/* ＢＧ 	*/
	PRIO_JIKI,			/* 自C 	*/
	PRIO_ENEMY,			/*  		*/
	PRIO_ENEMY_TAMA,	/*  	*/
	PRIO_JIKI_TAMA,		/* 自C 	*/
	PRIO_ITEM,			/* アイテム	*/
	PRIO_BOSS,			/* ボス		*/
	PRIO_GAME_OVER,		/* GAME OVER */
	PRIO_BTM = 255		/* 最後尾 	*/
};

/* アクションの状B ***********************************************************/
enum{
	ACT_STATE_ACT = 0,	/* 通常状B */
	ACT_STATE_DEAD		/* 死亡状B */
};
/* 属性 ***********************************************************************/
#define ACT_ATR_HIDE 			( 1 <<  1 )	/* 非表示 */
#define ACT_ATR_HIT 			( 1 <<  2 )	/* 当たり判定する */
#define ACT_ATR_HIT_BOX 		( 1 <<  3 )	/* 当たり判定 + ボックス表示 */
#define ACT_ATR_INFO 			( 1 <<  4 )	/* 一行インフォ表示 */
#define ACT_ATR_HIDE2 			( 1 <<  5 )	/* 非表示+あたり判定あり */
#define ACT_ATR_BTL_CMD_END 	( 1 <<  6 )	/* 殛Lコマンド入力K了フラグ */
#define ACT_ATR_TYPE_PC 		( 1 <<  7 )	/* ＰＣ */
#define ACT_ATR_TYPE_OTHER_PC 	( 1 <<  8 )	/* 他のＰＣ */
#define ACT_ATR_TYPE_PET 		( 1 <<  9 )	/* ペット */
#define ACT_ATR_TYPE_ITEM 		( 1 << 10 )	/* アイテム */
#define ACT_ATR_TYPE_GOLD 		( 1 << 11 )	/* お金 */
#define ACT_ATR_TYPE_OTHER 		( 1 << 12 )	/* その他（ドア、町の人、看板） */
#define ACT_ATR_HIT_BOX_ALL1 	( 1 << 13 )	/* 当たり判定 + ボックス表示 }数xk１ */
#define ACT_ATR_HIT_BOX_ALL2 	( 1 << 14 )	/* 当たり判定 + ボックス表示 }数xk２ */
#define ACT_ATR_HIT_BOX_ALL3 	( 1 << 15 )	/* 当たり判定 + ボックス表示 }数xk３ */
#define ACT_ATR_TRAVEL 			( 1 << 16 )	/* 旅行中 */

typedef unsigned char UCHAR;
typedef int                 BOOL;
typedef unsigned int        UINT;
#define FALSE 0
#define TRUE 1
/* アクション造体 *********动作结构*************************************************/
struct action{
	struct 	action *pPrev, *pNext;		/* 前後のアクションポインタ */
	void 	( *func )( struct action * );	/* g行v数へのポインタ */
	void 	*pYobi;						/* 予造体ポインタ */
	void 	*pOther;					/* 糜ポインタ */
	UCHAR 	prio;						/* I理の先位 */
	UCHAR 	dispPrio;					/* 表示の先位 */
	int 	x, y;						/* 表示座 */
	int		hitDispNo;					/* 当たり判定用番号 */
	BOOL	deathFlag;					/* アクション死亡フラグ */
	
	int 	dx, dy;						/* 分 */
	int 	dir;						/* 方向 */
	int 	delta;  					/* 合成ベクトル */
	
	char 	name[ 29 ];					/* 名前 */
	char 	freeName[ 13 ];				/* フリ`名前 */
	int 	hp;							/* ＨＰ */
	int 	maxHp;						/* 最大ＨＰ */
	int 	mp;							/* ＭＰ */
	int 	maxMp;						/* 最大ＭＰ */
	int 	level;						/* ＬＥＶＥＬ */
	int 	status;						/* ステ`タス */
	int 	itemNameColor;				/* アイテムの名前の色 */
	
	int		bmpNo;						/* スプライト番号 */
	int		atr;						/* 属性 */
	int		state;						/* 状B */
	int		actNo;						/* 行臃牛ㄗ杂嗓耸褂每桑 */
	int		damage;						/* ダメ`ジ */

	int		gx, gy;						// マップグリッド座耍ìF在地）
	int		nextGx, nextGy;				// マップグリッド座耍康牡兀
	int		bufGx[10], bufGy[10];		// 移酉茸摔违啸氓榨
	short	bufCount;					// 移幼摔O定されているか
	short	walkFlag;					// iきアニメから立ちアニメへ切り替えタイミングフラグ
	float	mx, my;						// マップ座
	float	vx, vy;						// 移分

// 属性
	short 	earth;						// 地 属性
	short 	water;						// 水 属性
	short 	fire;						// 火 属性
	short 	wind;						// L 属性

// ｒａｄｅｒ用
	int		dirCnt;						// 方向Qカウンタ`

//ｇｅｍｉｎｉ用  速度 方向
	int		spd;						// 移鹰攻冤`ド( ０～６３ )( ４で１ドット )
	int		crs;						// 方向( ０～３１ )( 上が０で右回り )
	int		h_mini;						// 小数点横
	int		v_mini;						// 小数点k

//ｐａｔｔｅｒｎ用 图案
	int		anim_chr_no;				// アニメ`ション番号（ anim_tbl.h の番号 )
	int		anim_chr_no_bak;			// 前回アニメ`ション番号
	int		anim_no;					// 幼鞣
	int		anim_no_bak;				// 前回の幼鞣
	int		anim_ang;					// アニメ`ション向き( ０～７ )( 下が０で右回り )
	int		anim_ang_bak;				// 前回のアニメ`ション向き
	int		anim_cnt;					// アニメ`ションカウンタ
	int		anim_frame_cnt;				// アニメ`ションフレ`ムカウンタ
	int		anim_x;						// オフセットＸ座 ( Sprbin + Adrnbin )
	int		anim_y;						// オフセットＹ座 ( Sprbin + Adrnbin )
	int		anim_hit;					// 当たり判定
};

typedef struct action ACTION;


/* 最初と最後のアクションリストポインタ */
extern ACTION *pActTop;
extern ACTION *pActBtm;

/* 自Cのポインタ */
extern ACTION *pJiki;

#ifdef _DEBUG		
/* アクション数カウント */
extern int ActCnt;
#endif

/* アクションリストに登h *****************************************************/
ACTION *GetAction( UCHAR prio, UINT yobiSize );

/* アクションリスト初期化 *****************************************************/
void InitAction( void );

/* アクション走らせる *********************************************************/
void RunAction( void );

/* アクションリストから抹************************************************/
void DeathAction( ACTION *pAct );

/* 全アクション抹 *******************************************************/
void DeathAllAction( void );

/* アクションリストから完全抹 ***********************************************/
void ClearAction( ACTION *pAct );

/* アクションK了I理 *********************************************************/
void EndAction( void );

#endif

