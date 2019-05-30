/************************/	
/*	gamemain.cpp 	*/
/************************/
#include "system.h"
#include <winuser.h>
#include <winbase.h>
#include "loadrealbin.h"
#include "loadsprbin.h"
#include "../oft/sys2.h"
#include "ime_sa.h"
#include "anim_tbl.h"
#include "battleMap.h"
#include "netmain.h"
#include "savedata.h"
#include "produce.h"
#include "battleProc.h"
#include "t_music.h"
#include "menu.h"
#include "handletime.h"
#include "battleMenu.h"

#include "pc.h"
#include "map.h"

#define NO_DRAW_MAX_CNT 20	// 描画しない最大フレ`ム数  绘图不最大框架
//格掉的系统 SKIP指针
#define FRAME_SKIP	0		// コマ落ちシステム ＯＮ：１　ＯＦＦ：０  格掉的系统

// ウィンドウモ`ド切り替えv数宣言 窗口模式转换函数宣言
void ChangeWindowMode( void );
// 解像度モ`ドO定I理
void SetResoMode(int Mode);

//调试模式3行 以前关闭我开启
//#ifndef _DEBUG
//#define _DEBUG	// とりあえずデバッグモ`ド  总之调试方式
//#endif

int testCnt;
static char no_wait_cnt = 0;

//---------------------------------------------------------------------------//
// グロ`バル涫x       全局变量定义                                                 //
//---------------------------------------------------------------------------//
int	GameState;					// ゲ`ムの状B 游戏的状态
double ProcTime = 16.666667;	// I理に使うrg、後は描画rg 
//处理时间，使用后绘制的时间
//double ProcTime = 33.333333;				// I理に使うrg、後は描画rg
double SystemTime = 16.666667;	// １シンクは 16 msc
//float SystemTime = 33.33333333;	// １シンクは 16 msc
double NowTime; 					// F在のrg
//绘图次数柜台
int	  NoDrawCnt = 1;			// 描画してない回数カウンタ`
//背部缓冲绘画方法
int BackBufferDrawType;			// バックバッファ`描画方法

// フレ`ムレ`ト算用  帧速率
int	  FrameRate;				// フレ`ムレ`ト  帧速率
//以每秒几张绘制计数
int	  DrawFrameCnt;				// 一秒gに何枚描画したかを
DWORD DrawFrameTime;			// 一秒gを数えるカウンタ` 1秒计数
#ifdef _DEBUG
static int palNo = 0;			// パレット番号  字体号码
static int fontNo = 0;			// フォント番号
#endif

