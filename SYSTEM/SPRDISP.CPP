/************************/
/*	sprdisp.c	������ʾ		*/
/************************/
#include "system.h"
#include "loadrealbin.h"
#include "loadsprbin.h"
#include "anim_tbl.h"
#include "map.h"

#define STOCK_DISP_BUFFER_NO_BMP	( 1 << 31 ) 	// �£ͣФǤʤ��r
#define STOCK_DISP_BUFFER_LINE		( 1 << 30 ) 	// �饤��
#define STOCK_DISP_BUFFER_BOX		( 1 << 29 ) 	// �ܥå���
#define STOCK_DISP_BUFFER_BOX_FILL	( 1 << 28 )		// �ܥå����T��Ĥ֤�
#define STOCK_DISP_BUFFER_CIRCLE	( 1 << 27 ) 	// ��

// ��ʾ�Хåե�������
DISP_BUFFER DispBuffer;

// �£ͣФΥ���`���ǩ`���ޤǤΥ��ե��å�
int BmpOffBits;
// �£ͣ��i���z���å�`���I��ؤΥݥ���
LPBITMAPINFO lpBmpInfo;
// Realbin �i���z���å�`���I��ؤΥݥ���
char *pRealBinBits;
// ��� Realbin �����i���z��£ͣФΥ�����
int RealBinWidth, RealBinHeight;
// BitBlt�Εr
HBITMAP	hBmp;

// ���`�ե����������`�ե饰
int SurfaceBusyFlag = 0;

// ������ɥ��ݥ���
extern ACTION* pActMenuWnd2;
// �ȥ���ե饰
extern unsigned int MenuToggleFlag;

// ���`�ȱ��^�v��
int SortComp( DISP_SORT *pDisp1, DISP_SORT *pDisp2 );
typedef int CMPFUNC( const void *, const void * );
// ��ʾ�Хåե��`����ܥå����ǩ`����ȡ�����
void GetBoxDispBuffer( DISP_INFO *pDispInfo, int bmpNo );

// ��ʾ�Хåե����`�� /////��ʾ���򻺳���//////////////////////////////////////////////////////
void SortDispBuffer( void )
{	
	//�����å����`�� ��������
	qsort( 	DispBuffer.DispSort,	// ������Υ��ɥ쥹 �ṹ�ĵ�ַ
			DispBuffer.DispCnt,		// ���^���낀��
			sizeof( DISP_SORT ), 	// ������Υ����� �ṹ�ߴ�
			( CMPFUNC * )SortComp 	// ���^�v���ؤΥݥ��� �ȽϺ�����ָ��
		);				
}

