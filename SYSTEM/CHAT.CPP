/************************/
/*	chat.c		����		*/
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

// ����åȥХåե��`  ���컺����
CHAT_BUFFER ChatBuffer[ MAX_CHAT_LINE ];

// �Է֤���������Хåե��`   �Լ����뻺����
STR_BUFFER MyChatBuffer;
//���ڵ����뻺��Խ����ָ��
// �F�ڤ������ե��`�����Хåե��ؤΥݥ��� 
STR_BUFFER *pNowStrBuffer = NULL;

// ����å��Х����󥿩`  ��/ /�����̨
int NowChatLine = 0;
// ��������
int NowMaxChatLine = DEF_CHAT_LINE;
// �������δ󤭤� ���ڵ������Ĵ�С
int NowMaxVoice = DEF_VOICE;
// ���`�����祫�����  �����˸����
int CursorFlashCnt = 0;
// ����`���������`������  ˳����������
int ChatLineSmoothY = 0;


//�����¼�ļ���
#define CAHT_HISTORY_STR_FILE_NAME 	"data\\chathis.dat" 	// ����åȤ��Ěs�ե�����ͩ`��
// ����åȤΥҥ��ȥ�`������
//�������ʷ�ṹ
CHAT_HISTORY ChatHistory;

// �ԣ��¥��`�I�� TAB������
void KeyboardTab( void );

// �����ե�����  ��־�ļ�
FILE *chatLogFile = NULL;
char chatLogFileName[] = "CHAT_000000.TXT";
void openChatLogFile( void );

#if 1
// ��ǥ��ʤ���ȫ�ǥ��ʉ�Q��������  
//���Ƭ������ʼȫ�Ǽ���ת����������
char *ZenkakuKana = "?��������?�򥡥������������"
					"�`������������������������������"
					"�����ĥƥȥʥ˥̥ͥΥϥҥեإۥ�"
					"�ߥ��������������a�b";
#endif

/* ����å��v�B���ڻ� **������س��ڻ�********************************************************/
void InitChat( void )
{
	// �Хåե��`���ꥢ �������
	memset( ChatBuffer, 0, sizeof( CHAT_BUFFER ) * MAX_CHAT_LINE );
	memset( &MyChatBuffer, 0, sizeof( STR_BUFFER ) );
	// ����å������Хåե��`�γ��ڻ�  �����������ݳ�ʼ��
	//���޸�ԭʼ70
	MyChatBuffer.len = 70;	// �����Ф��L��  �ִ�����̫����Ļ����
	//MyChatBuffer.len = 200;	// �����Ф��L��  �ִ�����
	//MyChatBuffer.len = 254;	// �����Ф��L��
	//MyChatBuffer.color = 0; // ɫ
	MyChatBuffer.fontPrio = FONT_PRIO_BACK;
	MyChatBuffer.x = 8; 	// ������
	MyChatBuffer.y = 432; 	// ������
	//MyChatBuffer.x = 8; 	// ������
	//MyChatBuffer.y = 200; 	// ������
	//MyChatBuffer.lineLen = 25; 	// ������
	//MyChatBuffer.lineDist = 16; 	// ������

	// ���椵��Ƥ������å������i�ߤ���  ������������Ϣ��ӵ��
	getUserChatOption();
	
	// ����åȥХåե����ͤ�줿�����Ф�����ե�����˳���
	//���컺�屻�͵�����������־�ļ����
	openChatLogFile();
	
}