//---------------------------------------------------------------------------//
// 概要 ：ウインドウ制御メインI理v数                                       //
// 引数 ：HWND hWnd : ウィンドウのReハンドル                               //
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
// 概要 ：ウインドウ制御メインI理v数                                       //
// 引数 ：HWND hWnd : ウィンドウのReハンドル                               //
//  概要：窗口控制主要处理函数/ // /参数：HWND hWnd:窗口的识别手柄
//---------------------------------------------------------------------------//
BOOL GameMain( void )
{
	// ゲ`ム_始I理
	if( InitGame() == FALSE ) return FALSE;
	
	// メインル`プ
	while( 1 )
	{
		// メッセ`ジル`プコ`ル  信息循环呼叫
		if( SystemTask() == FALSE ){
		
			EndGame(); // ゲ`ムK了I理
			
			return FALSE;
		}
		
		// ＳＡrgを取得
		RealTimeToSATime( &SaTime );
		
		// 表示カウンタ`初期化（コマ落ちのrも初期化するためここでする）
		DispBuffer.DispCnt = 0;
		// フォントカウンタ`初期化  字体柜台初期化
		FontCnt = 0;
		
		// ネットワ`クI理
#ifdef _DEBUG
		if( !offlineFlag )
			networkLoop();
#else
		networkLoop();
#endif
		
		// ＣＤＤＡ再生（ リピ`ト ）重复
		cdda_play( cdda_no );
		
		joy_read(); 	// キ`ボ`ド入力  键盘输入
#if 1
//		if( joy_trg[ 1 ] & JOY_PRINT_SCREEN )
		if( joy_trg[ 1 ] & JOY_F12 )
		{
			static unsigned int prePushTime = 0;

			// B射抑制（0.5秒g隔）
			if( prePushTime + 500 <= GetTickCount() )
			{
				// F在の画面をＢＭＰファイルに保存
				snapShot();
				prePushTime = GetTickCount();
			}
		}
#endif
		Process();		// タスクI理  任务处理
		
		MouseProc();	// マウスI理
		
		SortDispBuffer(); 	// 表示バッファソ`ト 表示排序缓冲区
		
		HitMouseCursor();	// マウスカ`ソルのあたり判定
		
		PaletteProc();		// パレットI理
		

#ifdef _DEBUG
		// 解像度涓
		if( joy_trg[ 1 ] & JOY_F9  && WindowMode ){
#else			
		// 解像度涓
		if( joy_trg[ 1 ] & JOY_F9  && WindowMode && LowResoCmdFlag ){
#endif
			// 演出中でなければ
			if( BackBufferDrawType != DRAW_BACK_PRODUCE ){
				
				// DirectDrawオブジェクトがあるr
				if( lpDraw != NULL ){
					// DirectDraw _放
					ReleaseDirectDraw();
					// フォントオブジェクトを抹
					DeleteObject( hFont );
					// 解像度モ`ドO定
					SetResoMode( (ResoMode + 1 ) & 1 );

					// ウィンドウモ`ドでなければ
//					if( !WindowMode ){
						// ウィンドウモ`ドフラグ涓
//						if( WindowMode == TRUE ) WindowMode = FALSE;
//						else WindowMode = TRUE;
//						WindowMode = TRUE;
//					}
					// DirectDraw 初期化
					InitDirectDraw();
					// オフスクリ`ンサ`フェスの作成 
					InitOffScreenSurface();
					// フォントオブジェクトの作成 */
					InitFont( 0 );
					// ウィンドウモ`ドなら
//					if( WindowMode ){
						// ウィンドウモ`ド切り替え
						ChangeWindowMode();
//					}

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
//					continue;
				}
			}
		}
#ifdef _DEBUG
#if 0
		// パレット涓
		if( joy_trg[ 1 ] & JOY_F8 ){	/* プラス */
			palNo++;
			if( palNo >= MAX_PAL ) palNo = 0;
			// パレットチェンジ
			PaletteChange( palNo, 300 );
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// フォント涓
		if( joy_trg[ 1 ] & JOY_F6 ){	/* プラス */
		}
		// フォント涓
		if( joy_trg[ 1 ] & JOY_F5 ){	/* プラス */
		// このアプリケ`ションの叵蕙欹佶毪蛟O定(排他レベル & フルスクリ`ン画面）
		//	if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_NORMAL ) != DD_OK )
		//	{
		//		MessageBox( hWnd, "SetCooperativeLevel Error", "_J", MB_OK | MB_ICONSTOP );
			//return FALSE;
		//	}
		}
		
		// フォント涓
		if( joy_trg[ 1 ] & JOY_F7 ){	/* プラス */
			// メ`ルデ`タのiみzみ
			//LoadMailHistory();
			// 全ウィンドウに再描画要求
		//	SendMessage( HWND_BROADCAST, WM_PAINT, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_PALETTECHANGED, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_ERASEBKGND, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_PAINTCLIPBOARD, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_PAINTICON, NULL, NULL );
		
		// このアプリケ`ションの叵蕙欹佶毪蛟O定(排他レベル & フルスクリ`ン画面）
		//if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) != DD_OK )
		//{
		//	MessageBox( hWnd, "SetCooperativeLevel Error", "_J", MB_OK | MB_ICONSTOP );
			//return FALSE;
		//}
			//MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失・筏蓼筏俊", "_J", MB_OK | MB_ICONSTOP );
			//BattleResultWndFlag = 2;
			//BattleResultWndFlag = 1;
			fontNo++;
			if( fontNo >= 3 ) fontNo = 0;
			// フォントチェンジ
			InitFont( fontNo );
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		
		extern void lssproto_MSG_recv( int fd,int aindex,char* text ,int color);
		// フォント涓
		if( joy_trg[ 1 ] & JOY_F5 ){	/* プラス */
			char moji[ 256 ];
			static cnt = 0;
			// メ`ルが届いたメッセ`ジ作成
			sprintf( moji," 9/%02d 12:15|１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０１２３４５６７８９０", cnt, cnt++ );
			// 墙欷い
			lssproto_MSG_recv( 0, 1, moji ,0 );
			// アイテム使う音
			//play_se( 212, 320, 240 );
			// メ`ルデ`タの保存
			//SaveMailHistory();
		}
#endif		
#if 0		

		// Ａパレット涓
		if( joy_auto[ 0 ] & JOY_A ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// Ｂパレット涓
		if( joy_auto[ 0 ] & JOY_B ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		
		// Ａパレット涓
		//if( joy_trg[ 0 ] & JOY_A ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// Ｂパレット涓
		//if( joy_trg[ 0 ] & JOY_B ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		
		
		// パレット涓
		//if( joy_trg[ 1 ] & JOY_CTRL_MINUS ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// Ａパレット涓
		//if( joy_trg[ 1 ] & JOY_CTRL_MINUS ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// パレット涓
		//if( joy_trg[ 1 ] & JOY_CTRL_CIRCUMFLEX ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// パレット涓
		if( joy_trg[ 1 ] & JOY_CTRL_YEN ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// パレット涓
		//if( joy_trg[ 1 ] & JOY_PRINT_SCREEN ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// パレット涓
		if( joy_trg[ 1 ] & JOY_RSHIFT ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// パレット涓
		if( joy_trg[ 1 ] & JOY_LSHIFT ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// パレット涓
		if( joy_trg[ 1 ] & JOY_TAB ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
		// パレット涓
		//if( joy_trg[ 1 ] & JOY_RETURN ){	/* プラス */
			// アイテム使う音
		//	play_se( 212, 320, 240 );
		//}
		// パレット涓
		if( joy_trg[ 1 ] & JOY_CTRL_W ){	/* プラス */
			// アイテム使う音
			play_se( 212, 320, 240 );
		}
#endif		
		
		// ショ`トカットキ`  快捷键
		if( joy_trg[ 0 ] & JOY_HOME ){
			strcpy( MyChatBuffer.buffer + MyChatBuffer.cnt, DebugKey0 );
			MyChatBuffer.cnt += strlen( DebugKey0 );
		}
		if( joy_trg[ 0 ] & JOY_END ){
			strcpy( MyChatBuffer.buffer + MyChatBuffer.cnt, DebugKey1 );
			MyChatBuffer.cnt += strlen( DebugKey1 );
		}
		if( joy_trg[ 0 ] & JOY_INS ){
			strcpy( MyChatBuffer.buffer + MyChatBuffer.cnt, DebugKey2 );
			MyChatBuffer.cnt += strlen( DebugKey2 );
		}
		
		// フレ`ムレ`トの算（外人が作成）  帧速率的计算（外国人作成
		DisplayFrameRate();
		
		// 情蟊硎
		{
			static int flag = 0;
			// PageUp 押したr情蟊硎荆ē去哎耄
			if( di_key[ DIK_PRIOR ] & 0x80 ){ 
				if( flag == 0 ) flag = 1;
				if( flag == 2 ) flag = 3;
			}else{
				if( flag == 1 ) flag = 2;
				if( flag == 3 ) flag = 0;
			}
			if( flag >= 1 ) InfoDisp();
		}
#endif	
	
#if FRAME_SKIP
		// 描画rgが残っていないときは描画しない ( すぐI理に )
		if( NowTime + ProcTime < GetTickCount() ){
			// 描画しなくてもいいrなら
			if( NoDrawCnt < NO_DRAW_MAX_CNT ){
				// システムタイムプラス（ R越 )
				NowTime += SystemTime;
				// 描画してない回数カウント
				NoDrawCnt++;
				continue;
			}
		}
#endif		
#ifdef _DEBUG		
		// F在表示しているサ`フェス数の初期化
		SurfaceDispCnt = 0;
		
		// 描画rg（v）（ デバッグ用 ）
		DrawDebugLine( 250 ); 
#endif		
		// バックバッファ`描画方法で分岐
		switch( BackBufferDrawType ){
		
			case DRAW_BACK_NON:		// 描画しない
				break;
				
			case DRAW_BACK_NORMAL:	// バックサ`フェスを\でクリア`
			
				ClearBackSurface();	
				break;
				
			case DRAW_BACK_PRODUCE:	// 演出中
				break;
				
			case DRAW_BACK_BATTLE:	// バトルマップ描画
			
				DrawBattleMap();
				break;
				
		}
		

#ifdef _DEBUG
		no_wait_cnt++;
		no_wait_cnt&=7;
		if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//ＳＨＩＦＴを押してたら
			if(!no_wait_cnt){
				PutBmp();	// ＢＭＰをバックサ`フェスにセット
			}
		} else {
			PutBmp();	// ＢＭＰをバックサ`フェスにセット
		}
#else
		
		PutBmp();	// ＢＭＰをバックサ`フェスにセット
#endif	
		
#ifdef _DEBUG
			// 文字描画rg（青）（ デバッグ用 ）
			//DrawDebugLine( 252 ); 
#endif	
		// フォントをバックサ`フェスにセット
		//PutText( FONT_PRIO_FRONT );	
		
		// ウィンドウモ`ドでないr
		//if( !WindowMode ){
			// ＢＭＰ表示バッファの初期化
		//	DispBuffer.DispCnt = 0;
			// フォントカウンタ`初期化
		//	FontCnt = 0;
			// マウスカ`ソルの表示（最先表示）
		//	StockDispBuffer( mouse.nowPoint.x + 8, mouse.nowPoint.y + 8, DISP_PRIO_TOP, 5200, 0 );
		//	PutBmp();	// ＢＭＰをバックサ`フェスにセット
		//}
		
#ifdef _DEBUG		
		
		// 余ったrg（\）（ デバッグ用 ）
		DrawDebugLine( 0 );	
#endif		
		
#ifdef _DEBUG		
		if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//ＳＨＩＦＴを押してたら
			if(!no_wait_cnt){
				Flip();	// 垂直同期rの切り替えI理
			}
		} else {
			Flip();	// 垂直同期rの切り替えI理
		}
#else
		
		Flip();	// 垂直同期rの切り替えI理
#endif		
		
#ifdef _DEBUG		
		// I理rg（赤）（ デバッグ用 ）
		DrawDebugLine( 249 ); 
#endif
		
#if FRAME_SKIP
#ifdef _DEBUG		
		if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//ＳＨＩＦＴを押してたら
			// ノ`ウエイト
			NowTime = GetTickCount();
		} else {
			// rgのずれを{整
//			while( NowTime >= GetTickCount() );
			while( NowTime >= GetTickCount() ){
				// ウィンドウモ`ドなら
				if( WindowMode ){
					Sleep(1);
				}
			}
		}
#else
		// rgのずれを{整
//		while( NowTime >= GetTickCount() );
		while( NowTime >= GetTickCount() ){
			// ウィンドウモ`ドなら
			if( WindowMode ){
				Sleep(1);
			}
		}
		
#endif
#endif

		// パレットチェンジフラグＯＮのr
		if( PalChangeFlag == TRUE ){
			// 一度_放
		//    lpDraw->lpPALETTE->Release();
			// 8BitインデックスをPalette涫浅跗诨贰⒔Y果をlpPALETTEへ返す。
		//	lpDraw->lpDD2->CreatePalette( DDPCAPS_8BIT, Palette, &lpDraw->lpPALETTE, NULL );
		
		
			// パレットの中身をO定
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			PalChangeFlag = FALSE; // フラグ初期化
		}
		
		// システムタイムプラス
		NowTime += SystemTime;
		// 描画してない回数初期化
		NoDrawCnt = 1;
		
#ifdef _DEBUG		
		// 一秒gに描画した枚数をカウント
    	DrawFrameCnt++;
#endif
		// サ`フェスを使用した日付を更新
		SurfaceDate++;
	}

	return FALSE;
}

#ifdef _DEBUG		
// 情蟊硎 *******************************************************************/
void InfoDisp( void )
{
	char szMoji[ 256 ];
	int x = 16;
	int y = 16;
	char c = 0;
	
	sprintf( szMoji,"ActionCnt        = %d", ActCnt );
	// フォント情螭颔啸氓榨・肆铯幛
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"DispCnt          = %d", DispBuffer.DispCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceCnt       = %d",SurfaceCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"VramSurfaceCnt   = %d",VramSurfaceCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SysramSurfaceCnt = %d",SysramSurfaceCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceUseCnt    = %d",SurfaceUseCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"SearchPoint      = %d",SurfaceSearchPoint );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceDispCnt   = %d",SurfaceDispCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceDate      = %d",SurfaceDate );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;c = 0;
	
	sprintf( szMoji,"FrameRate        = %d",FrameRate );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"HitDispNo        = %d",HitDispNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"HitFontNo        = %d",HitFontNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"MouseLevel       = %d",mouse.level );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"ProcNo           = %d",ProcNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"SubProcNo        = %d",SubProcNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"ActionSize       = %d",sizeof( ACTION ) );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"MouseX           = %d",mouse.nowPoint.x );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"MouseY           = %d",mouse.nowPoint.y );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"TimeZone         = %d",SaTime.hour );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16; c = 0;
	
	sprintf( szMoji,"PalNo            = %d",PalState.palNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"BattleMapNo      = %d",BattleMapNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"HogeCnt          = %d",testCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"EventEnemyFlag   = %d",eventEnemyFlag );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 0 ] = %d",pc.selectPetNo[ 0 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 1 ] = %d",pc.selectPetNo[ 1 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 2 ] = %d",pc.selectPetNo[ 2 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 3 ] = %d",pc.selectPetNo[ 3 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 4 ] = %d",pc.selectPetNo[ 4 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"BattlePetStMenCnt= %d",BattlePetStMenCnt );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"BattlePetReceiveFlag= %d",BattlePetReceiveFlag );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

}
#endif		