// �£ͣФ��軭 ///�趨����- -//////////////////////////////////////////////////////////////
void PutBmp( void )
{
	SURFACE_INFO *lpSurfaceInfo;
	DISP_SORT 	*pDispSort = DispBuffer.DispSort;
	DISP_INFO 	*pDispInfo;
	int i;
	int bmpNo;
	UCHAR putTextFlag = 0; // �ե���Ȥ��軭�������ե饰 ����ı�־ ���������ת�۵�
	unsigned char drawMapEffectFlag = 0;
	
	// ����줿��������`��  ����Ĵ���ѭ��
	for( i = 0; i < DispBuffer.DispCnt ; i++, pDispSort++ ){
		// ��ʾ�ǩ`������äƤ��륢�ɥ쥹�˥��å�
		pDispInfo = DispBuffer.DispInfo + pDispSort->no;
		bmpNo = pDispInfo->bmpNo; // �£ͣз���
		
		// �ե���Ȥα�ʾ( BACK )
		if( putTextFlag == 0 ){
			// ��˥�`��ǰ���ä���
			if( pDispSort->dispPrio >= DISP_PRIO_MENU ){
				// �ե���Ȥ�Хå����`�ե����˥��å�
				PutText( FONT_PRIO_BACK );	
				// �ե饰�ϣ�
				putTextFlag = 1;	
			}
		}
		// �ե���Ȥα�ʾ( FRONT )
		if( putTextFlag == 1 ){
			// �Ϥ���������������ɥ���ǰ���ä���
			if( pDispSort->dispPrio >= DISP_PRIO_YES_NO_WND ){
				// �ե���Ȥ�Хå����`�ե����˥��å�
				PutText( FONT_PRIO_FRONT );	
				// �ե饰�ϣ�
				putTextFlag = 2;	
			}
		}
		
		// ��˥�`�����K��ä���
		if( pDispSort->dispPrio > DISP_PRIO_MENU ){
			// �ޥåץ�����ɥ�����r
			if( MenuToggleFlag & JOY_CTRL_M && pActMenuWnd2 != NULL ){
				// ������ɥ����������äƤ�����
				if( pActMenuWnd2->hp > 0 ){
					// ���`�ȥޥå��軭
					drawAutoMap( pActMenuWnd2->x, pActMenuWnd2->y );
				}
			}
		}

		// �ޥåפΥ��ե�����
		if( pDispSort->dispPrio >= DISP_PRIO_RESERVE )
		{
			drawMapEffect();
			drawMapEffectFlag = 1;
		}


		// �£ͣ�����Εr
		if( bmpNo & STOCK_DISP_BUFFER_NO_BMP ){
			// �ܥå������ޤ��ϥ饤��Εr
			if( bmpNo & ( STOCK_DISP_BUFFER_BOX | STOCK_DISP_BUFFER_BOX_FILL | STOCK_DISP_BUFFER_LINE ) ){
				// ��ʾ�Хåե��`����ܥå����ǩ`����ȡ�����
				GetBoxDispBuffer( pDispInfo, bmpNo );
			}
			continue;
		}
		
		// �£ͣФΕr
		// �������ж��Τߤ��루�Ǳ�ʾ��
		if( pDispInfo->pAct != NULL ){
			if( pDispInfo->pAct->atr & ACT_ATR_HIDE2 ) continue;
		}
		// �֣ң��ͤˤ��ʤ��Ȥ��ϥϩ`�ɥǥ�����������`�ɤ���
		// �£ͣФ���`�ɤ���
		if( LoadBmp( bmpNo ) == FALSE ) continue;
		
		// ���`�ե����ꥹ�Ȥ���������`��
		for(lpSurfaceInfo = SpriteInfo[ bmpNo ].lpSurfaceInfo ;
			lpSurfaceInfo != NULL;
			lpSurfaceInfo = lpSurfaceInfo->pNext ){
				
			// ����åԥ󥰄I���ȥ��ե�����`�󥵩`�ե��`������Хå����`�ե����˸���ܞ��
			//���������������ɥåȥ�`�ɤΕr
			if(ResoMode == 1){
				//DrawSurfaceFast( 	pDispInfo->x/2 + lpSurfaceInfo->offsetX, 
				//					pDispInfo->y/2 + lpSurfaceInfo->offsetY, 
				//					lpSurfaceInfo->lpSurface );
				if( DrawSurfaceFast( 	pDispInfo->x/2 + lpSurfaceInfo->offsetX, 
									pDispInfo->y/2 + lpSurfaceInfo->offsetY, 
									lpSurfaceInfo->lpSurface ) == DDERR_SURFACEBUSY ) SurfaceBusyFlag = TRUE;
			} else {
				//DrawSurfaceFast( 	pDispInfo->x + lpSurfaceInfo->offsetX, 
				//					pDispInfo->y + lpSurfaceInfo->offsetY, 
				//					lpSurfaceInfo->lpSurface );
				if( DrawSurfaceFast( 	pDispInfo->x + lpSurfaceInfo->offsetX, 
									pDispInfo->y + lpSurfaceInfo->offsetY, 
									lpSurfaceInfo->lpSurface ) == DDERR_SURFACEBUSY ) SurfaceBusyFlag = TRUE;
					
			}
			
			// ���`�ե�����ʹ�ä����ո���ӛ��
			lpSurfaceInfo->date = SurfaceDate;
		}
	}
	
	// �ե���ȱ�ʾ����ֹ
	// �ե���Ȥα�ʾ( BACK )
	if( putTextFlag == 0 ){
		// �ե���Ȥ�Хå����`�ե����˥��å�
		PutText( FONT_PRIO_BACK );	
		// �ե饰�ϣ�
		putTextFlag = 1;	
	}
	// �ե���Ȥα�ʾ( FRONT )
	if( putTextFlag == 1 ){
		// �ե���Ȥ�Хå����`�ե����˥��å�
		PutText( FONT_PRIO_FRONT );	
	}
	// �ޥåפΥ��ե�����
	if( drawMapEffectFlag == 0 )
	{
		drawMapEffect();
	}
}

//**************************************************************************/
// 	��Ҫ��	�R�r��ʾ�ǩ`����Хåե�������
// 	������	UCHAR dispPrio����ʾ�Υץ饤����ƥ�
//		  	int x, int y����ʾ����
//			int bmpNo���£ͣз���
//			int chr_no������饯���`����
//			int pat_no���ѥ��`�󷬺�

/// /��Ҫ����ʱ��ʾ�����������
///������UCHAR dispPrio����ʾ�����ȼ�/ / 
///int x��y��ʾ��int����/ / 
///int bmpNo��BMP����/ / 
///int chr _ no����ɫ����/ / int pat _ no��ģʽ����
//**************************************************************************/
int StockDispBuffer( int x, int y, UCHAR dispPrio, int bmpNo, BOOL hitFlag )
{	
	short dx,dy;
	int BmpNo;
	// ��ر��椹������ޤǥ��ɥ쥹���M�ޤ���  ��α����չ����ַ
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	
	// ������ȥ��`�Щ`�Εr  ����ʱ������ʱ��
	if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) return -2;
	
	// �£ͣФ����åȤ���Ƥʤ��Ȥ�  ���õ�ʱ��
	if( -1 <= bmpNo && bmpNo <= CG_INVISIBLE ) return -2;
	
	// ͨ����ʾ�ΤȤ�
	if( bmpNo > CG_INVISIBLE )
	{
		realGetNo( bmpNo , (U4 *)&BmpNo );
		realGetPos( BmpNo  , &dx, &dy);
	}
	else
	{
		// �ܥå�����ʾ�ΤȤ��ω�Q���ʤ�  ���ʾ��ʱ��ת��
		dx = 0;
		dy = 0;
		BmpNo = bmpNo;
	}

	// ��ʾ���ȥ��`���Ø�����
	pDispSort->dispPrio = dispPrio;
	pDispSort->no = DispBuffer.DispCnt;
	// ��ʾ������壨 ���`�Ȥ��ʤ����� ��
	pDispInfo->x = x + dx;
	pDispInfo->y = y + dy;
	pDispInfo->bmpNo = BmpNo;
	pDispInfo->hitFlag = hitFlag;
	pDispInfo->pAct = NULL;

	// ��ʾ�����󥿩`�ץ饹
	return DispBuffer.DispCnt++;
}

//**************************************************************************/
// 	��Ҫ��	�R�r��ʾ�ǩ`����Хåե�������
// 	������	UCHAR dispPrio����ʾ�Υץ饤����ƥ�
//		  	int x, int y����ʾ����
//			int bmpNo���£ͣз���(realGetNo()�򤷤ʤ�)
//			int chr_no������饯���`����
//			int pat_no���ѥ��`�󷬺�
//**************************************************************************/
int StockDispBuffer2( int x, int y, UCHAR dispPrio, int bmpNo, BOOL hitFlag )
{	
	short dx,dy;
	int BmpNo;
	// ��ر��椹������ޤǥ��ɥ쥹���M�ޤ���
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	
	// ������ȥ��`�Щ`�Εr
	if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) return -2;
	
	// �£ͣФ����åȤ���Ƥʤ��Ȥ�
	if( -1 <= bmpNo && bmpNo <= CG_INVISIBLE ) return -2;
	
	// ͨ����ʾ�ΤȤ�
	if( bmpNo > CG_INVISIBLE ){
		BmpNo = bmpNo;
		realGetPos( BmpNo  , &dx, &dy);
	}else{
		// �ܥå�����ʾ�ΤȤ��ω�Q���ʤ�
		dx = 0;
		dy = 0;
		BmpNo = bmpNo;
	}

	// ��ʾ���ȥ��`���Ø�����
	pDispSort->dispPrio = dispPrio;
	pDispSort->no = DispBuffer.DispCnt;
	// ��ʾ������壨 ���`�Ȥ��ʤ����� ��
	pDispInfo->x = x + dx;
	pDispInfo->y = y + dy;
	pDispInfo->bmpNo = BmpNo;
	pDispInfo->hitFlag = hitFlag;
	pDispInfo->pAct = NULL;

	// ��ʾ�����󥿩`�ץ饹
	return DispBuffer.DispCnt++;
}