/* ����åȥ����ե�����Υ��`�ץ� **������־�ļ��Ŀ���********************************************/
void openChatLogFile( void )
{
	if( chatLogFile == NULL )
	{
#ifdef _DEBUG
		// ������ӕr����Ҏ����
		chatLogFile = fopen( chatLogFileName, "wt" );
#else
		struct tm *nowTime;
		time_t longTime;

		// �F�ڤ��Օr��ȡ��
		time( &longTime );
		nowTime = localtime( &longTime );

		sprintf( chatLogFileName, "CHAT_%02d%02d%02d.TXT",
			(nowTime->tm_year % 100), nowTime->tm_mon+1, nowTime->tm_mday );

		// ������ӕr����Ҏ����  ����������ʱ�½�
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

// ����åȤ��Ěs���֤α��� ****************************************************************/
BOOL SaveChatHistoryStr( int no )
{
	FILE *fp; 
	
	// �����z�ߥե����륪�`�ץ�
	if( ( fp = fopen( CAHT_HISTORY_STR_FILE_NAME, "r+b" ) ) == NULL ){
		return FALSE;
	}
	
	// ��ؕ����z�������ޤǥե�����ݥ��󥿤��M�ޤ���
	fseek( fp, sizeof( ChatHistory.str[ 0 ] ) * no, SEEK_SET );
	// �����Хǩ`�������z��
	if( fwrite( &ChatHistory.str[ no ], sizeof( ChatHistory.str[ 0 ] ), 1, fp ) < 1 ){
	
		fclose( fp );// �ե����륯���`��
		return FALSE;
	}
	
	// ���¤��Ěs���ŤΈ����ޤǥե�����ݥ����M�ޤ���
	fseek( fp, sizeof( ChatHistory.str[ 0 ] ) * MAX_CHAT_HISTORY, SEEK_SET );
	// ���¤��Ěs���Ťα���
	if( fwrite( &no, sizeof( int ), 1, fp ) < 1 ){
	
		fclose( fp );// �ե����륯���`��
		return FALSE;
	}
	
	// �ե����륯���`��
	fclose( fp );
	
	return TRUE;
}

// ����åȤ��Ěs���֤��i���z�� ****************************************************************/
BOOL LoadChatHistoryStr( void )
{
	FILE *fp;

	// �F�ڤΥ���å��Ěs���Ťγ��ڻ�
	ChatHistory.nowNo = -1;

	// �i���z�ߥե����륪�`�ץ�
	if( ( fp = fopen( CAHT_HISTORY_STR_FILE_NAME, "rb" ) ) == NULL ){
		// �ʤ��r�����ƵĤ�����
		if( ( fp = fopen( CAHT_HISTORY_STR_FILE_NAME, "wb" ) ) != NULL ){
			// �ǩ`�������z��
			fwrite( &ChatHistory, sizeof( CHAT_HISTORY ) - sizeof( int ), 1, fp );
			fclose( fp );	// �ե����륯���`��
		}
		
		// ���¤Υ���å��Ěs���Ťγ��ڻ�
		ChatHistory.nowNo = MAX_CHAT_HISTORY - 1;
		
		return FALSE;
	}
	
	// �ǩ`���i���z��
	if( fread( &ChatHistory, sizeof( CHAT_HISTORY ) - sizeof( int ), 1, fp ) < 1 ){
		
		fclose( fp );	// �ե����륯���`��
		return FALSE;
	}
	// �ե����륯���`��
	fclose( fp );
	
	return TRUE;
}

/* �����Ф�F�ڤ����������Ф��ͤ� ****������Ϊ���ڵ�������������************************************************/
void StrToNowStrBuffer( char *str )
{

	int strLen, i;
	
	// ���֤��L����ȡ��
	strLen = strlen( str );
	// ��ߥåȥ����å�
	if( strLen >= 256 ) strLen = 255;
	// ���֤���������`��
	for( i = 0 ; i < strLen ; i++ ){
		// ���֥Хåե�������
		StockStrBufferChar( str[ i ] );
	}
}

/* ����å��v�B�I�� *****������ش���*******************************************************/
void ChatProc( void )
{

	// �꥿�`�󥭩`Ѻ���줿�r ���»س���ʱ
	//if( joy_trg[ 1 ] & JOY_RETURN ) KeyboardReturn();
	
	// �ԣ��¥��`Ѻ���줿�r
	if( joy_trg[ 1 ] & JOY_TAB ) KeyboardTab();
	
	// �����ե��`����������å��֤ϥ�`��Εr
	if( pNowStrBuffer == &MyChatBuffer || pNowStrBuffer == &MailStr ){
	
		// ���h�����Ф�ʹ�ä����Ȥ������h�����Ф�F�ڤ����������Ф��ͤ�
		if(	joy_trg[ 1 ] & JOY_F1 ) StrToNowStrBuffer( chatRegistryStr[ 0 ].buffer );
		if( joy_trg[ 1 ] & JOY_F2 ) StrToNowStrBuffer( chatRegistryStr[ 1 ].buffer );
		if( joy_trg[ 1 ] & JOY_F3 ) StrToNowStrBuffer( chatRegistryStr[ 2 ].buffer );
		if( joy_trg[ 1 ] & JOY_F4 ) StrToNowStrBuffer( chatRegistryStr[ 3 ].buffer );
	
		if( joy_trg[ 1 ] & JOY_F5 ) StrToNowStrBuffer( chatRegistryStr[ 4 ].buffer );
		if( joy_trg[ 1 ] & JOY_F6 ) StrToNowStrBuffer( chatRegistryStr[ 5 ].buffer );
		if( joy_trg[ 1 ] & JOY_F7 ) StrToNowStrBuffer( chatRegistryStr[ 6 ].buffer );
		if( joy_trg[ 1 ] & JOY_F8 ) StrToNowStrBuffer( chatRegistryStr[ 7 ].buffer );
	}

	// �����ե��`����������åȤΕr���ɣͣťХåե������֤��o���r
	if( pNowStrBuffer == &MyChatBuffer && ImeBufferBak2 == NULL ){
		// �ϥ��`��Ѻ�����r
		if( joy_auto[ 0 ] & JOY_UP ){
			int bak = ChatHistory.nowNo;	// �Хå����å�
			// �ҥ��ȥ�`��`�ɤ���äƤʤ��r
			if( ChatHistory.nowNo == -1 ) ChatHistory.nowNo = ChatHistory.newNo;
			else ChatHistory.nowNo--;
			// ��ߥåȥ����å�
			if( ChatHistory.nowNo < 0 ) ChatHistory.nowNo = MAX_CHAT_HISTORY - 1;
			// �ҥ��ȥ�`������r���ġ�( һ�ܻؤäƤʤ��r�֤ϡ��ҥ��ȥ�`��`�ɤ���äƤ��ʤ��ä��r ��
			if( ChatHistory.str[ ChatHistory.nowNo ][ 0 ] != 0 && ( ChatHistory.nowNo != ChatHistory.newNo || bak == -1 ) ){
				// �������Ƥ������֤���ڻ�
				pNowStrBuffer->cnt = 0;
				pNowStrBuffer->buffer[ 0 ] = NULL;
				// �ҥ��ȥ�`�����Ф������Хåե����ͤ�
				StrToNowStrBuffer( ChatHistory.str[ ChatHistory.nowNo ] );
			}else{
				// �Хå����åפ������
				ChatHistory.nowNo = bak;
			}
		}else
		// �¥��`��Ѻ�����r
		if( joy_auto[ 0 ] & JOY_DOWN ){
			// �ҥ��ȥ�`��`�ɤ���äƤ�����
			if( ChatHistory.nowNo != -1 ){
				// �ҥ��ȥ�`��`�ɤ�i����r
				if( ChatHistory.nowNo == ChatHistory.newNo ){ 
					ChatHistory.nowNo = -1;
					// �������Ƥ������֤���ڻ�
					pNowStrBuffer->cnt = 0;
					pNowStrBuffer->buffer[ 0 ] = NULL;
				}else{
					ChatHistory.nowNo++;
					// ��ߥåȥ����å�
					if( ChatHistory.nowNo >= MAX_CHAT_HISTORY ) ChatHistory.nowNo = 0;
					// �������Ƥ������֤���ڻ�
					pNowStrBuffer->cnt = 0;
					pNowStrBuffer->buffer[ 0 ] = NULL;
					// �ҥ��ȥ�`�����Ф������Хåե����ͤ�
					StrToNowStrBuffer( ChatHistory.str[ ChatHistory.nowNo ] );
				}
			}
		}
	}
}

// ���`�ܩ`�ɥ��`������I�� **���̹����˸����************************************************/
void FlashKeyboardCursor( void )
{
	
	int lastByte, strWidth = 0;
	
	// �����������ʤ��r����
	if( pNowStrBuffer == NULL ) return;
	
	// ���`������I��
	if( CursorFlashCnt >= 20 ){
		// �ɣͣŤ��ϣƣƤΕr����ǥ��`���룩
		if( ImeOpenState() == FALSE ){
			// ���������`�Щ`�Εr
			if( pNowStrBuffer->cnt >= pNowStrBuffer->len ){
				// �����Ф����᤬��Ǥ�ȫ�Ǥ����{�٤�
				lastByte = GetStrLastByte( pNowStrBuffer->buffer );
				if( lastByte == 1 ) strWidth = FontHankakuWidth;
				else if( lastByte == 2 ) strWidth = FontZenkauWidth;
				
			}
			// ���`�����ե���ȥХåե�������
			StockFontBuffer( pNowStrBuffer->imeX - strWidth, pNowStrBuffer->imeY, pNowStrBuffer->fontPrio, pNowStrBuffer->color, "_" , 0 );
			
		}else{ 	// �ɣͣŤ��ϣΤΕr��ȫ�ǥ��`���룩
		
			// ���������`�Щ`�Εr
			if( pNowStrBuffer->cnt >= pNowStrBuffer->len ){
				// �����Ф����᤬��Ǥ�ȫ�Ǥ����{�٤�
				lastByte = GetStrLastByte( pNowStrBuffer->buffer );
				if( lastByte == 1 ) strWidth = FontHankakuWidth;
				else if( lastByte == 2 ) strWidth = FontZenkauWidth;
				
			}
			// �ɣͣťХåե������֤��o���r
			if( ImeBufferBak2 == NULL ){
				// ���`�����ե���ȥХåե�������
				StockFontBuffer( pNowStrBuffer->imeX + GetStrWidth( ImeBuffer ) - strWidth, pNowStrBuffer->imeY, pNowStrBuffer->fontPrio, pNowStrBuffer->color, "��" , 0 );
			}else{
				// ���`�����ե���ȥХåե�������
				StockFontBuffer( pNowStrBuffer->imeX + GetStrWidth( ImeBuffer ), pNowStrBuffer->imeY, pNowStrBuffer->fontPrio, pNowStrBuffer->color, "��" , 0 );
			}
		}
	}
	// ���`�����祫����
	CursorFlashCnt++;
	// ��ߥåȥ����å�
	if( CursorFlashCnt >= 40 ) CursorFlashCnt = 0;
}

// �£ӥ��`�I�� ***************************************************************/
void KeyboardBackSpace( void )
{
	int byte;
	
	// �����������ʤ��r����
	if( pNowStrBuffer == NULL ) return;
	
	// ��������֤���Ǥ�ȫ�Ǥ����{�٤�
	byte = GetStrLastByte( pNowStrBuffer->buffer ); 
		
	// �K��ӛ�Ť�����
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt - byte ] = '\0';
	// һ���֑���
	pNowStrBuffer->cnt -= byte;
			
	// ��ߥåȥ����å�
	if( *( pNowStrBuffer->buffer ) == '\0' ) pNowStrBuffer->cnt = 0;
	
	// �ե�å��奫���󥿩`��������ƣ�
	CursorFlashCnt = 20;
}	

// �ԣ��¥��`�I�� ***************************************************************/
void KeyboardTab( void )
{

	int i,flag = 0;
	
	// �����������ʤ��r���� ���룬û�еط������
	if( pNowStrBuffer == NULL ) return;
	
	// ���ֵ��h�����å� *******����ע����************************
	for( i = 0 ; i < MAX_CHAT_REGISTY_STR ; i++ ){
		if( pNowStrBuffer == &chatRegistryStr[ i ] ){ 
			flag = TRUE;
			break;
		}
	}
	// ���ֵ��h�Εr ******���ֵ�¼ʱ*************************
	if( flag == TRUE ){
		// ���եȥ��`����Ƥ�����   �� Shift���Ļ�
		if( joy_con[ 1 ] & JOY_RSHIFT || joy_con[ 1 ] & JOY_LSHIFT ){
			i--;
			// ��ߥåȥ����å� ���޼��
			if( i < 0 ) i = MAX_CHAT_REGISTY_STR - 1;
		}else{
			i++;
			// ��ߥåȥ����å�
			if( i >= MAX_CHAT_REGISTY_STR ) i = 0;
		}
		// �����ե��`�����Ƅ� ���뽹���ƶ�
		GetKeyInputFocus( &chatRegistryStr[ i ] );
	}
	
	// �ե�å��奫���󥿩`��������ƣ�
	CursorFlashCnt = 20;
}	

/* �꥿�`�󥭩`�I�� *******�س�������*****************************************************/
void KeyboardReturn( void )
{
	char bakNo;
	
	// �ɣͣťХåե��`�����֤�����r������  �������ֿյ�ʱ�򣬻�
	if( ImeBufferBak2 != NULL ) return;
	
	// ����å������Εr ***���������ʱ��****************************
	if( pNowStrBuffer == &MyChatBuffer ){;
	
		// �հ��Ф��ȸ��Ф��ʤ�  ���кͿհ��в���
		if( pNowStrBuffer->cnt == 0 ) return;
		
		// �K��ӛ�Ť�����  �ն˼Ǻŷ�0
		pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = '\0';
		
		// �����Ф����å��Ф��ͤꤳ��  ����������������
		
		//StockChatBufferLine( pNowStrBuffer->buffer, MyChatBuffer.color );
		
#ifdef _DEBUG
	
		// �Хȥ�ץ�������  ս������
		if( strstr( pNowStrBuffer->buffer, "[battlein]" ) ) EncountFlag = TRUE;
		if( strstr( pNowStrBuffer->buffer, "[battleout]" ) ){
			ChangeProc( PROC_GAME, 1 );
			DeathAllAction();	// �����������ڻ�  �������ڻ�
		} 
		
#endif	
		// �Хå����å�  ����
		bakNo = ChatHistory.newNo;
		// �ΤΥХåե����Ť�  ��һ�������/ /����
		ChatHistory.newNo++;
		// ��ߥåȥ����å� ���޼��


		if( ChatHistory.newNo >= MAX_CHAT_HISTORY ) ChatHistory.newNo = 0;
		// ǰ�ؤ��`�������ФΕr ���ϴβ�ͬ���ַ�����ʱ��
		if( strcmp( pNowStrBuffer->buffer, ChatHistory.str[ bakNo ] ) != 0 )
		{
			// ��ؤ������Ф�ҙ����  ��ε������м�
			strcpy( ChatHistory.str[ ChatHistory.newNo ], pNowStrBuffer->buffer );
			// ����åȤ��Ěs���֤α���  ������������ֱ���
			SaveChatHistoryStr( ChatHistory.newNo );
		}
		else
		{
			// �Хå����åפ������  �ӱ��ݻָ�
			ChatHistory.newNo = bakNo;
		}
		// �ҥ��ȥ�`���ų��ڻ�  ��ʷ������ڻ�
		ChatHistory.nowNo = -1;
	
		
#ifdef _DEBUG
		if( offlineFlag == FALSE )
#endif
		// ����å�����������  �����ַ�������
		
		chatStrSendForServer( pNowStrBuffer->buffer, MyChatBuffer.color );
		
		// ��ˤˑ��� ��˷���
		pNowStrBuffer->cnt = 0;
		
		// �Է֤���������Хåե��`�򥯥ꥢ  ���뻺������Լ���
		*( pNowStrBuffer->buffer )= '\0';
		// �K��ӛ�Ť����� �ն˼Ǻŷ�
	}
	else
	
	// �ƺŉ���Εr **********************************
	if( pNowStrBuffer == &shougouChange )
	{
		// ������ɥ�Ĩ��
		DeathAction( pActMenuWnd3 );
		pActMenuWnd3 = NULL;
		// �����ե��`��������
		GetKeyInputFocus( &MyChatBuffer );
		// ������ɥ��]������
		play_se( 203, 320, 240 );
		// ���������γƺŤ���
		//strcpy( pc.ptAct->freeName, shougouChange.buffer );
		// �ţգäˉ�Q
		sjisStringToEucString( shougouChange.buffer );
		// ��ǰ�������
		lssproto_FT_send( sockfd, shougouChange.buffer ) ; /* ../doc/lssproto.html line 1792 */
	}
	else
	
	// ��ǰ����Εr **********���ֱ����ʱ************************
	if( pNowStrBuffer == &petNameChange )
	{
		// ������ɥ�Ĩ��
		DeathAction( pActMenuWnd3 );
		pActMenuWnd3 = NULL;
		// �����ե��`��������
		GetKeyInputFocus( &MyChatBuffer );
		// ������ɥ��]������
		play_se( 203, 320, 240 );
		// �ţգäˉ�Q
		sjisStringToEucString( petNameChange.buffer );
		// ��ǰ�������
		lssproto_KN_send( sockfd, petStatusNo, petNameChange.buffer ) ; /* ../doc/lssproto.html line 1792 */
	}
	else
	
	// ��`��Εr ***�ʼ���ʱ��*******************************
	if( pNowStrBuffer == &MailStr )
	{
		// ������ФΕr�ϸ��Ф��ʤ�
		if( MailStr.cnt < MailStr.len - MailStr.lineLen ){
			// ��ˤΕr
			if( MailStr.cnt % MailStr.lineLen == 0 ){
				// һ���֤�������
				StockStrBufferChar( ' ' );
			}
			// �Фζˤޤǥ��ک`��������
			while( MailStr.cnt % MailStr.lineLen ) StockStrBufferChar( ' ' );
		}
	}
	else
	
	// ��`��Εr **********�ʼ���ʱ��*****�ѵ�������һ��?*******************
	if( pNowStrBuffer == &MailStr )
	{
		// ������ФΕr�ϸ��Ф��ʤ�
		if( MailStr.cnt < MailStr.len - MailStr.lineLen ){
			// ��ˤΕr
			if( MailStr.cnt % MailStr.lineLen == 0 ){
				// һ���֤�������
				StockStrBufferChar( ' ' );
			}
			// �Фζˤޤǥ��ک`��������
			while( MailStr.cnt % MailStr.lineLen ) StockStrBufferChar( ' ' );
		}
	}

	// �ɣĥ��`�����Εr ******���������ʱ��**********************
	extern STR_BUFFER *idPasswordStr;
	extern short idKeyReturn;
	if( pNowStrBuffer == idPasswordStr )
	{
		idKeyReturn = 1;
	}
	
	// �ե�å��奫���󥿩`��������ƣ�
	CursorFlashCnt = 20;
	
}

/* ���֤�Хåե������루���Х��ȣ�**���ֻ��崢��1�ֽڣ�****************************************/
void StockStrBufferChar( char c )
{

	static BOOL zenkakuFlag = FALSE; // ȫ�ǥե饰  ȫ��ת�۵�
	static BOOL zenkakuFlag2 = FALSE; // ȫ�ǥե饰
	
	// �����������ʤ��r����  ���룬û�еط������
	if( pNowStrBuffer == NULL ) return;
	
	// �v�S�ʤ����֤��w�Ф� ��ϵû�������ڷ�  С��1 ���� С��1f
	if( 0x01 <= c && c <= 0x1f /* && c != 0x08 && c != 0x0d */ ){
		return;
	}
	
	// ǰ�ؤ�ȫ�ǤǤʤ��ä���  �ϴ�ȫ�ǣ����û��
	if( zenkakuFlag == FALSE ){
		// ǰ�ؤ����Ȱ��һ���֤��������Ǥ���״�B��ȫ�Ǥ�Ϥ������r����ؤ����
		if( zenkakuFlag2 == TRUE ){
			zenkakuFlag2 = FALSE; // �ե饰�ϣƣ�
			return;
		}
		// ȫ�ǤΤȤ� ȫ�ǵ�ʱ��
		// ���֤���Ǥ�ȫ�Ǥ����{�٤�
		if( GetCharByte( c ) == 2 ){
			// ���Ȱ��һ���֤��������Ǥ���r��ȫ�Ǥ������Ǥ��ʤ�
			if( pNowStrBuffer->cnt >= pNowStrBuffer->len - 1 ){
				zenkakuFlag2 = TRUE; // �ե饰�ϣ�
				return;
			}
		// ��ǤΕr
		}else{
			// ���������Εr�������Ǥ��ʤ�
			if( pNowStrBuffer->cnt >= pNowStrBuffer->len/* || zenkakuFlag2 == TRUE*/ ){
				//zenkakuFlag2 = FALSE; // �ե饰�ϣƣ�
				return;
			}
#if 1
			// ��ǥ��ʤΕr��ȫ�Ǥ��ʤˉ�Q����
			if( ( char )0xa0 <= c && c <= ( char )0xdf ){ 
				// ���Ȱ��һ���֤��������Ǥ���r��ȫ�Ǥ������Ǥ��ʤ�
				if( pNowStrBuffer->cnt >= pNowStrBuffer->len - 1 )
				{
					return;
				}
				// һ�Х���Ŀ��Хåե��`������
				//��ȥ
				
			//	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = ZenkakuKana[ ( c - ( char )0xa0 ) * 2 ];
				// �Τ���������Хåե��`�ؤΥݥ��󥿤��M�ޤ���
			//	pNowStrBuffer->cnt++;
				// ���Х���Ŀ
			//	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = ZenkakuKana[ ( c - ( char )0xa0 ) * 2 + 1 ];
				// �K��ӛ�Ť�����
			//	pNowStrBuffer->buffer[ pNowStrBuffer->cnt + 1 ] = '\0';
	          
				// �Τ���������Хåե��`�ؤΥݥ��󥿤��M�ޤ���
			//	pNowStrBuffer->cnt++;
				// �ե�å��奫���󥿩`��������ƣ�
				
				CursorFlashCnt = 20;
				
				return;
			}
#endif
		}
	}
	
	// �Хåե��`�����֤�����
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = c;
	// �K��ӛ�Ť�����
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt + 1 ] = '\0';
	
	// �Τ���������Хåե��`�ؤΥݥ��󥿤��M�ޤ���
	pNowStrBuffer->cnt++;
	// �ե�å��奫���󥿩`��������ƣ�
	CursorFlashCnt = 20;
	
	// ǰ�ؤ�ȫ�ǤǤʤ��ä���
	if( zenkakuFlag == FALSE ){
		// ȫ�ǤΤȤ��ե饰�����ƤƤ���
		// ���֤���Ǥ�ȫ�Ǥ����{�٤�
		if( GetCharByte( c ) == 2 ) zenkakuFlag = TRUE;
			
	}else zenkakuFlag = FALSE;
	
}
//���Ų�����
#if 0
/* ����å����֤�Хåե������루���Х��ȣ�******�������ֻ��崢��2�ֽڣ�*****************************/
void StockChatBufferWord( WORD w )
{
	STR_BUFFER *chatBuffer = &ChatBuffer[ NowChatLine ];
	//STR_BUFFER *chatBuffer =(STR_BUFFER *) &ChatBuffer[ NowChatLine ];
	
	// ������ȥ��`�Щ`�Εr
//	if( ChatCnt >= CHAT_BUFFER_SIZE - 2 ) return;
	
	// ����å��������Хåե��`�����֤�����
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = w;
	// �K��ӛ�Ť�����
	pNowStrBuffer->buffer[ pNowStrBuffer->cnt + 2 ] = '\0';
	
	// �Τ���������Хåե��`�ؤΥݥ��󥿤��M�ޤ���
	pNowStrBuffer->cnt++;
}
#endif

