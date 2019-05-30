/************************/
/*	mouse.h				*/
/************************/
#ifndef _MOUSE_H_
#define _MOUSE_H_

//#include "menu.h"

// マウスの状B

#define	MOUSE_NO_CRICK						0		// 押されていない
#define	MOUSE_LEFT_CRICK			( 1 << 0 )		// 左クリック状B
#define	MOUSE_RIGHT_CRICK			( 1 << 1 )		// 右クリック状B
#define	MOUSE_LEFT_CRICK_UP			( 1 << 2 )		// 左クリックxした状B
#define	MOUSE_RIGHT_CRICK_UP		( 1 << 3 )		// 右クリックxした状B
#define	MOUSE_LEFT_DBL_CRICK		( 1 << 4 )		// 左ダブルクリック状B
#define	MOUSE_RIGHT_DBL_CRICK		( 1 << 5 )		// 右ダブルクリック状B
//#define	MOUSE_LEFT_AUTO_CRICK		( 1 << 6 )		// 左オ`トクリック状B
//#define	MOUSE_RIGHT_AUTO_CRICK		( 1 << 7 )		// 右オ`トクリック状B

// マウスの座恕造体
typedef struct{
	int x, y;	// マウスの( Ｘ,Ｙ )座
}MOUSE_POINT;

// マウス造体
typedef struct{
	MOUSE_POINT nowPoint;			// F在のマウスの座
	MOUSE_POINT crickLeftDownPoint;	// 最後に左クリックしたマウスの座
	MOUSE_POINT crickLeftUpPoint;	// 最後に左クリックをxしたマウスの座
	MOUSE_POINT crickRightDownPoint;// 最後に右クリックしたマウスの座
	MOUSE_POINT crickRightUpPoint;	// 最後に右クリックをxしたマウスの座
	int			state;				// マウスボタンの状Bビットフィ`ルド
	int			onceState;			// マウスボタンの状B。ただし、押されたフレ`ムにだけ毪
	int			autoState;			// マウスボタンの状Bオ`トリピ`ト
	UINT 		beforeLeftPushTime;	// 左クリックしたrのrgを
	UINT 		leftPushTime;		// 左クリックが押されているrg
	UINT 		beforeRightPushTime;// 右クリックしたrのrgを
	UINT 		rightPushTime;		// 右クリックが押されているrg
	UCHAR		level;				// F在のレベル（０～２５５）
	int 		itemNo;				// F在ドラッグ中のアイテムの番号
	BOOL flag;						// マウスカ`ソルの表示／非表示フラグ
}MOUSE;

// グロ`バル　マウス
extern MOUSE mouse;

// マウスカ`ソルに当たったフォント番号（フォント用）
extern int HitFontNo;
// マウスカ`ソルに当たったアクションポインタ（ＢＭＰ用）
extern int HitDispNo;

// 一行インフォ文字列
extern char OneLineInfoStr[];

// 表示ボックスの色
extern int BoxColor;

// マウス情螭纬跗诨 ////////////////////////////////////////////////////////
void MouseInit( void );

// F在のマウスの位置をする //////////////////////////////////////////////
void MouseNowPoint( int x, int y );

// マウスの左ボタンが押されたrのマウスの位置をする //////////////////////
void MouseCrickLeftDownPoint( int x, int y );

// マウスの右ボタンがxされたrのマウスの位置をする //////////////////////
void MouseCrickLeftUpPoint( int x, int y );

// マウスの左ボタンが押されたrのマウスの位置をする //////////////////////
void MouseCrickRightDownPoint( int x, int y );

// マウスの右ボタンがxされたrのマウスの位置をする //////////////////////
void MouseCrickRightUpPoint( int x, int y );

// 左ダブルクリックされたrのマウスの位置をする //////////////////////////
void MouseDblCrickLeftUpPoint( int x, int y );

// 右ダブルクリックされたrのマウスの位置をする //////////////////////////
void MouseDblCrickRightUpPoint( int x, int y );

// マウスI理　///////////////////////////////////////////////////////////////
void MouseProc( void );

/* マウスカ`ソルのあたり判定 **************************************************/
void HitMouseCursor( void );

#endif