// ��������ʾ�ǩ`����Хåե������� ///////////////////////////////////////////////////
void StockTaskDispBuffer( void )
{
	ACTION *pActLoop; 	/* ���^�Υꥹ�ȥݥ���ȡ�� */
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	
	/* ����β������ޤǥ�`��  �����֮ǰѭ��*/	
	//while( pActLoop != pActBtm ){
	for(pActLoop = pActTop->pNext ; 
		pActLoop != pActBtm ;
		/* �ΤΥݥ��󥿤򥻥å� */
		pActLoop = pActLoop->pNext )
		{
		
		// ������ȥ��`�Щ`�Εr
		if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) break;
		
		// �£ͣФ����åȤ���Ƥʤ��Ȥ� ���õ�ʱ��
		if( -1 <= pActLoop ->bmpNo && pActLoop ->bmpNo <= CG_INVISIBLE ) continue;
		
		// �����ե饰�����äƤ���Ȥ�
		if( pActLoop ->deathFlag == TRUE ) continue;
		
		/* �Ǳ�ʾ�ʤ�i���� */
		if( pActLoop->atr & ACT_ATR_HIDE ) continue;

		// ��ʾ���ȥ��`���Ø�����
		pDispSort->dispPrio = pActLoop->dispPrio;
		pDispSort->no = DispBuffer.DispCnt;
		// ��ʾ������壨 ���`�Ȥ��ʤ����� ��
		pDispInfo->x = pActLoop->x + pActLoop->anim_x;
		pDispInfo->y = pActLoop->y + pActLoop->anim_y;
		pDispInfo->bmpNo = pActLoop->bmpNo;
		pDispInfo->pAct = pActLoop;
		
		// �������ж����뤫��
		// �}���x�k�Εr
		if( pActLoop->atr & ACT_ATR_HIT_BOX_ALL3 ) pDispInfo->hitFlag = 5;
		else
		if( pActLoop->atr & ACT_ATR_HIT_BOX_ALL2 ) pDispInfo->hitFlag = 4;
		else 
		if( pActLoop->atr & ACT_ATR_HIT_BOX_ALL1 ) pDispInfo->hitFlag = 3;
		else // �ܥå�����ʾ
		if( pActLoop->atr & ACT_ATR_HIT_BOX ) pDispInfo->hitFlag = 2;
		else // �ж��Τ�
		if( pActLoop->atr & ACT_ATR_HIT ) pDispInfo->hitFlag = 1;
		else pDispInfo->hitFlag = FALSE;
		// �ޥ������`���뵱�����ж�����ӛ��
		pActLoop->hitDispNo = DispBuffer.DispCnt;
		
		// ��ʾ�����󥿩`�ץ饹
		DispBuffer.DispCnt++;
		// ���ɥ쥹�ץ饹
		pDispSort++;
		pDispInfo++;
	}
	
}

// ��������ʾ�ǩ`����Хåե������� ///////////////////////////////////////////////////
// ������prio1����prio2�ޤǤΤ�ΤτI�����ʤ�
void StockTaskDispBuffer2( int prio1, int prio2 )
{
	ACTION *pActLoop; 	/* ���^�Υꥹ�ȥݥ���ȡ�� */
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	int tmp;
	
	if( prio1 > prio2 )
	{
		tmp = prio1;
		prio1 = prio2;
		prio2 = tmp;
	}
	
	/* ����β������ޤǥ�`�� */	
	//while( pActLoop != pActBtm ){
	for(pActLoop = pActTop->pNext ; 
		pActLoop != pActBtm ;
		/* �ΤΥݥ��󥿤򥻥å� */
		pActLoop = pActLoop->pNext ){
		
		// ������ȥ��`�Щ`�Εr
		if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) break;
		
		// prio1����prio2��ָ������τI�����ʤ�
		if( prio1 <= pActLoop->dispPrio && pActLoop->dispPrio <= prio2 )
			continue;
		
		// �£ͣФ����åȤ���Ƥʤ��Ȥ�
		if( -1 <= pActLoop ->bmpNo && pActLoop ->bmpNo <= CG_INVISIBLE ) continue;
		
		// �����ե饰�����äƤ���Ȥ�
		if( pActLoop ->deathFlag == TRUE ) continue;
		
		/* �Ǳ�ʾ�ʤ�i���� */
		if( pActLoop->atr & ACT_ATR_HIDE ) continue;

		// ��ʾ���ȥ��`���Ø�����
		pDispSort->dispPrio = pActLoop->dispPrio;
		pDispSort->no = DispBuffer.DispCnt;
		// ��ʾ������壨 ���`�Ȥ��ʤ����� ��
		pDispInfo->x = pActLoop->x + pActLoop->anim_x;
		pDispInfo->y = pActLoop->y + pActLoop->anim_y;
		pDispInfo->bmpNo = pActLoop->bmpNo;
		pDispInfo->pAct = pActLoop;
		
		// �������ж����뤫��
		if( pActLoop->atr & ACT_ATR_HIT_BOX ) pDispInfo->hitFlag = 2;
		else 
		if( pActLoop->atr & ACT_ATR_HIT ) pDispInfo->hitFlag = 1;
		else pDispInfo->hitFlag = FALSE;
		// �ޥ������`���뵱�����ж�����ӛ��
		pActLoop->hitDispNo = DispBuffer.DispCnt;
		
		// ��ʾ�����󥿩`�ץ饹
		DispBuffer.DispCnt++;
		// ���ɥ쥹�ץ饹
		pDispSort++;
		pDispInfo++;
	}
	
}

