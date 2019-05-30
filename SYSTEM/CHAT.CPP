/************************/
/*	chat.c		聊天		*/
/************************/
#include "system.h"
#include "font.h"
#include "ime_sa.h"
#include "netproc.h"
#include "battleProc.h"
#include "savedata.h"
#include "time.h"
#include "menu.h"
#include "t_music.h"
#include "t_music.h"
#include "tool.h"
#include "lssproto_cli.h"
#include "netmain.h"

// チャットバッファ`  聊天缓冲器
CHAT_BUFFER ChatBuffer[ MAX_CHAT_LINE ];

// 自分の入力するバッファ`   自己输入缓冲区
STR_BUFFER MyChatBuffer;
//现在的输入缓冲对焦点的指针
// F在の入力フォ`カスバッファへのポインタ 
STR_BUFFER *pNowStrBuffer = NULL;

// チャット行カウンタ`  行/ /聊天柜台
int NowChatLine = 0;
// 初期行数
int NowMaxChatLine = DEF_CHAT_LINE;
// 初期声の大きさ 初期的声音的大小
int NowMaxVoice = DEF_VOICE;
// カ`ソル点绁Ε螗  光标闪烁计数
int CursorFlashCnt = 0;
// スム`ズスクロ`ル座  顺畅滚动坐标
int ChatLineSmoothY = 0;


//聊天记录文件名
#define CAHT_HISTORY_STR_FILE_NAME 	"data\\chathis.dat" 	// チャットの履sファイルネ`ム
// チャットのヒストリ`造体
//聊天的历史结构
CHAT_HISTORY ChatHistory;

// ＴＡＢキ`I理 TAB键处理
void KeyboardTab( void );

// ログファイル  日志文件
FILE *chatLogFile = NULL;
char chatLogFileName[] = "CHAT_000000.TXT";
void openChatLogFile( void );

#if 1
// 半角カナから全角カナQ用文字列  
//半角片假名开始全角假名转换用文字列
char *ZenkakuKana = "?。「」、?ヲァィゥェォャュョッ"
					"`アイウエオカキクケコサシスセソ"
					"タチツテトナニヌネノハヒフヘホマ"
					"ミムメモヤユヨラリルレロワンab";
#endif

/* チャットvB初期化 **聊天相关初期化********************************************************/
void InitChat( void )
{
	// バッファ`クリア 缓冲清除
	memset( ChatBuffer, 0, sizeof( CHAT_BUFFER ) * MAX_CHAT_LINE );
	memset( &MyChatBuffer, 0, sizeof( STR_BUFFER ) );
	// チャット入力バッファ`の初期化  聊天输入数据初始化
	//我修改原始70
	MyChatBuffer.len = 70;	// 文字列のLさ  字串长度太长屏幕不够
	//MyChatBuffer.len = 200;	// 文字列のLさ  字串长度
	//MyChatBuffer.len = 254;	// 文字列のLさ
	//MyChatBuffer.color = 0; // 色
	MyChatBuffer.fontPrio = FONT_PRIO_BACK;
	MyChatBuffer.x = 8; 	// Ｘ座
	MyChatBuffer.y = 432; 	// Ｙ座
	//MyChatBuffer.x = 8; 	// Ｘ座
	//MyChatBuffer.y = 200; 	// Ｙ座
	//MyChatBuffer.lineLen = 25; 	// Ｘ座
	//MyChatBuffer.lineDist = 16; 	// Ｙ座

	// 保存されているチャット情螭蛘iみこむ  保存着聊天信息读拥挤
	getUserChatOption();
	
	// チャットバッファに送られた文字列をログファイルに出力
	//聊天缓冲被送到了文字列日志文件输出
	openChatLogFile();
	
}


/* チャットログファイルのオ`プン **聊天日志文件的开放********************************************/
void openChatLogFile( void )
{
	if( chatLogFile == NULL )
	{
#ifdef _DEBUG
		// 今は起rに新鞒
		chatLogFile = fopen( chatLogFileName, "wt" );
#else
		struct tm *nowTime;
		time_t longTime;

		// F在の日rを取得
		time( &longTime );
		nowTime = localtime( &longTime );

		sprintf( chatLogFileName, "CHAT_%02d%02d%02d.TXT",
			(nowTime->tm_year % 100), nowTime->tm_mon+1, nowTime->tm_mday );

		// 今は起rに新鞒  现在在启动时新建
		chatLogFile = fopen( chatLogFileName, "a" );
		if( chatLogFile )
		{
			fprintf( chatLogFile, "----- Login: %02d/%02d/%02d %02d:%02d:%02d -----\n",
				(nowTime->tm_year % 100), nowTime->tm_mon+1, nowTime->tm_mday,
				nowTime->tm_hour, nowTime->tm_min, nowTime->tm_sec );
		}
#endif
	}
}

