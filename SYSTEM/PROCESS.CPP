/************************/
/*	process.c	进程重要处理事务	*/
/************************/
#include "system.h"
#include "../ohta/ohta.h"
#include "../dwaf/dwafTestProc.h"
#include "ime_sa.h"
#include "menu.h"
#include "pc.h"
#include "character.h"
#include "login.h"
#include "netproc.h"
#include "savedata.h"
#include "testView.h"
#include "battleProc.h"
#include "produce.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "battleMenu.h"
#include "t_music.h"
#include "field.h"
#include "handletime.h"

/* 命令处理隐藏*******************************************************************/
extern void kakushi_command(void);
/* 进程号 */
UINT ProcNo;
/* 子进程 */
UINT SubProcNo;

int palNo;//兄弟
int oldPalNo;

/* プロセスI理 ********************************************************************/
void Process( void )
{
	/* プロセス分岐 */
	switch( ProcNo ){
	
		case PROC_OPENNING:
		
			break;
			
		case PROC_INIT: 	/* 初期化プロセス */
		
			InitProc();
			
			break;
			

	// 身份证，密码处理
		case PROC_ID_PASSWORD:
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			idPasswordProc();
			RunAction();			// 执行动作
			StockTaskDispBuffer();	// 显示数据缓冲区的任务
			FlashKeyboardCursor();	// 处理光标闪烁键盘/ /
			ImeProc();				// 输入法有关的处理
			break;

		// 标题和菜单选择
		case PROC_TITLE_MENU:
			// /改变了退缓冲区
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			titleProc();
			TitleProduce();			// 职衔/ 
			RunAction();			// /执行动作
			StockTaskDispBuffer();	// 显示数据缓冲区的任务
			kakushi_command();		//隐藏的命令处理
			break;

		// 人物选择
		case PROC_CHAR_SELECT:		// 人物选择
			// 更改后的渲染处理
			BackBufferDrawType = DRAW_BACK_NORMAL;// 标准
			selectCharacterProc();
			break;



		// 创建人物
		case PROC_CHAR_MAKE:
			// 渲染处理
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			makeCharacterProc();
			break;

			
		// 开始登陆选定的人物
		case PROC_CHAR_LOGIN_START:
			// 转换人物
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			// charLoginStart();因此如何从网络上的数据备份缓冲区
			//初始化以保持其
			initMap();		// 地图
			initPcAll();	// PC的初始化信息（清除
			initCharObj();	// 初始化人物管理表
			initOpenServerWindow();	// 初始化的窗口中显示服务器的线路
			initFieldProc();		// 窗口初始化
			initMapEffect();		// 地图初始化的影响
			EncountFlag = FALSE;
			logOutFlag = FALSE;
			InitOhtaParam();	// 其他参数


			
			ChangeProc( PROC_CHAR_LOGIN );
			// ＢＧＭフェ`ドアウト_始
			fade_out_bgm();//退出GM？
			break;

		case PROC_CHAR_LOGIN:
			// バックバッファ`描画方法涓
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLoginProc();
			break;

		// ログアウト
		case PROC_CHAR_LOGOUT:
			// 日志输出
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLogoutProc();
			break;


		case PROC_GAME:     /*进程的游戏 */
			
			GameProc();
			break;

		case PROC_DISCONNECT_SERVER://断开游戏？
			switch( SubProcNo )
			{
				case 0:
					// 初期化
					// PCリセット  / /初始化/ /电脑复位
					resetPc();
					// 人物管理表
					initCharObj();
					// 取消动作
					DeathAllAction();


					//  /初始化生产
					ProduceInitFlag = TRUE;
					// 复制背景创建图像
					CopyBackBuffer();
					// 渲染方法
					BackBufferDrawType = DRAW_BACK_PRODUCE; 
					// 背景音乐
					fade_out_bgm();
					// 记录当前时间

					NowTime = GetTickCount();
					SubProcNo++;

					break;

				case 1:
					// 生成
					if( DrawProduce( PRODUCE_DOWN_ACCELE ) == TRUE )
					{
						BackBufferDrawType = DRAW_BACK_NORMAL; 
						SubProcNo++;
					}
					break;

				case 2:
					// 结束
					break;
			}
			// 断开警告Windows 窗口
			if( disconnectServer() )
			{
				// 初始化网络
				cleanupNetwork();
				// 恢复默认调色板
				PaletteChange( DEF_PAL, 0 );
				// 到标题画面
				ChangeProc( PROC_TITLE_MENU );
				// /消除行动
				DeathAllAction();
				disconnectServerFlag = FALSE;
				oldDisconnectServerFlag = FALSE;
				break;
			}
			RunAction();			// アクション走らせる  动作运行
			StockTaskDispBuffer();	// タスク表示デ`タをバッファに溜める  显示数据任务缓冲蓄
			break;


		case PROC_BATTLE:     /* バトルプロセス */
		
			BattleProc();
			break;
			
#ifdef _DEBUG		
		//case PROC_OHTA_TEST:     /* 太田テストプロセス */
		
		//	OhtaTestProc();
		//	break;
			
		//case PROC_TAKE_TEST:     /* 竹内さんテストプロセス */
		
		//	TakeTestProc();
		//	break;

		//case PROC_DWAF_TEST:	// DWAFテスト
		//	dwafTestProc();
		//	break;

		case PROC_SPR_VIEW:		// 初始验证
			SprViewProc();
			break;
			
		case PROC_ANIM_VIEW:	// 动画验证
			AnimViewProc();
			break;
			
		case PROC_SE_TEST:	//  SE的审查过程
			SeTestProc();
			break;
#endif
		case PROC_ENDING:
			
			break;
			
	}
}

