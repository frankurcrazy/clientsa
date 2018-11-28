/************************/	
/*	gamemain.cpp 	*/
/************************/
#include "system.h"
#include <winuser.h>
#include <winbase.h>
#include "loadrealbin.h"
#include "loadsprbin.h"
#include "../oft/sys2.h"
#include "ime_sa.h"
#include "anim_tbl.h"
#include "battleMap.h"
#include "netmain.h"
#include "savedata.h"
#include "produce.h"
#include "battleProc.h"
#include "t_music.h"
#include "menu.h"
#include "handletime.h"
#include "battleMenu.h"

#include "pc.h"
#include "map.h"

#define NO_DRAW_MAX_CNT 20	// �軭���ʤ����ե�`����  ��ͼ�������
//�����ϵͳ SKIPָ��
#define FRAME_SKIP	0		// ������������ƥ� �ϣΣ������ϣƣƣ���  �����ϵͳ

// ������ɥ���`���Ф��椨�v������ ����ģʽת����������
void ChangeWindowMode( void );
// ����ȥ�`���O���I��
void SetResoMode(int Mode);

//����ģʽ3�� ��ǰ�ر��ҿ���
//#ifndef _DEBUG
//#define _DEBUG	// �Ȥꤢ�����ǥХå���`��  ��֮���Է�ʽ
//#endif

int testCnt;
static char no_wait_cnt = 0;

//---------------------------------------------------------------------------//
// �����`�Х�������x       ȫ�ֱ�������                                                 //
//---------------------------------------------------------------------------//
int	GameState;					// ���`���״�B ��Ϸ��״̬
double ProcTime = 16.666667;	// �I����ʹ���r�g������軭�r�g 
//����ʱ�䣬ʹ�ú���Ƶ�ʱ��
//double ProcTime = 33.333333;				// �I����ʹ���r�g������軭�r�g
double SystemTime = 16.666667;	// �����󥯤� 16 msc
//float SystemTime = 33.33333333;	// �����󥯤� 16 msc
double NowTime; 					// �F�ڤΕr�gӛ��
//��ͼ������̨
int	  NoDrawCnt = 1;			// �軭���Ƥʤ����������󥿩`
//��������滭����
int BackBufferDrawType;			// �Хå��Хåե��`�軭����

// �ե�`���`��Ӌ����  ֡����
int	  FrameRate;				// �ե�`���`��  ֡����
//��ÿ�뼸�Ż��Ƽ���
int	  DrawFrameCnt;				// һ���g�˺�ö�軭��������ӛ��
DWORD DrawFrameTime;			// һ���g�������륫���󥿩` 1�����
#ifdef _DEBUG
static int palNo = 0;			// �ѥ�åȷ���  �������
static int fontNo = 0;			// �ե���ȷ���
#endif