// チャットの履s文字の保存 ****************************************************************/
BOOL SaveChatHistoryStr( int no )
{
	FILE *fp; 
	
	// zみファイルオ`プン
	if( ( fp = fopen( CAHT_HISTORY_STR_FILE_NAME, "r+b" ) ) == NULL ){
		return FALSE;
	}
	
	// 今回zむ鏊蓼钎榨ˉぅ毳荪ぅ螗郡蜻Mませる
	fseek( fp, sizeof( ChatHistory.str[ 0 ] ) * no, SEEK_SET );
	// 文字列デ`タzみ
	if( fwrite( &ChatHistory.str[ no ], sizeof( ChatHistory.str[ 0 ] ), 1, fp ) < 1 ){
	
		fclose( fp );// ファイルクロ`ズ
		return FALSE;
	}
	
	// 最新の履s番号の鏊蓼钎榨ˉぅ毳荪ぅ螗窟Mませる
	fseek( fp, sizeof( ChatHistory.str[ 0 ] ) * MAX_CHAT_HISTORY, SEEK_SET );
	// 最新の履s番号の保存
	if( fwrite( &no, sizeof( int ), 1, fp ) < 1 ){
	
		fclose( fp );// ファイルクロ`ズ
		return FALSE;
	}
	
	// ファイルクロ`ズ
	fclose( fp );
	
	return TRUE;
}

// チャットの履s文字のiみzみ ****************************************************************/
BOOL LoadChatHistoryStr( void )
{
	FILE *fp;

	// F在のチャット履s番号の初期化
	ChatHistory.nowNo = -1;

	// iみzみファイルオ`プン
	if( ( fp = fopen( CAHT_HISTORY_STR_FILE_NAME, "rb" ) ) == NULL ){
		// ないr、制的に作成
		if( ( fp = fopen( CAHT_HISTORY_STR_FILE_NAME, "wb" ) ) != NULL ){
			// デ`タzみ
			fwrite( &ChatHistory, sizeof( CHAT_HISTORY ) - sizeof( int ), 1, fp );
			fclose( fp );	// ファイルクロ`ズ
		}
		
		// 最新のチャット履s番号の初期化
		ChatHistory.nowNo = MAX_CHAT_HISTORY - 1;
		
		return FALSE;
	}
	
	// デ`タiみzみ
	if( fread( &ChatHistory, sizeof( CHAT_HISTORY ) - sizeof( int ), 1, fp ) < 1 ){
		
		fclose( fp );	// ファイルクロ`ズ
		return FALSE;
	}
	// ファイルクロ`ズ
	fclose( fp );
	
	return TRUE;
}

/* 文字列をF在の入力文字列に送る ****文字列为现在的输入文字列送************************************************/
void StrToNowStrBuffer( char *str )
{

	int strLen, i;
	
	// 文字のLさの取得
	strLen = strlen( str );
	// リミットチェック
	if( strLen >= 256 ) strLen = 255;
	// 文字の数だけル`プ
	for( i = 0 ; i < strLen ; i++ ){
		// 文字バッファに溜める
		StockStrBufferChar( str[ i ] );
	}
}

