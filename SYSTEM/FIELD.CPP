#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<direct.h>
#include<math.h>
//战斗地图


#include"system.h"
#include"loadrealbin.h"
#include"loadsprbin.h"
#include"anim_tbl.h"
#include"login.h"
#include"menu.h"
#include"map.h"
#include"lssproto_cli.h"
#include"pc.h"
#include"netmain.h"
#include"handletime.h"
#include"character.h"
#include"ime_sa.h"
#include "t_music.h"
#include "netproc.h"

#if 0

#include"netproc.h"
#include"character.h"
#include "battleProc.h"

#endif

#include "field.h"

// パネル?ボタン全体
short drawFieldButtonFlag = 1;
int fieldBtnHitId[FIELD_FUNC_END];	// マウスとの当たり判定用
short drawTimeAnimeFlag = 0;


// チャットボタン
short menuBtn = 0;
short menuBtnFocus = 0;
int menuBtnGraNo[] =
{
	CG_FIELD_MENU_BTN_OFF,
	CG_FIELD_MENU_BTN_ON
};

// 名刺ボタン
short cardBtn = 0;
short cardBtnFocus = 0;
int cardBtnGraNo[] =
{
	CG_FIELD_CARD_BTN_OFF,
	CG_FIELD_CARD_BTN_ON
};

// 仲gボタン
short partyBtn = 0;
short partyBtnFocus = 0;
short partyBtnEnableFlag = 1;
int partyBtnGraNo[] =
{
	CG_FIELD_GROUP_BTN_OFF,
	CG_FIELD_GROUP_BTN_ON
};

// メ`ルランプ
short mailLamp = 0;
short mailLampDrawFlag = 0;
unsigned int mailLampFlashTime = 0;

// 参楗堀骏
short joinBattleBtn = 0;
short joinBattleBtnFocus = 0;
short joinBattleBtnEnableFlag = 1;
short joinBattleHelpMsgFlag = 1;
int joinBattleBtnGraNo[] =
{
	CG_FIELD_JOIN_BTL_BTN_OFF,
	CG_FIELD_JOIN_BTL_BTN_ON
};

// 楗堀骏
short duelBtn = 0;
short duelBtnFocus = 0;
int duelBtnGraNo[] =
{
	CG_FIELD_DUEL_BTN_OFF,
	CG_FIELD_DUEL_BTN_ON
};

// アクションボタン
short actBtn = 0;
short actBtnForcus = 0;
int actBtnUpTime;
int nowActMode;
int actBtnGraNo[] =
{
	CG_FIELD_ACT_BTN_OFF,
	CG_FIELD_ACT_BTN_ON
};
short charActionAnimeChangeProcNo;
short charActionAnimeChangeWindowFlag;


// 仲g?参?椋希衰攻ぅ氓隶榨楗
short etcSwitchProcNo;
short etcSwitchChangeFlag = 0;


// 朝?夜アニメ
int amPmAnimeTime;
int amPmAnimeX;
int amPmAnimeGraNoIndex0, amPmAnimeGraNoIndex1;
int amPmAnimeGraNo[] =
{
	CG_FIELD_AM_PM_01,
	CG_FIELD_AM_PM_02,
	CG_FIELD_AM_PM_03,
	CG_FIELD_AM_PM_00
};


// サ`バへ何かのプロトコルを送った
short etcSendFlag = 0;

// メニュ`vB
short nowFieldMenuOpenFlag = 0;		// メニュ`がこのフレ`ムに_いているか


// フィ`ルドボタンを押してプロトコルを送ったrg
unsigned int fieldBtnPushTime = 0;

// ワ`プrにF在の位置を表示
unsigned int fieldInfoTime = 0;


// プロトタイプ
void initCharActionAnimeChange( void );
int charActionAnimeChange( void );
void initEtcSwitch( void );
int  etcSwitch( void );



struct tm *serverAliveTime;
time_t serverAliveLongTime;



// フィ`ルド上でのI理の初期化
void initFieldProc( void )
{
	int i;

	drawFieldButtonFlag = 1;

	menuBtn = 0;
	cardBtn = 0;
	partyBtn = 0;
// initPcAll()で味iを{べてO定してるのでここでは何もしない
//	mailLamp = 0;

	mailLampDrawFlag = 0;
	mailLampFlashTime = 0;

	joinBattleBtn = 0;
	duelBtn = 0;
	actBtn = 0;
	actBtnForcus = 0;

	nowActMode = 0;

#if 0
	drawTimeAnimeFlag = 0;
#else
	drawTimeAnimeFlag = 1;
#endif

	for( i = 0; i < FIELD_FUNC_END; i++ )
	{
		fieldBtnHitId[i] = -2;
	}

	etcSendFlag = 0;
	etcSwitchChangeFlag = 0;

	fieldInfoTime = 0;

	partyBtnEnableFlag = 1;
	joinBattleBtnEnableFlag = 1;
	joinBattleHelpMsgFlag = 1;
}


// フィ`ルドI理のリセット
void resetFieldProc( void )
{
	actBtn = 0;
	menuBtn = 0;
	fieldInfoTime = 0;
}


