// Dwaf Test Process

#include "system.h"
#include "dwafTestProc.h"
#include "ime_sa.h"
#include "menu.h"
#include "pc.h"
#include "character.h"

void dwafTestProc( void )
{
	/* サブプロセス番号で分岐  子进程/号码分歧*/*/
	switch( SubProcNo ){
	
		case 0:

			InitChat();
			// 入力フォ`カス取得  输入聚焦取得
			GetKeyInputFocus( &MyChatBuffer );
			SubProcNo++;
			break;
		
		case 1:

			drawMap();		// マップ表示  地图显示
			
			/* チャットI理   聊天处理*/
			ChatProc();
			// チャットバッファ`をフォントバッファに溜める  聊天缓冲字体缓冲蓄
			ChatBufferToFontBuffer(); 
			// メニュ`I理  菜单处理
			MenuProc();
			// ＩＭＥvBI理
			ImeProc();		
			
			break;
	}
}

