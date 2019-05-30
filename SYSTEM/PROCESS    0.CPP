/************************/
/*	process.c		进程重要处理事务	*/
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

/* 命令处理隐藏 *******************************************************************/
extern void kakushi_command(void);
/* 进程号 */
UINT ProcNo;
/* 子进程 */
UINT SubProcNo;

int palNo;//兄弟
int oldPalNo;

/* vZX ********************************************************************/
void Process( void )
{
	/* vZX */
	switch( ProcNo ){
	
		case PROC_OPENNING:
		
			break;
			
		case PROC_INIT: 	/* vZX */
		
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
			// afltF[hAEgJn
			fade_out_bgm();//退出GM？
			break;

		case PROC_CHAR_LOGIN:
			// obNobt@[`@X
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLoginProc();
			break;

		// OAEg
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
					// 
					// PCZbg  / /初始化/ /电脑复位
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
			RunAction();			// ANV
			StockTaskDispBuffer();	// ^XN\f[^obt@
			break;


		case PROC_BATTLE:     /* ogvZX */
		
			BattleProc();
			break;
			
#ifdef _DEBUG		
		//case PROC_OHTA_TEST:     /* ceXgvZX */
		
		//	OhtaTestProc();
		//	break;
			
		//case PROC_TAKE_TEST:     /* |eXgvZX */
		
		//	TakeTestProc();
		//	break;

		//case PROC_DWAF_TEST:	// DWAFeXg
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

/* vZXX ***********************************************************/
void ChangeProc( int procNo )
{
	// vZXL
	ProcNo = procNo;
	// TuvZX
	SubProcNo = 0;
}
/* vZXX ***********************************************************/
void ChangeProc( int procNo, int subProcNo )
{
	// vZXL
	ProcNo = procNo;
	// TuvZX
	SubProcNo = subProcNo;
}

/* Q[vZX ********************************************************************/
void GameProc( void )
{
	static int now_bgm;
	static BOOL produceFlag;	// otO

	/* TuvZX */
	switch( SubProcNo ){
	
		case 0:
#ifdef _DEBUG
			// ItC[h}bv
			if( offlineFlag )
			{
				initMap();		// }bv
			}
#endif

			//InitIme();		// hldA
			ClrImeCompositionString(); // EBhEYhldobt@[NA
			InitChat();		/* `bgA */
			// o_ij
			//ProduceNo = Rnd( 0, PRODUCE_END - 1 ) & 0xfffffffe;
			produceFlag = FALSE;	// otOnee
			EncountFlag = FALSE;	// GJEgtOnee
			//tB[haflJn
//			play_bgm(0);
			// Ao
			LoadAlbum();
			
			SubProcNo++;

		// OCo
		case 150:
			SubProcNo = 100;

		case 100:
			initPc();				// vC[L
			resetFieldProc();		// tB[hZbg
			restorePtActCharObjAll();	// Le[uANV
			initOpenServerWindow();	// T[ow\EBhE
			InitMenu();		// j[A
//			BattleStatusBak[ 0 ] = NULL;// og
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// tH[JX
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
			// o
			// obt@
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// LEp[cD
			RunAction();			// ANV
			StockTaskDispBuffer();	// ^XN\f[^obt@
			stop_bgm();				//afl~
			drawMap();				//	}bv\
			MenuProc();				// j[
			// sCtHhiFj
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// `bg
			ChatBufferToFontBuffer(); // `bgobt@[tHgobt@
			ImeProc();				// hldA
			SortDispBuffer(); 	// \obt@\[g
			// obNT[tFXNA[
			ClearBackSurface();	
			// aloobNT[tFXZbg
			PutBmp();	
			// obNT[tFXogT[tFXRs[
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// obt@
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

			play_bgm( map_bgm_no );

			// L
			NowTime = GetTickCount();
			// obNobt@[`@X
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// o
			ProduceInitFlag = TRUE;
			DrawProduce( PRODUCE_BRAN_BIG );
			SubProcNo++;
			break;

		case 103:
			// o
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_BRAN_BIG ) == TRUE )
			{
				SubProcNo = 3;
				produceFlag = TRUE;
				fieldInfoTime = GetTickCount();
			}
			break;


		// [vo
		case 200:
			// afltF[hAEgJn
//			fade_out_bgm();

			// SvgRC warpEffectProc();
			// 
			SubProcNo++;

		case 201:
			// o
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSIN ) == TRUE )
			{
				SubProcNo++;
			}
			drawField();	// tB[h\
			MenuProc();	// j[
			ImeProc();	// hldA
			break;

		case 202:
			if( !warpEffectStart || !warpEffectOk )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				drawField();	// tB[h\
				MenuProc();	// j[
				ImeProc();	// hldA
				break;
			}
			warpEffectStart = FALSE;
			warpEffectOk = FALSE;
			SubProcNo++;

		case 203:
			// o
			// obt@
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// LEp[cD
			RunAction();			// ANV
			StockTaskDispBuffer();	// ^XN\f[^obt@
			redrawMap();
			drawMap();				//	}bv\