/* チャットvBI理 *****聊天相关处理*******************************************************/
void ChatProc( void )
{

	// リタ`ンキ`押されたr 按下回车键时
	//if( joy_trg[ 1 ] & JOY_RETURN ) KeyboardReturn();
	
	// ＴＡＢキ`押されたr
	if( joy_trg[ 1 ] & JOY_TAB ) KeyboardTab();
	
	// 入力フォ`カスがチャット又はメ`ルのr
	if( pNowStrBuffer == &MyChatBuffer || pNowStrBuffer == &MailStr ){
	
		// 登h文字列を使用したとき、登h文字列をF在の入力文字列に送る
		if(	joy_trg[ 1 ] & JOY_F1 ) StrToNowStrBuffer( chatRegistryStr[ 0 ].buffer );
		if( joy_trg[ 1 ] & JOY_F2 ) StrToNowStrBuffer( chatRegistryStr[ 1 ].buffer );
		if( joy_trg[ 1 ] & JOY_F3 ) StrToNowStrBuffer( chatRegistryStr[ 2 ].buffer );
		if( joy_trg[ 1 ] & JOY_F4 ) StrToNowStrBuffer( chatRegistryStr[ 3 ].buffer );
	
		if( joy_trg[ 1 ] & JOY_F5 ) StrToNowStrBuffer( chatRegistryStr[ 4 ].buffer );
		if( joy_trg[ 1 ] & JOY_F6 ) StrToNowStrBuffer( chatRegistryStr[ 5 ].buffer );
		if( joy_trg[ 1 ] & JOY_F7 ) StrToNowStrBuffer( chatRegistryStr[ 6 ].buffer );
		if( joy_trg[ 1 ] & JOY_F8 ) StrToNowStrBuffer( chatRegistryStr[ 7 ].buffer );
	}

	// 入力フォ`カスがチャットのr、ＩＭＥバッファに文字がoいr
	if( pNowStrBuffer == &MyChatBuffer && ImeBufferBak2 == NULL ){
		// 上キ`を押したr
		if( joy_auto[ 0 ] & JOY_UP ){
			int bak = ChatHistory.nowNo;	// バックアップ
			// ヒストリ`モ`ドに入ってないr
			if( ChatHistory.nowNo == -1 ) ChatHistory.nowNo = ChatHistory.newNo;
			else ChatHistory.nowNo--;
			// リミットチェック
			if( ChatHistory.nowNo < 0 ) ChatHistory.nowNo = MAX_CHAT_HISTORY - 1;
			// ヒストリ`があるrかつ、( 一周回ってないr又は、ヒストリ`モ`ドに入っていなかったr ）
			if( ChatHistory.str[ ChatHistory.nowNo ][ 0 ] != 0 && ( ChatHistory.nowNo != ChatHistory.newNo || bak == -1 ) ){
				// 入力してある文字を初期化
				pNowStrBuffer->cnt = 0;
				pNowStrBuffer->buffer[ 0 ] = NULL;
				// ヒストリ`文字列を入力バッファへ送る
				StrToNowStrBuffer( ChatHistory.str[ ChatHistory.nowNo ] );
			}else{
				// バックアップから
				ChatHistory.nowNo = bak;
			}
		}else
		// 下キ`を押したr
		if( joy_auto[ 0 ] & JOY_DOWN ){
			// ヒストリ`モ`ドに入っていたら
			if( ChatHistory.nowNo != -1 ){
				// ヒストリ`モ`ドをiけるr
				if( ChatHistory.nowNo == ChatHistory.newNo ){ 
					ChatHistory.nowNo = -1;
					// 入力してある文字を初期化
					pNowStrBuffer->cnt = 0;
					pNowStrBuffer->buffer[ 0 ] = NULL;
				}else{
					ChatHistory.nowNo++;
					// リミットチェック
					if( ChatHistory.nowNo >= MAX_CHAT_HISTORY ) ChatHistory.nowNo = 0;
					// 入力してある文字を初期化
					pNowStrBuffer->cnt = 0;
					pNowStrBuffer->buffer[ 0 ] = NULL;
					// ヒストリ`文字列を入力バッファへ送る
					StrToNowStrBuffer( ChatHistory.str[ ChatHistory.nowNo ] );
				}
			}
		}
	}
}

