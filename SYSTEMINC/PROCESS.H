/************************/
/*	process.h			*/
/************************/
#ifndef _PROCESS_H_
#define _PROCESS_H_

/* プロセスの�N�� */
enum{
	PROC_INIT,
	PROC_ID_PASSWORD,				// ＩＤ、パスワ�`ド秘薦�I尖
	PROC_TITLE_MENU,				// タイトル鮫中メニュ�`燕幣
	PROC_CHAR_SELECT,				// キャラ�x�k
	PROC_CHAR_MAKE,					// キャラ恬撹
	PROC_CHAR_LOGIN_START,			// キャラログイン�_兵
	PROC_CHAR_LOGIN,				// キャラログイン嶄
	PROC_CHAR_LOGOUT,				// キャラログアウト
	PROC_OPENNING,
	PROC_GAME,						// ゲ�`ムプロセス
	PROC_BATTLE,					// バトルプロセス
	PROC_DISCONNECT_SERVER,			// ネットワ�`クが俳僅された�rの�I尖

#ifdef _DEBUG		

	PROC_TAKE_TEST,					// 幢坪テスト
	PROC_OHTA_TEST,					// 湊弥テスト
	PROC_DWAF_TEST,					// 防翫テスト
	PROC_SPR_VIEW,					// スプライト�_�Jプロセス
	PROC_ANIM_VIEW,					// アニメ�`ション�_�Jプロセス
	PROC_SE_TEST,					// ＳＥ�_�Jプロセス
	
#endif
	PROC_ENDING
	
};

/* プロセス桑催 */
extern UINT ProcNo;
/* サブプロセス桑催 */
extern UINT SubProcNo;

extern int palNo;
extern int oldPalNo;


/* プロセス�I尖 */
void Process( void );

/* プロセス�筝��I尖 */
void ChangeProc( int procNo );
/* プロセス�筝��I尖 */
void ChangeProc( int procNo, int subProcNo );

/* ゲ�`ムプロセス�I尖 */
void GameProc( void );

// ワ�`プ處竃喘
void warpEffectProc( void );

void paletteProc( void );

#ifdef _DEBUG		

/* 幢坪さんテストプロセス */
void TakeTestProc( void );

/* 湊弥テストプロセス */
void OhtaTestProc( void );

#endif

#endif