#if 1
			// GtFNgNA
			if( (mapEffectRainLevel == 0 && oldMapEffectRainLevel != 0 )
			 || (mapEffectSnowLevel == 0 && oldMapEffectSnowLevel != 0 ) )
			{
				initMapEffect();
			}
#endif

			// sCtHhiFj
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// `bg
			ChatBufferToFontBuffer(); // `bgobt@[tHgobt@
			SortDispBuffer(); 	// \obt@\[g
			// obNT[tFXNA[
			ClearBackSurface();	
#if 1
			if( (mapEffectRainLevel != 0 && oldMapEffectRainLevel == 0 )
			 || (mapEffectSnowLevel != 0 && oldMapEffectSnowLevel == 0 ) )
			{
				mapEffectProc2( 80 );		// }bvGtFNgiJEj
			}
#endif
			// aloobNT[tFXZbg
			PutBmp();	
			// obNT[tFXogT[tFXRs[
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// obt@
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

//			if( map_bgm_no != now_bgm )
//			{
//				stop_bgm();				//afl~
//				play_bgm( map_bgm_no );
//			}

			// L
			NowTime = GetTickCount();

			ProduceInitFlag = TRUE;

			SubProcNo++;

		case 204:
			// o
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSOUT ) == TRUE )
			{
				SubProcNo = 3;
				fieldInfoTime = GetTickCount();
			}
			drawField();	// tB[h\
			MenuProc();	// j[
			ImeProc();	// hldA
			break;


		case 1:
			initPc();				// vC[L
			resetFieldProc();		// tB[hZbg
			restorePtActCharObjAll();	// Le[uANV
			initOpenServerWindow();	// T[ow\EBhE
			InitMenu();		// j[A
//			BattleStatusBak[ 0 ] = NULL;// og
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// tH[JX
			GetKeyInputFocus( &MyChatBuffer );
			
			// o
			if( produceFlag == TRUE ){
			// EBhEtOnm
			//if( BattleResultWndFlag == TRUE ){

				// o
				// obt@
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				initCharPartsPrio();	// LEp[cD
				RunAction();			// ANV
				StockTaskDispBuffer();	// ^XN\f[^obt@
				stop_bgm();				//afl~
				drawMap();				//	}bv\
				//afl
				if(!draw_map_bgm_flg){
					//afl
					play_bgm(map_bgm_no = now_bgm);
					draw_map_bgm_flg = 1;
				}
				MenuProc();				// j[
				// sCtHhiFj
				//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
				ChatProc();				// `bg
				ChatBufferToFontBuffer(); // `bgobt@[tHgobt@
				ImeProc();				// hldA
				SortDispBuffer(); 	// \obt@\[g
				// obNT[tFXNA[
				ClearBackSurface();	
				// aloobNT[tFXZbg
				PutBmp();	
				// obNT[tFXogT[tFXRs[
				lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
				// obt@
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// L
				NowTime = GetTickCount();
				// obNobt@[`@X
				BackBufferDrawType = DRAW_BACK_PRODUCE; 
				// o
				DrawProduce( PRODUCE_4WAY_IN );
			}else{
				// otOnm
				produceFlag = TRUE;
				SubProcNo++;
			}
			SubProcNo++;
			
			break;
		
		case 2:	// tB[hCo
			
			// o
			if( DrawProduce( PRODUCE_4WAY_IN ) == TRUE ){
				SubProcNo++;
				warpEffectStart = FALSE;
				warpEffectOk = FALSE;
			}
			break;
			
		case 3:
			// obNobt@[`@X
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			
			paletteProc();
			
			initItemOverlapCheck();	// ACed`FbN
			drawGrid();				// ObhJ[\\
			fieldProc();			// tB[hA
			moveProc();				// 
			charObjProc();			// L
			initCharPartsPrio();	// LEp[cD
			addressBookProc();		// AhXubN
			openServerWindowProc();	// T[oEBhE
			drawFieldInfoWin();		// [v
			
			/* ANV */
			RunAction();
			// ^XN\f[^obt@
			// wXgbN
			StockTaskDispBuffer();

			mapEffectProc();		// }bvGtFNgiJEj

			drawMap();		// }bv\
			drawField();	// tB[h\

			/* `bg */
			ChatProc();
			// `bgobt@[tHgobt@
			ChatBufferToFontBuffer(); 
			// L[{[hJ[\_
			FlashKeyboardCursor();
			// j[
			MenuProc();
			// hldA
			ImeProc();		
			// pbg`FW
			TimeZoneProc();
			
