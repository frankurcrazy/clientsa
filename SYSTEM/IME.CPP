/************************/
/*	ime.cpp				*/
/************************/
#include "system.h"
#include "winnls32.h"
#include "font.h"
#include "ime_sa.h"
#include "menu.h"

// �ɣͣŤΥϥ�ɥ�
HIMC hImc;
HIMC hImcBegin;

// �ɣͣ�������`��ȡ����
DWORD conversion;
DWORD sentence;

// �ɣͣ�������`��������  IME�ַ�������ģʽ
/*
static char *ImeModeStr[] = { 
								"    ??/abc","","",
								"    ??/???"","","","","", // Nuke.1
								" ??/����",
								" ??/������","",
								" ??/������","","","","",
								"   ???/abc","","",
								"   ???/???"","","","","", // Nuke.2
								"???/����",
								"???/������","",
								"???/������",
								};
// �ɣͣ�������`�������У��ã����ã� IME���뷽ʽ�����У�Caps�ã�
static char *ImeModeStrCaps[] = { 
								"    ??/ABC","","",
								"    ??/???"","","","","", // Nuke.3
								" ??/���£�",
								" ??/������","",
								" ??/������","","","","",
								"   ???/ABC","","",
								"   ???/???"","","","","", // Nuke.4
								"???/���£�",
								"???/������","",
								"???/������",
								};*/

//IME�ַ�������ģʽСд
static char *ImeModeStr[] = { 
	"    ��/abc","","",
								"    ��/��?","","","","", // Nuke.1
								" ��/������",
								" ��/������","",
								" ��/�A�C�E","","","","",
								"   ۰?abc","","",
								"   ۰?��?","","","","", // Nuke.2
								"۰?������",
								"۰?������","",
								"۰?�A�C�E",
};
// IME���뷽ʽ�����У�Caps�ã���д
static char *ImeModeStrCaps[] = { 
	"    ��/ABC","","",
								"    ��/��?","","","","", // Nuke.3
								" ��/�`�a�b",
								" ��/������","",
								" ��/�A�C�E","","","","",
								"   ۰?ABC","","",
								"   ۰?��?","","","","", // Nuke.4
								"۰?�`�a�b",
								"۰?������","",
								"۰?�A�C�E",
								};
#if 0
// ��ǥ��ʤ���ȫ�ǥ��ʉ�Q��������
char *ZenkakuKana = "?��������?�򥡥������������"
					"�`������������������������������"
					"�����ĥƥȥʥ˥̥ͥΥϥҥեإۥ�"
					"�ߥ��������������a�b";
#endif

// �ɣͣťХåե��` IME������ ���޸� _asm
char ImeBuffer[ 256 ];
//char *ImeBuffer;
// �ɣͣ��Ĺ��Хåե��`  ���ﻺ����
char ImeBlock[ 256 ];
// �ɣͣťХåե��`�Хå����å�  ���屸�� ���޸� _asm
char ImeBufferBak2; 
//char *ImeBufferBak2;

// �ɣͣ��v�B�γ��ڻ� *********************************************************/
void InitIme( void )
{
	// �Хåե��`���ڻ�  ���޸� _asm

	ImeBuffer[ 0 ] = NULL;
	ImeBlock[ 0 ] = NULL;
	ImeBufferBak2 = NULL;
	
	// ��ǰ�ˣɣͣŤ����ä���������   ��ǰIME�Ļ���ʧ��
	if( hImc != NULL ){
		// �ɣͣť���ƥ������_��
		EndIme();
	}
	//hImc = ImmGetContext( hWnd );
	
	// �ɣͣť���ƥ����Ȥ�ȡ��  �����ĵ�ȡ��
	hImc = ImmCreateContext();
	
#ifdef _DEBUG
	// ʧ�������r  
	if( ( hImc = ImmCreateContext() ) == NULL )
	{
		MessageBox( hWnd, "IME��ʼ��ʧ�ܣ�̫��Ϊֹ������ϵ", "�_�J", MB_OK | MB_ICONSTOP );
	}
#endif
	
	hImcBegin = ImmAssociateContext( hWnd , hImc );
	/*
	����һ�����������ġ�Ҫ�����������ķ����һ�����ڣ���Ҫ����ImmAssociateContext������
	�������������һ�����������������ǰ�봰�ڹ��������������ĵľ����
	�����ǰû��������������Щ��������������������صľ������Ĭ�����������ģ��ľ������
	ͨ��������Ҫ������������Ĭ�Ͼ���������㲻����ʹ���Լ�������������ʱ���ٰ����봰��
	���¹����ϡ�
   */
	// �ɣͣ�  ������ʧ
    //WINNLSEnableIME( hWnd, FALSE );
   	//WINNLSEnableIME( NULL, TRUE );

}
	