// フィ`ルド上でのI理
void fieldProc( void )
{
	int selId;
	int pushId;
	int focusId;
	BOOL walkFlag = FALSE;

	// 乱数かき回す
	rand2();

#ifdef _DEBUG
#if 0
	if( joy_trg[1] & JOY_CTRL_YEN )
	{
		int i;
		int graNo;
		static mode = 0;
		int dx, dy;
		short dir;

		graNo = -1;
		for( i = 0; i < MAX_PET; i++ )
		{
			if( pet[i].useFlag )
			{
				graNo = pet[i].graNo;
				break;
			}
		}
		if( graNo >= 0 && pc.ptAct != NULL )
		{
			if( mode == 0 )
			{
				createPetAction( graNo, nowGx, nowGy, pc.ptAct->anim_ang, 0, (rand() % 8), -1 );
			}
			else
			if( mode == 1 )
			{
				getRouteData( pc.ptAct->anim_ang, &dx, &dy );
				dir = pc.ptAct->anim_ang+4;
				ajustClientDir( &dir );
				createPetAction( graNo, nowGx+dx, nowGy+dy, dir, 1, 0, -1 );
			}
			else
			if( mode == 2 )
			{
				getRouteData( pc.ptAct->anim_ang, &dx, &dy );
				dir = pc.ptAct->anim_ang+4;
				ajustClientDir( &dir );
				createPetAction( graNo, nowGx+dx, nowGy+dy, dir, 2, 0, -1 );
			}
			else
			if( mode == 3 )
			{
				getRouteData( pc.ptAct->anim_ang, &dx, &dy );
				dir = pc.ptAct->anim_ang+4;
				ajustClientDir( &dir );
				createPetAction( graNo, nowGx+dx, nowGy+dy, dir, 3, 0, -1 );
			}
			mode++;
			if( mode > 3 )
				mode = 0;
		}
	}
#endif
#endif


	// アクション涓违伐绌`トカットキ`のI理
	actionShortCutKeyProc();


	// メニュ`のフラグを浃à瑜Δ趣筏七B射抑制に引っかかったので
	// 待ちrgが^ぎてから送る
	if( etcSwitchChangeFlag
	 && eventWarpSendFlag == 0
	 && eventEnemySendFlag == 0
	 && sendEnFlag == 0 )
	{
		if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// B射抑制
		{
			lssproto_FS_send( sockfd, pc.etcFlag );
			fieldBtnPushTime = GetTickCount();
			etcSwitchChangeFlag = 0;
		}
	}


	// 他のメニュ`が出てるときは表示しない
	if( CheckMenuFlag()
	 || windowTypeWN > -1
	 || actBtn == 1
	 || menuBtn == 1 )
	{
		drawFieldButtonFlag = 0;
	}
	else
	{
		drawFieldButtonFlag = 1;
	}



	// 今フィ`ルドI理でなんかのウィンドウ出してるかチェック
	nowFieldMenuOpenFlag = 0;
	// サ`バリクエスト氓Ε％螗丧Δ蜷_いてる
	if( windowTypeWN != -1 )
	{
		nowFieldMenuOpenFlag = 1;
	}
	// アクション涓Ε％螗丧Δ蜷_いてる
	if( actBtn == 1 )
	{
		nowFieldMenuOpenFlag = 1;
	}
	// 仲g?参?椋希衰攻ぅ氓隶Ε％螗丧Δ蜷_いてる
	if( menuBtn == 1 )
	{
		nowFieldMenuOpenFlag = 1;
	}



	// ＳＡrgによる朝?昼?夜アニメ`ションI理（ＳＡrg1日は1024）
	amPmAnimeTime = (SaTime.hour+832)%1024;
	amPmAnimeGraNoIndex0 = amPmAnimeTime/256;
	amPmAnimeGraNoIndex1 = (amPmAnimeGraNoIndex0+1)%4;
	amPmAnimeX = (amPmAnimeTime%256)/4;


	// ＰＣが移婴筏皮い毪隶Д氓
	if( nowVx != 0 || nowVy != 0 )
		walkFlag = TRUE;

	// サ`バに送ったプロトコルの返事が返ってきたらフラグをリセット
	if( etcSendFlag )
	{
		if( prSendFlag == 0
		 && jbSendFlag == 0
		 && duelSendFlag == 0 )
		{
			etcSendFlag = 0;
		}
		else
		{
			etcSendFlag = 1;
		}
	}

	// 何らかのプロトコルをサ`バに送っているrは
	// 他のC能をg行できなくする
	// iいてるときも
	if( etcSendFlag == 0 && walkFlag == FALSE )
	{
		// 各Nボタンが押されたかチェック
		selId = selGraId( fieldBtnHitId, sizeof( fieldBtnHitId )/sizeof( int ) );
		pushId = pushGraId( fieldBtnHitId, sizeof( fieldBtnHitId )/sizeof( int ) );
		focusId = focusGraId( fieldBtnHitId, sizeof( fieldBtnHitId )/sizeof( int ) );
	}
	else
	{
		selId = -1;
	}

	// 特定のフロアでは仲gボタンを使用できなくする
	if( nowFloor == 31706
	 || nowFloor == 10204
	 || (10601 <= nowFloor && nowFloor <= 10605 )
	 || nowFloor == 10919 || nowFloor == 10920
	 || nowFloor == 20711 || nowFloor == 20712
	 || nowFloor == 1008 || nowFloor == 1021
	 || nowFloor == 3008 || nowFloor == 3021 )
	{
		partyBtnEnableFlag = 0;
	}
	else
	{
		partyBtnEnableFlag = 1;
	}

	// 特定のフロアでは参楗堀骏螭嫌Q楗堀骏螭趣筏Qわれるのでh明を涓工
	if( nowFloor < 10000 && (nowFloor/1000 > 0) && (nowFloor%1000 == 7) )
	{
		joinBattleHelpMsgFlag = 0;	// 参檎h明
		joinBattleBtnEnableFlag = 1;
	}
	else
	if( nowFloor == 1008 || nowFloor == 1021
	 || nowFloor == 3008 || nowFloor == 3021
	 || nowFloor == 10919 || nowFloor == 10920
	 || nowFloor == 20711 || nowFloor == 20712
	 || nowFloor == 10204
	)
	{
		joinBattleHelpMsgFlag = 2;	// 参椁扦胜ふh明。
		joinBattleBtnEnableFlag = 0;
	}
	else
	{
		joinBattleHelpMsgFlag = 1;	// Q檎h明
		joinBattleBtnEnableFlag = 1;
	}





	// メニュ`ボタンI理
	if( selId == FIELD_FUNC_MENU
	 || (joy_trg[1] & JOY_CTRL_Q) )
	{
		if( menuBtn == 0 )
		{
			// アクション涓Ε％螗丧Δ_いているかもしれないので]じる
			// （_いてないriんでも}oい）
			closeCharActionAnimeChange();

			// 他のウィンドウが出てたら消す
			if( CheckMenuFlag() )
			{
				InitMenu2();
			}

			initEtcSwitch();
			menuBtn = 1;
			play_se( 202, 320, 240 );	// ウィンドウ_く音
		}
		else
		{
			closeEtcSwitch();
		}
	}
	if( menuBtn == 1 )
	{
		if( etcSwitch() )
		{
			menuBtn = 0;
		}
	}
	if( focusId == FIELD_FUNC_MENU )
	{
		menuBtnFocus = 1;		// ボタンにフォ`カスがあっている
	}
	else
	{
		menuBtnFocus = 0;
	}


	// 名刺ボタンI理
	if( pushId == FIELD_FUNC_CARD )
	{
		cardBtn = 1;

		if( selId == FIELD_FUNC_CARD )
		{
			int dx, dy;
			int flag;

			// 自分の正面にキャラがいるか{べる
			getRouteData( pc.dir, &dx, &dy );
			flag = checkCharObjPoint( nowGx+dx, nowGy+dy, CHAROBJ_TYPE_USER_NPC );
			// パ`ティに入ってないrで正面にキャラがいる
			if( partyModeFlag == 0 && flag == TRUE
			 && eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// B射抑制
				{
					// 名刺交Q
					lssproto_AAB_send( sockfd, nowGx, nowGy );
					fieldBtnPushTime = GetTickCount();
				}
			}
			play_se( 217, 320, 240 );	// クリック音
		}
	}
	else
	{
		cardBtn = 0;
	}
	if( focusId == FIELD_FUNC_CARD )
	{
		cardBtnFocus = 1;		// ボタンにフォ`カスがあっている
	}
	else
	{
		cardBtnFocus = 0;
	}


	// 仲gボタンI理
	if( pushId == FIELD_FUNC_PARTY )
	{
		partyBtn = 1;
		if( partyBtnEnableFlag == 0 )
		{
			// 特定のマップでは仲gになれない
			partyBtn = 0;
			play_se( 220, 320, 240 );	// ＮＧ音
		}
		else
		if( selId == FIELD_FUNC_PARTY )
		{

			// パ`ティに入ってないrの幼
			if( partyModeFlag == 0 )
			{
				int dx, dy;
				int flag;

				// 正面に殛Lをしていないキャラがいるかチェック
				getRouteData( pc.dir, &dx, &dy );
				flag = checkCharObjPointNotStatus( nowGx+dx, nowGy+dy,
					CHAROBJ_TYPE_USER_NPC, CHR_STATUS_BATTLE );
				// 正面にキャラがいるので仲g要求
				if( flag == 1
				 && eventWarpSendFlag == 0
				 && eventEnemySendFlag == 0
				 && sendEnFlag == 0 )
				{
					if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// B射抑制
					{
						// lの仲gにもなってないr（自分がリ`ダ`でも）、仲g要求
						lssproto_PR_send( sockfd, nowGx, nowGy, 1 );
						prSendMode = 1;
						prSendFlag = 1;
						etcSendFlag = 1;
						fieldBtnPushTime = GetTickCount();
					}
				}
			}
			else
			// パ`ティのrの幼
			{
				if( eventWarpSendFlag == 0
				 && eventEnemySendFlag == 0
				 && sendEnFlag == 0 )
				{
					if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// B射抑制
					{
						// lかの仲gのr、除
						lssproto_PR_send( sockfd, nowGx, nowGy, 0 );
						prSendMode = 0;
						prSendFlag = 1;
						etcSendFlag = 1;
						fieldBtnPushTime = GetTickCount();
					}
				}
			}
			play_se( 217, 320, 240 );	// クリック音
		}
	}
	else
	{
		partyBtn = 0;
	}
	if( focusId == FIELD_FUNC_PARTY )
	{
		partyBtnFocus = 1;		// ボタンにフォ`カスがあっている
	}
	else
	{
		partyBtnFocus = 0;
	}


	// 参楗堀骏I理
	if( pushId == FIELD_FUNC_JOIN_BATTLE )
	{
		joinBattleBtn = 1;
		if( joinBattleBtnEnableFlag == 0 )
		{
			// 特定のマップでは参椁扦胜
			joinBattleBtn = 0;
			play_se( 220, 320, 240 );	// ＮＧ音
		}
		else
		if( selId == FIELD_FUNC_JOIN_BATTLE )
		{
			int dx, dy;
			int flag;

			// パ`ティに入ってないr
			// または、リ`ダのr
			// 正面に殛Lをしているキャラがいるかチェック
			getRouteData( pc.dir, &dx, &dy );
			flag = checkCharObjPointStatus( nowGx+dx, nowGy+dy,
				CHAROBJ_TYPE_USER_NPC, CHR_STATUS_BATTLE );
			// 正面に殛Lをしているキャラがいる
			if( (partyModeFlag == 0
			 || (partyModeFlag == 1 && (pc.status & CHR_STATUS_LEADER) != 0))
			 && flag == 2
			 && eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// B射抑制
				{
					// 殛L中の相手に参椁蛩亭
					lssproto_JB_send( sockfd, nowGx, nowGy );
					jbSendFlag = 1;
					etcSendFlag = 1;
					fieldBtnPushTime = GetTickCount();
				}
			}
			play_se( 217, 320, 240 );	// クリック音
		}
	}
	else
	{
		joinBattleBtn = 0;
	}
	if( focusId == FIELD_FUNC_JOIN_BATTLE )
	{
		joinBattleBtnFocus = 1;		// ボタンにフォ`カスがあっている
	}
	else
	{
		joinBattleBtnFocus = 0;
	}


	// 楗堀骏I理
	if( pushId == FIELD_FUNC_DUEL )
	{
		duelBtn = 1;

		if( selId == FIELD_FUNC_DUEL )
		{
			int dx, dy;
			int flag;

			// パ`ティに入ってないr
			// または、リ`ダのr
			// 正面にキャラがいるかチェック
			getRouteData( pc.dir, &dx, &dy );
			flag = checkCharObjPoint( nowGx+dx, nowGy+dy, CHAROBJ_TYPE_USER_NPC );
			// 正面にキャラがいるので樯辘忿zみ
			if( (partyModeFlag == 0
			 || (partyModeFlag == 1 && (pc.status & CHR_STATUS_LEADER) != 0))
			 && flag == TRUE
			 && eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// B射抑制
				{
					lssproto_DU_send( sockfd, nowGx, nowGy );
					duelSendFlag = 1;
					etcSendFlag = 1;
					fieldBtnPushTime = GetTickCount();
				}
			}
			play_se( 217, 320, 240 );	// クリック音
		}
	}
	else
	{
		duelBtn = 0;
	}
	if( focusId == FIELD_FUNC_DUEL )
	{
		duelBtnFocus = 1;	// ボタンにフォ`カスがあっている
	}
	else
	{
		duelBtnFocus = 0;
	}

	// アクションボタンI理
	if( selId == FIELD_FUNC_ACT
	 || (joy_trg[1] & JOY_CTRL_W) )
	{
		if( actBtn == 0 )
		{
			// 仲g?参?椋希衰攻ぅ氓隶榨楗挨_いているかもしれないので]じる
			// （_いてないriんでも}oい）
			closeEtcSwitch();

			// 他のウィンドウが出てたら消す
			if( CheckMenuFlag() )
			{
				InitMenu2();
			}

			initCharActionAnimeChange();
			actBtn = 1;
			play_se( 202, 320, 240 );	// ウィンドウ_く音
		}
		else
		{
			closeCharActionAnimeChange();
		}
	}
	if( actBtn == 1 )
	{
		if( charActionAnimeChange() )
		{
			actBtn = 0;
		}
	}
	if( focusId == FIELD_FUNC_ACT )
	{
		actBtnForcus = 1;	// ボタンにフォ`カスがあっている
	}
	else
	{
		actBtnForcus = 0;
	}


	// メ`ルランプI理
	if( mailLamp )
	{
		// 初期化
		if( mailLampFlashTime == 0 )
		{
			mailLampDrawFlag = 1;
			mailLampFlashTime = GetTickCount();
		}
		else
		if( mailLampFlashTime+FIELD_MAIL_LAMP_FLASH_TIME < GetTickCount() )
		{
			mailLampDrawFlag++;
			mailLampDrawFlag &= 1;
			mailLampFlashTime = GetTickCount();
		}
	}
	else
	{
		mailLampDrawFlag = 0;
		mailLampFlashTime = 0;
	}
}