//---------------------------------------------------------------------------//
// ��Ҫ ��������ɥ������ᥤ��I���v��                                       //
// ���� ��HWND hWnd : ������ɥ����R�e�ϥ�ɥ�                               //
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
// ��Ҫ ��������ɥ������ᥤ��I���v��                                       //
// ���� ��HWND hWnd : ������ɥ����R�e�ϥ�ɥ�                               //
//  ��Ҫ�����ڿ�����Ҫ��������/ // /������HWND hWnd:���ڵ�ʶ���ֱ�
//---------------------------------------------------------------------------//
BOOL GameMain( void )
{
	// ���`���_ʼ�I��
	if( InitGame() == FALSE ) return FALSE;
	
	// �ᥤ���`��
	while( 1 )
	{
		// ��å��`����`�ץ��`��  ��Ϣѭ������
		if( SystemTask() == FALSE ){
		
			EndGame(); // ���`��K�˄I��
			
			return FALSE;
		}
		
		// �ӣ��r�g��ȡ��
		RealTimeToSATime( &SaTime );
		
		// ��ʾ�����󥿩`���ڻ�����������Εr����ڻ����뤿�ᤳ���Ǥ��룩
		DispBuffer.DispCnt = 0;
		// �ե���ȥ����󥿩`���ڻ�  �����̨���ڻ�
		FontCnt = 0;
		
		// �ͥåȥ�`���I��
#ifdef _DEBUG
		if( !offlineFlag )
			networkLoop();
#else
		networkLoop();
#endif
		
		// �ãģģ������� ��ԩ`�� ���ظ�
		cdda_play( cdda_no );
		
		joy_read(); 	// ���`�ܩ`������  ��������
#if 1
//		if( joy_trg[ 1 ] & JOY_PRINT_SCREEN )
		if( joy_trg[ 1 ] & JOY_F12 )
		{
			static unsigned int prePushTime = 0;

			// �B�����ƣ�0.5���g����
			if( prePushTime + 500 <= GetTickCount() )
			{
				// �F�ڤλ����£ͣХե�����˱���
				snapShot();
				prePushTime = GetTickCount();
			}
		}
#endif
		Process();		// �������I��  ������
		
		MouseProc();	// �ޥ����I��
		
		SortDispBuffer(); 	// ��ʾ�Хåե����`�� ��ʾ���򻺳���
		
		HitMouseCursor();	// �ޥ������`����Τ������ж�
		
		PaletteProc();		// �ѥ�åȄI��
		

#ifdef _DEBUG
		// ����ȉ��
		if( joy_trg[ 1 ] & JOY_F9  && WindowMode ){
#else			
		// ����ȉ��
		if( joy_trg[ 1 ] & JOY_F9  && WindowMode && LowResoCmdFlag ){
#endif
			// �ݳ��ФǤʤ����
			if( BackBufferDrawType != DRAW_BACK_PRODUCE ){
				
				// DirectDraw���֥������Ȥ�����r
				if( lpDraw != NULL ){
					// DirectDraw �_��
					ReleaseDirectDraw();
					// �ե���ȥ��֥������Ȥ�Ĩ��
					DeleteObject( hFont );
					// ����ȥ�`���O��
					SetResoMode( (ResoMode + 1 ) & 1 );

					// ������ɥ���`�ɤǤʤ����
//					if( !WindowMode ){
						// ������ɥ���`�ɥե饰���
//						if( WindowMode == TRUE ) WindowMode = FALSE;
//						else WindowMode = TRUE;
//						WindowMode = TRUE;
//					}
					// DirectDraw ���ڻ�
					InitDirectDraw();
					// ���ե�����`�󥵩`�ե��������� 
					InitOffScreenSurface();
					// �ե���ȥ��֥������Ȥ����� */
					InitFont( 0 );
					// ������ɥ���`�ɤʤ�
//					if( WindowMode ){
						// ������ɥ���`���Ф��椨
						ChangeWindowMode();
//					}

					// ȫ������ɥ������軭Ҫ��
					//SendMessage( HWND_BROADCAST, WM_PAINT, NULL, NULL );
					
					// �ѥ�åȳ��ڻ�
					if( InitPalette() == FALSE ){
						PostMessage( hWnd, WM_CLOSE, 0, 0L );
					}
					// �Хȥ�ޥå��i���z�ߤȥХȥ륵�`�ե����λ�������
					if( ProcNo == PROC_BATTLE ){ 
						// �Хåե����ڻ�
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						ReadBattleMap( BattleMapNo );
						// �Хå����`�ե������\�ǥ��ꥢ�`
						ClearBackSurface();	
						// �£ͣФ�Хå����`�ե����˥��å�
						PutBmp();	
						// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
						lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
						// �Хåե����ڻ�
						DispBuffer.DispCnt = 0;
						FontCnt = 0;
						// �F�ڤΕr�g��ӛ��
						NowTime = GetTickCount();
					}
//					continue;
				}
			}
		}
#ifdef _DEBUG
#if 0
		// �ѥ�åȉ��
		if( joy_trg[ 1 ] & JOY_F8 ){	/* �ץ饹 */
			palNo++;
			if( palNo >= MAX_PAL ) palNo = 0;
			// �ѥ�åȥ�����
			PaletteChange( palNo, 300 );
			// �����ƥ�ʹ����
			play_se( 212, 320, 240 );
		}
		// �ե���ȉ��
		if( joy_trg[ 1 ] & JOY_F6 ){	/* �ץ饹 */
		}
		// �ե���ȉ��
		if( joy_trg[ 1 ] & JOY_F5 ){	/* �ץ饹 */
		// ���Υ��ץꥱ�`�����Θ��ޥ�٥���O��(������٥� & �ե륹����`���棩
		//	if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_NORMAL ) != DD_OK )
		//	{
		//		MessageBox( hWnd, "SetCooperativeLevel Error", "�_�J", MB_OK | MB_ICONSTOP );
			//return FALSE;
		//	}
		}
		
		// �ե���ȉ��
		if( joy_trg[ 1 ] & JOY_F7 ){	/* �ץ饹 */
			// ��`��ǩ`�����i���z��
			//LoadMailHistory();
			// ȫ������ɥ������軭Ҫ��
		//	SendMessage( HWND_BROADCAST, WM_PAINT, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_PALETTECHANGED, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_ERASEBKGND, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_PAINTCLIPBOARD, NULL, NULL );
		//	SendMessage( HWND_BROADCAST, WM_PAINTICON, NULL, NULL );
		
		// ���Υ��ץꥱ�`�����Θ��ޥ�٥���O��(������٥� & �ե륹����`���棩
		//if( lpDraw->lpDD2->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) != DD_OK )
		//{
		//	MessageBox( hWnd, "SetCooperativeLevel Error", "�_�J", MB_OK | MB_ICONSTOP );
			//return FALSE;
		//}
			//MessageBox( hWnd, "�ģ������ģ����γ��ڻ���ʧ�����ޤ�����", "�_�J", MB_OK | MB_ICONSTOP );
			//BattleResultWndFlag = 2;
			//BattleResultWndFlag = 1;
			fontNo++;
			if( fontNo >= 3 ) fontNo = 0;
			// �ե���ȥ�����
			InitFont( fontNo );
			// �����ƥ�ʹ����
			play_se( 212, 320, 240 );
		}
		
		extern void lssproto_MSG_recv( int fd,int aindex,char* text ,int color);
		// �ե���ȉ��
		if( joy_trg[ 1 ] & JOY_F5 ){	/* �ץ饹 */
			char moji[ 256 ];
			static cnt = 0;
			// ��`�뤬�줤����å��`������
			sprintf( moji," 9/%02d 12:15|��������������������������������������������������������������������������������������������������������������������������������������������", cnt, cnt++ );
			// ���ǽ줤��
			lssproto_MSG_recv( 0, 1, moji ,0 );
			// �����ƥ�ʹ����
			//play_se( 212, 320, 240 );
			// ��`��ǩ`���α���
			//SaveMailHistory();
		}
#endif		
#if 0		

		// ���ѥ�åȉ��
		if( joy_auto[ 0 ] & JOY_A ){	/* �ץ饹 */
			// �����ƥ�ʹ����
			play_se( 212, 320, 240 );
		}
		// �¥ѥ�åȉ��
		if( joy_auto[ 0 ] & JOY_B ){	/* �ץ饹 */
			// �����ƥ�ʹ����
			play_se( 212, 320, 240 );
		}
		
		// ���ѥ�åȉ��
		//if( joy_trg[ 0 ] & JOY_A ){	/* �ץ饹 */
			// �����ƥ�ʹ����
		//	play_se( 212, 320, 240 );
		//}
		// �¥ѥ�åȉ��
		//if( joy_trg[ 0 ] & JOY_B ){	/* �ץ饹 */
			// �����ƥ�ʹ����
		//	play_se( 212, 320, 240 );
		//}
		
		
		// �ѥ�åȉ��
		//if( joy_trg[ 1 ] & JOY_CTRL_MINUS ){	/* �ץ饹 */
			// �����ƥ�ʹ����
		//	play_se( 212, 320, 240 );
		//}
		// ���ѥ�åȉ��
		//if( joy_trg[ 1 ] & JOY_CTRL_MINUS ){	/* �ץ饹 */
			// �����ƥ�ʹ����
		//	play_se( 212, 320, 240 );
		//}
		// �ѥ�åȉ��
		//if( joy_trg[ 1 ] & JOY_CTRL_CIRCUMFLEX ){	/* �ץ饹 */
			// �����ƥ�ʹ����
		//	play_se( 212, 320, 240 );
		//}
		// �ѥ�åȉ��
		if( joy_trg[ 1 ] & JOY_CTRL_YEN ){	/* �ץ饹 */
			// �����ƥ�ʹ����
			play_se( 212, 320, 240 );
		}
		// �ѥ�åȉ��
		//if( joy_trg[ 1 ] & JOY_PRINT_SCREEN ){	/* �ץ饹 */
			// �����ƥ�ʹ����
		//	play_se( 212, 320, 240 );
		//}
		// �ѥ�åȉ��
		if( joy_trg[ 1 ] & JOY_RSHIFT ){	/* �ץ饹 */
			// �����ƥ�ʹ����
			play_se( 212, 320, 240 );
		}
		// �ѥ�åȉ��
		if( joy_trg[ 1 ] & JOY_LSHIFT ){	/* �ץ饹 */
			// �����ƥ�ʹ����
			play_se( 212, 320, 240 );
		}
		// �ѥ�åȉ��
		if( joy_trg[ 1 ] & JOY_TAB ){	/* �ץ饹 */
			// �����ƥ�ʹ����
			play_se( 212, 320, 240 );
		}
		// �ѥ�åȉ��
		//if( joy_trg[ 1 ] & JOY_RETURN ){	/* �ץ饹 */
			// �����ƥ�ʹ����
		//	play_se( 212, 320, 240 );
		//}
		// �ѥ�åȉ��
		if( joy_trg[ 1 ] & JOY_CTRL_W ){	/* �ץ饹 */
			// �����ƥ�ʹ����
			play_se( 212, 320, 240 );
		}
#endif		
		
		// ����`�ȥ��åȥ��`  ��ݼ�
		if( joy_trg[ 0 ] & JOY_HOME ){
			strcpy( MyChatBuffer.buffer + MyChatBuffer.cnt, DebugKey0 );
			MyChatBuffer.cnt += strlen( DebugKey0 );
		}
		if( joy_trg[ 0 ] & JOY_END ){
			strcpy( MyChatBuffer.buffer + MyChatBuffer.cnt, DebugKey1 );
			MyChatBuffer.cnt += strlen( DebugKey1 );
		}
		if( joy_trg[ 0 ] & JOY_INS ){
			strcpy( MyChatBuffer.buffer + MyChatBuffer.cnt, DebugKey2 );
			MyChatBuffer.cnt += strlen( DebugKey2 );
		}
		
		// �ե�`���`�Ȥ�Ӌ�㣨���ˤ����ɣ�  ֡���ʵļ��㣨���������
		DisplayFrameRate();
		
		// ����ʾ
		{
			static int flag = 0;
			// PageUp Ѻ�����r����ʾ���ȥ��룩
			if( di_key[ DIK_PRIOR ] & 0x80 ){ 
				if( flag == 0 ) flag = 1;
				if( flag == 2 ) flag = 3;
			}else{
				if( flag == 1 ) flag = 2;
				if( flag == 3 ) flag = 0;
			}
			if( flag >= 1 ) InfoDisp();
		}
#endif	
	
#if FRAME_SKIP
		// �軭�r�g���ФäƤ��ʤ��Ȥ����軭���ʤ� ( �����I���ˑ��� )
		if( NowTime + ProcTime < GetTickCount() ){
			// �軭���ʤ��Ƥ⤤���r�ʤ�
			if( NoDrawCnt < NO_DRAW_MAX_CNT ){
				// �����ƥॿ����ץ饹�� �RԽ )
				NowTime += SystemTime;
				// �軭���Ƥʤ������������
				NoDrawCnt++;
				continue;
			}
		}
#endif		
#ifdef _DEBUG		
		// �F�ڱ�ʾ���Ƥ��륵�`�ե������γ��ڻ�
		SurfaceDispCnt = 0;
		
		// �軭�r�g���v���� �ǥХå��� ��
		DrawDebugLine( 250 ); 
#endif		
		// �Хå��Хåե��`�軭�����Ƿ��
		switch( BackBufferDrawType ){
		
			case DRAW_BACK_NON:		// �軭���ʤ�
				break;
				
			case DRAW_BACK_NORMAL:	// �Хå����`�ե������\�ǥ��ꥢ�`
			
				ClearBackSurface();	
				break;
				
			case DRAW_BACK_PRODUCE:	// �ݳ���
				break;
				
			case DRAW_BACK_BATTLE:	// �Хȥ�ޥå��軭
			
				DrawBattleMap();
				break;
				
		}
		

#ifdef _DEBUG
		no_wait_cnt++;
		no_wait_cnt&=7;
		if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//�ӣȣɣƣԤ�Ѻ���Ƥ���
			if(!no_wait_cnt){
				PutBmp();	// �£ͣФ�Хå����`�ե����˥��å�
			}
		} else {
			PutBmp();	// �£ͣФ�Хå����`�ե����˥��å�
		}
#else
		
		PutBmp();	// �£ͣФ�Хå����`�ե����˥��å�
#endif	
		
#ifdef _DEBUG
			// �����軭�r�g���ࣩ�� �ǥХå��� ��
			//DrawDebugLine( 252 ); 
#endif	
		// �ե���Ȥ�Хå����`�ե����˥��å�
		//PutText( FONT_PRIO_FRONT );	
		
		// ������ɥ���`�ɤǤʤ��r
		//if( !WindowMode ){
			// �£ͣб�ʾ�Хåե��γ��ڻ�
		//	DispBuffer.DispCnt = 0;
			// �ե���ȥ����󥿩`���ڻ�
		//	FontCnt = 0;
			// �ޥ������`����α�ʾ����ȱ�ʾ��
		//	StockDispBuffer( mouse.nowPoint.x + 8, mouse.nowPoint.y + 8, DISP_PRIO_TOP, 5200, 0 );
		//	PutBmp();	// �£ͣФ�Хå����`�ե����˥��å�
		//}
		
#ifdef _DEBUG		
		
		// ��ä��r�g���\���� �ǥХå��� ��
		DrawDebugLine( 0 );	
#endif		
		
#ifdef _DEBUG		
		if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//�ӣȣɣƣԤ�Ѻ���Ƥ���
			if(!no_wait_cnt){
				Flip();	// ��ֱͬ�ھ��r���Ф��椨�I��
			}
		} else {
			Flip();	// ��ֱͬ�ھ��r���Ф��椨�I��
		}
#else
		
		Flip();	// ��ֱͬ�ھ��r���Ф��椨�I��
#endif		
		
#ifdef _DEBUG		
		// �I��r�g���ࣩ�� �ǥХå��� ��
		DrawDebugLine( 249 ); 
#endif
		
#if FRAME_SKIP
#ifdef _DEBUG		
		if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//�ӣȣɣƣԤ�Ѻ���Ƥ���
			// �Ω`��������
			NowTime = GetTickCount();
		} else {
			// �r�g�Τ�����{��
//			while( NowTime >= GetTickCount() );
			while( NowTime >= GetTickCount() ){
				// ������ɥ���`�ɤʤ�
				if( WindowMode ){
					Sleep(1);
				}
			}
		}
