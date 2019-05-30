/************************/
/*	battleMenu.h		*/
/************************/
#ifndef _BATTLE_MENU_H_
#define _BATTLE_MENU_H_

// BattleBpFlagのビット情
#define BATTLE_BP_JOIN				( 1 << 0 )		// 参
#define BATTLE_BP_PLAYER_MENU_NON	( 1 << 1 )		// プレイヤ`メニュ`なし
#define BATTLE_BP_BOOMERANG			( 1 << 2 )		// ブ`メラン
#define BATTLE_BP_PET_MENU_NON		( 1 << 3 )		// ペットメニュ`なし
#define BATTLE_BP_ENEMY_SURPRISAL	( 1 << 4 )		// 长灰獯颏膜いr（不意つかれたr）
#define BATTLE_BP_PLAYER_SURPRISAL	( 1 << 5 )		// プレイヤ`が不意打をついたr

// バトルコマンドサイズ
#define BATTLE_BUF_SIZE	4
#define BATTLE_COMMAND_SIZE			4096
// バトルコマンド文字列
extern char BattleCmd[ BATTLE_COMMAND_SIZE ];
extern char BattleCmdBak[ BATTLE_BUF_SIZE ][ BATTLE_COMMAND_SIZE ];
// バトルコマンド文字列ポインタ`
extern int BattleCmdReadPointer;
extern int BattleCmdWritePointer;
// バトルコマンド文字列バックアップ
//extern char BattleCmdBak[];
// バトル状B文字列
extern char BattleStatus[ BATTLE_COMMAND_SIZE ];
extern char BattleStatusBak[ BATTLE_BUF_SIZE ][ BATTLE_COMMAND_SIZE ];
// バトル状B文字列ポインタ`
extern int BattleStatusReadPointer;
extern int BattleStatusWritePointer;
// バトル状B文字列バックアップ
//extern char BattleStatusBak[];
// バトル状B文字列バックアップ
//extern char BattleStatusBak2[];
// バトルメニュ`フラグ
extern int battleMenuFlag2;
// 自分の配列番号
extern int BattleMyNo;
// 殛Lrの自分のＭＰ
extern int BattleMyMp;
// 殛L制K了フラグ
extern int BattleEscFlag;
// ペットスタメンカウンタ`
extern int BattlePetStMenCnt;
// 参楗讠氓确钮啸氓氓
extern int battlePetNoBak;
// 参楗讠氓确钮啸氓氓祝
extern int battlePetNoBak2;

// ＢＰフラグ
extern int BattleBpFlag;
// コマンド入力gみフラグ
extern int BattleAnimFlag;
// タ`ン受信フラグ
extern BOOL BattleTurnReceiveFlag;
// F在のクライアントタ`ン番号
extern int BattleCliTurnNo;
// F在のサ`バ`タ`ン番号
extern int BattleSvTurnNo;

// 殛LY果ウィンドウフラグ
extern BOOL BattleResultWndFlag;

// バトルメニュ`I理 *********************************************************/
void BattleMenuProc( void );
// ボタンフラグ全初期化　*****************************************************/
void ClearBattleButton( void );
// バトルメニュ`初期化 ********************************************************/
void InitBattleMenu( void );
// コマンド入力gみフラグのチェック ********************************************/
void CheckBattleAnimFlag( void );
// 一人用か二人用かチェック ********************************************/
int CheckBattle1P2P( void );
// 耐久力メ`タ`表示 ***********************************************************/
void HpMeterDisp( int no );

#endif