/* プロセス涓I理 ***********************************************************/
void ChangeProc( int procNo )
{
	// プロセス番号を
	ProcNo = procNo;
	// サブプロセス番号の初期化
	SubProcNo = 0;
}
/* プロセス涓I理 ***********************************************************/
void ChangeProc( int procNo, int subProcNo )
{
	// プロセス番号を
	ProcNo = procNo;
	// サブプロセス番号の初期化
	SubProcNo = subProcNo;
}

/* ゲ`ムプロセスI理 ********************************************************************/
void GameProc( void )
{
	static int now_bgm;
	static BOOL produceFlag;	// 演出フラグ

	/* サブプロセス番号で分岐 */
	switch( SubProcNo ){
	
		case 0:
#ifdef _DEBUG
			// オフラインモ`ドのrのマップ初期化
			if( offlineFlag )
			{
				initMap();		// マップ初期化
			}
#endif

			//InitIme();		// ＩＭＥvBの初期化
			ClrImeCompositionString(); // ウィンドウズのＩＭＥバッファ`をクリアする
			InitChat();		/* チャットvB初期化 */
			// 演出番号ランダム（偶数のみ）
			//ProduceNo = Rnd( 0, PRODUCE_END - 1 ) & 0xfffffffe;
			produceFlag = FALSE;	// 演出フラグＯＦＦ
			EncountFlag = FALSE;	// エンカウントフラグＯＦＦ
			//フィ`ルドＢＧＭ再生_始
//			play_bgm(0);
			// アルバムのiみzみ  专辑的读取  相簿
			LoadAlbum();
			
			SubProcNo++;

		// ログイン演出
		case 150:
			SubProcNo = 100;

		case 100:
			initPc();				// プレイヤ`キャラ作成
			resetFieldProc();		// フィ`ルドI理のリセット
			restorePtActCharObjAll();	// キャラ管理テ`ブルのアクション活
			initOpenServerWindow();	// サ`バの指示で表示するウィンドウの初期化
			InitMenu();		// メニュ`vBの初期化
//			BattleStatusBak[ 0 ] = NULL;// バトル状B初期化
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// 入力フォ`カス取得
			GetKeyInputFocus( &MyChatBuffer );
			SubProcNo++;

		case 101:
			if( loginFlag )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				break;
			}
			SubProcNo++;

		case 102:
			// 演出画像の作成
			// バッファ初期化
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// キャラ?パ`ツの先位Q定I理の初期化
			RunAction();			// アクション走らせる
			StockTaskDispBuffer();	// タスク表示デ`タをバッファに溜める
			stop_bgm();				//ＢＧＭ停止
			drawMap();				//	マップ表示
			MenuProc();				// メニュ`I理
			// 一行インフォTりつぶし（\色）
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// チャットI理
			//聊天缓冲字体缓冲蓄
			ChatBufferToFontBuffer(); // チャットバッファ`をフォントバッファに溜める
			ImeProc();				// ＩＭＥvBI理
			SortDispBuffer(); 	// 表示バッファソ`ト
			// バックサ`フェスを\でクリア`
			ClearBackSurface();	
			// ＢＭＰをバックサ`フェスにセット
			PutBmp();	
			// バックサ`フェスからバトルサ`フェスへコピ`
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// バッファ初期化
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

		//	_asm{int 3}  这个 开启就出错 我关闭
		//	play_bgm( map_bgm_no );

			// F在のrgを
			NowTime = GetTickCount();
			// バックバッファ`描画方法涓
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// 演出中
			ProduceInitFlag = TRUE;
			DrawProduce( PRODUCE_BRAN_BIG );
			SubProcNo++;
			break;

		case 103:
			// 演出中
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_BRAN_BIG ) == TRUE )
			{
				SubProcNo = 3;
				produceFlag = TRUE;
				fieldInfoTime = GetTickCount();
			}
			break;


		// ワ`プ演出
		case 200:
			// ＢＧＭフェ`ドアウト_始
