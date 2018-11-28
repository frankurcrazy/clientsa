/************************/
/*	sprdisp.h			*/
/************************/
#ifndef _SPRDISP_H_
#define _SPRDISP_H_

#define DISP_BUFFER_SIZE 4096 	// ��ʾ�Хåե�������

/* ��ʾ�����λ  dispPrio �΂� *************************************************/
enum{
	DISP_PRIO_BG 		= 0,	/* �£� 	*/
	DISP_PRIO_TILE 		= 0,	// �����루���`�໭��Υե��`��ɲ���
	DISP_PRIO_CHAR 		= 10,	// �����
	DISP_PRIO_PARTS 	= 10,	// �ѩ`�ģ�����ȣ�
	DISP_PRIO_RESERVE 	= 20,
	DISP_PRIO_JIKI 		= 30,	/* �ԙC 	*/
	DISP_PRIO_GRID 		= 100,	// ����åɥ��`����
	DISP_PRIO_BOX,				/* �ܥå��� */
	DISP_PRIO_IME1,				/* �ɣͣťܥå������Хå��á��\ɫ�� */
	DISP_PRIO_IME2,				/* �ɣͣťܥå������Хå��á���ɫ�� */
								/* �ե���ȥХå� */
	DISP_PRIO_MENU,				/* ��˥�` */
	DISP_PRIO_IME3,				/* �ɣͣťܥå������ե�����á��\ɫ�� */
	DISP_PRIO_IME4,				/* �ɣͣťܥå������ե�����á���ɫ�� */
	DISP_PRIO_BOX2,				/* �ܥå����� */
	DISP_PRIO_ITEM,				/* �����ƥ� */
								/* �ե���ȥե���� */
	DISP_PRIO_YES_NO_WND,		/* �_�J������ɥ� */
	DISP_PRIO_YES_NO_BTN,		/* �_�J�ܥ��� */
	DISP_PRIO_BOX3,				/* �ܥå����� */
	DISP_PRIO_DRAG,				/* �ɥ�å��� */
	DISP_PRIO_MOUSE,			/* �ޥ������`���� 	*/
	DISP_PRIO_TOP = 255			/* ��� 	*/
};


// ��ʾ�������
typedef struct {
	int x, y;			// ��ʾ����
	int bmpNo;			// �£ͣз���( �ܥå�����ʾ�Εr��ɫ���� )
	ACTION *pAct;		// ���������ݥ��� ����ָ��
	//ÿ���ж�  0 �����ߣ�  1 ���� 2��ʾ���ӣ�
	BOOL hitFlag;		// �������ж����뤫�ե饰���������ʤ����������롢�����£ϣر�ʾ
}DISP_INFO;

// ��ʾ���ȥ��`���Ø�����  ��������/ /��ʾ�ýṹ��
typedef struct {
	short no;			// �Хåե���{�r�η��� ����洢ʱ�ĺ���
	UCHAR	dispPrio; 	// ��ʾ�΃����λ
}DISP_SORT;

// ��ʾ�Хåե�������
typedef struct{
	DISP_INFO 	DispInfo[ DISP_BUFFER_SIZE ];
	DISP_SORT 	DispSort[ DISP_BUFFER_SIZE ];
	short 		DispCnt;	// �F�ڤθ�{��  ������ʾ��
}DISP_BUFFER;

// ��ʾ�Хåե�������
extern DISP_BUFFER 	DispBuffer;

// �£ͣ��i���z���å�`���I��ؤΥݥ���
extern LPBITMAPINFO lpBmpInfo;
// �£ͣФΥ���`���ǩ`���ޤǤΥ��ե��å�
extern int BmpOffBits;

// Realbin �i���z���å�`���I��ؤΥݥ���
extern char *pRealBinBits;
// ��� Realbin �����i���z��£ͣФΥ�����
extern int RealBinWidth, RealBinHeight;

// BitBlt�Εr
extern HBITMAP	hBmp;


// ���`�ե����������`�ե饰
extern int SurfaceBusyFlag;

// ��ʾ�Хåե����`�� ///////////////////////////////////////////////////////////
void SortDispBuffer( void );

/* ����饯���`���軭 **************************************************/
void PutBmp( void );

// ��ʾ�ǩ`����Хåե������� ///////////////////////////////////////////////////
int StockDispBuffer( int x, int y, UCHAR prio, int bmpNo, BOOL hitFlag );

// ��ʾ�ǩ`����Хåե������� ///////////////////////////////////////////////////
// �������Ф�realGetNo()����Фʤ�
int StockDispBuffer2( int x, int y, UCHAR prio, int bmpNo, BOOL hitFlag );

// ��������ʾ�ǩ`����Хåե������� ///////////////////////////////////////////////////
void StockTaskDispBuffer( void );

// ��������ʾ�ǩ`����Хåե������� ///////////////////////////////////////////////////
// ������prio1����prio2�ޤǤΤ�ΤτI�����ʤ�
void StockTaskDispBuffer2( int prio1, int prio2 );

// �ܥå�����ʾ�ǩ`����Хåե������� ***************************************/
void StockBoxDispBuffer( int x1, int y1, int x2, int y2, UCHAR dispPrio, int color, BOOL fill );

#endif
