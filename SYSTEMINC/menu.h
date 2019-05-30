/************************/
/*	menu.h				*/
/************************/
#ifndef _MENU_H_
#define _MENU_H_

#include "pc.h"
#include "chat.h"


#define MAIL_STR_LEN 140		// メ`ル文字のLさ
#define MAIL_DATE_STR_LEN 20	// 日付の文字列のLさ
#define MAIL_MAX_HISTORY 20		// 履sの件数
#define MAX_CHAT_REGISTY_STR		8		// チャットの文字登hの数

// アイテムバッファ`造体
typedef struct{
	int 	x, y;			// 表示座
	int 	defX, defY;		// デフォルト座
	int 	bmpNo;			// 画像番号
	int 	dispPrio;		// 表示先
	BOOL	dragFlag;		// ドラッグフラグ
	BOOL	mixFlag;		// 合成フラグ
}ITEM_BUFFER;

// メ`ルの履s造体
typedef struct{
	char 	str[ MAIL_MAX_HISTORY ][ MAIL_STR_LEN + 1 ];			// メ`ルの履s文字列
	char 	dateStr[ MAIL_MAX_HISTORY ][ MAIL_DATE_STR_LEN + 1 ];	// 日付の文字列
	int 	noReadFlag[ MAIL_MAX_HISTORY ];							// 未iフラグ（受信ペットのグラフィック番号）
	int 	petLevel[ MAIL_MAX_HISTORY ];							// 受信ペットのグラフィック番号
	char 	petName[ MAIL_MAX_HISTORY ][ PET_NAME_LEN + 1 ];		// ペットの名前
	int 	itemGraNo[ MAIL_MAX_HISTORY ];							// 届いたアイテムのグラフィック番号
	int 	newHistoryNo;											// 最新履sの番号
}MAIL_HISTORY;

// ウィンドウ表示アクション予造体
typedef struct{
	int sizeX,sizeY;	// サイズ
	int titleNo;		// タイトル番号
	int titleX, titleY;	// タイトルの座
	int mx, my;			// 中心座
	int nowX, nowY;		// 大中のサイズ
	int cnt;			// 中心座
	int wndType;		// ウィンドウのN
	int hitFlag;		// 当たり判定有りoし
	int yesDispNo;		// はいの当たり判定番号
	int noDispNo;		// いいえの当たり判定番号
	int boxDispPrio;	// 大ボックスの表示先位
	int yesNoResult;	// YesNoのY果（０：いいえ　１：はい）
}WINDOW_DISP;

#define MAX_PET_KIND 	124		// ペットのN
#define MAX_PET_TBL 	181		// ペットテ`ブルのサイズ
#define MAX_PET_PAGE_SIZE 	8	// １ペ`ジに表示出きるペットの数。

// アルバム造体
typedef struct{
	char name[ PET_NAME_LEN + 1 ];	// ペットの名前
	char freeName[ PET_NAME_LEN + 1 ];	// ペットのあだ名
	int faceGraNo;					// ペットのグラフィック番号
	int level;						// レベル
	int maxHp;						// 最大ＨＰ
	int str;						// 攻牧
	int quick;						// 素早さ
	int def;						// 防御力
	int earth, water, fire, wind;	// 地水火L
	int flag;						// 登hフラグ（０：たことない １：た ２：写真撮った（新 ３：登hgみ ）
}PET_ALBUM;

// アルバム用ペットデ`タテ`ブル造体
typedef struct{
	int albumNo;					// アルバムのとうろく番号
	char name[ PET_NAME_LEN + 1 ];	// ペットの名前
	int faceGraNo;					// ペットのグラフィック番号
}PET_ALBUM_TBL;


// タスクバ`フラグ
extern BOOL TaskBarFlag;
// トグルフラグ
extern unsigned int MenuToggleFlag;
// アイテムバッファ`
extern ITEM_BUFFER ItemBuffer[];
// ステ`タスアップポイント
extern int StatusUpPoint;
// 参楗讠氓涓抓恁去偿胨托钮榨楗
extern BOOL BattlePetReceiveFlag;		
// バトル受信中のペット番号
extern int BattlePetReceivePetNo; 
// メ`ルの履s文字列
extern MAIL_HISTORY MailHistory[];	
// チャットの文字登hの入力バッファ`
extern STR_BUFFER chatRegistryStr[];
// メ`ルの入力文字列
extern STR_BUFFER MailStr;		
// ウィンドウアクションポインタ
extern ACTION *pActMenuWnd3;
// チャットの文字登hの入力バッファ`
extern STR_BUFFER chatRegistryStr[];
// プレイヤ`の称号用文字列
extern STR_BUFFER shougouChange;
// ペットの名前涓梦淖至
extern STR_BUFFER petNameChange;
// ウィンドウに表示されているペットの番号
extern int  petStatusNo;

extern int mailHistoryWndPageNo;	// 履sウィンドウのペ`ジ番号
extern int mailHistoryWndSelectNo;	// xkされている名刺番号	
//extern int mailReceivePetGraNo;		// ペットメ`ル受信rのペットのグラフィック番号

// 殛L後に自婴钎蕙氓抓Ε％螗杀硎兢工毳榨楗
extern int MapWmdFlagBak;

// 殛LY果ウィンドウが消えるまでのフレ`ム数
#define RESULT_WND_CLOSE_TIME 	300	// ５秒
// 殛LY果ウィンドウが消えるまでのタイマ`
extern int ResultWndTimer;

// 合成出来たフラグ
extern BOOL ItemMixRecvFlag;

// メニュ`vBの初期化 *******************************************************/
void InitMenu( void );

// メニュ`vBの初期化２ *******************************************************/
void InitMenu2( void );

// メニュ`出てるかチェック *******************************************************/
BOOL CheckMenuFlag( void );

// メニュ`I理 ***************************************************************/
void MenuProc( void );

// ウィンドウ表示タスク作成 ***************************************************/
ACTION *MakeWindowDisp( int x, int y, int sizeX, int sizeY, int titleNo, int wndType );

// 当たり判定ボックス作成 ******************************************************/
BOOL MakeHitBox( int x1, int y1, int x2, int y2, int dispPrio );

// メ`ルデ`タの保存 ****************************************************************/
BOOL SaveMailHistory( int no );

// メ`ルデ`タのiみzみ ****************************************************************/
BOOL LoadMailHistory( void );

// メ`ル未iチェック ****************************************************************/
BOOL CheckMailNoReadFlag( void );

// ログインrの初期化（太田用） **************************************************/
void InitOhtaParam( void );

// 手アニメ`ション抹 **********************************************************/
void DeathLetterAction( void ); 

// チャットの登h文字の保存 ****************************************************************/
BOOL SaveChatRegistyStr( void );

// チャットの登h文字のiみzみ ****************************************************************/
BOOL LoadChatRegistyStr( void );

// 文字列のセンタ`リング *****************************************************/
void CenteringStr( char *inStr, char *outStr, int max );

// 新讠氓趣违隶Д氓 ****************************************************************/
void CheckNewPet( int sprNo );

// 殛Lrの新讠氓去隶Д氓 **********************************************************/
void CheckBattleNewPet( void );

// アルバムのiみzみ ****************************************************************/
BOOL LoadAlbum( void );

// アルバムの保存 ****************************************************************/
BOOL SaveAlbum( int no );

#endif