//			fade_out_bgm();

			// SプロトコルのCで warpEffectProc();が呼ばれて
			// 画面を作るのでここではしなくて良い
			SubProcNo++;

		case 201:
			// 演出中
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSIN ) == TRUE )
			{
				SubProcNo++;
			}
			drawField();	// フィ`ルド画面にいろいろ表示
			MenuProc();	// メニュ`I理
			ImeProc();	// ＩＭＥvBI理
			break;

		case 202:
			if( !warpEffectStart || !warpEffectOk )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				drawField();	// フィ`ルド画面にいろいろ表示
				MenuProc();	// メニュ`I理
				ImeProc();	// ＩＭＥvBI理
				break;
			}
			warpEffectStart = FALSE;
			warpEffectOk = FALSE;
			SubProcNo++;

		case 203:
			// 演出画像の作成
			// バッファ初期化  /演出图像的作成/ /缓冲初期化
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// キャラ?パ`ツの先位Q定I理の初期化
			RunAction();			// アクション走らせる
			StockTaskDispBuffer();	// タスク表示デ`タをバッファに溜める
			redrawMap();
			drawMap();				//	マップ表示

#if 1
			// エフェクトのクリア
			if( (mapEffectRainLevel == 0 && oldMapEffectRainLevel != 0 )
			 || (mapEffectSnowLevel == 0 && oldMapEffectSnowLevel != 0 ) )
			{
				initMapEffect();
			}
#endif

			// 一行インフォTりつぶし（\色）
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// チャットI理
			ChatBufferToFontBuffer(); // チャットバッファ`をフォントバッファに溜める
			SortDispBuffer(); 	// 表示バッファソ`ト
			// バックサ`フェスを\でクリア`
			ClearBackSurface();	
#if 1
			if( (mapEffectRainLevel != 0 && oldMapEffectRainLevel == 0 )
			 || (mapEffectSnowLevel != 0 && oldMapEffectSnowLevel == 0 ) )
			{
				mapEffectProc2( 80 );		// マップエフェクト（雨?雪等）
			}
#endif
			// ＢＭＰをバックサ`フェスにセット
			PutBmp();	
			// バックサ`フェスからバトルサ`フェスへコピ`
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// バッファ初期化
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

//			if( map_bgm_no != now_bgm )
//			{
//				stop_bgm();				//ＢＧＭ停止
//				play_bgm( map_bgm_no );
//			}

			// F在のrgを
			NowTime = GetTickCount();

			ProduceInitFlag = TRUE;

			SubProcNo++;

		case 204:
			// 演出中
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSOUT ) == TRUE )
			{
				SubProcNo = 3;
				fieldInfoTime = GetTickCount();
			}
			drawField();	// フィ`ルド画面にいろいろ表示
			MenuProc();	// メニュ`I理
			ImeProc();	// ＩＭＥvBI理
			break;


		case 1:
			initPc();				// プレイヤ`キャラ作成
			resetFieldProc();		// フィ`ルドI理のリセット
			restorePtActCharObjAll();	// キャラ管理テ`ブルのアクション活
			initOpenServerWindow();	// サ`バの指示で表示するウィンドウの初期化
			InitMenu();		// メニュ`vBの初期化