// キ`ボ`ドカ`ソル点I理 **键盘光标闪烁处理************************************************/
void FlashKeyboardCursor( void )
{
	
	int lastByte, strWidth = 0;
	
	// 入力鏊胜r
	if( pNowStrBuffer == NULL ) return;
	
	// カ`ソル点I理
	if( CursorFlashCnt >= 20 ){
		// ＩＭＥがＯＦＦのr（半角カ`ソル）
		if( ImeOpenState() == FALSE ){
			// 文字数オ`バ`のr
			if( pNowStrBuffer->cnt >= pNowStrBuffer->len ){
				// 文字列の最後が半角か全角かを{べる
				lastByte = GetStrLastByte( pNowStrBuffer->buffer );
				if( lastByte == 1 ) strWidth = FontHankakuWidth;
				else if( lastByte == 2 ) strWidth = FontZenkauWidth;
				
			}
			// カ`ソルをフォントバッファへ溜める
			StockFontBuffer( pNowStrBuffer->imeX - strWidth, pNowStrBuffer->imeY, pNowStrBuffer->fontPrio, pNowStrBuffer->color, "_" , 0 );
			
		}else{ 	// ＩＭＥがＯＮのr（全角カ`ソル）
		
			// 文字数オ`バ`のr
			if( pNowStrBuffer->cnt >= pNowStrBuffer->len ){
				// 文字列の最後が半角か全角かを{べる
				lastByte = GetStrLastByte( pNowStrBuffer->buffer );
				if( lastByte == 1 ) strWidth = FontHankakuWidth;
				else if( lastByte == 2 ) strWidth = FontZenkauWidth;
				
			}
			// ＩＭＥバッファに文字がoいr
			if( ImeBufferBak2 == NULL ){
				// カ`ソルをフォントバッファへ溜める
				StockFontBuffer( pNowStrBuffer->imeX + GetStrWidth( ImeBuffer ) - strWidth, pNowStrBuffer->imeY, pNowStrBuffer->fontPrio, pNowStrBuffer->color, "＿" , 0 );
			}else{
				// カ`ソルをフォントバッファへ溜める
				StockFontBuffer( pNowStrBuffer->imeX + GetStrWidth( ImeBuffer ), pNowStrBuffer->imeY, pNowStrBuffer->fontPrio, pNowStrBuffer->color, "＿" , 0 );
			}
		}
	}
	// カ`ソル点绁Ε螗
	CursorFlashCnt++;
	// リミットチェック
	if( CursorFlashCnt >= 40 ) CursorFlashCnt = 0;
}

// ＢＳキ`I理 ***************************************************************/
void KeyboardBackSpace( void )
{
	int byte;
	
	// 入力鏊胜r
	if( pNowStrBuffer == NULL ) return;
	
	// 最後の文字が半角か全角かを{べる
	byte = GetStrLastByte( pNowStrBuffer->buffer ); 
		
	// K端号を入れる
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt - byte ] = '\0';
	// 一文字
	pNowStrBuffer->cnt -= byte;
			
	// リミットチェック
	if( *( pNowStrBuffer->buffer ) == '\0' ) pNowStrBuffer->cnt = 0;
	
	// フラッシュカウンタ`梗ǖ愕疲
	CursorFlashCnt = 20;
}	

// ＴＡＢキ`I理 ***************************************************************/
void KeyboardTab( void )
{

	int i,flag = 0;
	
	// 入力鏊胜r 输入，没有地方点回来
	if( pNowStrBuffer == NULL ) return;
	
	// 文字登hチェック *******文字注册检查************************
	for( i = 0 ; i < MAX_CHAT_REGISTY_STR ; i++ ){
		if( pNowStrBuffer == &chatRegistryStr[ i ] ){ 
			flag = TRUE;
			break;
		}
	}
	// 文字登hのr ******文字登录时*************************
	if( flag == TRUE ){
		// シフトキ`されていたら   被 Shift键的话
		if( joy_con[ 1 ] & JOY_RSHIFT || joy_con[ 1 ] & JOY_LSHIFT ){
			i--;
			// リミットチェック 极限检查
			if( i < 0 ) i = MAX_CHAT_REGISTY_STR - 1;
		}else{
			i++;
			// リミットチェック
			if( i >= MAX_CHAT_REGISTY_STR ) i = 0;
		}
		// 入力フォ`カス移 输入焦点移动
		GetKeyInputFocus( &chatRegistryStr[ i ] );
	}
	
	// フラッシュカウンタ`梗ǖ愕疲
	CursorFlashCnt = 20;
}	

