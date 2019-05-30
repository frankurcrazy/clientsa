/************************/
/*	battleProc.cpp		*/
/************************/
#include "system.h"
#include "ime_sa.h"
#include "map.h"
#include "menu.h"
#include "battlemap.h"
#include "battleProc.h"
#include "battleMenu.h"
#include "battleMap.h"
#include "produce.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "t_music.h"
#include "pc.h"
#include "character.h"
#include "handletime.h"
#include "savedata.h"

extern	ACTION* oft_test();
extern void damage_num( ACTION *a0 );
extern void WindowDisp( ACTION *a0 );
extern int piyo_point;
extern void set_bc( void );
extern void disp_kanji(ACTION *a0);

int flash_pal_cnt;
PALETTEENTRY	Palette2[256];	// パレット造体

// エンカウントフラグ
BOOL EncountFlag = FALSE;
// デュエルフラグ
BOOL DuelFlag = FALSE;
// ヘルプoしのr
BOOL NoHelpFlag = FALSE;
// エンカウントオフフラグ
BOOL EncountOffFlag = FALSE;

// 一人用か二人用かフラグ
BOOL Battle1P2PFlag;

// 不意打ち表示ウィンドウ
ACTION* pActSurprisalWnd;
ACTION* pActAudienceExitWnd;

// 全绁隶Д氓I理
extern int check_all_dead( void );

/* 殛L地震 *********************************************************/
void battle_quake( void )
{
	switch(quake_vct_no){
	case 0:		//I理oし
		break;
	case 1:		//地震_始
		quake_flg = 60;
		quake_vct_no = 2;
		break;
	case 2:		//地震中
		if(quake_flg >= 16){
			piyo_point += 16;
		} else {
			piyo_point += quake_flg;
		}
		piyo_point &= 63;
		if(!--quake_flg){		//地震K了なら
			quake_vct_no = 0;
		}
		break;
	}
}


// 不意打ち表示I理 **************************************************************/
void SurprisalDisp( void ){
	
	// 不意ついたr、つかれたr
	if( BattleBpFlag & BATTLE_BP_ENEMY_SURPRISAL || BattleBpFlag & BATTLE_BP_PLAYER_SURPRISAL ){
		// ウィンドウoかったら
		if( pActSurprisalWnd == NULL ){
			// ウィンドウ表示タスク作成
			pActSurprisalWnd = MakeWindowDisp( 320 - 96, 240 - 48, 3, 2, NULL, 1 );
			// ウィンドウ_く音
			play_se( 202, 320, 240 );
		}
		// ウィンドウ出来あがっていたら
		if( pActSurprisalWnd->hp > 0 ){
			// 长灰猡颏膜いr（不意つかれたr）
			if( BattleBpFlag & BATTLE_BP_ENEMY_SURPRISAL ){
				StockFontBuffer( pActSurprisalWnd->x + 38, pActSurprisalWnd->y + 40, FONT_PRIO_FRONT, 0, 	"不意をつかれた", 0 );
			}
			// プレイヤ`が不意をついたr
			if( BattleBpFlag & BATTLE_BP_PLAYER_SURPRISAL ){
				StockFontBuffer( pActSurprisalWnd->x + 38, pActSurprisalWnd->y + 40, FONT_PRIO_FRONT, 0, 	" 不意をついた ", 0 );
			}
		}
	}
}

// バトルプロセス サブプロセス番号
enum{
	BATTLE_SUBPROC_INIT,				// ０：初期化
	BATTLE_SUBPROC_IN_PRODUCE,			// １：バトルイン演出
	BATTLE_SUBPROC_RECEIVE_BC,			// ２：ＢＣ受信待ち
	BATTLE_SUBPROC_CHAR_IN,				// ３：登鲅莩
	BATTLE_SUBPROC_CMD_INPUT,			// ４：コマンド入力
	BATTLE_SUBPROC_RECEIVE_MOVIE,		// ５：ム`ビ`受信待ち
	BATTLE_SUBPROC_MOVIE,				// ６：ム`ビ`I理
	BATTLE_SUBPROC_OUT_PRODUCE_INIT,	// ７：K了演出初期化
	BATTLE_SUBPROC_OUT_PRODUCE,			// ８：K了演出
};
	