// �ɣͣŤνK�� ***************************************************************/
void EndIme( void )
{
	// �ɣͣť���ƥ������_��
	ImmReleaseContext( hWnd, hImc );
	ImmAssociateContext( hWnd , hImcBegin );
	// �ɣͣť�����ɥ�����
    //WINNLSEnableIME( NULL, FALSE );
}

//*****************************************************************************/
// �ɣͣŤΣϣΡ��ϣƣ�״�B�δ_�J 
// ���ꂎ��	�ϣΡ���TRUE
// 			�ϣƣƣ�FALSE
//*****************************************************************************/
BOOL ImeOpenState( void )
{
    return ImmGetOpenStatus( hImc );
}
//*****************************************************************************/
// �ףɣΣģϣףӤΣɣͣťХåե��`�򥯥ꥢ����   ����ͨ��
//*****************************************************************************/
void ClrImeCompositionString( void )
{
	// �ɣͣŤ��ϣΤΕr
    if( ImeOpenState() == TRUE ){
		// �ɣͣţϣƣƤˤ���
    	ImmSetOpenStatus( hImc, FALSE );
		// �ɣͣţϣΤˤ���
    	ImmSetOpenStatus( hImc, TRUE );
		// �ϣƣƤˤ��ƣϣΤˤ������
	}
}
#if 0
//*****************************************************************************/
// ��ǥ��ʤ�����r��ȫ�ǥ��ʤˉ�Q����  ���Ƭ������ʱ��ȫ�Ǽ���ת��
//*****************************************************************************/
void HanKanaToZenKana( char *str )
{
	char work[ 512 ];
	int i = 0, j = 0;
	
	// �������֥�`��
	while( str[ i ] != NULL ){
		// ȫ�ǤΤȤ�
		// ���֤���Ǥ�ȫ�Ǥ����{�٤�
		if( GetCharByte( str[ i ] ) == 2 ){
			// ���֥Хåե�������
			work[ j++ ] = str[ i++ ];
			work[ j++ ] = str[ i++ ];
		}else{
			// ��ǥ��ʤΕr��ȫ�Ǥ��ʤˉ�Q����
			if( ( char )0xa0 <= str[ i ] && str[ i ] <= ( char )0xdf )
			{ 
				// ���֥Хåե�������
				
				work[ j++ ] = ZenkakuKana[ ( str[ i ] - ( char )0xa0 ) * 2 ];
				work[ j++ ] = ZenkakuKana[ ( str[ i++ ] - ( char )0xa0 ) * 2 + 1 ];
				
				
			}
			else
			{
				// ���֥Хåե�������
				work[ j++ ] = str[ i++ ];
			}
		}
	}
	// �K��ӛ�Ť�����
	if( j >= 256 ) work[ 255 ] = NULL;
	else work[ j ] = NULL;
	
	// ��Q���������Ф򥳥ԩ`����  ת��������ָ���
	strcpy( str, work );
}
#endif

