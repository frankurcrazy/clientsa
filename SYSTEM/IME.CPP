/************************/
/*	ime.cpp				*/
/************************/
#include "system.h"
#include "winnls32.h"
#include "font.h"
#include "ime_sa.h"
#include "menu.h"

// ＩＭＥのハンドル
HIMC hImc;
HIMC hImcBegin;

// ＩＭＥ入力モ`ド取得用
DWORD conversion;
DWORD sentence;

// ＩＭＥ入力モ`ド文字列  IME字符串输入模式
/*
static char *ImeModeStr[] = { 
								"    ??/abc","","",
								"    ??/???"","","","","", // Nuke.1
								" ??/ａｂｃ",
								" ??/あいう","",
								" ??/アイウ","","","","",
								"   ???/abc","","",
								"   ???/???"","","","","", // Nuke.2
								"???/ａｂｃ",
								"???/あいう","",
								"???/アイウ",
								};
// ＩＭＥ入力モ`ド文字列（Ｃａｐｓ用） IME输入方式文字列（Caps用）
static char *ImeModeStrCaps[] = { 
								"    ??/ABC","","",
								"    ??/???"","","","","", // Nuke.3
								" ??/ＡＢＣ",
								" ??/あいう","",
								" ??/アイウ","","","","",
								"   ???/ABC","","",
								"   ???/???"","","","","", // Nuke.4
								"???/ＡＢＣ",
								"???/あいう","",
								"???/アイウ",
								};*/

//IME字符串输入模式小写
static char *ImeModeStr[] = { 
	"    杜/abc","","",
								"    杜/辈?","","","","", // Nuke.1
								" 杜/",
								" 杜/","",
								" 杜/ACE","","","","",
								"   郯?abc","","",
								"   郯?辈?","","","","", // Nuke.2
								"郯?",
								"郯?","",
								"郯?ACE",
};
// IME输入方式文字列（Caps用）大写
static char *ImeModeStrCaps[] = { 
	"    杜/ABC","","",
								"    杜/辈?","","","","", // Nuke.3
								" 杜/`ab",
								" 杜/","",
								" 杜/ACE","","","","",
								"   郯?ABC","","",
								"   郯?辈?","","","","", // Nuke.4
								"郯?`ab",
								"郯?","",
								"郯?ACE",
								};
#if 0
// 半角カナから全角カナQ用文字列
char *ZenkakuKana = "?。「」、?ヲァィゥェォャュョッ"
					"`アイウエオカキクケコサシスセソ"
					"タチツテトナニヌネノハヒフヘホマ"
					"ミムメモヤユヨラリルレロワンab";
#endif

// ＩＭＥバッファ` IME缓冲器 我修改 _asm
char ImeBuffer[ 256 ];
//char *ImeBuffer;
// ＩＭＥ文バッファ`  短语缓冲器
char ImeBlock[ 256 ];
// ＩＭＥバッファ`バックアップ  缓冲备份 我修改 _asm
char ImeBufferBak2; 
//char *ImeBufferBak2;

// ＩＭＥvBの初期化 *********************************************************/
void InitIme( void )
{
	// バッファ`初期化  我修改 _asm

	ImeBuffer[ 0 ] = NULL;
	ImeBlock[ 0 ] = NULL;
	ImeBufferBak2 = NULL;
	
	// 以前にＩＭＥがあったら消す。   以前IME的话消失。
	if( hImc != NULL ){
		// ＩＭＥコンテキスト_放
		EndIme();
	}
	//hImc = ImmGetContext( hWnd );
	
	// ＩＭＥコンテキストの取得  上下文的取得
	hImc = ImmCreateContext();
	
#ifdef _DEBUG
	// 失・筏r  
	if( ( hImc = ImmCreateContext() ) == NULL )
	{
		MessageBox( hWnd, "IME初始化失败，太田为止，请联系", "_J", MB_OK | MB_ICONSTOP );
	}
#endif
	
	hImcBegin = ImmAssociateContext( hWnd , hImc );
	/*
	创建一个输入上下文。要把输入上下文分配给一个窗口，需要调用ImmAssociateContext函数。
	这个函数将返回一个句柄，这个句柄是先前与窗口关联的输入上下文的句柄。
	如果先前没有输入上下文与些窗口相关联，函数将返回的句柄将是默认输入上下文（的句柄）。
	通常，你需要保存这个句柄（默认句柄），当你不想再使用自己的输入上下文时，再把它与窗口
	重新关联上。
   */
	// ＩＭＥ  窗口消失
    //WINNLSEnableIME( hWnd, FALSE );
   	//WINNLSEnableIME( NULL, TRUE );

}
	
