/**** SYSTEM INCLUDE ****/
#include "system.h"
#include "winnls32.h"
#include <ime.h>
#include <imm.h>
#include "ime_sa.h"
#include "../resource.h"
#include "netmain.h"
#include "battleMap.h"
#include "battleProc.h"
#include "netmain.h"


#if 1
// 通常r
#define REALBIN_DIR		"data\\real_1.bin" 		// REAL.BINの鏊
#define ADRNBIN_DIR		"data\\adrn_1.bin" 		// ADRN.BINの鏊
#define SPRBIN_DIR		"data\\spr_1.bin" 		// SPR.BINの鏊
#define SPRADRNBIN_DIR	"data\\spradrn_1.bin" 	// SPRADRN.BINの鏊

#else

// グラフィックマシン用
#define REALBIN_DIR		"..\\realbin\\data\\real.bin" 		// REAL.BINの鏊
#define ADRNBIN_DIR		"..\\realbin\\data\\adrn.bin" 		// ADRN.BINの鏊
#define SPRBIN_DIR		"..\\spr\\spr.bin" 					// SPR.BINの鏊
#define SPRADRNBIN_DIR	"..\\spr\\spradrn.bin" 				// SPRADRN.BINの鏊

#endif


//---------------------------------------------------------------------------//
// 概要 ：グロ`バル涫x                                                 //
//---------------------------------------------------------------------------//
HINSTANCE	hInst;			// インスタンスハンドル定x 实例窗口句柄定义
HWND hWnd;					// ウィンドウハンドル  窗口
int	CmdShow;				// WinMainv数の引数をグロ`バルにする  WinMain函数的论据和全球的
LPSTR CmdLine;				// WinMainv数のコマンドライン引数をグロ`バルにする  WinMain函数的命令行参数全球的
HANDLE hMutex;				// 同r起鹰隶Д氓芝弗Д趣违膝螗丧  同时启动检查对象的方向盘

BOOL WindowMode = TRUE;		// ウィンドウモ`ド/ / / /窗口模式
//BOOL WindowMode = FALSE;	// フルスクリ`ンモ`ド/ /????????
							//	TRUE  : WindowMode
							//	FALSE : FullScreen
BOOL NoDelay = TRUE;		// TCP_NODELAY を使うかどうか。add by ringo  TCP_NODELAY是否使用 一种封包优化
int ResoMode = 0;			// 解像度モ`ド（０：６４０＊４８０　１：３２０＊２４０　２：可洌┓直媛誓Ｊ
int LowResoCmdFlag = 0;		// 解像度涓偿蕙螗丧榨楗  分辨率变更指令转折点

#ifdef _DEBUG
//BOOL offlineFlag = FALSE;	// オフラインフラグ 离线转折点  脱机模式
BOOL offlineFlag = FALSE;  //没用以后查原因
char DebugKey0[ 256 ];		// デバックキ`０ 调试键0
char DebugKey1[ 256 ];		// デバックキ`１
char DebugKey2[ 256 ];		// デバックキ`２
#endif


char realBinName[256];
char adrnBinName[256];
char sprBinName[256];
char sprAdrnBinName[256];


/* コマンドライン解析   命令行解析 */
void AnalyzeCmdLine( void );
// ウィンドウモ`ド切り替えv数宣言
//窗口模式转换函数宣言
void ChangeWindowMode( void );
// 解像度モ`ドO定I理
//分辨率模式设定处理
void SetResoMode(int Mode);

// Windowsのメッセ`ジをフックする  的信息的钩
//HRESULT CALLBACK HookProc( int nCode, WPARAM wParam, LPARAM lParam );				//メインプロシ`ジャ`