// フィ`ルド上でのI理を反映して描画
void drawField( void )
{
	int leftUpPanelX = 0;		// 左上ボタンの台座の座
	int leftUpPanelY = 0;
	int rightUpPanelX = 504;	// 右上ボタンの台座の座
	int rightUpPanelY = 0;
	int i;


#if 0
#ifdef _DEBUG_MSG

	char msg[256];

	// サ`バからEchoの受信を行ったrのrgを表示
	//  通常ゲ`ムのプロトコルが送信されているgは更新されない
	sprintf( msg, "%s Server Alive -> %02d/%02d/%02d %02d:%02d:%02d",
		selectServerName2[selectServerIndex],
		(serverAliveTime->tm_year % 100), serverAliveTime->tm_mon+1, serverAliveTime->tm_mday,
		serverAliveTime->tm_hour, serverAliveTime->tm_min, serverAliveTime->tm_sec );
	StockFontBuffer( 108, 8, FONT_PRIO_FRONT, 0, msg, 0 );

#endif
#endif


	if( drawFieldButtonFlag )
	{
		// 左上ボタンの台座
		StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
			DISP_PRIO_MENU, CG_FIELD_MENU_LEFT, 0 );

		// 左上メニュ`ボタン
		fieldBtnHitId[FIELD_FUNC_MENU] =
			StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
				DISP_PRIO_IME3, menuBtnGraNo[menuBtn], 2 );
		if( menuBtnFocus )
		{
			// メニュ`ボタンh明
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"各NO定を行います。", 0 );
		}

		// 左上名刺ボタン
		fieldBtnHitId[FIELD_FUNC_CARD] =
			StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
				DISP_PRIO_IME3, cardBtnGraNo[cardBtn], 2 );
		if( cardBtnFocus )
		{
			// 名刺ボタンh明
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"他のキャラクタ`と名刺交Qをします。", 0 );
		}

		// 左上仲gボタン
		fieldBtnHitId[FIELD_FUNC_PARTY] =
			StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
				DISP_PRIO_IME3, partyBtnGraNo[partyBtn], 2 );
		if( partyBtnFocus )
		{
#if 0
			// 仲gボタンh明
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"他のキャラクタ`の仲gに入ります。", 0 );
#else
			// 仲gボタンh明
			// 特定のマップでは仲gボタンを使えなくするため
			if( partyBtnEnableFlag == 0 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"ここでは仲gに入る事が出来ません。", 0 );
			}
			else
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"他のキャラクタ`の仲gに入ります。", 0 );
			}