// ＩＭＥのK了 ***************************************************************/
void EndIme( void )
{
	// ＩＭＥコンテキスト_放
	ImmReleaseContext( hWnd, hImc );
	ImmAssociateContext( hWnd , hImcBegin );
	// ＩＭＥウィンドウ消す
    //WINNLSEnableIME( NULL, FALSE );
}

//*****************************************************************************/
// ＩＭＥのＯＮ、ＯＦＦ状Bの_J 
// 	ＯＮ　：TRUE
// 			ＯＦＦ：FALSE
//*****************************************************************************/
BOOL ImeOpenState( void )
{
    return ImmGetOpenStatus( hImc );
}
//*****************************************************************************/
// ＷＩＮＤＯＷＳのＩＭＥバッファ`をクリアする   缓冲通关
//*****************************************************************************/
void ClrImeCompositionString( void )
{
	// ＩＭＥがＯＮのr
    if( ImeOpenState() == TRUE ){
		// ＩＭＥＯＦＦにする
    	ImmSetOpenStatus( hImc, FALSE );
		// ＩＭＥＯＮにする
    	ImmSetOpenStatus( hImc, TRUE );
		// ＯＦＦにしてＯＮにするだけ
	}
}
#if 0
//*****************************************************************************/
// 半角カナがあるr、全角カナにQする  半角片假名的时候，全角假名转换
//*****************************************************************************/
void HanKanaToZenKana( char *str )
{
	char work[ 512 ];
	int i = 0, j = 0;
	
	// 文字数分ル`プ
	while( str[ i ] != NULL ){
		// 全角のとき
		// 文字が半角か全角かを{べる
		if( GetCharByte( str[ i ] ) == 2 ){
			// 文字バッファに溜める
			work[ j++ ] = str[ i++ ];
			work[ j++ ] = str[ i++ ];
		}else{
			// 半角カナのr、全角かなにQする
			if( ( char )0xa0 <= str[ i ] && str[ i ] <= ( char )0xdf )
			{ 
				// 文字バッファに溜める
				
				work[ j++ ] = ZenkakuKana[ ( str[ i ] - ( char )0xa0 ) * 2 ];
				work[ j++ ] = ZenkakuKana[ ( str[ i++ ] - ( char )0xa0 ) * 2 + 1 ];
				
				
			}
			else
			{
				// 文字バッファに溜める
				work[ j++ ] = str[ i++ ];
			}
		}
	}
	// K端号を入れる
	if( j >= 256 ) work[ 255 ] = NULL;
	else work[ j ] = NULL;
	
	// Qした文字列をコピ`する  转换后的文字复制
	strcpy( str, work );
}
#endif