/* リタ`ンキ`I理 *******回车键处理*****************************************************/
void KeyboardReturn( void )
{
	char bakNo;
	
	// ＩＭＥバッファ`に文字があるr、  缓冲文字空的时候，回
	if( ImeBufferBak2 != NULL ) return;
	
	// チャット入力のr ***聊天输入的时候****************************
	if( pNowStrBuffer == &MyChatBuffer ){;
	
		// 空白行だと改行しない  换行和空白行不。
		if( pNowStrBuffer->cnt == 0 ) return;
		
		// K端号を入れる  终端记号放0
		pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = '\0';
		
		// 文字列をチャット行に送りこむ  文字列行送入聊天
		
		//StockChatBufferLine( pNowStrBuffer->buffer, MyChatBuffer.color );
		
#ifdef _DEBUG
	
		// バトルプロセスへ  战斗工序
		if( strstr( pNowStrBuffer->buffer, "[battlein]" ) ) EncountFlag = TRUE;
		if( strstr( pNowStrBuffer->buffer, "[battleout]" ) ){
			ChangeProc( PROC_GAME, 1 );
			DeathAllAction();	// アクション初期化  动作初期化
		} 
		
#endif	
		// バックアップ  备份
		bakNo = ChatHistory.newNo;
		// 次のバッファ番号へ  下一个缓冲对/ /号码
		ChatHistory.newNo++;
		// リミットチェック 极限检查


		if( ChatHistory.newNo >= MAX_CHAT_HISTORY ) ChatHistory.newNo = 0;
		// 前回と`う文字列のr 和上次不同的字符串的时候
		if( strcmp( pNowStrBuffer->buffer, ChatHistory.str[ bakNo ] ) != 0 )
		{
			// 今回の文字列をえる  这次的文字列记
			strcpy( ChatHistory.str[ ChatHistory.newNo ], pNowStrBuffer->buffer );
			// チャットの履s文字の保存  聊天的履历文字保存
			SaveChatHistoryStr( ChatHistory.newNo );
		}
		else
		{
			// バックアップから  从备份恢复
			ChatHistory.newNo = bakNo;
		}
		// ヒストリ`番号初期化  历史号码初期化
		ChatHistory.nowNo = -1;
	
		
#ifdef _DEBUG
		if( offlineFlag == FALSE )
#endif
		// チャット文字列送信  聊天字符串发送
		
		chatStrSendForServer( pNowStrBuffer->buffer, MyChatBuffer.color );
		
		// 左端に 左端返回
		pNowStrBuffer->cnt = 0;
		
		// 自分の入力するバッファ`をクリア  输入缓冲清楚自己的
		*( pNowStrBuffer->buffer )= '\0';
		// K端号を入れる 终端记号放
	}
	else
	
	// 称号涓r **********************************
	if( pNowStrBuffer == &shougouChange )
	{
		// ウィンドウ抹
		DeathAction( pActMenuWnd3 );
		pActMenuWnd3 = NULL;
		// 入力フォ`カス
		GetKeyInputFocus( &MyChatBuffer );
		// ウィンドウ]じる音
		play_se( 203, 320, 240 );
		// アクションの称号も涓
		//strcpy( pc.ptAct->freeName, shougouChange.buffer );
		// ＥＵＣにQ
		sjisStringToEucString( shougouChange.buffer );
		// 名前涓托
		lssproto_FT_send( sockfd, shougouChange.buffer ) ; /* ../doc/lssproto.html line 1792 */
	}
	else
	
	// 名前涓r **********名字变更的时************************
	if( pNowStrBuffer == &petNameChange )
	{
		// ウィンドウ抹
		DeathAction( pActMenuWnd3 );
		pActMenuWnd3 = NULL;
		// 入力フォ`カス
		GetKeyInputFocus( &MyChatBuffer );
		// ウィンドウ]じる音
		play_se( 203, 320, 240 );
		// ＥＵＣにQ
		sjisStringToEucString( petNameChange.buffer );
		// 名前涓托
		lssproto_KN_send( sockfd, petStatusNo, petNameChange.buffer ) ; /* ../doc/lssproto.html line 1792 */
	}
	else
	
	// メ`ルのr ***邮件的时候*******************************
	if( pNowStrBuffer == &MailStr )
	{
		// 最後の行のrは改行しない
		if( MailStr.cnt < MailStr.len - MailStr.lineLen ){
			// 左端のr
			if( MailStr.cnt % MailStr.lineLen == 0 ){
				// 一文字だけ入れる
				StockStrBufferChar( ' ' );
			}
			// 行の端までスペ`スを入れる
			while( MailStr.cnt % MailStr.lineLen ) StockStrBufferChar( ' ' );
		}
	}
	else
	
	// メ`ルのr **********邮件的时候*****难道和上面一样?*******************
	if( pNowStrBuffer == &MailStr )
	{
		// 最後の行のrは改行しない
		if( MailStr.cnt < MailStr.len - MailStr.lineLen ){
			// 左端のr
			if( MailStr.cnt % MailStr.lineLen == 0 ){
				// 一文字だけ入れる
				StockStrBufferChar( ' ' );
			}
			// 行の端までスペ`スを入れる
			while( MailStr.cnt % MailStr.lineLen ) StockStrBufferChar( ' ' );
		}
	}

	// ＩＤキ`入力のr ******键盘输入的时候**********************
	extern STR_BUFFER *idPasswordStr;
	extern short idKeyReturn;
	if( pNowStrBuffer == idPasswordStr )
	{
		idKeyReturn = 1;
	}
	
	// フラッシュカウンタ`梗ǖ愕疲
	CursorFlashCnt = 20;
	
}