// �ܥå�����ʾ�ǩ`����Хåե������� ***************************************/
void StockBoxDispBuffer( int x1, int y1, int x2, int y2, UCHAR dispPrio, int color, BOOL fill )
{
	int col;	// ɫ��ָ��
	// int �� short �򣲂������
	int x = ( x1 << 16 ) | x2; 	// ��λ���Х��Ȥ���λ���Х��Ȥˤ����
	int y = ( y1 << 16 ) | y2;	// ��λ���Х��Ȥ���λ���Х��Ȥˤ����
	
	// �ܥå����ΤȤ�
	if( fill == 0 ) col = color | STOCK_DISP_BUFFER_BOX | STOCK_DISP_BUFFER_NO_BMP ;
	// �T��Ĥ֤��ΤȤ�
	else if( fill == 1 ) col = color | STOCK_DISP_BUFFER_BOX_FILL | STOCK_DISP_BUFFER_NO_BMP ;
	// �饤��Εr
	else if( fill == 2 ) col = color | STOCK_DISP_BUFFER_LINE | STOCK_DISP_BUFFER_NO_BMP ;
	
	// �ܥå�����Хåե�������
	StockDispBuffer( x, y, dispPrio, col, 0 );
}

// ��ʾ�Хåե��`����ܥå����ǩ`����ȡ����� *********************************/
void GetBoxDispBuffer( DISP_INFO *pDispInfo, int bmpNo )
{
	// �I��Υ��å�
	RECT rect;
	int color; // ɫ
	BOOL fill = FALSE; // �T��Ĥ֤��ե饰
	int DispInfoX = pDispInfo->x;
	int DispInfoY = pDispInfo->y;
	
	// ���ˤ�ȡ�����
	rect.right =	( LONG )( pDispInfo->x & 0xffff );
	rect.bottom =	( LONG )( pDispInfo->y & 0xffff );
	
	//rect.left = 	( LONG )( ( pDispInfo->x >> 16 ) & 0xffff );
	//rect.top = 		( LONG )( ( pDispInfo->y >> 16 ) & 0xffff );
	
	// ����饤�󥢥���֥�
	_asm{
		// rect.left ��ȡ�����
		mov		cl,16				// ���ե��������򥻥å�
		mov		eax, [ DispInfoX ]
		sar		eax, cl				// ���Ÿ��ҥ��ե�
		mov		[ rect.left ], eax
		// rect.top ��ȡ�����
		mov		eax, [ DispInfoY ]
		sar		eax, cl
		mov		[ rect.top ], eax
	}

	
	// �ܥå����ΤȤ�
	if( bmpNo & STOCK_DISP_BUFFER_BOX ){ 
		color = bmpNo & 0xff; 			// ɫ�ǩ`����ȡ�����
		DrawBox( &rect, color, 0 );	// �ܥå����軭
		
	// �T��Ĥ֤��ΤȤ�
	}else if( bmpNo & STOCK_DISP_BUFFER_BOX_FILL ){ 
		color = color = bmpNo & 0xff;	// ɫָ��
		DrawBox( &rect, color, 1 );	// �ܥå����軭
		
	}else if( bmpNo & STOCK_DISP_BUFFER_LINE ){ 
		color = color = bmpNo & 0xff;	// ɫָ��
		DrawBox( &rect, color, 2 );	// �饤���軭
	}
}			

// ���`�ȱ��^�v�� //////////////////////////////////////////////////////////////
int SortComp( DISP_SORT *pDisp1, DISP_SORT *pDisp2 )
{
	// pDisp1 �α�ʾ���ȶȤη����󤭤��r������椨
	if( pDisp1->dispPrio > pDisp2->dispPrio ){
		return 1;
	}
	
	// pDisp2 �α�ʾ���ȶȤη����󤭤��r�����Τޤ�
	if( pDisp1->dispPrio < pDisp2->dispPrio ){
		return -1;
	}
	// �Ȥ����r�ϵ��h�����
	// pDisp1 �η����礫�ä��r�����Τޤޣ��ΤϤ�����
	if( pDisp1->no > pDisp2->no ){
		return -1;
	}
	// �ɤ�Ǥ�ʤ��r������椨���ΤϤ�����
	return 1;
}
