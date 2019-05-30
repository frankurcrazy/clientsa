/************************/
/*	ime.h				*/
/************************/
#ifndef _IME_H_
#define _IME_H_

// ＩＭＥのハンドル  IME的方向盘
//Handle to input context.
extern HIMC hImc;

// ＩＭＥバッファ` 缓冲器  我修改 _asm
extern char ImeBuffer[];
//extern char *ImeBuffer;

// ＩＭＥ文バッファ`   短语缓冲器
extern char ImeBlock[];
// ＩＭＥバッファ`バックアップ  备份缓冲器
extern char ImeBufferBak2;

// ＩＭＥ入力モ`ド取得用  输入方式取得用
extern DWORD conversion;
extern DWORD sentence;

// ＩＭＥvBの初期化 *********************************************************/
void InitIme( void );

// ＩＭＥのK了 ***************************************************************/
void EndIme( void );

//*****************************************************************************/
// ＩＭＥのＯＮ、ＯＦＦ状Bの_J 
// 	ＯＮ　：TRUE
// 			ＯＦＦ：FALSE
//*****************************************************************************/
BOOL ImeOpenState( void );

//*****************************************************************************/
// ＷＩＮＤＯＷＳのＩＭＥバッファ`をクリアする 
//*****************************************************************************/
void ClrImeCompositionString( void );

// WM_IME_COMPOSITION メッセ`ジのI理 ****************************************/
void ImeComposition( LPARAM lParam );

// ＩＭＥvBI理 *************************************************************/
void ImeProc( void );

/** ＩＭＥvBメッセ`ジI理 **************************************************/
//void ImeMsgProc( UINT Message, WPARAM wParam, LPARAM lParam );

#endif