int xxx;
/* バトルプロセスI理 ********************************************************************/
void BattleProc( void )
{
	int i;
	//static no = Rnd( 0, BATTLE_MAP_FILES - 1 );	// 今だけランダムバトルマップ
	s_timer++;	// フレ`ムタイマ`


	xxx = 0;
	
	/* サブプロセス番号で分岐 */
	switch( SubProcNo ){
	
		case BATTLE_SUBPROC_INIT:				// 初期化
		
			// アクション抹
			DeathAllAction();
			
			slow_flg = 0;
			action_inf = 0;
			/* 自C タスク作成 */
			oft_test();
			
			// メニュ`vBの初期化
			InitMenu();
			// バトルメニュ`初期化
			InitBattleMenu();
			// 不意打ちウィンド初期化
			//DeathAction( pActSurprisalWnd );
			pActSurprisalWnd = NULL;
			pActAudienceExitWnd = NULL;
			// プロデュ`ス初期化
			ProduceInitFlag = TRUE;
			/* チャットvB初期化 */
			//InitChat();
			// 入力フォ`カス取得
			GetKeyInputFocus( &MyChatBuffer );
			
			// 演出画像の作成
			// バッファ初期化
			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			// バトルマップiみzみとバトルサ`フェスの画像作成 */
			ReadBattleMap( BattleMapNo );
			//MenuProc();				// メニュ`I理
			//PaletteProc();			// パレットI理
			ChatProc();				// チャットI理
			ChatBufferToFontBuffer(); // チャットバッファ`をフォントバッファに溜める
			//ImeProc();				// ＩＭＥvBI理
			//SortDispBuffer(); 	// 表示バッファソ`ト
			// バックサ`フェスを\でクリア`
			ClearBackSurface();	
			// ＢＭＰをバックサ`フェスにセット
			PutBmp();	
			// バックサ`フェスからバトルサ`フェスへコピ`
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// バッファ初期化
			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			//固定长钎濂ē毪胜
			if(DuelFlag == TRUE || eventEnemyFlag == 1){
				//ボス殛LＢＧＭ再生
				play_bgm( 6 );
			} else {
				//通常殛LＢＧＭ再生
				play_bgm( 5 );
			}
			// F在のrgを
			NowTime = GetTickCount();
			// バックバッファ`描画方法涓
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// 演出中
			DrawProduce( PRODUCE_HAGARE_OCHI_IN );
			
			// 点绀oくす
			MenuProc();				// メニュ`I理
			ImeProc();				// ＩＭＥvBI理
			
			// メニュ`フラグＯＮ 
			battleMenuFlag2 = TRUE;
			
			// いないペットチェック
			for( i = 0 ; i < 5 ; i++ ){
				// この冥衰讠氓趣い胜r
				if( pet[ i ].useFlag == FALSE ){
					// バトルを解除
					if( i == pc.battlePetNo ) pc.battlePetNo = -1;
					// メ`ルのr
					if( i == pc.mailPetNo ) pc.mailPetNo = -1;
					// とりあえず２にする
					pc.selectPetNo[ i ] = FALSE;
					
				}
#if 0
				// バトルのときかつメ`ルのr（ラグでなるときがある）
				if( pc.battlePetNo == i && pc.mailPetNo == i ){
					// メ`ルなしにする
					pc.mailPetNo = -1;
					// とりあえず待Cにする
					pc.selectPetNo[ i ] = TRUE;
				}
#endif
			}
			// パラメ`タセ`ブ
			saveUserSetting();
			
			// プロセス番号プラス
			SubProcNo++;
			break;
			
		case BATTLE_SUBPROC_IN_PRODUCE:			// バトルイン演出
		
			// 演出中
			if( DrawProduce( PRODUCE_HAGARE_OCHI_IN ) == TRUE ){
				// バッファ初期化
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// バトルマップiみzみとバトルサ`フェスの画像作成 */
				ReadBattleMap( BattleMapNo );
				//no++;
				//if( no >= BATTLE_MAP_FILES ) no = 0;
				//no = Rnd( 0, BATTLE_MAP_FILES - 1 );
				// バックサ`フェスを\でクリア`
				ClearBackSurface();	
				//SortDispBuffer(); 	// 表示バッファソ`ト
				// ＢＭＰをバックサ`フェスにセット
				PutBmp();
				// バトルサ`フェスを初期化
				//ClearSurface( lpBattleSurface );
				// バックサ`フェスからバトルサ`フェスへコピ`
				lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
				// バッファ初期化
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// F在のrgを
				NowTime = GetTickCount();
				
				// バックバッファ`描画方法涓
				BackBufferDrawType = DRAW_BACK_BATTLE; 
				
				// バックバッファ`に描く
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
				// パレットI理
				//PaletteProc();
				
				SubProcNo++;
			}else{
				MenuProc();	// メニュ`I理
				ImeProc();	// ＩＭＥvBI理
			}
		
			break;
			
		case BATTLE_SUBPROC_RECEIVE_BC:			// ＢＣ受信待ち
		
#ifdef _DEBUG
			if(offlineFlag == TRUE){
				strcpy(BattleStatus,"BC|3|"
				"0|ishikawa|title|18768|1|20|20|0|"
				"1|ishikawa|title|18768|1|20|20|0|"
				"2|ishikawa|title|18768|1|20|20|0|"
				"3|ishikawa|title|18768|1|20|20|0|"
				"4|ishikawa|title|18768|1|20|20|0|"
				"5|beron|title|18813|1|20|20|0|"
				"6|beron|title|18813|1|20|20|0|"
				"7|beron|title|18813|1|20|20|0|"
				"8|beron|title|18813|1|20|20|0|"
				"9|beron|title|18813|1|20|20|0|"
				"A|ishikawa|title|18813|1|20|20|0|"
				"B|ishikawa|title|18813|1|20|20|0|"
				"C|ishikawa|title|18813|1|20|20|0|"
				"D|ishikawa|title|18813|1|20|20|0|"
				"E|ishikawa|title|18813|1|20|20|0|"
				"F|beron|title|187BA|1|20|20|0|"
				"10|beron|title|187BA|1|20|20|0|"
				"11|beron|title|187BA|1|20|20|0|"
				"12|beron|title|187BA|1|20|20|0|"
				"13|beron|title|187BA|1|20|20|0|"
				);

	strcpy(BattleStatus,"BC|0|0|命のの十常寺||186F7|1F|B6|B6|5|F|疾Lのコズぴよ||18786|20|11D|11D|1|10|リンダ||187B2|1D|F8|F8|1|");
//	strcpy(BattleStatus,"BC|0|0|サガット|目衰螗镜|186DF|E|2A8|2BC|4|F|ダックル||187B6|F|7D|7D|0|10|ダックル||187B6|10|1|91|0|");

				// ＢＣO定
				set_bc();
				SubProcNo++;
				break;
			}
#endif
			
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
			
			// 参椁r
			if( BattleBpFlag & BATTLE_BP_JOIN ){
				// 一行インフォ
				strcpy( OneLineInfoStr,"タ`ンK了待ち。");
			}
				
			// ２タ`ンめ以降はアクション走らせる
			if( EncountFlag == FALSE ){
				// 违楗攻咯`スクロ`ル
				if( !( s_timer & 7 ) ){
					piyo_point++;
					piyo_point &= 63;
				}
				/* アクション走らせる */
				RunAction();
				// タスク表示デ`タをバッファに溜める
				StockTaskDispBuffer();
			}
			
			// サ`バ`の答待ち
#if 0
			if( BattleStatusBak[ 0 ] != NULL ){
				strcpy( BattleStatus, BattleStatusBak );
				BattleStatusBak[ 0 ] = NULL;
				// エンカウントフラグ初期化
				EncountFlag = FALSE;
				// ＢＣO定
				set_bc();
				// 一人用か二人用かチェック
				Battle1P2PFlag = CheckBattle1P2P();
				SubProcNo++;

				//Q楗猢`ドなら
//				if(xxx == 0 && BattleMyNo >= 20){
//					BattleCmdBak[ 0 ] = NULL;
//					xxx = 1;
//				}
#else
			if( BattleStatusReadPointer != BattleStatusWritePointer ){
				strcpy( BattleStatus, BattleStatusBak[ BattleStatusReadPointer ] );
				BattleStatusReadPointer = ( BattleStatusReadPointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
				// エンカウントフラグ初期化
				EncountFlag = FALSE;
				// ＢＣO定
				set_bc();
				// 殛L中の新讠氓去隶Д氓
				CheckBattleNewPet();
				// Q楗猢`ドでないr
				if( BattleMyNo < 20 ){
					// ＭＰをO定
					p_party[ BattleMyNo ]->mp = BattleMyMp;
				}
				
				// 一人用か二人用かチェック
				Battle1P2PFlag = CheckBattle1P2P();
				SubProcNo++;

				//Q楗猢`ドなら
//				if(xxx == 0 && BattleMyNo >= 20){
//					BattleCmdBak[ 0 ] = NULL;
//					xxx = 1;
//				}
#endif

			}
			// Qrに右クリックされたr
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// Q榻K了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			// Q椁r、ウィンドウの表示
			if( BattleMyNo >= 20 ){
				// ウィンドウoかったら
				if( pActAudienceExitWnd == NULL ){
					// ウィンドウ表示タスク作成
					pActAudienceExitWnd = MakeWindowDisp( 444, 4, 3, 2, NULL, 1 );
					// ウィンドウ_く音
					play_se( 202, 320, 240 );
				}
			}
			
			break;
			
		case BATTLE_SUBPROC_CHAR_IN:			// 登鲅莩
			
			// 违楗攻咯`スクロ`ル
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			// 不意打ち表示I理
			SurprisalDisp();
			
			// コマンド入力gみフラグのチェック
			CheckBattleAnimFlag();
			/* アクション走らせる */
			RunAction();
			// タスク表示デ`タをバッファに溜める
			StockTaskDispBuffer();
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
			
			// 登鼋Kったら
			if( action_inf == 3 ){
				action_inf = 0;
				// 殛L参楗榨楗埃希疲
				BattleBpFlag &= ~BATTLE_BP_JOIN; 
				// 全绁隶Д氓I理
				if( check_all_dead() == 1 ){
					SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;
					break;
				}
				// 不意打ちウィンドウ出ていたら
				if( pActSurprisalWnd != NULL ){
					// ウィンドウ抹
					DeathAction( pActSurprisalWnd );
					pActSurprisalWnd = NULL;
					// 不意打ちフラグ初期化
					BattleBpFlag &= ~BATTLE_BP_ENEMY_SURPRISAL; 
					BattleBpFlag &= ~BATTLE_BP_PLAYER_SURPRISAL; 
					
				}
				// 次のプロセスへ
				SubProcNo++;
			}
			//殛L脱出なら  战斗逃脱的话
			if( action_inf == 2 ){		
				SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;		//殛LK了演出へ
				action_inf = 0;
				// バトルコマンド初期化
				BattleCmd[ 0 ] = NULL;
			}
			// Qrに右クリックされたr
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// Q榻K了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			
			break;
			
		case BATTLE_SUBPROC_CMD_INPUT:			// コマンド入力
		
#ifdef _DEBUG
			if(offlineFlag == TRUE){
				SubProcNo++;
				break;
			}
#endif
			// 违楗攻咯`スクロ`ル
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			
			// バトルメニュ`I理  
			BattleMenuProc();
			// コマンド入力gみフラグのチェック
			CheckBattleAnimFlag();
			
			/* アクション走らせる */
			RunAction();
			// タスク表示デ`タをバッファに溜める
			StockTaskDispBuffer();
			
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
			
			// Qrに右クリックされたr
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// Q榻K了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			
			break;
			
		case BATTLE_SUBPROC_RECEIVE_MOVIE:		// ム`ビ`受信待ち
		
#ifdef _DEBUG
			if(offlineFlag == TRUE){
//				strcpy( BattleCmd,"BH|aCr0|f2|d10E|rC|f11|counter156|FF|bg|b|BH|aEr0|f2|dDE|rE|f30|counter0|FF|BH|a"
//				"Dr0|f2|d17E|FF|BE|e0|f1|");

//				strcpy( BattleCmd,"BS jibun_2 flg_0 BS jibun_2 flg_1 pet_186A2");
#if 0
				strcpy( BattleCmd,"BO jibun_0 teki_13 flg_0 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_1 teki_13 flg_8 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_2 teki_13 flg_20 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_3 teki_13 flg_4 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_4 teki_13 flg_0 damage_0 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				                 " BO jibun_0 teki_E flg_0 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_1 teki_E flg_8 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_2 teki_E flg_20 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_3 teki_E flg_4 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_4 teki_E flg_0 damage_0 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				                 " BO jibun_0 teki_9 flg_0 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_1 teki_9 flg_8 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_2 teki_9 flg_20 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_3 teki_9 flg_4 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_4 teki_9 flg_0 damage_0 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
//				                 " BO jibun_0 teki_4 flg_0 damage_4 teki_2 flg_0 damage_4                       teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_1 teki_4 flg_8 damage_4 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_2 teki_4 flg_20 damage_4 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_3 teki_4 flg_4 damage_4 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_4 teki_4 flg_0 damage_0 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
				                 " BO jibun_A teki_8 flg_0 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_B teki_8 flg_8 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_C teki_8 flg_20 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_D teki_8 flg_4 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_E teki_8 flg_0 damage_0 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				                 " BO jibun_A teki_3 flg_0 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_B teki_3 flg_8 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_C teki_3 flg_20 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_D teki_3 flg_4 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_E teki_3 flg_0 damage_0 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				                 " BO jibun_A teki_12 flg_0 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_B teki_12 flg_8 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_C teki_12 flg_20 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_D teki_12 flg_4 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_E teki_12 flg_0 damage_0 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF");
#endif
#if 1
				strcpy( BattleCmd,"BY teki_A jibun_0 flg_0 damage_0 jibun_1 flg_0 damage_0 jibun_2 flg_0 damage_0"
				" jibun_3 flg_0 damage_0 jibun_4 flg_0 damage_0 jibun_5 flg_0 damage_0"
				" jibun_6 flg_0 damage_0 jibun_7 flg_0 damage_0 jibun_8 flg_0 damage_0"
				" jibun_9 flg_0 damage_1 FF");

		strcpy( BattleCmd,"bn|f|BH|a0|rF|f20|d0|r0|f10|counter33|FF|BH|a10|r0|f2|d1|FF|");
//		strcpy( BattleCmd,"BH|a0|rF|fC|d18|FF|BH|a10|r0|f20|d0|r10|f31|counter1|FF|bg|f|");
#endif
				BattleMyNo = 1;
				SubProcNo++;
				break;
			}
#endif

			// 违楗攻咯`スクロ`ル
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			// コマンド入力gみフラグのチェック
			CheckBattleAnimFlag();
			/* アクション走らせる */
			RunAction();
			// タスク表示デ`タをバッファに溜める
			StockTaskDispBuffer();

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
			
			// サ`バ`の答待ち
#if 0
			if( BattleCmdBak[ 0 ] != NULL ){
				strcpy( BattleCmd, BattleCmdBak );
				BattleCmdBak[ 0 ] = NULL;
				action_inf = 0;
				SubProcNo++;
			}
#else
			if( BattleCmdReadPointer != BattleCmdWritePointer ){
				strcpy( BattleCmd, BattleCmdBak[ BattleCmdReadPointer ] );
				BattleCmdReadPointer = ( BattleCmdReadPointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
				action_inf = 0;
				SubProcNo++;
			}
#endif
			// 二人用のr
			if( Battle1P2PFlag == 2 ){
				// 一行インフォ
				strcpy( OneLineInfoStr,"他のプレイヤ`の入力待ち。");
			}
			// Q椁r
			if( BattleMyNo >= 20 ){
				// 人数分ル`プ
				for( i = 0 ; i < 20 ; i++ ){
					// 耐久力メ`タ`表示
					HpMeterDisp( i );
				}
			}else{
				// 右下グル`プのr
				if( BattleMyNo < 10 ){
					for( i  = 0 ; i < 10 ; i++ ) HpMeterDisp( i ); // 耐久力メ`タ`表示
				}else 
				// 右下グル`プのr
				if( BattleMyNo >= 10 ){ 
					for( i = 10 ; i < 20 ; i++ ) HpMeterDisp( i ); // 耐久力メ`タ`表示
				}
			}
			
			// Qrに右クリックされたr
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// Q榻K了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			break;
			
		case BATTLE_SUBPROC_MOVIE:				// ム`ビ`I理
		
#ifdef _DEBUG
			// 人数分ル`プ
			//for( i = 0 ; i < 20 ; i++ ){
				// 耐久力メ`タ`表示
			//	HpMeterDisp( i );
			//}
			if(offlineFlag == TRUE){
				if(joy_trg[0]&JOY_A){		//リセットなら
					DeathAllAction();
					SubProcNo = 0;
					BattleCmd[0] = NULL;
					break;
				}
			}
#endif
			//スロ`中なら（死亡のr）
			if( slow_flg ){		
				if( !( s_timer & 31 ) ){	// ８フレ`ムに一回
					piyo_point++;	// ラスタ`スクロ`ルMませる
					piyo_point &= 63;	// リミットチェック
				}
				if( s_timer & 3 ){		// ４フレ`ムに一回（ここはいじるな！！！）
					damage_dispx();		// スロ`中のダメ`ジ硎
				}else{
					RunAction();		/* アクション走らせる */
					battle_quake();		//殛L地震
				}
			} else { // スロ`でないr
				if( !( s_timer & 7 ) ){	
					piyo_point++;
					piyo_point &= 63;
				}
				RunAction();		/* アクション走らせる */
				battle_quake();		//殛L地震
			}
			
			//死亡フラッシュなら
			if( flash_vct_no ) die_flash();	//死亡フラッシュ
			
			// タスク表示デ`タをバッファに溜める
			StockTaskDispBuffer();

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
			//タ`ンK了なら
			if( action_inf == 1 ){		
				//if( BattleBpFlag & BATTLE_BP_JOIN ){	// 参椁r
				//	BattleBpFlag &= ~BATTLE_BP_JOIN;
				//	SubProcNo = BATTLE_SUBPROC_CMD_INPUT;		//メニュ`に
				//}else{
					SubProcNo = BATTLE_SUBPROC_RECEIVE_BC;		//ＢＣに
					BattleCliTurnNo++;							// クライアントタ`ンMめる
				//}
#ifdef _DEBUG
				if(offlineFlag == TRUE){
					SubProcNo = BATTLE_SUBPROC_RECEIVE_BC;		//メニュ`に椁胜
				}
				
#endif
				action_inf = 0;
				// バトルコマンド初期化
				BattleCmd[ 0 ] = NULL;
				// メニュ`フラグＯＮ 
				battleMenuFlag2 = TRUE;
			}
			
			//殛L脱出なら
			if( action_inf == 2 ){		
				SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;		//殛LK了演出へ
				action_inf = 0;
				// バトルコマンド初期化
				BattleCmd[ 0 ] = NULL;
			}
			
			// Qrに右クリックされたr
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// Q榻K了プロトコル送る
			//	lssproto_B_send( sockfd, "U" );
			//}
			break;
			
		case BATTLE_SUBPROC_OUT_PRODUCE_INIT:	// K了演出初期化
			
			//ＢＧＭフェ`ドアウト
			fade_out_bgm();
			// バトルサ`フェスの画像作成 
			CopyBackBuffer();
			// バックバッファ`描画方法涓
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
			break;
			
		case BATTLE_SUBPROC_OUT_PRODUCE:		// K了演出
			
			// 演出中
			if( DrawProduce( PRODUCE_4WAY_OUT ) == TRUE ){
				// プロセスチェンジ
				ChangeProc( PROC_GAME, 1 );
				// アクション全抹
				DeathAllAction();
				// キャラ管理テ`ブルのアクションを抹消
				clearPtActCharObj();
				// この後からのＣ、ＣＡプロトコルを受け付ける
				encountNowFlag = 0;
				// クライアントが最後のム`ビ`をKわったときに送る。
				lssproto_EO_send( sockfd, 0 );
				//フィ`ルドのＢＧＭ再生
//				play_bgm(map_bgm);		//フィ`ルドＢＧＭ再生
			//	SubProcNo++;
				// 殛LY果ウィンドウ出すr（逃げたr、アルチメットのr出さない
				if( battleResultMsg.useFlag >= 1 ){ 
					BattleResultWndFlag = battleResultMsg.useFlag;	// メニュ`フラグＯＮ
					battleResultMsg.useFlag = FALSE; // 造体のフラグ初期化
				}
				return;
			}
			break;
			
		case 12:	// 演出
			break;
	}
	
	// プロデュ`ス中でないとき  演出中没有的时候
	if( BackBufferDrawType != DRAW_BACK_PRODUCE ){ 
		// 殛L制K了のr
		if( BattleEscFlag == TRUE ){	
			// K了プロセスへ
			SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;
			// フラグ初期化
			BattleEscFlag = FALSE;
		}
		// Qrに右クリックされたr
		if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
			// Q榻K了プロトコル送る
			lssproto_B_send( sockfd, "U" );
		}
		// ウィンドウあるr
		if( pActAudienceExitWnd != NULL ){
			// Q榻K了文字表示ウィンドウが出来あがっていたら
			if( pActAudienceExitWnd->hp > 0 ){
				// 文字を表示
				StockFontBuffer( pActAudienceExitWnd->x + 30, pActAudienceExitWnd->y + 28, FONT_PRIO_FRONT, 0, 	"右クリックで", 0 );
				StockFontBuffer( pActAudienceExitWnd->x + 30, pActAudienceExitWnd->y + 52, FONT_PRIO_FRONT, 0, 	"Q椁蚪K了します", 0 );
				//StockFontBuffer( pActInfoWnd->x + 38, pActInfoWnd->y + 28, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	"タ`ゲットを", 0 );
				//StockFontBuffer( pActInfoWnd->x + 38, pActInfoWnd->y + 52, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	"xkして下さい", 0 );
			}
		}
	}
}