// �ɣͣ��v�B�I�� ********��ش���*****************************************************/
void ImeProc( void )
{

	//int strWidth;	// �����Фκ�����ɥå�����
	int strWidth2;	// �����Фκ�����ɥå����� �����еĿ���������
	int blockXcnt = 0; 		// �Ĺ��ܥå��������ˤΥ����󥿩`  ����������Ĺ�̨
	int blockX1 = 0,blockX2 = 0; 	// �Ĺ��ܥå���������  ����������
	int blockFlag = FALSE; 	// �Ĺ��ե饰  ����ת�۵�
	char *ImeBufferBak = ImeBuffer;	// ���I��
	char *ImeBlockBak = ImeBlock;	// ���I��
	BOOL imeOpenFlag;				// �ɣͣŤΣϣΡ��ϣƣƥե饰
	BOOL capsLockFlag;				// capsLock �ΣϣΡ��ϣƣƥե饰
	char keyBoardState[ 256 ];		// ���`�ܩ`�ɤ�״�B  ���̵�״̬
	char imePrio;					// �ɣͣťܥå����α�ʾ���ȶ�  �����ʾ���ȶ�
	
	
	// �Хå����å�  ���� ���޸� _asm

	ImeBufferBak2 = ImeBuffer[ 0 ];
	//ImeBufferBak2 = ImeBuffer;
	// �ɣͣŤ�״�B��ӛ�������ϣΡ��ϣƣơ���  �����״̬
	imeOpenFlag = ImeOpenState();
	
	// capsLock ���ж�
	// ���`�ܩ`�ɤ�״�B��ȤäƤ��루ȫ�ƤΥ��`��
	//�ж�/ /���̵�״̬ץ��������Կ
	GetKeyboardState( ( BYTE *) keyBoardState );
	// capsLock ״�B�����ϣΡ��ϣƣơ���  �õ�  capsLock ״�B
	capsLockFlag = keyBoardState[ VK_CAPITAL ];
	
	// �ɣͣ�������`�������Фα�ʾ
	// �������Щ`���Ǳ�ʾ�Εr
	/// / IME������������ʾģʽ/ /����������ʾ��ʱ��
	if( TaskBarFlag == FALSE )
	{
		// �ɣͣŤ��ϣΤΕr   ON��ʱ��
		if( imeOpenFlag )
		{
			// CapsLock �Εr   �����Сд�Ĵ���
			if( capsLockFlag )
			{
				if( conversion >= 28 )
				{
					// ���ֱ�ʾ�Хåե������� ���ֱ�ʾ������
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       ABC", 0 );
				}
				else
				{
					// ���ֱ�ʾ�Хåե�������
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, ImeModeStrCaps[ conversion ], 0 );
				}
			}
			else
			{
				if( conversion >= 28 )
				{
					// ���ֱ�ʾ�Хåե�������
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       abc", 0 );
				}
				else
				{
					// ���ֱ�ʾ�Хåե�������
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, ImeModeStr[ conversion ], 0 );
				}
			}
		}
		// �ɣͣŤ��ϣƣƤΕr
		else
		{	
			// CapsLock �Εr
			if( capsLockFlag )
			{
				// ���ֱ�ʾ�Хåե�������
				StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       ABC", 0 );
			}
			else
			{
				// ���ֱ�ʾ�Хåե�������
				StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       abc", 0 );
			}
		}
	}
	
	// �ɣͣ������Фα�ʾ
	// �ɣͣŤ��ϣΤΕr���ģɣͣťХåե������֤�����Ȥ�
	/// / IME�ַ����ı�ʾ/ / IME ONʱ��IME�������ֵ�ʱ��
	if( imeOpenFlag && *ImeBuffer != NULL )
	{
		
		// �����������ʤ��r����   ���룬û�еط������
		if( pNowStrBuffer == NULL )
			return;
		
		// �������֤΃��ȶȤ���ɣͣťܥå����΃��ȶȤ�Q���  
		//�������ֵ����ȶ�IME������ȶȾ���
		if( pNowStrBuffer->fontPrio == FONT_PRIO_BACK )
		{
			imePrio = DISP_PRIO_IME1;
		}
		else 
			imePrio = DISP_PRIO_IME3;
		
		// �����Фκ��������
		//strWidth = GetStrWidth( pNowStrBuffer->buffer );
		// ���ֱ�ʾ�Хåե�������
		StockFontBuffer( pNowStrBuffer->imeX, pNowStrBuffer->imeY, pNowStrBuffer->fontPrio, 0, ImeBuffer, 0 );
		
		// ���֤α������軭
		// �ɣͣťХåե��`�������Фκ��������
		strWidth2 = GetStrWidth( ImeBuffer );
		//���������������ɥåȥ�`�ɤΕr
		if(ResoMode == 1)
		{
			// �ܥå�����ʾ�ǩ`����Хåե�������
			StockBoxDispBuffer( pNowStrBuffer->imeX - 1, 
				pNowStrBuffer->imeY   , 
				pNowStrBuffer->imeX + strWidth2, 
				pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
				imePrio, 0, 1 );
		}
		else 
		{
			// �ܥå�����ʾ�ǩ`����Хåե�������
			StockBoxDispBuffer( pNowStrBuffer->imeX - 1, 
				pNowStrBuffer->imeY -1, 
				pNowStrBuffer->imeX + strWidth2, 
				pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
				imePrio, 0, 1 );
		}
		// �Ĺ������ˤ�ȡ��   �Ľ������ȡ��
		while( 1 )
		{
			// �Ĺ���ʼ�ޤ���ä���  �Ľڿ�ʼ�Ļ�
			if( ( *ImeBlockBak == 1 || *ImeBlockBak == 3 ) && blockFlag == FALSE )
			{
				// �Ĺ���ʼ�ޤ������
				blockX1 = blockXcnt;
				// �Ĺ��ե饰�ϣ�
				blockFlag = TRUE;
				//blockFlag = *ImeBlockBak;
			}
			
			// �Ĺ��K�����ä��� �Ľڽ����Ļ�
			if( ( *ImeBlockBak == 2 || *ImeBlockBak == NULL ) && blockFlag >= TRUE )
			{
				// �Ĺ��νK��������
				blockX2 = blockXcnt;
				break;
			}
			
			// �K������
			if( *ImeBlockBak == NULL ) break;
			
			// ���ɥ쥹�����ˤθ���
			// ���֤�ȫ�ǤΕr / /��ַ������ĸ���ȫ��/ /���ֵ�ʱ��
			
			if( GetCharByte( *ImeBufferBak ) == 2 )
			{
				
				ImeBufferBak += 2;	// ���Х����M���
				ImeBlockBak += 2;	// ���Х����M���
				blockXcnt += FontZenkauWidth; // ȫ�ǥɥåȷ��㤹ȫ��Բ��ּ�
				
			}

			else
			{ // ��ǤΤȤ�
				
				ImeBufferBak += 1;	// ���Х����M���
				ImeBlockBak += 1;	// ���Х����M���
				blockXcnt += FontHankakuWidth; // ��ǥɥåȷ��㤹
			}
		}
		
		//�Ĺ�������Ȥ�
		if( blockFlag >= TRUE ){
			//���������������ɥåȥ�`�ɤΕr
			if(ResoMode == 1){
				// �ܥå�����ʾ�ǩ`����Хåե�������
				StockBoxDispBuffer( pNowStrBuffer->imeX + blockX1 - 1, 
									pNowStrBuffer->imeY    , 
									pNowStrBuffer->imeX + blockX2, 
									pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
									imePrio + 1, 252, 1 );
			} else {
				// �ܥå�����ʾ�ǩ`����Хåե�������
				StockBoxDispBuffer( pNowStrBuffer->imeX + blockX1 - 1, 
									pNowStrBuffer->imeY - 1, 
									pNowStrBuffer->imeX + blockX2, 
									pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
									imePrio + 1, 252, 1 );
			}
		}
	}
}
	
	
// WM_IME_COMPOSITION ��å��`���΄I�� **��Ϣ����**************************************/
void ImeComposition( LPARAM lParam )
{
	
	int strLen;
	char str[ 256 ];
	int i;
	BOOL zenkakuFlag = FALSE; // ȫ�ǥե饰
	
	// ���ִ_�������Ȥ� ����ȷ����ʱ��
	if ( lParam & GCS_RESULTSTR ){
		// ���֤��L����ȡ��  ���ֵĳ��ȵ�ȡ��
		strLen = ImmGetCompositionString( hImc, GCS_RESULTSTR, NULL, 0 );
		// ��ߥåȥ����å� ���޼��
		if( strLen >= 256 ) strLen = 255;
		// �����Ф�ȡ�äƤ���  ����������
		ImmGetCompositionString( hImc, GCS_RESULTSTR, str, strLen );
		// �K��ӛ��
		str[ strLen ] = NULL;
		// ��ǥ��ʤ�����r��ȫ�ǥ��ʤˉ�Q����
		//HanKanaToZenKana( str );
		// ��Q������֤��L����ȡ��  ת��������ֵĳ��ȵ�ȡ��
		strLen = strlen( str );
		// ��ߥåȥ����å�
		if( strLen >= 256 ) strLen = 255;
		// ���֤���������`��
		//pNowStrBuffer->buffer[256] =str[256];//�Ҽ�  ���Ϻ����˵ķ���NPC��Ϣ��ʧ
		//��ȥ ������������������
		
		for( i = 0 ; i < strLen ; i++ )
		{
			// ���֥Хåե�������  ���ֻ����� ��ȥ
			StockStrBufferChar( str[ i ] );
		}
	}
	
	// ��ʾ����Ƥ������֣��_��ǰ�� ��ʾ�����֣�ȷ��ǰ��
	if( lParam & GCS_COMPSTR )
	{
		
		// ���֤��L����ȡ��
		strLen = ImmGetCompositionString( hImc, GCS_COMPSTR, NULL, 0 );
		// ��ߥåȥ����å�
		if( strLen >= 256 ) strLen = 255;
		// �����Ф�ȡ�äƤ���
		ImmGetCompositionString( hImc, GCS_COMPSTR, ImeBuffer, strLen );
		// �K��ӛ�� ��ȥ
		ImeBuffer[ strLen ] = NULL;
			//pNowStrBuffer->buffer[256] =str[256];//�Ҽ�
		// ��ǥ��ʤ�����r��ȫ�ǥ��ʤˉ�Q����
		//HanKanaToZenKana( ImeBuffer );
		
	}
//	else{	// �����һ�����������Ȥ�
		// ���֤��L����ȡ��
//		strLen = ImmGetCompositionString( hImc, GCS_COMPSTR, NULL, 0 );
//		if( strLen == 0 ){
//			// �����Ф�դˤ���
//			*ImeBuffer = NULL;
//		}
//	}
	
	// �Ĺ���ȡ�äƤ���  ��������
	if( lParam & GCS_COMPATTR )
	{
		// ���֤��L����ȡ��
		strLen = ImmGetCompositionString( hImc, GCS_COMPATTR, NULL, 0 );
		// ��ߥåȥ����å�
		if( strLen >= 256 ) strLen = 255;
		// �����Ф�ȡ�äƤ���
		ImmGetCompositionString( hImc, GCS_COMPATTR, ImeBlock, strLen );
		// �K��ӛ��
		ImeBlock[ strLen ] = NULL;
	}
#if 0	
	// ��ꁢ����ȡ��
	if( lParam & GCS_COMPREADSTR ){
		// ���֤��L����ȡ��
		strLen = ImmGetCompositionString( hImc, GCS_COMPREADSTR, NULL, 0 );
		// �����Ф�ȡ�äƤ���
		ImmGetCompositionString( hImc, GCS_COMPREADSTR, str, strLen );
	}
	
	if( lParam & GCS_COMPCLAUSE ){
		// �K��ӛ��
		str[ 0 ] = NULL;
		// ���֤��L����ȡ��
		strLen = ImmGetCompositionString( hImc, GCS_COMPCLAUSE, NULL, 0 );
		// �����Ф�ȡ�äƤ���
		ImmGetCompositionString( hImc, GCS_COMPCLAUSE, str, strLen );
	}
	
	if( lParam & GCS_COMPREADCLAUSE ){
		// ���֤��L����ȡ��
		strLen = ImmGetCompositionString( hImc, GCS_COMPREADCLAUSE, NULL, 0 );
		// �����Ф�ȡ�äƤ���
		ImmGetCompositionString( hImc, GCS_COMPREADCLAUSE, str, strLen );
	}
#endif
}
	