#endif
		}


		// 左上メ`ルランプ
		if( mailLampDrawFlag )
			StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
				DISP_PRIO_IME3, CG_FIELD_MAIL_LAMP, 0 );


		// 右上ボタンの台座
		StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
			DISP_PRIO_MENU, CG_FIELD_MENU_RIGHT, 0 );

		// 右上参楗堀骏
		fieldBtnHitId[FIELD_FUNC_JOIN_BATTLE] =
			StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
				DISP_PRIO_IME3, joinBattleBtnGraNo[joinBattleBtn], 2 );
		if( joinBattleBtnFocus )
		{
			if( joinBattleHelpMsgFlag == 1 )
			{
				// 参楗堀骏笳h明
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"殛L中のキャラクタ`の手助けとして参椁筏蓼埂", 0 );
			}
			else
			if( joinBattleHelpMsgFlag == 2 )
			{
				// 参楗堀骏笳h明
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"ここでは参椁扦蓼护蟆", 0 );
			}
			else
			{
				// Q楗堀骏笳h明
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"デュエルをQ椁筏蓼埂", 0 );
			}
		}

		// 右上楗堀骏
		fieldBtnHitId[FIELD_FUNC_DUEL] =
			StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
				DISP_PRIO_IME3, duelBtnGraNo[duelBtn], 2 );
		if( duelBtnFocus )
		{
			// デュエルボタンh明
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"他のプレイヤ`キャラクタ`にデュエル()を申しzみます。", 0 );
		}

		// 右上アクションボタン
		fieldBtnHitId[FIELD_FUNC_ACT] =
			StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
				DISP_PRIO_IME3, actBtnGraNo[actBtn], 2 );
		if( actBtnForcus )
		{
			// アクションボタンh明
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"キャラクタ`に色々なアクションをさせます。", 0 );
		}

		// rgアニメ
		if( drawTimeAnimeFlag )
		{
			StockDispBuffer( rightUpPanelX+104-amPmAnimeX, rightUpPanelY+21,
				DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex0], 0 );
			StockDispBuffer( rightUpPanelX+104+64-amPmAnimeX, rightUpPanelY+21,
				DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex1], 0 );
		}
		else
		{
			// rgアニメを表示しないr（ダンジョンとか）はバック画像を出す
			StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
				DISP_PRIO_MENU, CG_FIELD_MENU_RIGHT_BACK, 0 );
		}

	}
	else
	{
		for( i = 0; i < FIELD_FUNC_END; i++ )
		{
			fieldBtnHitId[i] = -2;
		}
	}
}




