/************************/
/*	ohta_proc.cpp		*/
/************************/
#include "system.h"
#include "ohta.h"
#include "ime_sa.h"
#include "map.h"
#include "menu.h"

/* 太田プロセスI理 ********************************************************************/
void OhtaTestProc( void )
{
	/* サブプロセス番号で分岐 */
	switch( SubProcNo ){
	
		case 0:
		
			/* 自C タスク作成 */
			MakeJiki();
			SubProcNo++;
			/* チャットvB初期化 */
			InitChat();
			// 入力フォ`カス取得
			GetKeyInputFocus( &MyChatBuffer );
			
			break;
		
		case 1:
			// キ`ボ`ドカ`ソル点I理
			FlashKeyboardCursor();
			
			// マウスレベルがメニュ`以下のr
			if( mouse.level < DISP_PRIO_MENU && TaskBarFlag == FALSE ){
				drawGrid();		// グリッドカ`ソル表示
				moveProc();		// 移I理
			}
			
			/* アクション走らせる */
			RunAction();
			// タスク表示デ`タをバッファに溜める
			StockTaskDispBuffer();
			
			drawMap();	// マップ表示

			/* チャットI理 */
			ChatProc();
			// チャットバッファ`をフォントバッファに溜める
			ChatBufferToFontBuffer(); 
			// メニュ`I理
			MenuProc();
			// ＩＭＥvBI理
			ImeProc();		
			// パレットI理
			PaletteProc();
			
			break;
	}
}