/* ����å����֤�Хåե������루һ�У�******�������ֻ��崢���У�*********************************/
void StockChatBufferLine( char *str, unsigned char color )
{
	int splitPoint = 0;
	char splitStr[ STR_BUFFER_SIZE + 1 ];
	
	// һ�Ф˅��ޤ�ʤ��r  һ��û����¼��ʱ��
	if( strlen( str ) > MyChatBuffer.len )
	{
		// �ָ�����������
		strncpy( splitStr, str, MyChatBuffer.len );
		// �K��ӛ�Ť�����
		*( splitStr + MyChatBuffer.len ) = NULL;
		// �����ȫ�����֤��ָ��Ƥ���r  ����ȫ�����ֱ��ָ��ʱ��
		
		if( GetStrLastByte( splitStr ) == 3 )
		{ 
			// һ���֑��� һ���ͻָ�
			splitPoint = MyChatBuffer.len - 1;
			// �K��ӛ�Ť�����
			*( splitStr + MyChatBuffer.len - 1 ) = NULL; 
		}
		else splitPoint = MyChatBuffer.len; 

      //�Ҽ� ԭʼ�������
      // splitPoint = MyChatBuffer.len;
		
		// �����Ф����å��Ф��ͤꤳ��
		strncpy( ChatBuffer[ NowChatLine ].buffer, str, splitPoint );
		// �K��ӛ�Ť�����
		*( ChatBuffer[ NowChatLine ].buffer + splitPoint ) = NULL; 
	}
	else
	{
		// �����Ф����å��Ф��ͤꤳ��
		strcpy( ChatBuffer[ NowChatLine ].buffer, str );
	}
	// ����åȥ����ե�����˳���
	if( chatLogFile )
	{
		fprintf( chatLogFile, "%s\n", ChatBuffer[ NowChatLine ].buffer );
	}
	// ɫ��ָ��
	ChatBuffer[ NowChatLine ].color = color;
	
	NowChatLine++;	// ����åȸ���
	// ��ߥåȥ����å�
	if( NowChatLine >= MAX_CHAT_LINE ) NowChatLine = 0;

	// һ�Ф˅��ޤ�ʤ��ä���
	if( splitPoint != 0 ){
		// �Τ��Ф��ͤ������Ф����å��Ф��ͤꤳ�ࣨ�َ��I����
		StockChatBufferLine( str + splitPoint, color );
	}
	// ����`���������`������
	//ChatLineSmoothY += 20;
	ChatLineSmoothY = 20;
	
}

