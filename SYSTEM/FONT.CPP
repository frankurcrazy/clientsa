/************************/
/*	font.c				*/
/************************/
#include "system.h"

#define ASCII(a) a-'A'+10
#define ASCII_DEC(a) a-'0'+ 35

#define FONT_BUFFER_SIZE 1024 	// �ե���ȥХåե�������  ���建������С

// �ե���ȥХåե��`  ���建����
FONT_BUFFER FontBuffer[ FONT_BUFFER_SIZE ];

// �ե���ȥ����󥿩`  �����̨
int FontCnt = 0;

// ȫ�����֤κ�����ɥå�����  ����
int FontZenkauWidth;
// ������֤κ�����ɥå�����
int	FontHankakuWidth;

/* �ե��������Хåե�������  �������Ϣ������ *********************************************/
int StockFontBuffer( int x, int y, char fontPrio, int color, char *str, BOOL hitFlag )
{


	// ������ȥ��`�Щ`�Εr  ������� ���ڻ�������ʱ��
	if( FontCnt >= FONT_BUFFER_SIZE ) return -2;
	//���建����Ϣ����
	// �ե���ȥХåե��`����������
	FontBuffer[ FontCnt ].x = x;
	FontBuffer[ FontCnt ].y = y;
	FontBuffer[ FontCnt ].fontPrio = fontPrio;
	FontBuffer[ FontCnt ].color = color;
	FontBuffer[ FontCnt ].hitFlag = hitFlag;
	// �����Х��ԩ`  �����п���
	
	strcpy( FontBuffer[ FontCnt ].str, str );
	
	// �ե���ȥ����󥿩`�ץ饹 �����������
	//FontCnt++;
	return FontCnt++;
	
}

