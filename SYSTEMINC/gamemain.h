// gamemain.cpp ヘッダファイル

#ifndef _GAMEMAIN_H_
#define _GAMEMAIN_H_

// ゲ`ムの状B
enum{
	GAME_LOGIN,					// ログイン
	GAME_FIELD,					// フィ`ルドマップ
	GAME_FIELD_TO_ENCOUNT,		// フィ`ルドマップからエンカウント演出中
	GAME_ENCOUNT_TO_BATTLE,		// エンカウント演出から殛L
	GAME_BATTLE				// エンカウント演出から殛L
};


extern int	GameState;	// ゲ`ムの状B
extern double NowTime;	// F在のrg

//---------------------------------------------------------------------------//
// v数プロトタイプ宣言                                                      //
//---------------------------------------------------------------------------//
BOOL GameMain( void );				// ゲ`ムI理メインv数

// ゲ`ム_始I理
BOOL InitGame( void );

// ゲ`ムK了I理
void EndGame( void );

#ifdef _DEBUG		
// 情蟊硎
void InfoDisp( void );
#endif

// フレ`ムレ`トの算（外人が作成）
void DisplayFrameRate( void );

#endif