/* ����å����֤�ȫ�ƥ��ꥢ���� *******��������ȫ�����****************************************/
void ClearChatBuffer( void )
{
	int i;
	
	// ����åȤ������֥�`��
	for( i = 0 ; i < MAX_CHAT_LINE ; i++ ){
		// �K��ӛ�Ť�����
		ChatBuffer[ i ].buffer[ 0 ] = '\0';
	}
}

/* ����åȥХåե��`��ե���ȥХåե������� **���컺�����建����******************************/
void ChatBufferToFontBuffer( void )
{
	#define CHAT_ZOUBUN_Y 20;
	int i, j, k = 0;
	//char moji[ 256 ];
	int x = 8, y = 400; // ��ʾ����
	
	// ����ʼ����Ф��O��  ��ʼд�е��趨
	j = NowChatLine - 1;
	// ��ߥåȥ����å�
	if( j < 0 ) j = MAX_CHAT_LINE - 1;
	
	// �������`���Ф�һ����֤˱�ʾ ������һ�ж����ʾ
	if( ChatLineSmoothY > 0 ) k = NowMaxChatLine + 1;
	else k = NowMaxChatLine;
	
	// ��ߥåȥ����å�  ���޼��
	if( k > MAX_CHAT_LINE )  k = MAX_CHAT_LINE;
	
	// ����åȤ������֥�`��  �������������ѭ��
	for( i = 0 ; i < k; i++ ){
	//for( i = 0 ; i < NowMaxChatLine; i++ ){
		// �Τ����֤����ä���
		if( *ChatBuffer[ j ].buffer != NULL ){
			// �ե���ȥХåե�������
			StockFontBuffer( x, y + ChatLineSmoothY, FONT_PRIO_BACK, ChatBuffer[ j ].color, ( char *)ChatBuffer[ j ].buffer, 0 );
		}
		y -= CHAT_ZOUBUN_Y;  // �����Ƅ�
		// �ΤΥХåե�
		j--;
		// ��ߥåȥ����å�
		if( j < 0 ) j = MAX_CHAT_LINE - 1;
	}
	// ����`���������`��Εr
	if( ChatLineSmoothY > 0 ) ChatLineSmoothY--;
	
	// �Τ����֤����ä���
	//if( *MyChatBuffer.buffer != NULL ){
		// �Է֤�������ե���ȥХåե�������
		StockFontBuffer2( &MyChatBuffer );
		//StockFontBuffer( MyChatBuffer.x, MyChatBuffer.y, FONT_PRIO_BACK, MyChatBuffer.color, ( char *)MyChatBuffer.buffer, 0 );
	//}
}