// ＩＭＥvBI理 ********相关处理*****************************************************/
void ImeProc( void )
{

	//int strWidth;	// 文字列の横幅（ドット数）
	int strWidth2;	// 文字列の横幅（ドット数） 文字列的宽（点数）
	int blockXcnt = 0; 		// 文ボックスの座摔违Ε螗咯`  短语框的坐标的柜台
	int blockX1 = 0,blockX2 = 0; 	// 文ボックスの座  短语框的坐标
	int blockFlag = FALSE; 	// 文フラグ  短语转折点
	char *ImeBufferBak = ImeBuffer;	// 作I用
	char *ImeBlockBak = ImeBlock;	// 作I用
	BOOL imeOpenFlag;				// ＩＭＥのＯＮ、ＯＦＦフラグ
	BOOL capsLockFlag;				// capsLock のＯＮ、ＯＦＦフラグ
	char keyBoardState[ 256 ];		// キ`ボ`ドの状B  键盘的状态
	char imePrio;					// ＩＭＥボックスの表示先度  箱的显示优先度
	
	
	// バックアップ  备份 我修改 _asm

	ImeBufferBak2 = ImeBuffer[ 0 ];
	//ImeBufferBak2 = ImeBuffer;
	// ＩＭＥの状Bを（　ＯＮ、ＯＦＦ　）  记忆的状态
	imeOpenFlag = ImeOpenState();
	
	// capsLock の判定
	// キ`ボ`ドの状Bをとってくる（全てのキ`）
	//判定/ /键盘的状态抓起（所有密钥
	GetKeyboardState( ( BYTE *) keyBoardState );
	// capsLock 状B（　ＯＮ、ＯＦＦ　）  得到  capsLock 状B
	capsLockFlag = keyBoardState[ VK_CAPITAL ];
	
	// ＩＭＥ入力モ`ド文字列の表示
	// タスクバ`が非表示のr
	/// / IME输入文字列显示模式/ /任务栏不显示的时候
	if( TaskBarFlag == FALSE )
	{
		// ＩＭＥがＯＮのr   ON的时候
		if( imeOpenFlag )
		{
			// CapsLock のr   负责大小写的处理
			if( capsLockFlag )
			{
				if( conversion >= 28 )
				{
					// 文字表示バッファに溜める 文字表示缓冲蓄
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       ABC", 0 );
				}
				else
				{
					// 文字表示バッファに溜める
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, ImeModeStrCaps[ conversion ], 0 );
				}
			}
			else
			{
				if( conversion >= 28 )
				{
					// 文字表示バッファに溜める
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       abc", 0 );
				}
				else
				{
					// 文字表示バッファに溜める
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, ImeModeStr[ conversion ], 0 );
				}
			}
		}
		// ＩＭＥがＯＦＦのr
		else
		{	
			// CapsLock のr
			if( capsLockFlag )
			{
				// 文字表示バッファに溜める
				StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       ABC", 0 );
			}
			else
			{
				// 文字表示バッファに溜める
				StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       abc", 0 );
			}
		}
	}
	
	// ＩＭＥ文字列の表示
	// ＩＭＥがＯＮのrかつＩＭＥバッファに文字があるとき
	/// / IME字符串的表示/ / IME ON时且IME缓冲文字的时候
	if( imeOpenFlag && *ImeBuffer != NULL )
	{
		
		// 入力鏊胜r   输入，没有地方点回来
		if( pNowStrBuffer == NULL )
			return;
		
		// 入力文字の先度からＩＭＥボックスの先度をQめる  
		//输入文字的优先度IME框的优先度决定
		if( pNowStrBuffer->fontPrio == FONT_PRIO_BACK )
		{
			imePrio = DISP_PRIO_IME1;
		}
		else 
			imePrio = DISP_PRIO_IME3;
		
		// 文字列の横幅を求める
		//strWidth = GetStrWidth( pNowStrBuffer->buffer );
		// 文字表示バッファに溜める
		StockFontBuffer( pNowStrBuffer->imeX, pNowStrBuffer->imeY, pNowStrBuffer->fontPrio, 0, ImeBuffer, 0 );
		
		// 文字の背景を描画
		// ＩＭＥバッファ`の文字列の横幅を求める
		strWidth2 = GetStrWidth( ImeBuffer );
		//３２０＊２４０ドットモ`ドのr
		if(ResoMode == 1)
		{
			// ボックス表示デ`タをバッファに溜める
			StockBoxDispBuffer( pNowStrBuffer->imeX - 1, 
				pNowStrBuffer->imeY   , 
				pNowStrBuffer->imeX + strWidth2, 
				pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
				imePrio, 0, 1 );
		}
		else 
		{
			// ボックス表示デ`タをバッファに溜める
			StockBoxDispBuffer( pNowStrBuffer->imeX - 1, 
				pNowStrBuffer->imeY -1, 
				pNowStrBuffer->imeX + strWidth2, 
				pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
				imePrio, 0, 1 );
		}
		// 文の座摔稳〉   文节坐标的取得
		while( 1 )
		{
			// 文の始まりだったら  文节开始的话
			if( ( *ImeBlockBak == 1 || *ImeBlockBak == 3 ) && blockFlag == FALSE )
			{
				// 文の始まりの座
				blockX1 = blockXcnt;
				// 文フラグＯＮ
				blockFlag = TRUE;
				//blockFlag = *ImeBlockBak;
			}
			
			// 文Kわりだったら 文节结束的话
			if( ( *ImeBlockBak == 2 || *ImeBlockBak == NULL ) && blockFlag >= TRUE )
			{
				// 文のKわりの座
				blockX2 = blockXcnt;
				break;
			}
			
			// K了条件
			if( *ImeBlockBak == NULL ) break;
			
			// アドレスと座摔胃
			// 文字が全角のr / /地址和坐标的更新全角/ /文字的时候
			
			if( GetCharByte( *ImeBufferBak ) == 2 )
			{
				
				ImeBufferBak += 2;	// ２バイトMめる
				ImeBlockBak += 2;	// ２バイトMめる
				blockXcnt += FontZenkauWidth; // 全角ドット分足す全角圆点分加
				
			}

			else
			{ // 半角のとき
				
				ImeBufferBak += 1;	// １バイトMめる
				ImeBlockBak += 1;	// １バイトMめる
				blockXcnt += FontHankakuWidth; // 半角ドット分足す
			}
		}
		
		//文があるとき
		if( blockFlag >= TRUE ){
			//３２０＊２４０ドットモ`ドのr
			if(ResoMode == 1){
				// ボックス表示デ`タをバッファに溜める
				StockBoxDispBuffer( pNowStrBuffer->imeX + blockX1 - 1, 
									pNowStrBuffer->imeY    , 
									pNowStrBuffer->imeX + blockX2, 
									pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
									imePrio + 1, 252, 1 );
			} else {
				// ボックス表示デ`タをバッファに溜める
				StockBoxDispBuffer( pNowStrBuffer->imeX + blockX1 - 1, 
									pNowStrBuffer->imeY - 1, 
									pNowStrBuffer->imeX + blockX2, 
									pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
									imePrio + 1, 252, 1 );
			}
		}
	}
}
	
	
// WM_IME_COMPOSITION メッセ`ジのI理 **信息处理**************************************/
void ImeComposition( LPARAM lParam )
{
	
	int strLen;
	char str[ 256 ];
	int i;
	BOOL zenkakuFlag = FALSE; // 全角フラグ
	
	// 文字_定したとき 文字确定的时候
	if ( lParam & GCS_RESULTSTR ){
		// 文字のLさの取得  文字的长度的取得
		strLen = ImmGetCompositionString( hImc, GCS_RESULTSTR, NULL, 0 );
		// リミットチェック 极限检查
		if( strLen >= 256 ) strLen = 255;
		// 文字列を取ってくる  文字列拿来
		ImmGetCompositionString( hImc, GCS_RESULTSTR, str, strLen );
		// K端号
		str[ strLen ] = NULL;
		// 半角カナがあるr、全角カナにQする
		//HanKanaToZenKana( str );
		// Q後の文字のLさの取得  转换后的文字的长度的取得
		strLen = strlen( str );
		// リミットチェック
		if( strLen >= 256 ) strLen = 255;
		// 文字の数だけル`プ
		//pNowStrBuffer->buffer[256] =str[256];//我加  加上后服务端的返回NPC消息消失
		//我去 中文输入问题在这里
		
		for( i = 0 ; i < strLen ; i++ )
		{
			// 文字バッファに溜める  文字缓冲蓄 我去
			StockStrBufferChar( str[ i ] );
		}
	}
	
	// 表示されている文字（_定前） 显示的文字（确定前）
	if( lParam & GCS_COMPSTR )
	{
		
		// 文字のLさの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPSTR, NULL, 0 );
		// リミットチェック
		if( strLen >= 256 ) strLen = 255;
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPSTR, ImeBuffer, strLen );
		// K端号 我去
		ImeBuffer[ strLen ] = NULL;
			//pNowStrBuffer->buffer[256] =str[256];//我加
		// 半角カナがあるr、全角カナにQする
		//HanKanaToZenKana( ImeBuffer );
		
	}