/* WM_IME_NOTIFY ��å��`���΄I�� *****����IME��Ϣ����****************************************/
void ImeNotify( WPARAM wParam ,LPARAM lParam )
{
	switch ( wParam )
	{
	
	case IMN_OPENCANDIDATE:
		//�������ԣ��任�򲹽����С���ڴ򿪵�ʱ����
		// ���Υ�å��`���ϡ���Q���a����äƤ�����ä��㤤������ɥ����_�������ߥ󥰤����롣
	case IMN_CHANGECANDIDATE:// ���Υ�å��`���ϡ���Q���a��䤨�뤿�Ӥ����롣
		//�������ԣ�ÿ�����ı�任�򲹡�
		break;
		
	case IMN_CLOSECANDIDATE:
		// ���Υ�å��`���ϡ���Q���a����äƤ�����ä��㤤������ɥ��������륿���ߥ󥰤Ǥ��롣
		//�������ԣ��任�򲹽����С������ʧ��ʱ����
		break;
		
#if 0	
	case IMN_SETOPENSTATUS:// IME���Є��ˤ�����o���ˤ����ꤷ���Ȥ����롣
		break;
#endif
    }
}
#if 0
/** �ɣͣ��v�B��å��`���I�� **************************************************/
void ImeMsgProc( UINT Message, WPARAM wParam, LPARAM lParam )
{
	// ��å��`���Ƿ��
	switch( Message ){
	
		//IME��С���ڱ仯��ʱ��
		case WM_IME_COMPOSITION: // �ɣͣŤΤ��ä��㤤������ɥ��ˉ仯������r
	
			// WM_IME_COMPOSITION ��å��`���΄I�� ��Ϣ����
			ImeComposition( lParam );
			break;
		case WM_IME_ENDCOMPOSITION: // ���֤�_������ֱ������롣��Q�Ф����֤�ȫ��BS�Ǥ������Ȥ��⤯�롣
			// �ɣͣťХåե��`��դˤ���
			*ImeBuffer = NULL;
			break;
		
		case WM_IME_NOTIFY:
			/* WM_IME_NOTIFY ��å��`���΄I�� */
			//ImeNotify( wParam ,lParam );
			// ������DEFWINDOWPROC������ȡ���Q���a������ɥ�������褦����
			break;
		case WM_IME_SETCONTEXT:
			//���ڻ������Ȥ����Ƥ�Τ��ʣ��ե��`�������Ƥä��Ȥ�������褦����
			break;
#if 0
		case WM_IME_STARTCOMPOSITION:// IME��ON��״�B�ǡ����֤���������ֱ������롣
			break;
		case WM_IME_CONTROL:
			break;
		case WM_IME_SELECT: // IME�򤨤����Ȥ�����餷�����Хåե�����ڻ��Ȥ�����Ф����Τ��ʣ�
			break;
		case WM_IME_KEYDOWN:   // IME�Υ��`���`��
			// ������DefWindowProc������ȡ�WM_KEYDOWN�����ɤ���
			break;
		case WM_IME_CHAR:
			// ������DefWindowProc������ȡ�WM_CHAR�����ɤ���
			break;
#endif
	}
}
#endif