// フレ`ムレ`トの算（外人が作成）
void DisplayFrameRate( void )
{
	// 一秒たったら更新
    if( GetTickCount() - DrawFrameTime >= 1000 ){
	
		// フレ`ムレイトを
        FrameRate = DrawFrameCnt;
		// 今のrgを
        DrawFrameTime = GetTickCount();
		// 一秒gに描画した枚数を初期化
        DrawFrameCnt = 0;
    }
}

// ロゴの表示 ***标志的表示**************************************************************/
void PutLogo( void )
{
	
	// ＢＭＰ表示バッファの初期化
	DispBuffer.DispCnt = 0;
	StockDispBuffer( 320, 240, DISP_PRIO_TOP, CG_TITLE_NOW_LOADING, 0 );
	ClearBackSurface();	// バックサ`フェスを\でクリア`  清理接口开始下次位图
	PutBmp();			// ＢＭＰをバックサ`フェスにセット 设置位图
	Flip();				// 垂直同期rの切り替えI理  垂直线时的转换处理
}	

// ゲ`ム_始I理 ////游戏开始处理//////////////////////////////////////////////////////////
BOOL InitGame( void )
{
	
	// DirectDraw 初期化  这里不熟悉以后学习里面有个可以不用16位色调换系统不用
	if( InitDirectDraw() == FALSE ){
		MessageBox( hWnd, "ＤｉｒｅｃｔＤｒａｗの初期化に失・筏蓼筏俊", "_J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	/* オフスクリ`ンサ`フェスの作成  初始化 表面接口为显示做准备*/
	if( InitOffScreenSurface() == FALSE ){
		MessageBox( hWnd, "オフスクリ`ンサ`フェイスの作成に失・筏蓼筏俊", "_J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}

	// パレット初期化 调色板重点要学习完 拿来用
	if( InitPalette() == FALSE ) return FALSE;
	
	//_asm{int 3}
	// Realbin vBの初期化  重点了 这里到了地图图片数据文件
	if( initRealbinFileOpen( realBinName, adrnBinName ) == FALSE ) {
	//if( initRealbinFileOpen( "..\\real.bin", "..\\adrn.bin" ) == FALSE ) {
		MessageBox( hWnd, "Real.bin のオ`プンに失・筏蓼筏俊", "_J", MB_OK | MB_ICONSTOP );
		//MessageBox( hWnd, "ＳａＵｐｄａｔｅ.ｅｘｅから起婴筏皮坤丹ぁ", "_J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	// ロゴの表示
	PutLogo();
	
	// オ`トマップ初期化I理
	initAutoMapColor( adrnBinName );

	// SprBin vBの初期化
	if( InitSprBinFileOpen( sprBinName, sprAdrnBinName ) == FALSE ){
		MessageBox( hWnd, "Spr.bin のオ`プンに失・筏蓼筏俊", "_J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}

	/* アクション初期化 */
	InitAction();
		
	// フォントオブジェクトの作成 */
	InitFont( 0 );
	//InitFont( 2 );
	
	// 乱数系列初期化
	srand( ( unsigned int )GetTickCount() );
	initRand2();
	
	// DirectSound 初期化
	t_music_init();

	// ダイレクトインプット初期化
	InitDInput();
	
	// ＣＤＤＡ再生
	//cdda_play( Rnd( 2, 34 ) );
	
	// ＩＭＥvBの初期化
	InitIme();
	
	// マウスI理の初期化
	MouseInit();

	// CDキ`、パスワ`ドiみzみ
	if( loadUserSetting() == FALSE )
	{
		char msg[1024];
		sprintf( msg, SAVE_ERRMSG_loadNowState, savedataErrorCode );
		MessageBox( hWnd, msg, "_J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	// チャットの登h文字のiみzみ  聊天的注册文字的读取
	LoadChatRegistyStr();				
	
	// F在のrgを
	NowTime = GetTickCount();
	DrawFrameTime = GetTickCount();

// デバッグ版でないとき	
#ifndef _DEBUG
	// マウスカ`ソルをLす
	ShowCursor( FALSE );
#endif	
	return TRUE;
}	

// ゲ`ムK了I理 //////////////////////////////////////////////////////////////
void EndGame( void )
{
	//DirectDraw _放
	ReleaseDirectDraw();
	// ダイレクトインプット_放
	EndDInput();
	// DirectSound _放
	t_music_end();
	// ＣＤＤＡ停止
	cdda_stop();
	// Rialbin ]じる
	cleanupRealbin();
	// フォントオブジェクトを抹
	DeleteObject( hFont );
	// ＩＭＥのK了
	EndIme();
	// アクションK了I理
	EndAction();
	// ネットワ`クK了I理
	cleanupNetwork();
	// 同r起鹰隶Д氓芝弗Д乳_放
	ReleaseMutex( hMutex );
	// パラメ`タセ`ブ
	saveUserSetting();
	// チャットの登h文字の保存
	SaveChatRegistyStr();
	
	// タイマ`K了
//	timeKillEvent(timerId);
	
	//{
		// ALT+TAB 有郡摔工
	//	int nOldVal;
	//	SystemParametersInfo (SPI_SCREENSAVERRUNNING, FALSE, &nOldVal, 0);
	//}
}