//	else{	// 最後の一文字消したとき
		// 文字のLさの取得
//		strLen = ImmGetCompositionString( hImc, GCS_COMPSTR, NULL, 0 );
//		if( strLen == 0 ){
//			// 文字列を空にする
//			*ImeBuffer = NULL;
//		}
//	}
	
	// 文を取ってくる  短语拿来
	if( lParam & GCS_COMPATTR )
	{
		// 文字のLさの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPATTR, NULL, 0 );
		// リミットチェック
		if( strLen >= 256 ) strLen = 255;
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPATTR, ImeBlock, strLen );
		// K端号
		ImeBlock[ strLen ] = NULL;
	}
#if 0	
	// 振り⒚蛉〉
	if( lParam & GCS_COMPREADSTR ){
		// 文字のLさの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPREADSTR, NULL, 0 );
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPREADSTR, str, strLen );
	}
	
	if( lParam & GCS_COMPCLAUSE ){
		// K端号
		str[ 0 ] = NULL;
		// 文字のLさの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPCLAUSE, NULL, 0 );
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPCLAUSE, str, strLen );
	}
	
	if( lParam & GCS_COMPREADCLAUSE ){
		// 文字のLさの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPREADCLAUSE, NULL, 0 );
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPREADCLAUSE, str, strLen );
	}