//			BattleStatusBak[ 0 ] = NULL;// バトル状B初期化
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// 入力フォ`カス取得
			GetKeyInputFocus( &MyChatBuffer );
			
			// 演出するr
			if( produceFlag == TRUE ){
			// 殛LY果ウィンドウフラグＯＮ
			//if( BattleResultWndFlag == TRUE ){

				// 演出画像の作成
				// バッファ初期化
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				initCharPartsPrio();	// キャラ?パ`ツの先位Q定I理の初期化
				RunAction();			// アクション走らせる
				StockTaskDispBuffer();	// タスク表示デ`タをバッファに溜める
				stop_bgm();				//ＢＧＭ停止
				drawMap();				//	マップ表示
				//ＢＧＭがなければ
				if(!draw_map_bgm_flg){
					//さっきのＢＧＭ再生
					play_bgm(map_bgm_no = now_bgm);
					draw_map_bgm_flg = 1;
				}
				MenuProc();				// メニュ`I理
				// 一行インフォTりつぶし（\色）
				//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
				ChatProc();				// チャットI理
				ChatBufferToFontBuffer(); // チャットバッファ`をフォントバッファに溜める
				ImeProc();				// ＩＭＥvBI理
				SortDispBuffer(); 	// 表示バッファソ`ト
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
				// バックバッファ`描画方法涓
				BackBufferDrawType = DRAW_BACK_PRODUCE; 
				// 演出中
				DrawProduce( PRODUCE_4WAY_IN );
			}else{
				// 演出フラグＯＮ
				produceFlag = TRUE;
				SubProcNo++;
			}
			SubProcNo++;
			
			break;
		
		case 2:	// フィ`ルドイン演出
			
			// 演出中
			if( DrawProduce( PRODUCE_4WAY_IN ) == TRUE ){
				SubProcNo++;
				warpEffectStart = FALSE;
				warpEffectOk = FALSE;
			}
			break;
			
		case 3:
			// バックバッファ`描画方法涓
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			
			paletteProc();
			
			initItemOverlapCheck();	// アイテム重なりチェックI理の初期化
			drawGrid();				// グリッドカ`ソル表示
			fieldProc();			// フィ`ルドvBI理
			moveProc();				// 移I理
			charObjProc();			// キャラ管理I理
			initCharPartsPrio();	// キャラ?パ`ツの先位Q定I理の初期化
			addressBookProc();		// アドレスブックI理
			openServerWindowProc();	// サ`バ制ウィンドウ
			drawFieldInfoWin();		// ワ`プ後の鏊
			
			/* アクション走らせる */
			RunAction();
			// タスク表示デ`タをバッファに溜める
			// 指定欷膝攻去氓筏胜
			StockTaskDispBuffer();

			mapEffectProc();		// マップエフェクト（雨?雪等）

			drawMap();		// マップ表示
			drawField();	// フィ`ルド画面にいろいろ表示

			/* チャットI理 */
			ChatProc();
			// チャットバッファ`をフォントバッファに溜める
			ChatBufferToFontBuffer(); 
			// キ`ボ`ドカ`ソル点I理
			
			FlashKeyboardCursor();
			// メニュ`I理
			MenuProc();
			// ＩＭＥvBI理
			ImeProc();		
			// rgI理とパレットチェンジI理
			TimeZoneProc();
			
#ifdef _DEBUG
			// エンカウントボタン
			if( joy_trg[ 0 ] & JOY_P_DOWN ){
				if( pc.ptAct != NULL ){
					resetMap();					// マップI理リセット
					// エンカウント送信
					lssproto_EN_send( sockfd, pc.ptAct->gx, pc.ptAct->gy );
					eventEnemyFlag = 0;
				}
			}