// メニュ`_いてるかどうか
BOOL checkFieldMenuFlag( void )
{
	return nowFieldMenuOpenFlag;
}




// キャラアクション涓Ε％螗丧
// 初期化
void initCharActionAnimeChange( void )
{
	charActionAnimeChangeProcNo = 0;
}


// I理をKわらせる
void closeCharActionAnimeChange( void )
{
	charActionAnimeChangeProcNo = 1000;
}


// アクション涓I理
//  : 0 ... I理中
//          !0 ... I理完了
int charActionAnimeChange( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	static int btnId[13];
	int id = 0;
	int focusId = 0;
	char *msg[] =
	{
		" 坐下 ",
			" 站立 ",
			" 挥手",
			" 走动 ",
			" 点头",
			" 晕倒 ",
			" 高兴 ",
			" 攻击 ",
			" 生气 ",
			" 防御 ",
			" 悲伤 ",
			" 受伤",
			" 投掷 "
	};
	char *msg2[] =
	{
		"坐下(Ctrl+1)",
			"站立(Ctrl+7)",
			"挥手(Ctrl+2)",
			"走动(Ctrl+8)",
			"点头(Ctrl+3)",
			"晕倒(Ctrl+9)",
			"高兴(Ctrl+4)",
			"攻击(Ctrl+0)",
			"生气(Ctrl+5)",
			"防御(Ctrl+-)",
			"投掷(Ctrl+6)",
			"悲伤(Ctrl+^)",
			"投掷(Ctrl+\\)"
	};
	/*
	char *msg[] =
	{
		" 座　る ",
		" 立　つ ",
		"手を振る",
		" i　く ",
		"うなずく",
		" 倒れる ",
		" 喜　ぶ ",
		" 攻　 ",
		" 怒　る ",
		" ガ`ド ",
		" 悲しむ ",
		"ダメ`ジ",
		" 投げる "
	};
	char *msg2[] =
	{
		"座るアクションをします。(Ctrl+1)",
		"立つアクションをします。(Ctrl+7)",
		"手を振るアクションをします。(Ctrl+2)",
		"iくアクションをします。(Ctrl+8)",
		"うなずくアクションをします。(Ctrl+3)",
		"倒れるアクションをします。(Ctrl+9)",
		"喜ぶアクションをします。(Ctrl+4)",
		"攻磨伐绁螭颏筏蓼埂(Ctrl+0)",
		"怒るアクションをします。(Ctrl+5)",
		"ガ`ドアクションをします。(Ctrl+-)",
		"悲しむアクションをします。(Ctrl+6)",
		"ダメ`ジアクションをします。(Ctrl+^)",
		"投げるアクションをします。(Ctrl+\\)"
	};*/
	int i, j;
	int x1, y1, x2, y2;
	int chgTbl[] =
	{
	   5,	//  座　る 
	   3,	//  立　つ 
	   6,	// 手を振る
	   4,	//  i　く 
	  11,	// うなずく
	   2,	//  倒れる 
	   7,	//  喜　ぶ 
	   0,	//  攻　 
	   8,	//  怒　る 
	  10,	//  ガ`ド 
	   9,	//  悲しむ 
	   1,	// ダメ`ジ
	  12	// 投げる
	};


	if( charActionAnimeChangeProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 3;
		h = 6;
		x = 440;
		//y = 72;
		y = 16;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		charActionAnimeChangeProcNo++;
	}

	if( ptActMenuWin != NULL )
	{
		id = -1;
		focusId = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンのxk判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
			focusId = focusGraId( btnId, sizeof( btnId )/sizeof( int ) );

			for( i = 0; i < 6; i++ )
			{
				for( j = 0; j < 2; j++ )
				{
					x1 = x+84*j+18;
					y1 = y+i*30+21;
					x2 = x1 + 73;
					y2 = y1 + 22;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							id = i*2+j+1;
						}
						focusId = i*2+j+1;
					}
				}
			}

			i = 6;
			j = 0;
			x1 = x+42+18;
			y1 = y+i*30+21;
			x2 = x1 + 73;
			y2 = y1 + 22;
			if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
			{
				if( mouse.onceState & MOUSE_LEFT_CRICK )
				{
					id = i*2+j+1;
				}
				focusId = i*2+j+1;
			}
		}

		// 他のメニュ`が出るのでこっちのウィンドウを]じる
		if( CheckMenuFlag() )
		{
			id = 100;
		}
		else
		// ESCが押されたのでK了
		if( (joy_trg[ 0 ] & JOY_ESC) && ImeBufferBak2 == NULL )
		{
			id = 100;
			play_se( 203, 320, 240 );	// ウィンドウ]じる音
		}
		else
		// K了v数が呼ばれたのでK了
		if( charActionAnimeChangeProcNo == 1000 )
		{
			id = 100;
		}

		// キャンセル
		if( id == 0 )
		{
			play_se( 203, 320, 240 );	// ウィンドウ]じる音
		}
		else
		if( 1 <= id && id <= 13 )
		{
			id--;
//			if( id >= 3 )
//				id++;
			if( eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// B射抑制
				{
					lssproto_AC_send( sockfd, nowGx, nowGy, chgTbl[id] );
					setPcAction( chgTbl[id] );
					fieldBtnPushTime = GetTickCount();
				}
			}
			// メニュ`の目をxんでもウィンドウを]じないようにする
			id = -1;
			play_se( 217, 320, 240 );	// クリック音
		}

		if( id >= 0 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return 1;
		}

		if( ptActMenuWin->hp >= 1 )
		{
			btnId[0] = StockDispBuffer( x+96, y+258,
					DISP_PRIO_MENU, CG_CLOSE_BTN, 2 );
			if( focusId == 0 )
			{
				// メニュ`目h明
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"アクションウィンドウを]じます。", 0 );
			}

			for( i = 0; i < 6; i++ )
			{
				for( j = 0; j < 2; j++ )
				{
					StockFontBuffer( x+84*j+20, y+i*30+24,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i*2+j], 0 );
					if( i*2+j == focusId-1 )
					{
						// メニュ`目h明
						StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
							msg2[i*2+j], 0 );
					}
				}
			}
			i = 6;
			j = 0;
			StockFontBuffer( x+42+20, y+i*30+24,
				FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i*2+j], 0 );
			if( i*2+j == focusId-1 )
			{
				// メニュ`目h明
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					msg2[i*2+j], 0 );
			}
		}
	}

	return 0;
}