#endif
}
	
/* WM_IME_NOTIFY メッセ`ジのI理 *****处理IME消息函数****************************************/
void ImeNotify( WPARAM wParam ,LPARAM lParam )
{
	switch ( wParam )
	{
	
	case IMN_OPENCANDIDATE:
		//这条留言，变换候补进入的小窗口打开的时机来
		// このメッセ`ジは、Q候aが入っているちっちゃいウインドウが_くタイミングで来る。
	case IMN_CHANGECANDIDATE:// このメッセ`ジは、Q候aを浃à毪郡婴死搐搿
		//这条留言，每次来改变变换候补。
		break;
		
	case IMN_CLOSECANDIDATE:
		// このメッセ`ジは、Q候aが入っているちっちゃいウインドウが消えるタイミングでくる。
		//这条留言，变换候补进入的小窗口消失的时机。
		break;
		
#if 0	
	case IMN_SETOPENSTATUS:// IMEを有郡摔筏郡o郡摔筏郡辘筏郡趣搐搿
		break;
#endif
    }
}
#if 0
/** ＩＭＥvBメッセ`ジI理 **************************************************/
void ImeMsgProc( UINT Message, WPARAM wParam, LPARAM lParam )
{
	// メッセ`ジで分岐
	switch( Message ){
	
		//IME的小窗口变化的时候
		case WM_IME_COMPOSITION: // ＩＭＥのちっちゃいウィンドウに浠r
	
			// WM_IME_COMPOSITION メッセ`ジのI理 信息处理
			ImeComposition( lParam );
			break;
		case WM_IME_ENDCOMPOSITION: // 文字を_定した直後に来る。Q中の文字を全部BSでけしたときもくる。
			// ＩＭＥバッファ`を空にする
			*ImeBuffer = NULL;
			break;
		
		case WM_IME_NOTIFY:
			/* WM_IME_NOTIFY メッセ`ジのI理 */
			//ImeNotify( wParam ,lParam );
			// ここにDEFWINDOWPROCがあると、Q候aウインドウを出すようだ。
			break;
		case WM_IME_SETCONTEXT:
			//初期化したとき来てるのかな？フォ`カスが移ったときも来るようだ。
			break;
#if 0
		case WM_IME_STARTCOMPOSITION:// IMEがONの状Bで、文字を入力した直後に来る。
			break;
		case WM_IME_CONTROL:
			break;
		case WM_IME_SELECT: // IMEをえらんだとき来るらしい。バッファを初期化とかすればいいのかな？
			break;
		case WM_IME_KEYDOWN:   // IMEのキ`コ`ド
			// ここにDefWindowProcがあると、WM_KEYDOWNを生成する
			break;
		case WM_IME_CHAR:
			// ここにDefWindowProcがあると、WM_CHARを生成する
			break;
#endif
	}
}
#endif