/* 文字をバッファに溜める（１バイト）**文字缓冲储（1字节）****************************************/
void StockStrBufferChar( char c )
{

	static BOOL zenkakuFlag = FALSE; // 全角フラグ  全角转折点
	static BOOL zenkakuFlag2 = FALSE; // 全角フラグ
	
	// 入力鏊胜r  输入，没有地方点回来
	if( pNowStrBuffer == NULL ) return;
	
	// vSない文字はwばす 关系没有文字在飞  小于1 并且 小于1f
	if( 0x01 <= c && c <= 0x1f /* && c != 0x08 && c != 0x0d */ ){
		return;
	}
	
	// 前回が全角でなかったら  上次全角，如果没有
	if( zenkakuFlag == FALSE ){
		// 前回があと半角一文字だけ入力できる状Bで全角をはじいたr、今回も
		if( zenkakuFlag2 == TRUE ){
			zenkakuFlag2 = FALSE; // フラグＯＦＦ
			return;
		}
		// 全角のとき 全角的时候
		// 文字が半角か全角かを{べる
		if( GetCharByte( c ) == 2 ){
			// あと半角一文字だけ入力できるr、全角は入力できない
			if( pNowStrBuffer->cnt >= pNowStrBuffer->len - 1 ){
				zenkakuFlag2 = TRUE; // フラグＯＮ
				return;
			}
		// 半角のr
		}else{
			// 文字列最大のr、入力できない
			if( pNowStrBuffer->cnt >= pNowStrBuffer->len/* || zenkakuFlag2 == TRUE*/ ){
				//zenkakuFlag2 = FALSE; // フラグＯＦＦ
				return;
			}
#if 1
			// 半角カナのr、全角かなにQする
			if( ( char )0xa0 <= c && c <= ( char )0xdf ){ 
				// あと半角一文字だけ入力できるr、全角は入力できない
				if( pNowStrBuffer->cnt >= pNowStrBuffer->len - 1 )
				{
					return;
				}
				// 一バイト目をバッファ`に溜める
				//我去
				
			//	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = ZenkakuKana[ ( c - ( char )0xa0 ) * 2 ];
				// 次に入力するバッファ`へのポインタをMませる
			//	pNowStrBuffer->cnt++;
				// ２バイト目
			//	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = ZenkakuKana[ ( c - ( char )0xa0 ) * 2 + 1 ];
				// K端号を入れる
			//	pNowStrBuffer->buffer[ pNowStrBuffer->cnt + 1 ] = '\0';
	          
				// 次に入力するバッファ`へのポインタをMませる
			//	pNowStrBuffer->cnt++;
				// フラッシュカウンタ`梗ǖ愕疲
				
				CursorFlashCnt = 20;
				
				return;
			}
#endif
		}
	}
	
	// バッファ`に文字を溜める
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = c;
	// K端号を入れる
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt + 1 ] = '\0';
	
	// 次に入力するバッファ`へのポインタをMませる
	pNowStrBuffer->cnt++;
	// フラッシュカウンタ`梗ǖ愕疲
	CursorFlashCnt = 20;
	
	// 前回が全角でなかったら
	if( zenkakuFlag == FALSE ){
		// 全角のときフラグを立てておく
		// 文字が半角か全角かを{べる
		if( GetCharByte( c ) == 2 ) zenkakuFlag = TRUE;
			
	}else zenkakuFlag = FALSE;
	
}
//开放测试下
#if 0
/* チャット文字をバッファに溜める（２バイト）******聊天文字缓冲储（2字节）*****************************/
void StockChatBufferWord( WORD w )
{
	STR_BUFFER *chatBuffer = &ChatBuffer[ NowChatLine ];
	//STR_BUFFER *chatBuffer =(STR_BUFFER *) &ChatBuffer[ NowChatLine ];
	
	// カウントオ`バ`のr
//	if( ChatCnt >= CHAT_BUFFER_SIZE - 2 ) return;
	
	// チャット文字数バッファ`に文字を溜める
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = w;
	// K端号を入れる
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt + 2 ] = '\0';
	
	// 次に入力するバッファ`へのポインタをMませる
	pNowStrBuffer->cnt++;
}
#endif