#else
		// �r�g�Τ�����{��
//		while( NowTime >= GetTickCount() );
		while( NowTime >= GetTickCount() ){
			// ������ɥ���`�ɤʤ�
			if( WindowMode ){
				Sleep(1);
			}
		}
		
#endif
#endif

		// �ѥ�åȥ����󥸥ե饰�ϣΤΕr
		if( PalChangeFlag == TRUE ){
			// һ���_��
		//    lpDraw->lpPALETTE->Release();
			// 8Bit����ǥå�����Palette�����ǳ��ڻ������Y����lpPALETTE�ط�����
		//	lpDraw->lpDD2->CreatePalette( DDPCAPS_8BIT, Palette, &lpDraw->lpPALETTE, NULL );
		
		
			// �ѥ�åȤ��������O��
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
			PalChangeFlag = FALSE; // �ե饰���ڻ�
		}
		
		// �����ƥॿ����ץ饹
		NowTime += SystemTime;
		// �軭���Ƥʤ��������ڻ�
		NoDrawCnt = 1;
		
#ifdef _DEBUG		
		// һ���g���軭����ö���򥫥����
    	DrawFrameCnt++;
#endif
		// ���`�ե�����ʹ�ä����ո������
		SurfaceDate++;
	}

	return FALSE;
}