// 仲g?参?椋希衰攻ぅ氓隶Ε％螗丧
// 初期化
void initEtcSwitch( void )
{
	etcSwitchProcNo = 0;
}


// I理をKわらせる
void closeEtcSwitch( void )
{
	etcSwitchProcNo = 1000;
}


// 仲g?参?椋希衰攻ぅ氓I理
//  : 0 ... I理中
//          !0 ... I理完了
/// /伙伴？参战？对战OK开关处理/ /返回值:0...处理中/ /0...处理完毕！
int etcSwitch( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	static int btnId[5];
	int id = 0;
	int focusId = 0;
	//这里修改按钮
	char *msg[] =
		//“集团：",“：",决斗“交换名片：",“聊天：“
	{
		"组队：",
		"决斗",
		"名片",
		"聊天"
	};
	char *msg2[] =
	{
		" Ｎ　Ｏ",
		" ＹＥＳ"
	};
	char *msg3[] =
	{
		"全体",
		"家族"
	};
	char *msg4[] =
	{
		"其他角色的设定是否能成为同伴",
		"其他角色的对战报名接受的设定",
		"交换名片是否接受的设定",
		"聊天消息的范围设定"
	};
	int mask[] =
	{
		PC_ETCFLAG_PARTY,
		PC_ETCFLAG_DUEL,
		PC_ETCFLAG_MAIL,
		PC_ETCFLAG_CHAT_MODE
	};
	int i;
	int x1, y1, x2, y2;

	if( etcSwitchProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 3;
		h = 4;
		x = 16;
		//x = 440;
		//y = 72;
		y = 16;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		etcSwitchProcNo++;
	}

	if( ptActMenuWin != NULL )
	{
		id = -1;
		focusId = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンのxk判定
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
			focusId = focusGraId( btnId, sizeof( btnId )/sizeof( int ) );

			for( i = 0; i < sizeof( msg )/sizeof( char * ); i++ )
			{
				x1 = x+18;
				y1 = y+i*30+23;
				x2 = x1 + 157;
				y2 = y1 + 22;
				if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
				{
					if( mouse.onceState & MOUSE_LEFT_CRICK )
					{
						id = i+1;
					}
					focusId = i+1;
				}
			}
		}

		// 他のメニュ`が出るのでこっちのウィンドウを]じる
		if( CheckMenuFlag() )
		{
			id = 100;
		}
		else
		// ESCが押されたのでK了
		if( ((joy_trg[ 0 ] & JOY_ESC) && ImeBufferBak2 == NULL) )
		{
			id = 100;
			play_se( 203, 320, 240 );	// ウィンドウ]じる音
		}
		else
		// K了v数が呼ばれたのでK了
		if( etcSwitchProcNo == 1000 )
		{
			id = 100;
		}

		// キャンセル
		if( id == 0 )
		{
			play_se( 203, 320, 240 );	// ウィンドウ]じる音
		}
		else
		if( 1 <= id && id <= sizeof( msg )/sizeof( char * ) )
		{
			id--;
			// ビッドが上がってたら下ろす
			if( pc.etcFlag & mask[id] )
			{
				pc.etcFlag &= ~mask[id];
			}
			else
			// 下がってたら上げる
			{
				pc.etcFlag |= mask[id];
			}
			if( eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// B射抑制
				{
					lssproto_FS_send( sockfd, pc.etcFlag );
					fieldBtnPushTime = GetTickCount();
				}
				else
				{
					// フラグを浃à瑜Δ趣筏郡B射抑制に引っかかったので
					// 後で送る
					etcSwitchChangeFlag = 1;
				}
			}
			// メニュ`の目をxんでもウィンドウを]じないようにする
			id = -1;
			play_se( 217, 320, 240 );	// クリック音
		}

		if( id >= 0 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return 1;
		}

		if( ptActMenuWin->hp >= 1 )
		{

#if 0
			btnId[0] = StockFontBuffer( x+54, y+154,
//						FONT_PRIO_FRONT, FONT_PAL_WHITE, "キャンセル", 2 );
						FONT_PRIO_FRONT, FONT_PAL_WHITE, "　]じる　", 2 );
#else
			btnId[0] = StockDispBuffer( x+96, y+162,
					DISP_PRIO_MENU, CG_CLOSE_BTN, 2 );
#endif
			if( focusId == 0 )
			{
				// メニュ`目h明
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"メニュ`ウィンドウを]じます。", 0 );
			}

			for( i = 0; i < sizeof( msg )/sizeof( char * ); i++ )
			{
				StockFontBuffer( x+20, y+i*30+26,
					FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i], 0 );
				if( i != 3 )
				{
					StockFontBuffer( x+105, y+i*30+26, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						msg2[((pc.etcFlag & mask[i])?1:0)], 0 );
				}
				else
				{
					StockFontBuffer( x+105, y+i*30+26, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						msg3[((pc.etcFlag & mask[i])?1:0)], 0 );
				}
				if( i == focusId-1 )
				{
					// メニュ`目h明
					StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						msg4[i], 0 );
				}
			}
		}
	}

	return 0;
}