/*******************************************************************************/
/* �����Ф����᤬��Ǥ�ȫ�Ǥ����{�٤�
/* ���ꂎ��	��ǣ���
/* 			ȫ�ǣ���
/* 			���⣺��
/*******************************************************************************/
int GetStrLastByte( char *str )
{
	int byte = 0;
	
	// �����Ф�����ޤǥ�`��
	while( !( *str == '\0' ) ){
	//		( *str == ( char )0x81 && *( str + 1 ) == ( char )0x51 ) ) ){
	
		// ȫ�ǤΤȤ�
		if( ( char )0x80 <= *str && *str <= ( char )0x9f ||
			( char )0xe0 <= *str && *str <= ( char )0xff ){
			
			// ȫ�Ǥ����֤��ָ��Ƥ���r
			if( *( str + 1 ) == NULL ) return 3; // ����
			
			str += 2; 	// ���Х����w�Ф�
			byte = 2;	// ȫ�ǤǤ�
			
		}else{	// ��ǤΤȤ�
		
			str ++; 	// ���Х����w�Ф�
			byte = 1;	// ��ǤǤ�
		}
	}
	return byte;
}

/*******************************************************************************/
/* ���֤���Ǥ�ȫ�Ǥ����{�٤�
/* ���ꂎ��	��ǣ���
/* 			ȫ�ǣ���  ȫ�ǰ��/�����𣿵��鷵��ֵ/��ǣ�1/ȫ�ǣ�2
/*******************************************************************************/
int GetCharByte( char c )
{
	// ȫ�ǤΤȤ�
	if( ( char )0x80 <= c && c <= ( char )0x9f ||
		( char )0xe0 <= c && c <= ( char )0xff ){
		
		return 2;	// ȫ�ǤǤ�
		
	}else{	// ��ǤΤȤ�
	
		return  1;	// ��ǤǤ�
	}
}