/* チャット文字をバッファに溜める（一行）******聊天文字缓冲储（行）*********************************/
void StockChatBufferLine( char *str, unsigned char color )
{
	int splitPoint = 0;
	char splitStr[ STR_BUFFER_SIZE + 1 ];
	
	// 一行にГ蓼椁胜r  一行没有收录的时候
	if( strlen( str ) > MyChatBuffer.len )
	{
		// 分割文字列作成
		strncpy( splitStr, str, MyChatBuffer.len );
		// K端号を入れる
		*( splitStr + MyChatBuffer.len ) = NULL;
		// 最後の全角文字が分割されているr  最后的全角文字被分割的时候
		
		if( GetStrLastByte( splitStr ) == 3 )
		{ 
			// 一文字 一字型恢复
			splitPoint = MyChatBuffer.len - 1;
			// K端号を入れる
			*( splitStr + MyChatBuffer.len - 1 ) = NULL; 
		}
		else splitPoint = MyChatBuffer.len; 

      //我加 原始是上面的
      // splitPoint = MyChatBuffer.len;
		
		// 文字列をチャット行に送りこむ
		strncpy( ChatBuffer[ NowChatLine ].buffer, str, splitPoint );
		// K端号を入れる
		*( ChatBuffer[ NowChatLine ].buffer + splitPoint ) = NULL; 
	}
	else
	{
		// 文字列をチャット行に送りこむ
		strcpy( ChatBuffer[ NowChatLine ].buffer, str );
	}
	// チャットログファイルに出力
	if( chatLogFile )
	{
		fprintf( chatLogFile, "%s\n", ChatBuffer[ NowChatLine ].buffer );
	}
	// 色の指定
	ChatBuffer[ NowChatLine ].color = color;
	
	NowChatLine++;	// チャット改行
	// リミットチェック
	if( NowChatLine >= MAX_CHAT_LINE ) NowChatLine = 0;

	// 一行にГ蓼椁胜盲郡
	if( splitPoint != 0 ){
		// 次の行に送る文字列をチャット行に送りこむ（再I理）
		StockChatBufferLine( str + splitPoint, color );
	}
	// スム`ズスクロ`ル座
	//ChatLineSmoothY += 20;
	ChatLineSmoothY = 20;
	
}

/* チャット文字を全てクリアする *******聊天文字全部清除****************************************/
void ClearChatBuffer( void )
{
	int i;
	
	// チャットの行数分ル`プ
	for( i = 0 ; i < MAX_CHAT_LINE ; i++ ){
		// K端号を入れる
		ChatBuffer[ i ].buffer[ 0 ] = '\0';
	}
}

