// main.cpp ヘッダファイル

#ifndef _MAIN_H_
#define _MAIN_H_

//---------------------------------------------------------------------------//
// 概要 ：マクロ（define)宣言                                                //
//---------------------------------------------------------------------------//
//标题栏的名字
#define DEF_APPNAME		"魔力" // "ＳｔｏｎｅＡｇｅ" //タイトルバ`の名前   好像没用窗口模式下 
#define DEF_APPSIZEX	640								//作成するWINDOWSの横幅640	
#define DEF_APPSIZEY	480							//作成するWINDOWSのk幅480
//#define DEF_APPSIZEX	320								//作成するWINDOWSの横幅
//#define DEF_APPSIZEY	240								//作成するWINDOWSのk幅

#define SCREEN_WIDTH_CENTER			DEF_APPSIZEX/2
#define SCREEN_HEIGHT_CENTER		DEF_APPSIZEY/2

//---------------------------------------------------------------------------//
// 概要 ：v数プロトタイプ宣言                                               //
//---------------------------------------------------------------------------//

int PASCAL WinMain( HINSTANCE 		hInstance, 	HINSTANCE 		hPrevInstance, 	LPSTR lpCmdLine, 	int nCmdShow );	// 初期化v数

LRESULT CALLBACK PASCAL WindMsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );				//メインプロシ`ジャ`
BOOL SystemTask( void );																		// メッセ`ジル`プv数

//#define P( a ) 	printf( #a "\n")
//#define Pd( a ) printf( #a " = %d\n",a )
//#define Pf( a ) printf( #a " = %f\n",a )
//#define Pp( a ) printf( #a " = %p\n",a )

// グロ`バル涫
extern HWND hWnd;		// ウィンドウハンドル
extern BOOL WindowMode;		// ウィンドウモ`ド
extern int ResoMode;
extern int LowResoCmdFlag;		// 解像度涓偿蕙螗丧榨楗
extern int	CmdShow;	// WinMainv数の引数をグロ`バルにする
extern LPSTR CmdLine; 	// WinMainv数のコマンドライン引数をグロ`バルにする
extern HANDLE hMutex;	// 同r起鹰隶Д氓芝弗Д趣违膝螗丧

extern char realBinName[];
extern char adrnBinName[];
extern char sprBinName[];
extern char sprAdrnBinName[];

#ifdef _DEBUG
extern BOOL offlineFlag;
extern char DebugKey0[ 256 ];		// デバックキ`０
extern char DebugKey1[ 256 ];		// デバックキ`１
extern char DebugKey2[ 256 ];		// デバックキ`２
#endif

#endif

