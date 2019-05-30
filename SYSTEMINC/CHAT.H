/************************/
/*	chat.h				*/
/************************/
#ifndef _CHAT_H_
#define _CHAT_H_
//聊天缓冲区大小
#define STR_BUFFER_SIZE 	256 		// チャットバッファサイズ
//聊天的行数
#define MAX_CHAT_LINE 		20 			// チャットの行数
//初期设定聊天的行数
#define DEF_CHAT_LINE 		20 			// 初期O定チャットの行数
//聊天历史的数量
#define MAX_CHAT_HISTORY 	64			// チャットヒストリ`の数
//聊天的声音的大小
#define DEF_VOICE 			3			// チャットの声の大きさ
//声音的最大值
#define MAX_VOICE 			5			// チャットの声の最大

// 文字列バッファ`造体  字符串缓冲结构
typedef struct{
	char 	buffer[ STR_BUFFER_SIZE ]; // 一行分の文字列をする 一行分的文字列的记忆
	unsigned char 	len;		// 文字のLさ  文字的长度
	unsigned char 	lineLen;	// 一行の文字のLさ 一行的文字的长度
	unsigned char 	lineDist;	// 行と行のg隔さ（Ｙサイズ）行行的间隔（Y尺寸）
	unsigned char 	cnt;		// F在の文字の数 现在的文字数
	unsigned char 	color;		// 文字の色 文字的颜色
	int	x, y;					// 文字列の座  文字列的坐标
	int	imeX, imeY;				// ＩＭＥ文字列の表示_始座    IME文字的显示开始坐标
	int	fontPrio;				// 表示の先度  表示的优先度
	BOOL blindFlag;				// 文字をえなくする（＊＊＊＊＊）  文字看不见的（＊＊＊＊＊）
	int hitFontNo;				// 自分の配列の番号をえる（当たり判定用） 自己的排列的电话号码记（每判定用）
	
}STR_BUFFER;

// 文字列バッファ`造体  字符串缓冲结构
typedef struct{
	char 	buffer[ STR_BUFFER_SIZE + 1 ]; // 一行分の文字列をする  一行分的文字列的记忆
	unsigned char 	color;	// 文字の色
}CHAT_BUFFER;

// チャットのヒストリ`造体  聊天的历史
typedef struct{
	char str[ MAX_CHAT_HISTORY ][ STR_BUFFER_SIZE + 1 ];	// チャットのヒストリ`文字列  聊天的历史字符串
	int newNo;												// 最新のチャットヒストリ`番号  最新的聊天历史号码
	int nowNo;												// F在のチャットヒストリ`番号  现在聊天历史号码
}CHAT_HISTORY;

// F在のキ`ボ`ドフォ`カスバッファへのポインタ  现在的键盘/ /焦点缓冲的指针
extern STR_BUFFER *pNowStrBuffer;

// チャットバッファ`  / /聊天缓冲器
extern CHAT_BUFFER ChatBuffer[];
// 自分の入力するバッファ`  自己输入缓冲区
extern STR_BUFFER MyChatBuffer;

// チャット行カウンタ`  行/ /聊天柜台
extern int NowChatLine;

// F在のチャットの行数  现在聊天的行数
extern int NowMaxChatLine;

// 初期声の大きさ 初期的声音的大小
extern int NowMaxVoice;

/* 文字をバッファに溜める（１バイト）   文字缓冲储（1打工*/
void StockStrBufferChar( char c );
//我开
#if 0
/* 文字をバッファに溜める（２バイト）  文字缓冲储（2打工*/
void StockChatBufferWord( WORD w );
#endif

/* チャットバッファ`をフォントバッファに溜める   聊天字体缓冲蓄缓冲器*/
void ChatBufferToFontBuffer( void );

/* チャットvBI理 聊天相关处理 */
void ChatProc( void );

/* チャット文字をバッファに溜める（一行）聊天文字缓冲储（一行*/
void StockChatBufferLine( char *str, unsigned char color );

/* チャット文字を全てクリアする 聊天文字全部清除*/
void ClearChatBuffer( void );

/*******************************************************************************/
/* 文字列の最後が半角か全角かを{べる
/* 	半角：１
/* 			全角：２
/文字列的最后码调查全角半角返回值/半角：1/全角：2
/*******************************************************************************/
int GetStrLastByte( char *str );

/*******************************************************************************/
/* 文字列の横幅（ドット数）を{べる
/* 	ドット数
/文字列的宽（点数）调查/返回值点数
/*******************************************************************************/
int GetStrWidth( char *str );

/*******************************************************************************/
/* 文字が半角か全角かを{べる
/* 	半角：１
/* 			全角：２
全角半角/文字吗？调查返回值/半角：1/全角：2
/*******************************************************************************/
int GetCharByte( char c );

/*******************************************************************************/
/* キ`入力フォ`カス取得  键盘输入聚焦取得
/*******************************************************************************/
void GetKeyInputFocus( STR_BUFFER *pStrBuffer );

/* チャットvB初期化 聊天相关初期化**********************************************************/
void InitChat( void );

/* チャットvBI理  聊天相关处理************************************************************/
void ChatProc( void );

// キ`ボ`ドカ`ソル点I理 键盘光标闪烁处理**************************************************/
void FlashKeyboardCursor( void );

/* リタ`ンキ`I理 回车键处理************************************************************/
void KeyboardReturn( void );

// ＢＳキ`I理 BS键处理***************************************************************/
void KeyboardBackSpace( void );

// チャットの履s文字の保存 *聊天的履历文字保存***************************************************************/
BOOL SaveChatHistoryStr( int no );
// チャットの履s文字のiみzみ *聊天的履历文字的读取***************************************************************/
BOOL LoadChatHistoryStr( void );

#endif