#ifdef _DEBUG
			// GJEg{^
			if( joy_trg[ 0 ] & JOY_P_DOWN ){
				if( pc.ptAct != NULL ){
					resetMap();					// }bvZbg
					// GJEgM
					lssproto_EN_send( sockfd, pc.ptAct->gx, pc.ptAct->gy );
					eventEnemyFlag = 0;
				}
			}
#endif
			// GJEg
			if( EncountFlag == TRUE ){
				resetPc();				// PCZbg
				resetCharObj();			// Le[uZbg
				resetMap();				// }bvZbg
				clearPtActPartyParam();	// ANV|C^NULL
				fieldInfoTime = 0;		// o
				drawFieldInfoWin();		// [v
				nowEncountPercentage = minEncountPercentage;// GJEg
				sendEnFlag = 0;
				encountNowFlag = 1;
				eventEnemySendFlag = 0;
				duelSendFlag = 0;
				jbSendFlag = 0;
				// }bvEBhEtOobNAbv
				if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
				// ^C}[
				ResultWndTimer = RESULT_WND_CLOSE_TIME;
				InitMenu();			// j[A
				BattleCmd[ 0 ] = NULL;		// ogR}h
//				BattleCmdBak[ 0 ] = NULL;	// ogR}h
//				BattleCmdReadPointer = BattleCmdWritePointer =0;
				BattleStatus[ 0 ] = NULL;	// og
				//BattleStatusBak[ 0 ] = NULL;// og
//				BattleStatusReadPointer = BattleStatusWritePointer =0;
				BattleTurnReceiveFlag = TRUE;	// ^[MtOnm
				// GJEg
				play_se( 215, 320, 240 );
				// afl
				now_bgm = t_music_bgm_no;
				// afl~
				stop_bgm();
				SubProcNo++;
			}
			break;
			
		case 4:	// obNobt@[opT[tFXRs[
			// ogT[tFX 
			CopyBackBuffer();
			// obNobt@[`@X
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
			
			break;
			
		case 5:	// GJEgo
		
			// o
			if( DrawProduce( PRODUCE_HAGARE_OCHI_OUT ) == TRUE ){
			//if( GameState == GAME_ENCOUNT_TO_BATTLE ){ 
				// vZX`FW
				ChangeProc( PROC_BATTLE );
			}
			MenuProc();	// j[
			ImeProc();	// hldA
			
			break;
	}
}


// [vop
void warpEffectProc( void )
{
	oldMapEffectRainLevel = mapEffectRainLevel;
	oldMapEffectSnowLevel = mapEffectSnowLevel;

	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// }bvEBhEtOobNAbv
	if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
	InitMenu2();

	fieldProc();			// tB[hA
	charObjProc();			// L
	initCharPartsPrio();	// LEp[cD
	openServerWindowProc();	// T[oEBhE

	fieldInfoTime = 0;		// o
	drawFieldInfoWin();		// [v


	/* ANV */
	RunAction();
	// ^XN\f[^obt@
	StockTaskDispBuffer();

	mapEffectProc();		// }bvGtFNgiJEj

	redrawMap();
	drawMap();		// }bv\

	/* `bg */
	ChatProc();
	// `bgobt@[tHgobt@
	ChatBufferToFontBuffer(); 
	// pbg`FW
//	TimeZoneProc();

	SortDispBuffer(); 	// \obt@\[g

	// obNT[tFXNA[
	ClearBackSurface();	
	// aloobNT[tFXZbg
	PutBmp();	


	// obNT[tFXogT[tFXRs[
	lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );

	// obt@
	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// L
	NowTime = GetTickCount();

	ProduceInitFlag = TRUE;
}


void paletteProc( void )
{
	if( palNo == -1 )
	{
		// pbg`FW
		PaletteChange( SaTimeZoneNo, 0 );
		// pbg`FW
		TimeZonePalChangeFlag = TRUE;
		palNo = -2;
	}
	else
	if( palNo >= 0 )
	{
		// pbg
		PaletteChange( palNo, 0 );// pbg`FW
		// pbg`FW
		TimeZonePalChangeFlag = FALSE;
		palNo = -2;
	}
}