// サ`バとの接Aが途切れた
//    0 ... I理中
//            1 ... "はい"ボタンが押された
int disconnectServer( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	static int btnId[1];
	int id = 0;
	char *msg[] =
	{
		"サ`バとの接Aが途切れました。",
		"タイトルへ辘蓼埂"
	};
	int i;
	int ret = 0;
	int xx, yy;


	if( ptActMenuWin == NULL )
	{
		// 他のウィンドウが出てたら消す
		if( CheckMenuFlag() )
		{
			InitMenu2();
		}
		closeEtcSwitch();
		closeCharActionAnimeChange();


		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ウィンドウ作成
		w = 5;
		h = 3;
		x = (640 - w*64)/2;
		y = (456 - h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}

	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// ボタンのxk判定
			id = selFontId( btnId, sizeof( btnId )/sizeof( int ) );
			if( id >= 0 )
			{
				DeathAction( ptActMenuWin );
				ptActMenuWin = NULL;
				ret = 1;
			}

			yy = (h*48)/4;
			for( i = 0; i < sizeof( msg )/sizeof( char * ); i++ )
			{
				xx = (w*64-strlen( msg[i] )/2*17)/2;
				StockFontBuffer( x+xx, y+(i+1)*yy,
					FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i], 0 );
			}

			xx = (w*64-strlen( "はい" )/2*17)/2;
			btnId[0] = StockFontBuffer( x+xx, y+3*yy,
						FONT_PRIO_FRONT, FONT_PAL_YELLOW, "はい", 2 );
		}
	}

	return ret;
}