//---------------------------------------------------------------------------//
// 概要 ：メインル`プI理v数                                               //
// 引数 ：HANDLE hInstance                                                   //
//        HANDLE hPrevInstance                                               //	
//        LPSTR lpCmdLine                                                    //
//        int   nCmdShow                                                     //
//---------------------------------------------------------------------------//
int PASCAL WinMain( HINSTANCE hInstance ,HINSTANCE hPrevInstance ,LPSTR lpCmdLine ,int nCmdShow )
{
	WNDCLASS wndclass;
	hInst = hInstance;		// ???????でもらった???????????を????????に代入

	CmdShow = nCmdShow;		// WinMainv数の引数をグロ`バルにする
	CmdLine = lpCmdLine;	// WinMainv数のコマンドライン引数をグロ`バルにする	
	
#ifndef _DEBUG
//#ifdef _DEBUG
	// 同r起鹰隶Д氓芝弗Д茸鞒  同时启动检查对象作成 没起作用以后查看下
	hMutex = CreateMutex( NULL, TRUE, DEF_APPNAME );
	// 同rに二つ立ち上げたr  同时两个启动的时候
	if( GetLastError() == ERROR_ALREADY_EXISTS ){
		MessageBox( hWnd, "ＳｔｏｎｅＡｇｅ已经启动，无法多次启动。", "_J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	// アップデ`トからg行したかチェック  从检查更新实行了吗
	if( !strstr( CmdLine, "updated" ) ){
		MessageBox( hWnd, "ＳｔｏｎｅＡｇｅ.ｅｘｅ请启动。", "_J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
#endif

    //*** ウインドウクラスの生成 ***窗口班的生成
	//如果其他的实例启动中省略的话初期化
    if( !hPrevInstance )	// もし他のインスタンスが起又肖胜槌跗诨÷ 
	{
		wndclass.style = CS_BYTEALIGNCLIENT | CS_DBLCLKS;	// クラススタイル
		wndclass.lpfnWndProc = WindMsgProc;		// ウインドウプロシ`ジャ`アドレス定x
		wndclass.cbClsExtra = 0;				// クラス挨窝a足定xなし
		wndclass.cbWndExtra = 0;				// ウインドウ挨窝a足デ`タoし
		wndclass.hInstance = hInstance;			// インスタンスハンドル定x
//		wndclass.hIcon = LoadIcon(hInstance ,"ICON1");	// ????からICON1をロ`ド
	    wndclass.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(SA_ICON2));
		//wndclass.hIcon = LoadIcon( NULL, IDI_APPLICATION);

	
#ifdef _DEBUG
		wndclass.hCursor = (LoadCursor(hInstance ,MAKEINTRESOURCE(SA_MOUSE)));	// 光标を矢印にO定SA_MOUSE
#else		
		wndclass.hCursor = LoadCursor(NULL ,IDC_ARROW);	// ???????を矢印にO定  箭设定
		//wndclass.hCursor = LoadCursor(NULL ,MAKEINTRESOURCE(SA_MOUSE));	// ???????を矢印にO定  箭设定
#endif
		
		//窗口的背景色
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);// ウィンドウの背景色
		wndclass.lpszMenuName= NULL;			// メニュ`なし  没有菜单
		wndclass.lpszClassName = DEF_APPNAME;	// タイトルバ`文字O定  标题栏文字设定
		if( !RegisterClass(&wndclass) )
			return FALSE;						// 作成に失・筏郡榻K了する
	}

	// iみzみファイル名のO定  读取文件名的设定
	strcpy( realBinName, REALBIN_DIR );			// REAL.BINの鏊
	strcpy( adrnBinName, ADRNBIN_DIR );			// ADRN.BINの鏊
	strcpy( sprBinName, SPRBIN_DIR );			// SPR.BINの鏊
	strcpy( sprAdrnBinName, SPRADRNBIN_DIR );	// SPRADRN.BINの鏊
	
	//コマンドライン解析   命令行
	AnalyzeCmdLine();
	
	// 解像度モ`ドO定   分辨率模式设定
	SetResoMode( ResoMode );
	
	// ウィンドウの作成  窗口的作成
	ChangeWindowMode();
	// ウィンドウの作成
	//ChangeWindowMode();
	
	// 全てのウィンドウからキ`ボ`ド入力をZう  从所有的窗口键盘输入夺走
	//SetFocus( hWnd );

	//{
		// ALT+TAB o郡摔工
	//	int nOldVal;
	//	SystemParametersInfo (SPI_SCREENSAVERRUNNING, TRUE, &nOldVal, 0);
	//}
	
	// Windowsのメッセ`ジをフックプロジ`ジャをセットする
	///SetWindowsHookEx( WH_SYSMSGFILTER, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_MSGFILTER, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_KEYBOARD, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_CALLWNDPROC, HookProc, hInstance, 0 );
	//SetWindowsHookEx( WH_CBT, HookProc, hInstance, 0 );
	
	// ????????
	GameMain();
	
	return FALSE;
}

#ifdef _DEBUG

// デバックキ`解析 **调试键解析*********************************************************/
void DebugKey( char *str )
{
	char *work;
	int i = 0;
	
	// コマンドラインオプションに key0: があるとき
	//命令行选项key0:的时候
	if( ( work = strstr( str, "KEY0:" ) ) )
	{
		work += 5;	// 文字列のあるところまでポインタMめる
		// : がくるまでル`プ
		//文字列的地方为止推进指针/ /:到循环
		while( *work != ':' ){
			DebugKey0[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}
	DebugKey0[ i ] = NULL;	// K端号
	i = 0;
	// コマンドラインオプションに key1: があるとき
	if( ( work = strstr( str, "KEY1:" ) ) )
	{
		work += 5;	// 文字列のあるところまでポインタMめる
		// : がくるまでル`プ
		while( *work != ':' ){
			DebugKey1[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}	
	DebugKey1[ i ] = NULL;	// K端号
	i = 0;
	// コマンドラインオプションに key1: があるとき
	if( ( work = strstr( str, "KEY2:" ) ) )
	{
		work += 5;	// 文字列のあるところまでポインタMめる
		// : がくるまでル`プ
		while( *work != ':' ){
			DebugKey2[ i ] = *work++;
			i++;
			if( i == 255 ){ 
				i = 0;
				break;
			}
		}
	}	
	DebugKey2[ i ] = NULL;	// K端号
}

#endif
/* コマンドライン解析 ****命令行解析*****************************************************/
void AnalyzeCmdLine( void )
{
	char *addr;

	// iみzみファイル名の涓
	if( addr = strstr( CmdLine, "realbin:" ) )
	{
		sprintf( realBinName, "data\\real_%d.bin", atoi(addr+strlen("realbin:")) );
	}
	if( addr = strstr( CmdLine, "adrnbin:" ) )
	{
		sprintf( adrnBinName, "data\\adrn_%d.bin", atoi(addr+strlen("adrnbin:")) );
	}
	if( addr = strstr( CmdLine, "sprbin:" ) )
	{
		sprintf( sprBinName, "data\\spr_%d.bin", atoi(addr+strlen("sprbin:")) );
	}
	if( addr = strstr( CmdLine, "spradrnbin:" ) )
	{
		sprintf( sprAdrnBinName, "data\\spradrn_%d.bin", atoi(addr+strlen("spradrnbin:")) );
	}

	// コマンドラインオプションに windowmode があるとき
	/// /?郢M????d?Sindowmode / / nodelay???G? ????????????????????0.4???
	if( strstr( CmdLine, "windowmode" ) ) WindowMode = TRUE;
	
	// nodelay???G? ????????????????????0.4??? オプションのO定。（通信待ち０．４秒ありにするr）
	if( strstr( CmdLine, "nodelay" ) ) NoDelay = FALSE;
	
#ifdef _DEBUG
	// オフラインモ`ドのO定  脱机模式的设定
	if( strstr( CmdLine, "offline" ) )
	{
		offlineFlag = TRUE;
	}
	else
	{
		offlineFlag = FALSE;
	}
	if( offlineFlag )
	{
		ProcNo = PROC_GAME;	
	}
	else
	{
		ProcNo = PROC_ID_PASSWORD;
	}
	if( strstr( CmdLine, "PROC_OHTA_TEST" ) ) 
	{
		ProcNo = PROC_OHTA_TEST;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "PROC_TAKE_TEST" ) )
	{
	 	ProcNo = PROC_TAKE_TEST;
		offlineFlag = TRUE;
	 }
	if( strstr( CmdLine, "PROC_DWAF_TEST" ) )
	{
	 	ProcNo = PROC_DWAF_TEST;
		offlineFlag = TRUE;
	 }
	if( strstr( CmdLine, "PROC_BATTLE" ) )
	{
		ProcNo = PROC_BATTLE;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "sprview" ) )
	{
	 	ProcNo = PROC_SPR_VIEW;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "animview" ) )
	{
	 	ProcNo = PROC_ANIM_VIEW;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "setest" ) )
	{
	 	ProcNo = PROC_SE_TEST;
		offlineFlag = TRUE;
	}
	if( strstr( CmdLine, "encountoff" ) )
	{
		EncountOffFlag = TRUE;
	}
	if( strstr( CmdLine, "lowreso" ) )
	{
		ResoMode = 1;
	}
#ifdef _LOG_MSG
	if( strstr( CmdLine, "RECVDATA:" ) )
	{
		char *pt = strstr( CmdLine, "RECVDATA:" )+9;
		char *name;
		int i;

		name = debugLogFileName;
		for( i = 0; i < 255; i++ )
		{
			if( *pt != ' ' && *pt != '\0' && *pt != '\t' )
			{
				*name++ = *pt++;
			}
			else
			{
				break;
			}
		}
		*name = '\0';
	}
#endif
	//  ???Vg????
	DebugKey( CmdLine );
#else
	// デフォルトの龊悉悉长 默认的情况这是
	ProcNo = PROC_ID_PASSWORD;
#endif
	// 命令行的IP地址解析  コマンドラインのIPアドレス解析
	ipAddressAnalyze( CmdLine );
}
/* ウィンドウモ`ド切り替え ***************************************************/
void ChangeWindowMode( void )
{
	RECT clientRect;		// クライアントの表示I域
	DWORD windowStyle;		// ウィンドウスタイル

	// ウィンドウモ`ドのr
	if( WindowMode ){
	
		// ウィンドウスタイル
		windowStyle = WS_MINIMIZEBOX | WS_SYSMENU | 
			            //我开启1
						WS_THICKFRAME |
						WS_CAPTION | WS_OVERLAPPED;
							
		// ウィンドウ作成rでなかったら
		if( DDinitFlag ){
			// マウスカ`ソルを表示
			//ShowCursor( TRUE );
		}
	}
	// フルスクリ`ンのr
	else{	
	
		// ウィンドウスタイル
		windowStyle = WS_VISIBLE | WS_POPUP;
		// マウスカ`ソルをLす
		//ShowCursor( FALSE );
	}
	
	// クライアントI域のO定
	SetRect( &clientRect, 0, 0, lpDraw->xSize, lpDraw->ySize );
	// クライアントI域のサイズからウィンドウサイズを算し clientRect へ入れる
	//从窗口尺寸大小客户领域的计算clientRect放进
	AdjustWindowRectEx( &clientRect, windowStyle, FALSE, NULL );
	// ウィンドウを作成するとき
	//if( !DDinitFlag ){	// DirectDrawが初期化されてないとき
	if( hWnd == NULL ){	// DirectDrawが初期化されてないとき
	
		// ウインドウの生成
		hWnd = CreateWindowEx( 	NULL, 
								DEF_APPNAME, 
								DEF_APPNAME, 
								windowStyle,
								//CW_USEDEFAULT, 
								//CW_USEDEFAULT, 
								0, 
								0, 
								clientRect.right - clientRect.left, 
								clientRect.bottom - clientRect.top, 
								NULL, 
								NULL, 
								hInst, 
								NULL );
	}
	// ウィンドウモ`ドを涓工毪趣
	else{	
		// ウィンドウスタイルの涓
		SetWindowLong( hWnd, GWL_STYLE, windowStyle );
		// 作成したウインドウを表示
		ShowWindow( hWnd, CmdShow );
		// ウィンドウの位置とサイズの涓
		SetWindowPos( 	hWnd, 
						HWND_NOTOPMOST, 
						0, 
						0, 
						clientRect.right - clientRect.left, 
						clientRect.bottom - clientRect.top, 
						//SWP_SHOWWINDOW );
						SWP_FRAMECHANGED );
	}
	
	// 作成したウインドウを表示
	ShowWindow( hWnd, CmdShow );

	// ウィンドウを表示更新
	UpdateWindow( hWnd );
}

#if 0
// Windowsのメッセ`ジをフックする
HRESULT CALLBACK HookProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	int a = 0;
	
	switch( nCode ){
	//switch( wParam ){
		case MSGF_NEXTWINDOW:
		//case HCBT_SYSCOMMAND:
			a++;
			break;
		
		case HCBT_ACTIVATE:
			a++;
			break;
	}
	return 0;
}
#endif

//---------------------------------------------------------------------------//
// 概要 ：メッセ`ジル`プI理v数                                                     //
// 引数 ：なし   信息循环处理函数/ // /参数：无                                                            //
//---------------------------------------------------------------------------//
BOOL SystemTask( void )
{
	MSG msg;
	
	// ☆★☆★☆ メッセ`ジル`プ ☆★☆★☆
	// メッセ`ジがあるgル`プ  信息有间循环
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )){
		// メッセ`ジを受け取る、WM_QUIT を受け取るとFALSEが返ってきてK了
		//if( GetMessage( &msg, NULL, 0, 0 ) == FALSE ) return FALSE; 
		// WM_QUIT を受け取るとK了
		if( msg.message == WM_QUIT ) return FALSE;
		
		TranslateMessage(&msg);		// Qが必要なメッセ`ジはQする
		
		DispatchMessage(&msg);		// ウィンドウプロシ`ジャへメッセ`ジを送る
			
	}
	return TRUE;
}

