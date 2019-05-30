/************************/
/*	process.h			*/
/************************/
#ifndef _PROCESS_H_
#define _PROCESS_H_

/* プロセスのN */
enum{
	PROC_INIT,
	PROC_ID_PASSWORD,				// ＩＤ、パスワ`ド入力I理
	PROC_TITLE_MENU,				// タイトル画面メニュ`表示
	PROC_CHAR_SELECT,				// キャラxk
	PROC_CHAR_MAKE,					// キャラ作成
	PROC_CHAR_LOGIN_START,			// キャラログイン_始
	PROC_CHAR_LOGIN,				// キャラログイン中
	PROC_CHAR_LOGOUT,				// キャラログアウト
	PROC_OPENNING,
	PROC_GAME,						// ゲ`ムプロセス
	PROC_BATTLE,					// バトルプロセス
	PROC_DISCONNECT_SERVER,			// ネットワ`クが切断されたrのI理

#ifdef _DEBUG		

	PROC_TAKE_TEST,					// 竹内テスト
	PROC_OHTA_TEST,					// 太田テスト
	PROC_DWAF_TEST,					// 松村テスト
	PROC_SPR_VIEW,					// スプライト_Jプロセス
	PROC_ANIM_VIEW,					// アニメ`ション_Jプロセス
	PROC_SE_TEST,					// ＳＥ_Jプロセス
	
#endif
	PROC_ENDING
	
};

/* プロセス番号 */
extern UINT ProcNo;
/* サブプロセス番号 */
extern UINT SubProcNo;

extern int palNo;
extern int oldPalNo;


/* プロセスI理 */
void Process( void );

/* プロセス涓I理 */
void ChangeProc( int procNo );
/* プロセス涓I理 */
void ChangeProc( int procNo, int subProcNo );

/* ゲ`ムプロセスI理 */
void GameProc( void );

// ワ`プ演出用
void warpEffectProc( void );

void paletteProc( void );

#ifdef _DEBUG		

/* 竹内さんテストプロセス */
void TakeTestProc( void );

/* 太田テストプロセス */
void OhtaTestProc( void );

#endif

#endif