#ifdef _DEBUG		
// ����ʾ *******************************************************************/
void InfoDisp( void )
{
	char szMoji[ 256 ];
	int x = 16;
	int y = 16;
	char c = 0;
	
	sprintf( szMoji,"ActionCnt        = %d", ActCnt );
	// �ե��������Хåե�������
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"DispCnt          = %d", DispBuffer.DispCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceCnt       = %d",SurfaceCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"VramSurfaceCnt   = %d",VramSurfaceCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SysramSurfaceCnt = %d",SysramSurfaceCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceUseCnt    = %d",SurfaceUseCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"SearchPoint      = %d",SurfaceSearchPoint );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceDispCnt   = %d",SurfaceDispCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"SurfaceDate      = %d",SurfaceDate );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;c = 0;
	
	sprintf( szMoji,"FrameRate        = %d",FrameRate );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"HitDispNo        = %d",HitDispNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"HitFontNo        = %d",HitFontNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"MouseLevel       = %d",mouse.level );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"ProcNo           = %d",ProcNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"SubProcNo        = %d",SubProcNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"ActionSize       = %d",sizeof( ACTION ) );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"MouseX           = %d",mouse.nowPoint.x );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"MouseY           = %d",mouse.nowPoint.y );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"TimeZone         = %d",SaTime.hour );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16; c = 0;
	
	sprintf( szMoji,"PalNo            = %d",PalState.palNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"BattleMapNo      = %d",BattleMapNo );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;
	
	sprintf( szMoji,"HogeCnt          = %d",testCnt );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

	sprintf( szMoji,"EventEnemyFlag   = %d",eventEnemyFlag );
	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 0 ] = %d",pc.selectPetNo[ 0 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 1 ] = %d",pc.selectPetNo[ 1 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 2 ] = %d",pc.selectPetNo[ 2 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 3 ] = %d",pc.selectPetNo[ 3 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"selectPetNo[ 4 ] = %d",pc.selectPetNo[ 4 ] );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"BattlePetStMenCnt= %d",BattlePetStMenCnt );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

