/************************/
/*	sprdisp.h			*/
/************************/
#ifndef _SPRDISP_H_
#define _SPRDISP_H_

#define DISP_BUFFER_SIZE 4096 	// 表示バッファサイズ

/* 表示先位  dispPrio の *************************************************/
enum{
	DISP_PRIO_BG 		= 0,	/* ＢＧ 	*/
	DISP_PRIO_TILE 		= 0,	// タイル（ゲ`ム画面のフィ`ルド部）
	DISP_PRIO_CHAR 		= 10,	// キャラ
	DISP_PRIO_PARTS 	= 10,	// パ`ツ（建物等）
	DISP_PRIO_RESERVE 	= 20,
	DISP_PRIO_JIKI 		= 30,	/* 自C 	*/
	DISP_PRIO_GRID 		= 100,	// グリッドカ`ソル
	DISP_PRIO_BOX,				/* ボックス */
	DISP_PRIO_IME1,				/* ＩＭＥボックス（バック用　\色） */
	DISP_PRIO_IME2,				/* ＩＭＥボックス（バック用　青色） */
								/* フォントバック */
	DISP_PRIO_MENU,				/* メニュ` */
	DISP_PRIO_IME3,				/* ＩＭＥボックス（フロント用　\色） */
	DISP_PRIO_IME4,				/* ＩＭＥボックス（フロント用　青色） */
	DISP_PRIO_BOX2,				/* ボックス２ */
	DISP_PRIO_ITEM,				/* アイテム */
								/* フォントフロント */
	DISP_PRIO_YES_NO_WND,		/* _Jウィンドウ */
	DISP_PRIO_YES_NO_BTN,		/* _Jボタン */
	DISP_PRIO_BOX3,				/* ボックス３ */
	DISP_PRIO_DRAG,				/* ドラッグ中 */
	DISP_PRIO_MOUSE,			/* マウスカ`ソル 	*/
	DISP_PRIO_TOP = 255			/* 最先 	*/
};


// 表示情造体
typedef struct {
	int x, y;			// 表示座
	int bmpNo;			// ＢＭＰ番号( ボックス表示のrは色番号 )
	ACTION *pAct;		// アクションポインタ 动作指针
	//每次判断  0 不能走？  1 可以 2显示盒子？
	BOOL hitFlag;		// 当たり判定するかフラグ　０：しない、１：する、２：ＢＯＸ表示
}DISP_INFO;

// 表示先ソ`ト用造体  优先排序/ /表示用结构体
typedef struct {
	short no;			// バッファ格{rの番号 缓冲存储时的号码
	UCHAR	dispPrio; 	// 表示の先位
}DISP_SORT;

// 表示バッファ造体
typedef struct{
	DISP_INFO 	DispInfo[ DISP_BUFFER_SIZE ];
	DISP_SORT 	DispSort[ DISP_BUFFER_SIZE ];
	short 		DispCnt;	// F在の格{数  格子显示数
}DISP_BUFFER;

// 表示バッファ造体
extern DISP_BUFFER 	DispBuffer;

// ＢＭＰiみzみ用ワ`クI域へのポインタ
extern LPBITMAPINFO lpBmpInfo;
// ＢＭＰのイメ`ジデ`タまでのオフセット
extern int BmpOffBits;

// Realbin iみzみ用ワ`クI域へのポインタ
extern char *pRealBinBits;
// 今回 Realbin からiみzむＢＭＰのサイズ
extern int RealBinWidth, RealBinHeight;

// BitBltのr
extern HBITMAP	hBmp;


// サ`フェスヴィジ`フラグ
extern int SurfaceBusyFlag;

// 表示バッファソ`ト ///////////////////////////////////////////////////////////
void SortDispBuffer( void );

/* キャラクタ`の描画 **************************************************/
void PutBmp( void );

// 表示デ`タをバッファに溜める ///////////////////////////////////////////////////
int StockDispBuffer( int x, int y, UCHAR prio, int bmpNo, BOOL hitFlag );

// 表示デ`タをバッファに溜める ///////////////////////////////////////////////////
// ただし中でrealGetNo()を呼ばない
int StockDispBuffer2( int x, int y, UCHAR prio, int bmpNo, BOOL hitFlag );

// タスク表示デ`タをバッファに溜める ///////////////////////////////////////////////////
void StockTaskDispBuffer( void );

// タスク表示デ`タをバッファに溜める ///////////////////////////////////////////////////
// ただしprio1からprio2までのものはI理しない
void StockTaskDispBuffer2( int prio1, int prio2 );

// ボックス表示デ`タをバッファに溜める ***************************************/
void StockBoxDispBuffer( int x1, int y1, int x2, int y2, UCHAR dispPrio, int color, BOOL fill );

#endif