/*******************************************************************************/
/* �����Фκ�����ɥå��������{�٤�
/* ���ꂎ��	�ɥå���
/*******************************************************************************/
int GetStrWidth( char *str )
{
	int width = 0;
	
	// �����Ф�����ޤǥ�`��
	while( !( *str == '\0' ) ){
	
		// ȫ�ǤΤȤ�
		if( ( char )0x80 <= *str && *str <= ( char )0x9f ||
			( char )0xe0 <= *str && *str <= ( char )0xff ){
			
			str += 2; 	// ���Х����w�Ф�
			width += FontZenkauWidth; // ȫ�ǥ��������㤹
			
		}else{	// ��ǤΤȤ�
		
			str ++; 	// ���Х����w�Ф�
			width += FontHankakuWidth; // ��ǥ��������㤹
		}
	}
	
	return width;
}

/*******************************************************************************/
/* �����ե��`����ȡ��  ����۽�ȡ��
/*******************************************************************************/
void GetKeyInputFocus( STR_BUFFER *pStrBuffer )
{
	
	// ��������������r
	//if( pNowStrBuffer != NULL ){
		// ���`���뤬�ФäƤ���r
	//	if(	pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] != '\0' ){
			// �K��ӛ�Ť�����
	//		pNowStrBuffer->buffer[ pNowStrBuffer->cnt ] = '\0';
	//	}
	//}
	// �ɣͣťХåե��`����ڻ�
	//*ImeBuffer = '\0';
	// �����Хåե��ݥ��󥿤���
	pNowStrBuffer = pStrBuffer;
}