//---------------------------------------------------------------------------//
// 概要 ：ウインドウメッセ`ジI理v数                                       //
// 引数 ：HWND hWnd:     ウィンドウのReハンドル                            //
//        UINT Message:  I理する制御メッセ`ジ                              //
//        WPARAM wParam: I理する制御メッセ`ジa足情螅                    //
//        LPARAM lParam: I理する制御メッセ`ジa足情螅                    //
//---------------------------------------------------------------------------//
LRESULT CALLBACK WindMsgProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch( Message ){
		//case MSGF_NEXTWINDOW: // ALT+TAB 押したとき
			//SetActiveWindow( hWnd );
		//	{ 
		//		int a; 
		//		a++;
		//	}
		//	break;
		
/** システムvBメッセ`ジ ***系统相关信息**************************************************/

		//这个应用软件激活或者非激活了的时候  当应用失去焦点或停止活动时
		case WM_ACTIVATE:	// このアプリケ`ションがアクティブ又は非アクティブになったr
			
			// ウィンドウがアクティブにされるとき 窗口被激活的时候或是被设置为焦点的时间
			if( ( wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE ) && lpDraw != NULL){
			
				// サ`フェスが LOST していたら  表面有LOST了
				if( CheckSurfaceLost() == TRUE ){
					// DirectDraw _放
					ReleaseDirectDraw();
					// 解像度モ`ドO定
					SetResoMode( ResoMode );
					// DirectDraw 初期化
					if( InitDirectDraw() == FALSE ){
						MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失・筏蓼筏俊", "_J", MB_OK | MB_ICONSTOP );
						// ＷＩＮＤＯＷＳに WM_CLOSE メッセ`ジを送らせる
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// ウィンドウモ`ド切り替え
					//ChangeWindowMode();
					// オフスクリ`ンサ`フェスの作成   清洁表面的作成  应该是界面
					InitOffScreenSurface();
					// パレット初期化
					if( InitPalette() == FALSE ){
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// バトルマップiみzみとバトルサ`フェスの画像作成
					//战斗地图，加载和战斗回程的画像作成
					if( ProcNo == PROC_BATTLE ){ 
						// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						ReadBattleMap( BattleMapNo );
						// バックサ`フェスを\でクリア`
						ClearBackSurface();	
						// ＢＭＰをバックサ`フェスにセット
						PutBmp();	
						// バックサ`フェスからバトルサ`フェスへコピ`  背部实体表面复制到战斗
						//函数复制一个后台缓存中的矩形区域到主页面上。

						lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
						// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						// F在のrgを
						NowTime = GetTickCount();
					}
				}
				// パレットオブジェクトがないr  托盘对象的时候
				if( lpDraw->lpPALETTE != NULL ){
					// ウィンドウモ`ドのr  窗口方式的时候
					if( WindowMode ){
						// 作成しておいたパレットに浃à  制作方面的托盘改变
						//调色板
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// マウスクリック入力の初期化  鼠标点击输入的初期化
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// rgのWれ忘れさせる  的时间晚让人忘了 获得当前时间系统启动--》
				NowTime = GetTickCount();
			}
			
			//ダイレクトインプットの修正
			//キ`ボ`ドＯＫなら
			// /直接输入的修正/ /键盘OK的话
			if(keyboad_flg == TRUE)
			{		//キ`ボ`ドＯＫなら
				// キ`ボ`ドオブジェクトがあるとき  键盘对象的时候
				if(pDInputDevice != NULL){
					// ウィンドウが非アクティブにされるとき  窗口非被激活的时候
					if(wParam == WA_INACTIVE)
					{
						// DInput 停止
						DInputActiveFlag = FALSE;
						// キ`バッファの初期化  键缓冲的初期化  实际创建键盘对象
						for( int i = 0 ; i < 256 ; i++ ) di_key[ i ] = 0;
						pDInputDevice->Unacquire();
					}
					else
					{
						
						// DInput 活  获得鼠标  找鼠标的
						DInputActiveFlag = TRUE;
						pDInputDevice->Acquire();
					
					}
				}
			}

			//ジョイスティックＯＫなら  控制杆OK的话
			if(joy_flg == TRUE){		
				// ジョイスティックオブジェクトがあるとき  控制杆对象的时候
				if(pDInputDevice2 == NULL)
					break;

				// ウィンドウが非アクティブにされるとき  窗口非被激活的时候
				if(wParam == WA_INACTIVE){
					// DInput 停止
					DInputActiveFlag = FALSE;
					pDInputDevice2->Unacquire();
				}
				else{
					// DInput 活
					DInputActiveFlag = TRUE;
					pDInputDevice2->Acquire();
				}
			}
			break;

//		case WM_ACTIVATEAPP: // 制御がこのアプリケ`ションに移ったr
		
			// ALT+TAB切り替えにてLOSTした情螭蛐
			//if( ( wParam ) && lpDraw != NULL ){
			//break;
			
		case WM_CLOSE:				// ウィンドウ]じるとき
		
			DestroyWindow( hWnd );	// ウィンドウを破する、同rに
									// WM_DESTROY メッセ`ジが送られる
			break;

		case WM_DESTROY:			// ウィンドウが破されたとき

			PostQuitMessage( 0 );	// WM_QUIT メッセ`ジを送る ( I理のK了 )
			break;
			
#if 0			
		case WM_PALETTECHANGED:		// パレットが涓丹欷郡趣
			// DirectDrawオブジェクトがないr
			if( lpDraw == NULL ) break;
			// パレットオブジェクトがないr
			if( lpDraw->lpPALETTE == NULL ) break;
			// ウィンドウモ`ドのr
			//我开启
			if( WindowMode ){
				// 作成しておいたパレットに浃à
				lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			}  //我开启
			break;
			//if( (HWND)wParam == hWnd )  break;
			
		case WM_PALETTEISCHANGING:		// パレットが涓丹欷郡趣
			// DirectDrawオブジェクトがないr
			if( lpDraw == NULL ) break;
			// パレットオブジェクトがないr
			if( lpDraw->lpPALETTE == NULL ) break;
			// ウィンドウモ`ドのr
			//if( WindowMode ){
				// 作成しておいたパレットに浃à
				lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			//}
			break;
		
		case WM_QUERYNEWPALETTE:	// パレット涓扦毪趣ēΕ％螗丧Ε猢`ドのrのみ）
		
			// DirectDrawオブジェクトがないr
			if( lpDraw == NULL ) break;
			// パレットオブジェクトがないr
			if( lpDraw->lpPALETTE == NULL ) break;
			
			// ウィンドウモ`ドのr
			//if( WindowMode ){
				// 作成しておいたパレットに浃à
				lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			//}
			
			break;
#endif			
			
/** キ`入力メッセ`ジ *****键输入信息************************************************/

      	case WM_KEYDOWN:	// キ`入力I理
		
			// サ`フェスヴィジ`のr
			///键盘输入处理维`/ /回程的时候
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				//MessageBox( hWnd, "失去的图像数据修复。。", "_J", MB_OK | MB_ICONSTOP );
			
				// DirectDraw _放
				ReleaseDirectDraw();
				// 解像度モ`ドO定
				SetResoMode( ResoMode );
				// DirectDraw 初期化
				if( InitDirectDraw() == FALSE ){
					MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失・筏蓼筏俊", "_J", MB_OK | MB_ICONSTOP );
					// ＷＩＮＤＯＷＳに WM_CLOSE メッセ`ジを送らせる
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// ウィンドウモ`ド切り替え
				//ChangeWindowMode();
				// オフスクリ`ンサ`フェスの作成 
				InitOffScreenSurface();
				// パレット初期化
				if( InitPalette() == FALSE ){
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// バトルマップiみzみとバトルサ`フェスの画像作成
				if( ProcNo == PROC_BATTLE ){ 
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					ReadBattleMap( BattleMapNo );
					// バックサ`フェスを\でクリア`
					ClearBackSurface();	
					// ＢＭＰをバックサ`フェスにセット
					PutBmp();	
					// バックサ`フェスからバトルサ`フェスへコピ`
					lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					// F在のrgを
					NowTime = GetTickCount();
				}
				// パレットオブジェクトがないr
				if( lpDraw->lpPALETTE != NULL ){
					// ウィンドウモ`ドのr
					if( WindowMode ){
						// 作成しておいたパレットに浃à
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// マウスクリック入力の初期化
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// rgのWれ忘れさせる
				NowTime = GetTickCount();
			}
			
            switch( wParam ){
                case VK_ESCAPE: // ＥＳＣキ`押されたとき
					// ＷＩＮＤＯＷＳに WM_CLOSE メッセ`ジを送らせる
					//ESC键被推的时候对WM _ CLOSE / / WINDOWS信息延缓
					//PostMessage( hWnd, WM_CLOSE, 0, 0L );
					break;
					
				case VK_RETURN:	// リタ`ンキ`押されたr   
				
					/* リタ`ンキ`I理   按下回车键时回车键 /处理*/
					KeyboardReturn();
				
					break;
					
				case VK_BACK:	// BS で一文字削除  在一个字删除
				
					// ＢＳキ`I理   / / BS/ / BS键处理
					KeyboardBackSpace();
				
					break;
					
				case VK_DELETE:	// チャットバッファクリア
				
					/* チャット文字を全てクリアする */
					ClearChatBuffer();
					
					break;
					
				case VK_F5:	// チャットバッファクリア
					
					/* チャット文字を全てクリアする  缓冲清除聊天/聊天文字全部清除 */
					wParam = wParam;
					
					break;
			}
			break;
			
		case WM_SYSKEYDOWN:	// ALTキ`押されたとき  按下ALT键的时候
		
			// サ`フェスヴィジ`のr 实体维信息的时候
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				//MessageBox( hWnd, "失われた画像デ`タを修します。", "_J", MB_OK | MB_ICONSTOP );
			
				// DirectDraw _放
				ReleaseDirectDraw();
				// 解像度モ`ドO定
				SetResoMode( ResoMode );
				// DirectDraw 初期化
				if( InitDirectDraw() == FALSE ){
					MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失・筏蓼筏俊", "_J", MB_OK | MB_ICONSTOP );
					// ＷＩＮＤＯＷＳに WM_CLOSE メッセ`ジを送らせる
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// ウィンドウモ`ド切り替え
				//ChangeWindowMode();
				// オフスクリ`ンサ`フェスの作成 
				InitOffScreenSurface();
				// パレット初期化
				if( InitPalette() == FALSE ){
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// バトルマップiみzみとバトルサ`フェスの画像作成
				if( ProcNo == PROC_BATTLE ){ 
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					ReadBattleMap( BattleMapNo );
					// バックサ`フェスを\でクリア`
					ClearBackSurface();	
					// ＢＭＰをバックサ`フェスにセット
					PutBmp();	
					// バックサ`フェスからバトルサ`フェスへコピ`
					lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					// F在のrgを
					NowTime = GetTickCount();
				}
				// パレットオブジェクトがないr
				if( lpDraw->lpPALETTE != NULL ){
					// ウィンドウモ`ドのr
					if( WindowMode ){
						// 作成しておいたパレットに浃à
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// マウスクリック入力の初期化
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// rgのWれ忘れさせる
				NowTime = GetTickCount();
			}
			
			switch( wParam )
			{
					//在窗口模式变更  ALT+RETURN
				//按下回车键时回车键 /处理*/
				case VK_RETURN:	// ALT+RETURN でウィンドウモ`ド涓  在窗口模式变更
				
					// 演出中はiける  演出中穿过
					if( BackBufferDrawType == DRAW_BACK_PRODUCE ) break; 
					
					// DirectDrawオブジェクトがないr  对象的时候
					if( lpDraw == NULL ) break;
					
					// ウィンドウモ`ドフラグ涓
					if( WindowMode == TRUE ) WindowMode = FALSE;
					else WindowMode = TRUE;
					
					// DirectDraw _放
					ReleaseDirectDraw();

					// 解像度モ`ドO定
					SetResoMode( ResoMode );

					// DirectDraw 初期化
					InitDirectDraw();
					// オフスクリ`ンサ`フェスの作成 
					InitOffScreenSurface();
					// ウィンドウモ`ド切り替え
					ChangeWindowMode();
					// 全ウィンドウに再描画要求
					//SendMessage( HWND_BROADCAST, WM_PAINT, NULL, NULL );
		
					// パレット初期化
					if( InitPalette() == FALSE ){
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// バトルマップiみzみとバトルサ`フェスの画像作成
					if( ProcNo == PROC_BATTLE ){ 
						// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						ReadBattleMap( BattleMapNo );
						// バックサ`フェスを\でクリア`
						ClearBackSurface();	
						// ＢＭＰをバックサ`フェスにセット
						PutBmp();	
						// バックサ`フェスからバトルサ`フェスへコピ`
						lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
						// バッファ初期化
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						// F在のrgを
						NowTime = GetTickCount();
					}	
					
					break;
				default:
					// 未I理メッセ`ジをデフォルトI理v数へ渡してI理させる
					//未处理的信息处理函数默认交给处理
					return DefWindowProc( hWnd, Message, wParam, lParam );
			}
			break;
			
      	case WM_CHAR:	// 文字入力I理  文字输入处理
		
				// 文字バッファに溜める  文字缓冲蓄

			// StockChatBufferWord( WORD wParam );

				StockStrBufferChar( ( char )wParam );
			
			break;
			
/** マウス入力メッセ`ジ *********鼠标输入信息********************************************/

		case WM_MOUSEMOVE:		// マウスを婴筏r  移动鼠标的时候
		
			// F在のマウスの位置をする
			MouseNowPoint( LOWORD(lParam), HIWORD(lParam) );
			if( mouse.flag == TRUE ){ 
				ShowCursor( FALSE ); // マウスカ`ソルを消す
				mouse.flag = FALSE;
			}
			// サ`フェスヴィジ`のr
			if( SurfaceBusyFlag == TRUE ){
				SurfaceBusyFlag = FALSE;
				//MessageBox( hWnd, "失われた画像デ`タを修します。", "_J", MB_OK | MB_ICONSTOP );
			
				// DirectDraw _放
				ReleaseDirectDraw();
				// 解像度モ`ドO定
				SetResoMode( ResoMode );
				// DirectDraw 初期化
				if( InitDirectDraw() == FALSE ){
					MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失・筏蓼筏俊", "_J", MB_OK | MB_ICONSTOP );
					// ＷＩＮＤＯＷＳに WM_CLOSE メッセ`ジを送らせる
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// ウィンドウモ`ド切り替え
				//ChangeWindowMode();
				// オフスクリ`ンサ`フェスの作成 
				InitOffScreenSurface();
				// パレット初期化
				if( InitPalette() == FALSE ){
					PostMessage( hWnd, WM_CLOSE, 0, 0L );
				}
				// バトルマップiみzみとバトルサ`フェスの画像作成
				if( ProcNo == PROC_BATTLE ){ 
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					ReadBattleMap( BattleMapNo );
					// バックサ`フェスを\でクリア`
					ClearBackSurface();	
					// ＢＭＰをバックサ`フェスにセット
					PutBmp();	
					// バックサ`フェスからバトルサ`フェスへコピ`
					lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
					// バッファ初期化
					DispBuffer.DispCnt = 0;
					FontCnt = 0;
					// F在のrgを
					NowTime = GetTickCount();
				}
				// パレットオブジェクトがないr
				if( lpDraw->lpPALETTE != NULL ){
					// ウィンドウモ`ドのr
					if( WindowMode ){
						// 作成しておいたパレットに浃à
						lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
					}
				}
				// マウスクリック入力の初期化
				mouse.state = MOUSE_NO_CRICK;
				mouse.onceState = MOUSE_NO_CRICK;
				// rgのWれ忘れさせる
				NowTime = GetTickCount();
			}

			break;
			
		case WM_NCMOUSEMOVE:	// マウスがウィンドウからはみ出たr  鼠标从窗口露出了的时候
		
			// マウスクリック入力の初期化
			mouse.state = MOUSE_NO_CRICK;
			mouse.onceState = MOUSE_NO_CRICK;
			if( mouse.flag == FALSE ){ 
				ShowCursor( TRUE ); // マウスカ`ソルを表示
				mouse.flag = TRUE;
			}
			
			break;
			
		case WM_LBUTTONDOWN:	// 左クリック(押したr)
		
			// 左クリックされたrのマウスの位置をする
			MouseCrickLeftDownPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_LBUTTONUP:		// 左クリック(xしたr)
		
			// 左クリックされたrのマウスの位置をする
			MouseCrickLeftUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;

		case WM_LBUTTONDBLCLK:		// 左ダブルクリック
		
			// 左ダブルクリックされたrのマウスの位置をする
			MouseDblCrickLeftUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_RBUTTONDOWN:	// 右クリック(押したr)
		
			// 左クリックされたrのマウスの位置をする
			MouseCrickRightDownPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_RBUTTONUP:		// 右クリック(xしたr)
		
			// 左クリックされたrのマウスの位置をする
			MouseCrickRightUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;

		case WM_RBUTTONDBLCLK:		// 右ダブルクリック
		
			// 右ダブルクリックされたrのマウスの位置をする
			MouseDblCrickRightUpPoint( LOWORD(lParam), HIWORD(lParam) );
			
			break;
			
		case WM_MBUTTONDOWN:	// 中クリック(押したr)
			break;

/** ＩＭＥvBメッセ`ジI理 *******有关信息处理*******************************************/
			
			//的小窗口变化的时候
		case WM_IME_COMPOSITION: // ＩＭＥのちっちゃいウィンドウに浠r
	
			// WM_IME_COMPOSITION メッセ`ジのI理
			ImeComposition( lParam );
			
			break;

			//文字来确定后。变换中的文字都在的时候也会乱码BS。/ / IME缓冲在空中的
		case WM_IME_ENDCOMPOSITION: // 文字を_定した直後に来る。Q中の文字を全部BSでけしたときもくる。
			// ＩＭＥバッファ`を空にする
			*ImeBuffer = '\0';
			break;
		
		case WM_IME_NOTIFY:
			/* WM_IME_NOTIFY メッセ`ジのI理   信息处理*/
			//ImeNotify( wParam ,lParam );
			// ここにDEFWINDOWPROCがあると、Q候aウインドウを出すようだ。
			// ＩＭＥ入力モ`ドの取得
			//这里有DEFWINDOWPROC，变换候补窗口发出。/ / IME输入方式的取得
			ImmGetConversionStatus(  hImc, &conversion, &sentence  );
			break;
			
		case WM_IME_CHAR:
			// ここにDefWindowProcがあると、WM_CHARを生成する
			//这里有DefWindowProc，WM _ CHAR生成
			break;
#if 0
		//case WM_IME_SETCONTEXT:
			//初期化したとき来てるのかな？フォ`カスが移ったときも来るようだ。
			//DefWindowProc( hWnd, Message, wParam, lParam );
		//	break;
		case WM_IME_STARTCOMPOSITION:// IMEがONの状Bで、文字を入力した直後に来る。
		//	DefWindowProc( hWnd, Message, wParam, lParam );
			break;
		case WM_IME_CONTROL:
			break;
		case WM_IME_SELECT: // IMEをえらんだとき来るらしい。バッファを初期化とかすればいいのかな？
			break;
		case WM_IME_KEYDOWN:   // IMEのキ`コ`ド
			// ここにDefWindowProcがあると、WM_KEYDOWNを生成する
			break;
#endif
	
		default:
			// 未I理メッセ`ジをデフォルトI理v数へ渡してI理させる
			return DefWindowProc( hWnd, Message, wParam, lParam );
	}

	return 0;
}

/** 解像度モ`ドO定I理 *****分辨率模式设定处理*********************************************/
void SetResoMode(int Mode){
	ResoMode = Mode;
	// DirectDraw管理造体g体_保   初始化DIRECT_DRAW返回指针
	lpDraw = ( DIRECT_DRAW *)GlobalAllocPtr( GPTR, ( DWORD )sizeof( DIRECT_DRAW ) );
	if( lpDraw == NULL ){
		MessageBox( hWnd, "GlobalAllocPtr Error ( DIRECT_DRAW )", "_J", MB_OK | MB_ICONSTOP );
		return;
	}

	switch(Mode){
	case 0:
		//６４０＊４８０  ?640 480
		lpDraw->xSize = 640;//windows  窗口的大小 和Direct 没关系
		lpDraw->ySize = 480;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
	case 1:
		//３２０＊２４０
		lpDraw->xSize = 320;
		lpDraw->ySize = 240;
		SurfaceSizeX = 32;
		SurfaceSizeY = 24;
		break;
	case 2:
		//可
		lpDraw->xSize = 640;
		lpDraw->ySize = 480;
		SurfaceSizeX = 64;
		SurfaceSizeY = 48;
		break;
	}
}