// ワ`プ後その鏊蚊挨虮硎
void drawFieldInfoWin( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	int xx, yy;

	if( fieldInfoTime == 0 )
	{
		if( ptActMenuWin != NULL )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
		return;
	}

	if( MapWmdFlagBak )
	{
		return;
	}

	if( ptActMenuWin == NULL )
	{
		// 他のウィンドウが出てたら消す
		if( CheckMenuFlag() )
		{
			// マップウィンドウフラグバックアップ
			if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
			InitMenu2();
		}
		closeEtcSwitch();
		closeCharActionAnimeChange();

		// ウィンドウ作成
		w = 4;
		h = 2;
		x = (640 - w*64)/2;
		y = (456 - h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 3 );

		play_se( 202, 320, 240 );	// ウィンドウ_く音
	}

	if( ptActMenuWin != NULL )
	{
		if( fieldInfoTime+2000 <= GetTickCount()
		 || CheckMenuFlag()
		 || windowTypeWN > -1
		 || actBtn == 1
		 || menuBtn == 1 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			fieldInfoTime = 0;
			return;
		}
		if( ptActMenuWin->hp >= 1 )
		{

			xx = (w*64-strlen( nowFloorName )/2*17)/2;
			yy = (h*48-16)/2;
			StockFontBuffer( x+xx, y+yy,
				FONT_PRIO_FRONT, FONT_PAL_WHITE, nowFloorName, 0 );
		}
	}
}



// アクション涓违伐绌`トカットキ`のI理
void actionShortCutKeyProc( void )
{
	unsigned int key[] =
	{
		JOY_CTRL_0,				// 0
		JOY_CTRL_CIRCUMFLEX,	// 1
		JOY_CTRL_9,				// 2
		JOY_CTRL_7,				// 3
		JOY_CTRL_8,				// 4
		JOY_CTRL_1,				// 5
		JOY_CTRL_2,				// 6
		JOY_CTRL_4,				// 7
		JOY_CTRL_5,				// 8
		JOY_CTRL_6,				// 9
		JOY_CTRL_MINUS,			// 10
		JOY_CTRL_3,				// 11
		JOY_CTRL_YEN			// 12
	};
	int i;
	BOOL pushFlag = FALSE;

	// ＰＣが移婴筏皮い郡楹韦猡筏胜
	if( nowVx != 0 || nowVy != 0 )
		return;

	for( i = 0; i < sizeof( key )/sizeof( unsigned int ); i++ )
	{
		if( joy_trg[ 1 ] & key[i] )
		{
			pushFlag = TRUE;
			break;
		}
	}

	if( !pushFlag )
	{
		return;
	}

	if( eventWarpSendFlag == 0
	 && eventEnemySendFlag == 0
	 && sendEnFlag == 0 )
	{
		if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// B射抑制
		{
			lssproto_AC_send( sockfd, nowGx, nowGy, i );
			setPcAction( i );
			fieldBtnPushTime = GetTickCount();
		}
	}
}