#endif
			// エンカウントしたr  遇敌的时候
			if( EncountFlag == TRUE ){
				resetPc();				// PCリセット
				resetCharObj();			// キャラ管理テ`ブルリセット
				resetMap();				// マップI理リセット
				clearPtActPartyParam();	// 仲g情螭违伐绁螗荪ぅ螗郡坤堡NULLにする
				fieldInfoTime = 0;		// 鏊螭訾皮郡橄
				drawFieldInfoWin();		// ワ`プ後の鏊
				nowEncountPercentage = minEncountPercentage;// エンカウント率を最小に
				sendEnFlag = 0;
				encountNowFlag = 1;
				eventEnemySendFlag = 0;
				duelSendFlag = 0;
				jbSendFlag = 0;
				// マップウィンドウフラグバックアップ
				if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
				// 消えるまでのrgタイマ`
				ResultWndTimer = RESULT_WND_CLOSE_TIME;
				InitMenu();			// メニュ`vBの初期化
				BattleCmd[ 0 ] = NULL;		// バトルコマンド初期化
//				BattleCmdBak[ 0 ] = NULL;	// バトルコマンド初期化
//				BattleCmdReadPointer = BattleCmdWritePointer =0;
				BattleStatus[ 0 ] = NULL;	// バトル状B初期化
				//BattleStatusBak[ 0 ] = NULL;// バトル状B初期化
//				BattleStatusReadPointer = BattleStatusWritePointer =0;
				BattleTurnReceiveFlag = TRUE;	// タ`ン受信フラグＯＮ
				// エンカウント音
				play_se( 215, 320, 240 );
				// F在のＢＧＭ保存
				now_bgm = t_music_bgm_no;
				// ＢＧＭ停止
				stop_bgm();
				SubProcNo++;
			}
			break;
			
		case 4:	// バックバッファ`を演出用サ`フェスにコピ`
			// バトルサ`フェスの画像作成 
			CopyBackBuffer();
			// バックバッファ`描画方法涓
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
			
			break;
			
		case 5:	// エンカウント演出
		
			// 演出中
			if( DrawProduce( PRODUCE_HAGARE_OCHI_OUT ) == TRUE ){
			//if( GameState == GAME_ENCOUNT_TO_BATTLE ){ 
				// プロセスチェンジ
				ChangeProc( PROC_BATTLE );
			}
			MenuProc();	// メニュ`I理
			ImeProc();	// ＩＭＥvBI理
			
			break;
	}
}


// ワ`プ演出用
void warpEffectProc( void )
{
	oldMapEffectRainLevel = mapEffectRainLevel;
	oldMapEffectSnowLevel = mapEffectSnowLevel;

	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// マップウィンドウフラグバックアップ
	if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
	InitMenu2();

	fieldProc();			// フィ`ルドvBI理
	charObjProc();			// キャラ管理I理
	initCharPartsPrio();	// キャラ?パ`ツの先位Q定I理の初期化
	openServerWindowProc();	// サ`バ制ウィンドウ

	fieldInfoTime = 0;		// 鏊螭訾皮郡橄
	drawFieldInfoWin();		// ワ`プ後の鏊


	/* アクション走らせる */
	RunAction();
	// タスク表示デ`タをバッファに溜める
	StockTaskDispBuffer();

	mapEffectProc();		// マップエフェクト（雨?雪等）

	redrawMap();
	drawMap();		// マップ表示

	/* チャットI理 */
	ChatProc();
	// チャットバッファ`をフォントバッファに溜める
	ChatBufferToFontBuffer(); 
	// rgI理とパレットチェンジI理
//	TimeZoneProc();

	SortDispBuffer(); 	// 表示バッファソ`ト

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

	ProduceInitFlag = TRUE;
}


void paletteProc( void )
{
	if( palNo == -1 )
	{
		// パレットチェンジ
		PaletteChange( SaTimeZoneNo, 0 );
		// rgでパレットチェンジするr
		TimeZonePalChangeFlag = TRUE;
		palNo = -2;
	}
	else
	if( palNo >= 0 )
	{
		// 固定パレットO定
		PaletteChange( palNo, 0 );// パレットチェンジ
		// rgでパレットチェンジしないr
		TimeZonePalChangeFlag = FALSE;
		palNo = -2;
	}
}