/* チャットバッファ`をフォントバッファに溜める **聊天缓冲字体缓冲蓄******************************/
void ChatBufferToFontBuffer( void )
{
	#define CHAT_ZOUBUN_Y 20;
	int i, j, k = 0;
	//char moji[ 256 ];
	int x = 8, y = 400; // 表示座
	
	// 激幛胄肖卧O定  开始写行的设定
	j = NowChatLine - 1;
	// リミットチェック
	if( j < 0 ) j = MAX_CHAT_LINE - 1;
	
	// スクロ`ル中は一行余分に表示 滚动中一行多余表示
	if( ChatLineSmoothY > 0 ) k = NowMaxChatLine + 1;
	else k = NowMaxChatLine;
	
	// リミットチェック  极限检查
	if( k > MAX_CHAT_LINE )  k = MAX_CHAT_LINE;
	
	// チャットの行数分ル`プ  聊天的行数分钟循环
	for( i = 0 ; i < k; i++ ){
	//for( i = 0 ; i < NowMaxChatLine; i++ ){
		// 何かも字があったら
		if( *ChatBuffer[ j ].buffer != NULL ){
			// フォントバッファへ溜める
			StockFontBuffer( x, y + ChatLineSmoothY, FONT_PRIO_BACK, ChatBuffer[ j ].color, ( char *)ChatBuffer[ j ].buffer, 0 );
		}
		y -= CHAT_ZOUBUN_Y;  // 座艘
		// 次のバッファ
		j--;
		// リミットチェック
		if( j < 0 ) j = MAX_CHAT_LINE - 1;
	}
	// スム`ズスクロ`ルのr
	if( ChatLineSmoothY > 0 ) ChatLineSmoothY--;
	
	// 何か文字があったら
	//if( *MyChatBuffer.buffer != NULL ){
		// 自分の入力をフォントバッファへ溜める
		StockFontBuffer2( &MyChatBuffer );
		//StockFontBuffer( MyChatBuffer.x, MyChatBuffer.y, FONT_PRIO_BACK, MyChatBuffer.color, ( char *)MyChatBuffer.buffer, 0 );
	//}
}

/*******************************************************************************/
/* 文字列の最後が半角か全角かを{べる
/* 	半角：１
/* 			全角：２
/* 			例外：３
/*******************************************************************************/
int GetStrLastByte( char *str )
{
	int byte = 0;
	
	// 文字列の最後までル`プ
	while( !( *str == '\0' ) ){
	//		( *str == ( char )0x81 && *( str + 1 ) == ( char )0x51 ) ) ){
	
		// 全角のとき
		if( ( char )0x80 <= *str && *str <= ( char )0x9f ||
			( char )0xe0 <= *str && *str <= ( char )0xff ){
			
			// 全角の文字が分割されているr
			if( *( str + 1 ) == NULL ) return 3; // 例外
			
			str += 2; 	// ２バイトwばす
			byte = 2;	// 全角です
			
		}else{	// 半角のとき
		
			str ++; 	// １バイトwばす
			byte = 1;	// 半角です
		}
	}
	return byte;
}

/*******************************************************************************/
/* 文字が半角か全角かを{べる
/* 	半角：１
/* 			全角：２  全角半角/文字吗？调查返回值/半角：1/全角：2
/*******************************************************************************/
int GetCharByte( char c )
{
	// 全角のとき
	if( ( char )0x80 <= c && c <= ( char )0x9f ||
		( char )0xe0 <= c && c <= ( char )0xff ){
		
		return 2;	// 全角です
		
	}else{	// 半角のとき
	
		return  1;	// 半角です
	}
}

/*******************************************************************************/
/* 文字列の横幅（ドット数）を{べる
/* 	ドット数
/*******************************************************************************/
int GetStrWidth( char *str )
{
	int width = 0;
	
	// 文字列の最後までル`プ
	while( !( *str == '\0' ) ){
	
		// 全角のとき
		if( ( char )0x80 <= *str && *str <= ( char )0x9f ||
			( char )0xe0 <= *str && *str <= ( char )0xff ){
			
			str += 2; 	// ２バイトwばす
			width += FontZenkauWidth; // 全角サイズを足す
			
		}else{	// 半角のとき
		
			str ++; 	// １バイトwばす
			width += FontHankakuWidth; // 半角サイズを足す
		}
	}
	
	return width;
}

/*******************************************************************************/
/* 入力フォ`カス取得  输入聚焦取得
/*******************************************************************************/
void GetKeyInputFocus( STR_BUFFER *pStrBuffer )
{
	
	// 入力鏊r
	//if( pNowStrBuffer != NULL ){
		// カ`ソルが残っているr
	//	if(	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] != '\0' ){
			// K端号を入れる
	//		pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = '\0';
	//	}
	//}
	// ＩＭＥバッファ`を初期化
	//*ImeBuffer = '\0';
	// 入力バッファポインタを涓
	pNowStrBuffer = pStrBuffer;
}


