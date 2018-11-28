/************************/
/*	sprmgr.h			*/
/************************/
#ifndef _SPRMGR_H_
#define _SPRMGR_H_

//#define BMP_FILE			120000	// �£ͣХե��������
#define MAX_GRAPHICS	 	130000	// ����������
#define	DEF_COLORKEY		0	// ͸��ɫ�Ȥ���ɫ���� ͸��ɫ��ɫ��
//#define	DEF_COLORKEY		240	// ͸��ɫ�Ȥ���ɫ����

#define SURFACE_WIDTH   64 	// �ָ�`�ե������η� �ָ�ӿڵĿ���
#define SURFACE_HEIGHT  48  // �ָ�`�ե������θߤ�
//#define SURFACE_WIDTH   32 	// �ָ�`�ե������η�
//#define SURFACE_HEIGHT  24  // �ָ�`�ե������θߤ�

// ���`�ե�������ե������� �س���Ϣ�ṹ
struct surfaceInfo{

	LPDIRECTDRAWSURFACE lpSurface; 	// ָ��

	int	bmpNo;						// λͼ����
	//���ֵ- 1��ʱ����������ǿա�
	
	// ���΂��� -1 �Εr�Ϥ��Υ��`�ե����ϿդǤ���
	//ʹ��������ӿ����ڼ���
	UINT date;						// ���Υ��`�ե�����ʹ�ä����ո���ӛ��
	//��ʾλ������X����
	short offsetX;					// ��ʾλ�ä���Υ��ե��åȣ�����
	short offsetY;					// ��ʾλ�ä���Υ��ե��åȣ�����

	surfaceInfo	*pNext;				// �ΤΥ��`�ե�������ե�������ؤΥݥ���
	
};

typedef struct surfaceInfo SURFACE_INFO;

// ���ץ饤�ȥ���ե�������  ���ֵ���Ϣ�ṹ��
typedef struct{

//	LPBITMAPINFO lpBmp; 			//λͼ��Ϣ�ṹ��ָ��
	
	SURFACE_INFO *lpSurfaceInfo; 	// ʹ�ä��Ƥ��륵�`�ե����ꥹ�Ȥ����^���ɥ쥹
	/// /ʹ�õı���������ǰͷ��ַ/ /���ֵNULL��ʱ�����BMP��VRAMû��
	// ���΂��� NULL �Εr�Ϥ��Σ£ͣФ� VRAM �ˤϤ��ʤ�
	short width,height;				//BMP�ĳߴ�
	
}SPRITE_INFO;



// ȫ�ֱ��� ////////////////////////////////////////////////////////////

// ���ץ饤�ȥ���ե�  ����Ľӿ�
extern SPRITE_INFO SpriteInfo[];

// ���`�ե�������ե�
extern SURFACE_INFO SurfaceInfo[];

// �Хȥ륵�`�ե����Υݥ���
//ս��ͼƬָ�룿
extern LPDIRECTDRAWSURFACE lpBattleSurface;

// ���`�ե�������  �ӿ�����
extern int SurfaceCnt;

// VRAM �����ɤ������`�ե��������������
//ȫ���ӿ� ������
extern int VramSurfaceCnt;

// SYSTEMRAM �����ɤ������`�ե��������������
extern int SysramSurfaceCnt;

// ���`�ե�����ʹ�ä����ո�
//�ӿ�ʹ������
extern UINT SurfaceDate;

// �F�ڤΥ��`�ե�������λ��
//�ӿ�λ�ü��
extern int SurfaceSearchPoint;

#ifdef _DEBUG		
// �F��ʹ�äƤ��륵�`�ե��������������
extern int SurfaceUseCnt;

// �F�ڱ�ʾ���Ƥ��륵�`�ե��������������
extern int SurfaceDispCnt;
#endif
// �ָ�`�ե������η�
extern int SurfaceSizeX;
// �ָ�`�ե������θߤ�
extern int SurfaceSizeY;

// �v���������� //////////////////////////////////////////////////////////////

/* �ӥåȥޥåפ��i���z�� λͼ����  *****************************************************/
void OpenBmp( void );      

/* ���ե�����`�󥵩`�ե��������� *********************************************/
BOOL InitOffScreenSurface( void );

/* ���`�ե�������ե�������γ��ڻ���******************************************/
void InitSurfaceInfo( void );

/* ���ץ饤�ȥ���ե�������γ��ڻ���**************************************/
void InitSpriteInfo( void );

/* �£ͣФ򥵩`�ե����ظ�굱�� ***********************************************/
void AllocateBmpToSurface( int bmpNo );

// �£ͣФ���`�ɤ��� *********************************************************/
BOOL LoadBmp( int bmpNo );
//���ת����
// �£ͣФ���`�ɤ��루����ե��å����ŉ�Q�⤹�룩 ***************************/
BOOL LoadBmp2( int bmpNo );

#endif