//	sprintf( szMoji,"BattlePetReceiveFlag= %d",BattlePetReceiveFlag );
//	StockFontBuffer( x, y, FONT_PRIO_FRONT, c++, szMoji, 0 );y += 16;

}
#endif		

// �ե�`���`�Ȥ�Ӌ�㣨���ˤ����ɣ�
void DisplayFrameRate( void )
{
	// һ�뤿�ä������
    if( GetTickCount() - DrawFrameTime >= 1000 ){
	
		// �ե�`��쥤�Ȥ�ӛ��
        FrameRate = DrawFrameCnt;
		// ��Εr�g��ӛ��
        DrawFrameTime = GetTickCount();
		// һ���g���軭����ö������ڻ�
        DrawFrameCnt = 0;
    }
}

// �����α�ʾ ***��־�ı�ʾ**************************************************************/
void PutLogo( void )
{
	
	// �£ͣб�ʾ�Хåե��γ��ڻ�
	DispBuffer.DispCnt = 0;
	StockDispBuffer( 320, 240, DISP_PRIO_TOP, CG_TITLE_NOW_LOADING, 0 );
	ClearBackSurface();	// �Хå����`�ե������\�ǥ��ꥢ�`  �����ӿڿ�ʼ�´�λͼ
	PutBmp();			// �£ͣФ�Хå����`�ե����˥��å� ����λͼ
	Flip();				// ��ֱͬ�ھ��r���Ф��椨�I��  ��ֱ��ʱ��ת������
}	