/* �ե��������Хåե������루������ɤ���***�������Ϣ���崢�������彻����*****************************/
void StockFontBuffer2( STR_BUFFER *strBuffer )
{
	int lineDist = 0; // ���Ϥ���Σ٥��ե��å����� �����Ͻǵ�Y����ƫ��
	int splitPoint = 0;
	char splitStr[ 256 ];
	
	// ������ȥ��`�Щ`�Εr
	if( FontCnt >= FONT_BUFFER_SIZE ){ 
		strBuffer->hitFontNo = -2;
		return;
	}
	
	// ���Ф���r
	if( strBuffer->lineLen != 0 ){
		// һ�Ф˅��ޤ�ʤ��r
		while( strlen( strBuffer->buffer + splitPoint ) >= strBuffer->lineLen ){
		// һ�Ф˅��ޤ�ʤ��r
		//if( strlen( strBuffer.buffer + splitPoint ) > strBuffer.lineLen ){
			// �ָ�����������
			strncpy( splitStr, strBuffer->buffer + splitPoint, strBuffer->lineLen );
			// �K��ӛ�Ť�����
			*( splitStr + strBuffer->lineLen ) = NULL;
			// �����ȫ�����֤��ָ��Ƥ���r  ����ȫ�����ֱ��ָ��ʱ��
			if( GetStrLastByte( splitStr ) == 3 ){ 
			
				// һ���֑��� һ���ͻָ�
				splitPoint = strBuffer->lineLen - 1 + splitPoint;
				// �K��ӛ�Ť�����
				*( splitStr + strBuffer->lineLen - 1 ) = NULL; 
			}
			else splitPoint = strBuffer->lineLen + splitPoint;
	
			// ���ֱ�ʾ�Хåե�������  ���ֱ�ʾ������
			StockFontBuffer( strBuffer->x, strBuffer->y + lineDist, strBuffer->fontPrio, 0, splitStr, 0 );
			// �����˸���
			lineDist += strBuffer->lineDist;
		}
		// ��ä������Ф��ʾ ��������ֱ�ʾ
		StockFontBuffer( strBuffer->x, strBuffer->y + lineDist, strBuffer->fontPrio, 0, strBuffer->buffer + splitPoint, 0 );
		
		// �����Ф��äѤ��Εr���ġ�������Ф��Σգ̣̤Εr  ����������ʱ���ң����һ�е�ʱ��NULL
		if( strBuffer->cnt >= strBuffer->len &&  *( strBuffer->buffer + splitPoint ) == NULL ){
			// �ɣͣ������Фα�ʾ�_ʼ���ˤ�����  �����п�ʼѰ����ʾ����
			strBuffer->imeX = strBuffer->x + GetStrWidth( strBuffer->buffer + splitPoint - strBuffer->lineLen );
			strBuffer->imeY = strBuffer->y + lineDist - strBuffer->lineDist;
		}else{
			// �ɣͣ������Фα�ʾ�_ʼ���ˤ�����
			strBuffer->imeX = strBuffer->x + GetStrWidth( strBuffer->buffer + splitPoint );
			strBuffer->imeY = strBuffer->y + lineDist;
		}
		
	}else{
	
		// �ե���ȥХåե��`����������
		FontBuffer[ FontCnt ].x = strBuffer->x;
		FontBuffer[ FontCnt ].y = strBuffer->y;
		FontBuffer[ FontCnt ].fontPrio = strBuffer->fontPrio;
		FontBuffer[ FontCnt ].color = strBuffer->color;
		FontBuffer[ FontCnt ].hitFlag = 0;	// �������ж��Ͻ~�����ʤ�
		// ������Ҋ���ʤ�����r
		if( strBuffer->blindFlag >= TRUE ){
			int i;
			// ����������
			for( i = 0 ; i < strBuffer->cnt ; i++ ){
				FontBuffer[ FontCnt ].str[ i ] = '*';
			}
			FontBuffer[ FontCnt ].str[ i ] = NULL;
			//sprintf( FontBuffer[ FontCnt ].str, "%*s", strBuffer->cnt, '*' );
		}else{
			// �����Х��ԩ`
			strcpy( FontBuffer[ FontCnt ].str, strBuffer->buffer );
		}
	
		// �ɣͣ������Фα�ʾ�_ʼ���ˤ�����
		strBuffer->imeX = strBuffer->x + GetStrWidth( strBuffer->buffer );
		strBuffer->imeY = strBuffer->y;
		
		// �ե���ȥ����󥿩`�ץ饹
		strBuffer->hitFontNo = FontCnt++;
	}
}

// �ե���ȱ�ʾ�v�� //������ʾ����/////////////////////////////////////////////////////////
void FontPrint( char *moji, int x, int y, int dispPrio )
{
	
	// ���֤���������`��
	while( *moji ){
		// �հפΤȤ��w�Ф�
		if( *moji != 0x20 ){
			// ���֤ΤȤ� ���ֵ�ʱ��
			if( '0' <= *moji && *moji <= '9' )
			{
				StockDispBuffer( x, y, dispPrio, ASCII_DEC( *moji ), 0 );
			}
			else if( 'A' <= *moji && *moji <= 'Z' ){
				// ����ե��٥åȤΕr  ������ĸ��ʱ��
				//StockDispBuffer( dispPrio, x, y, 32, 0 );
			}

			else if( 'A' <= *moji && *moji <= 'Z' ){
				// ����ե��٥åȤΕr  ������ĸ��ʱ��
				//StockDispBuffer( dispPrio, x, y, 32, 0 );
			}

		}
		// �ݥ����M���
		moji++;
		// ��ʾ���˥ץ饹
		x += 16;
	}
}
	
// ���֥ե���ȱ�ʾ�v�� //����������ʾ����/////////////////////////////////////////////////////
void FontPrintDec( char *moji, int x, int y, int dispPrio )
{
	// ���֤���������`��
	while( *moji ){
		if( *moji != 0x20 ){ 
			// һ���֤��ĥХåե��ˤ����
			StockDispBuffer( x, y, dispPrio, 32, 0 );
		}
		// �ݥ����M���
		moji++;
		// ��ʾ���˥ץ饹
		x += 16;
	}
}
	