// ���`���_ʼ�I�� ////��Ϸ��ʼ����//////////////////////////////////////////////////////////
BOOL InitGame( void )
{
	
	// DirectDraw ���ڻ�  ���ﲻ��Ϥ�Ժ�ѧϰ�����и����Բ���16λɫ����ϵͳ����
	if( InitDirectDraw() == FALSE ){
		MessageBox( hWnd, "�ģ������ģ����γ��ڻ���ʧ�����ޤ�����", "�_�J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	/* ���ե�����`�󥵩`�ե���������  ��ʼ�� ����ӿ�Ϊ��ʾ��׼��*/
	if( InitOffScreenSurface() == FALSE ){
		MessageBox( hWnd, "���ե�����`�󥵩`�ե����������ɤ�ʧ�����ޤ�����", "�_�J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}

	// �ѥ�åȳ��ڻ� ��ɫ���ص�Ҫѧϰ�� ������
	if( InitPalette() == FALSE ) return FALSE;
	
	//_asm{int 3}
	// Realbin �v�B�γ��ڻ�  �ص��� ���ﵽ�˵�ͼͼƬ�����ļ�
	if( initRealbinFileOpen( realBinName, adrnBinName ) == FALSE ) {
	//if( initRealbinFileOpen( "..\\real.bin", "..\\adrn.bin" ) == FALSE ) {
		MessageBox( hWnd, "Real.bin �Υ��`�ץ��ʧ�����ޤ�����", "�_�J", MB_OK | MB_ICONSTOP );
		//MessageBox( hWnd, "�ӣ�գ������.����夫�����Ӥ��Ƥ���������", "�_�J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	// �����α�ʾ
	PutLogo();
	
	// ���`�ȥޥå׳��ڻ��I��
	initAutoMapColor( adrnBinName );

	// SprBin �v�B�γ��ڻ�
	if( InitSprBinFileOpen( sprBinName, sprAdrnBinName ) == FALSE ){
		MessageBox( hWnd, "Spr.bin �Υ��`�ץ��ʧ�����ޤ�����", "�_�J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}

	/* �����������ڻ� */
	InitAction();
		
	// �ե���ȥ��֥������Ȥ����� */
	InitFont( 0 );
	//InitFont( 2 );
	
	// ����ϵ�г��ڻ�
	srand( ( unsigned int )GetTickCount() );
	initRand2();
	
	// DirectSound ���ڻ�
	t_music_init();

	// �����쥯�ȥ���ץåȳ��ڻ�
	InitDInput();
	
	// �ãģģ�����
	//cdda_play( Rnd( 2, 34 ) );
	
	// �ɣͣ��v�B�γ��ڻ�
	InitIme();
	
	// �ޥ����I���γ��ڻ�
	MouseInit();

	// CD���`���ѥ���`���i���z��
	if( loadUserSetting() == FALSE )
	{
		char msg[1024];
		sprintf( msg, SAVE_ERRMSG_loadNowState, savedataErrorCode );
		MessageBox( hWnd, msg, "�_�J", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	// ����åȤε��h���֤��i���z��  �����ע�����ֵĶ�ȡ
	LoadChatRegistyStr();				
	
	// �F�ڤΕr�g��ӛ��
	NowTime = GetTickCount();
	DrawFrameTime = GetTickCount();

// �ǥХå���Ǥʤ��Ȥ�	
#ifndef _DEBUG
	// �ޥ������`������L��
	ShowCursor( FALSE );
#endif	
	return TRUE;
}	

// ���`��K�˄I�� //////////////////////////////////////////////////////////////
void EndGame( void )
{
	//DirectDraw �_��
	ReleaseDirectDraw();
	// �����쥯�ȥ���ץå��_��
	EndDInput();
	// DirectSound �_��
	t_music_end();
	// �ãģģ�ֹͣ
	cdda_stop();
	// Rialbin �]����
	cleanupRealbin();
	// �ե���ȥ��֥������Ȥ�Ĩ��
	DeleteObject( hFont );
	// �ɣͣŤνK��
	EndIme();
	// ���������K�˄I��
	EndAction();
	// �ͥåȥ�`���K�˄I��
	cleanupNetwork();
	// ͬ�r���ӥ����å����֥��������_��
	ReleaseMutex( hMutex );
	// �ѥ��`�����`��
	saveUserSetting();
	// ����åȤε��h���֤α���
	SaveChatRegistyStr();
	
	// �����ީ`�K��
//	timeKillEvent(timerId);
	
	//{
		// ALT+TAB �Є��ˤ���
	//	int nOldVal;
	//	SystemParametersInfo (SPI_SCREENSAVERRUNNING, FALSE, &nOldVal, 0);
	//}
}
