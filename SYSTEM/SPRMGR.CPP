/************************/
/*	sprmgr.c			*/
/************************/
#include "system.h"
#include "loadrealbin.h"
#include "loadsprbin.h"

//#define SURACE_INFO_MAX 512 	// ���`�ե�������ե�������������
//#define SURACE_INFO_MAX 1024 	// �ӿ���Ϣ�ṹ����������
#define SURACE_INFO_MAX 2048 	// ���`�ե�������ե�������������
//#define SURACE_INFO_MAX 4096 	// ���`�ե�������ե�������������

//�����SYSRAM�س̵ķֽ���
#define VRAM_SYSRAM_POINT 0 	// VRAM ���`�ե����� SYSRAM ���`�ե����ξ�Ŀ
//�����ϵ�BMPĨɱ������Ϊֹ������
#define SURACE_BMP_DEATH_DATE 2 // ���`�ե����ϤΣ£ͣФ�Ĩ�����S�ɤ����ޤǤ�����

// ���ץ饤�ȥ���ե�
SPRITE_INFO SpriteInfo[ MAX_GRAPHICS ];

// ���`�ե�������ե�
SURFACE_INFO SurfaceInfo[ SURACE_INFO_MAX ];

// �Хȥ륵�`�ե����Υݥ���
LPDIRECTDRAWSURFACE lpBattleSurface; 	

// ���`�ե�����ʹ�ä����ո�   ����ʹ�õ�����
UINT SurfaceDate = 3;

// �F�ڤΥ��`�ե�������λ��
int SurfaceSearchPoint;

#ifdef _DEBUG		
// �F��ʹ�äƤ��륵�`�ե��������������
int SurfaceUseCnt;

// �F�ڱ�ʾ���Ƥ��륵�`�ե�������������� ������ʾ�Ļس̵���������
int SurfaceDispCnt;
#endif

// ���ɤ������`�ե��������������
int SurfaceCnt;

// VRAM �����ɤ������`�ե��������������
int VramSurfaceCnt;

// SYSTEMRAM �����ɤ������`�ե��������������
int SysramSurfaceCnt;

// �ָ�`�ե������η�
int SurfaceSizeX;
// �ָ�`�ե������θߤ�
int SurfaceSizeY;

/* �£ͣФ򥵩`�ե����ظ�굱��  �ѶԱ������ ***********************************************/
void AllocateBmpToSurface( int bmpNo )
{
	//int bmpWidth  = lpBmpInfo->bmiHeader.biWidth;	// �£ͣФκ᥵����
	//int bmpHeight = lpBmpInfo->bmiHeader.biHeight; 	// �£ͣФοk������

	int bmpWidth  = RealBinWidth;	// �£ͣФκ᥵����
	int bmpHeight = RealBinHeight; 	// �£ͣФοk������
	
	int offsetX, offsetY; 			// �ͤ�£ͣФΥ��ե��åȥݥ����  ��BMP�������
	int sizeX, sizeY;				// ܞ�ͤ��륵����
	int surfaceCntX;				// ��Ҫ�ʥ��`�ե����κ�ö��
	int surfaceCntY;				// ��Ҫ�ʥ��`�ե����κ�ö��
	int totalSurface;				// ��Ҫ�ʥ��`�ե����ξt��
	int totalSurfaceCnt = 0;		// �F�ڤδ_���������`�ե�����ö���� ��`�ץ����� ��
	//���μ����˽���Ϊ�������λ�ü���
	int	SurfaceSearchPointBak = SurfaceSearchPoint; // һ�ܗ���������K�ˤ��뤿�������λ�ä�ӛ��
	int amariSizeX = FALSE;		// �����꤬���뤫�ե饰  �Ա߶���ת�۵�
	int amariSizeY = FALSE;		// �k����꤬���뤫�ե饰
	BOOL vramFullFlag = FALSE; 		// VRAM�����äѤ����ɤ���
	SURFACE_INFO *prevSurfaceInfo; 	// ǰ�Υ��`�ե�������ե�������Υ��ɥ쥹
	
	// �ͤ�£ͣФΥ��ե��åȥݥ����   ��BMP�������
	offsetX = 0; 
	offsetY = bmpHeight;
	
	// ��Ҫ�ʥ��`�ե����κ�ö��Ӌ��
	surfaceCntX = bmpWidth / SURFACE_WIDTH;
	
	// �����꤬���ä����˥�������ӛ��
	if( ( amariSizeX = bmpWidth % SURFACE_WIDTH ) ){ 
		surfaceCntX++;		// �⤦�ҤȤı�Ҫ
	}
	
	// ��Ҫ�ʥ��`�ե����οkö��Ӌ��
	surfaceCntY = bmpHeight / SURFACE_HEIGHT;
	
	// �k����꤬���ä����˥�������ӛ��
	if( ( amariSizeY = bmpHeight % SURFACE_HEIGHT ) ){ 
		surfaceCntY++;		// �⤦�ҤȤı�Ҫ
	}
	// ��Ҫ�ʥ��`�ե����ξt��Ӌ��
	totalSurface  = surfaceCntX * surfaceCntY;
	
#if 0
	// ����򣴤ΥХ������ˤ���
	if( ( lpBmpInfo->bmiHeader.biWidth & 3 ) ){
		lpBmpInfo->bmiHeader.biWidth += 4 - lpBmpInfo->bmiHeader.biWidth & 3;
	}
#else	// Realbin �i���z��Ȥ�
	// ����򣴤ΥХ������ˤ���
	if( ( RealBinWidth & 3 ) ){
		RealBinWidth += 4 - RealBinWidth & 3;
	}
#endif

	// �դ��Ƥ��륵�`�ե�����̽��
	// ���`�ե�������������`��
	/// /�ӿڵı���Ѱ��/ /���������ѭ��
	while( 1 ){
		// ʹ�äǤ���״�B�ʤ�  ��ʹ��״̬�Ļ�
		if( SurfaceInfo[ SurfaceSearchPoint ].date < SurfaceDate - SURACE_BMP_DEATH_DATE ){
#ifdef _DEBUG		
			// �F��ʹ�äƤ��륵�`�ե��������������
			SurfaceUseCnt++;
#endif
			// �ϕ�������r
			if( SurfaceInfo[ SurfaceSearchPoint ].bmpNo != -1 ){
				// ���`�ե�������ե�������Υ��ɥ쥹
				SURFACE_INFO *lpSurfaceInfo;
				
				// ���`�ե����ꥹ�Ȥ����^���ɥ쥹��ӛ��
				lpSurfaceInfo = SpriteInfo[ SurfaceInfo[ SurfaceSearchPoint ].bmpNo ].lpSurfaceInfo;
				
				// ǰ�ˤ����£ͣФȥ��`�ե����Υ�󥯤�����
				SpriteInfo[ SurfaceInfo[ SurfaceSearchPoint ].bmpNo ].lpSurfaceInfo = NULL;
				
				// ���`�ե����ꥹ�Ȥγ��ڻ���`��
				for( ; lpSurfaceInfo != NULL;
					lpSurfaceInfo = lpSurfaceInfo->pNext ){
					
					// �£ͣз��ų��ڻ�
					lpSurfaceInfo->bmpNo = -1;
					
#ifdef _DEBUG		
					// �F��ʹ�äƤ��륵�`�ե��������ޥ��ʥ�
					SurfaceUseCnt--;
#endif
				}
				
			}
			// ���`�ե����ꥹ�Ȥ����^���ɥ쥹��ӛ������Ȥ�
			if( SpriteInfo[ bmpNo ].lpSurfaceInfo == NULL ){
				// ���ץ饤�ȥ���ե������^���ɥ쥹��ӛ������
				SpriteInfo[ bmpNo ].lpSurfaceInfo = &SurfaceInfo[ SurfaceSearchPoint ];
				
			}else{ // �Ĥʤ���Ȥ�
			
				// ҙ���Ƥ�����ǰ�Υ��`�ե�������ե�������˥��ɥ쥹��̤���
				prevSurfaceInfo->pNext = &SurfaceInfo[ SurfaceSearchPoint ];
			}
			
			// �£ͣз��Ť�ӛ������
			SurfaceInfo[ SurfaceSearchPoint ].bmpNo = bmpNo;
			
			// ���ե��å����ˤ�ҙ����
			//���������������ɥåȥ�`�ɤΕr
			//��ӡ�����320��240����/ /ģʽ��ʱ��
			if(ResoMode == 1){
				SurfaceInfo[ SurfaceSearchPoint ].offsetX = offsetX / 2;
				SurfaceInfo[ SurfaceSearchPoint ].offsetY = ( bmpHeight - offsetY ) / 2;
			} else {
				//���������������ɥåȥ�`������Εr
				SurfaceInfo[ SurfaceSearchPoint ].offsetX = offsetX;
				SurfaceInfo[ SurfaceSearchPoint ].offsetY = bmpHeight - offsetY;
			}
			
			// �����꤬����Ȥ���ܞ�ͤ���ǰ�˥��`�ե������\�ǥ��ꥢ�`
			if( offsetX >= bmpWidth - SURFACE_WIDTH && amariSizeX ){
				ClearSurface( SurfaceInfo[ SurfaceSearchPoint ].lpSurface );
				// ��ꥵ����
				sizeX = amariSizeX;				
			}else sizeX = SURFACE_WIDTH;
			
			// �k����꤬����Ȥ���ܞ�ͤ���ǰ�˥��`�ե������\�ǥ��ꥢ�`
			if( offsetY - SURFACE_HEIGHT <= 0 && amariSizeY ){
				// �Ȥ��\�ǥ��ꥢ�`����Ƥʤ��Ȥ�
				if( sizeX != amariSizeX ){
					ClearSurface( SurfaceInfo[ SurfaceSearchPoint ].lpSurface );
				}
				// ��ꥵ����
				sizeY = amariSizeY;
			}else sizeY = SURFACE_HEIGHT;
			
			// �£ͣФ򥵩`�ե�����ܞ��
			DrawBitmapToSurface2( 	SurfaceInfo[ SurfaceSearchPoint ].lpSurface, 
									offsetX, 
									offsetY - 1, 
									sizeX,
									sizeY,
									lpBmpInfo );

			// �F�ڤδ_���������`�ե�����ö���򥫥����
			totalSurfaceCnt++;

			// ���٤ƴ_�����K��ä���
			if( totalSurfaceCnt >= totalSurface ){
				//��pNext �ˣΣգ̣̤����ƽK��
				SurfaceInfo[ SurfaceSearchPoint ].pNext = NULL;
				// ����λ�ä��M�ޤ���
				SurfaceSearchPoint++;
				
#if 0			// VRAM����ʹ�åЩ`�����

				// VRAM�Υ��`�ե������������Ȥ�
				if( vramFullFlag == FALSE ){
					// ��ߥåȥ����å�
					if( SurfaceSearchPoint >= VramSurfaceCnt ) SurfaceSearchPoint = 0;
				}else{
					// ����λ�ä����
					SurfaceSearchPoint = SurfaceSearchPointBak;
				}
				
#else			// VRAM��SYSRAM��ͬ��٥��ʹ�åЩ`�����

				// ��ߥåȥ����å�
				if( SurfaceSearchPoint >= SurfaceCnt ) SurfaceSearchPoint = 0;
#endif
				break;
				
			}else{
				// ��Υ��`�ե�������ե�������Υ��ɥ쥹��ҙ����
				prevSurfaceInfo = &SurfaceInfo[ SurfaceSearchPoint ];
				
				// �Ҷˤޤ��ͤä���
				if( offsetX >= bmpWidth - SURFACE_WIDTH ){ 
					offsetX = 0;
					offsetY -= SURFACE_HEIGHT;
				}else{ 
					offsetX += SURFACE_WIDTH;
				}
			}
		}
		// ����λ�ä��M�ޤ���
		SurfaceSearchPoint++;
		
#if 0	// VRAM����ʹ�åЩ`�����

		// VRAM�Υ��`�ե������������Ȥ�
		if( vramFullFlag == FALSE ){
			// VRAM���`�ե���������ޤǗ��������Ȥ�
			if( SurfaceSearchPoint >= VramSurfaceCnt ) SurfaceSearchPoint = 0;
			// һ�ܗ���������VRAM�˿դ��ʤ�״�B�Ǥ�������
			if( SurfaceSearchPoint == SurfaceSearchPointBak ){ 
				//MessageBox( hWnd, "�֣ң��ͤ˿դ�������ޤ���", "�_�J", MB_OK );
				// ����λ�ä�SYSTEM���`�ե��������ڤ���Ȥ������Ƅ�
				SurfaceSearchPoint = VramSurfaceCnt + 1;
				vramFullFlag = TRUE;
			}
		}
		// SYSTEMRAM�Υ��`�ե������������Ȥ�
		if( vramFullFlag == TRUE ){
			// ����ޤǗ���������
			if( SurfaceSearchPoint >= SurfaceCnt ){ 
				// ����λ�ä����
				SurfaceSearchPoint = SurfaceSearchPointBak;
				//MessageBox( hWnd, "���`�ե�������Ǥ���", "�_�J", MB_OK );
				break;
			}
		}
		
#else	// VRAM��SYSRAM��ͬ��٥��ʹ�åЩ`�����
		
		// ����ޤǗ���������
		if( SurfaceSearchPoint >= SurfaceCnt ){ 
			// ����ˑ���
			SurfaceSearchPoint = 0;
		}
		// һ�ܗ��������饵�`�ե����˿դ��ʤ�״�B�Ǥ�������
		if( SurfaceSearchPoint == SurfaceSearchPointBak ){ 
			//MessageBox( hWnd, "���`�ե�������Ǥ���", "�_�J", MB_OK );
			break;
		}
	}
#endif
}

/* ���ե�����`�󥵩`�ե��������� *********************************************/
BOOL InitOffScreenSurface( void )
{
	int i;
	BOOL vramFullFlag = FALSE; // VRAM�����äѤ����ɤ��� �Ƿ��кܶ�
	
	// ���`�ե������� ������ѵ�����ʾ����
	SurfaceCnt = 0;
	// VRAM �����ɤ������`�ե�������
	VramSurfaceCnt = 0;
	// SYSTEMRAM �����ɤ������`�ե�������
	SysramSurfaceCnt = 0;
	
	// �Хȥ륵�`�ե���������
	if( ( lpBattleSurface = CreateSurface( DEF_APPSIZEX, DEF_APPSIZEY, DEF_COLORKEY, DDSCAPS_VIDEOMEMORY )) == NULL ){
		// �Хȥ륵�`�ե���������
		if( ( lpBattleSurface = CreateSurface( DEF_APPSIZEX, DEF_APPSIZEY, DEF_COLORKEY, DDSCAPS_SYSTEMMEMORY )) == NULL ){
			MessageBox( hWnd ,"�Хȥ륵�`�ե��������ɤ�ʧ�����ޤ�����" ,"�_�J",MB_OK | MB_ICONSTOP );
			return FALSE;
		}
	}
	// SURACE_INFO_MAX �ޤǥ��`�ե���������   ���浽����*/
	for( i = 0 ; i < SURACE_INFO_MAX ; i++ ){
	
#ifdef _DEBUG		
#if VRAM_SYSRAM_POINT
		// ������� �����å��� )  ֻ�����ڣ��ã����
		if( i == VRAM_SYSRAM_POINT ){
			 vramFullFlag = TRUE;
		}
#endif
#endif

		// VRAM�˥��`�ե��������ɤǤ���Ȥ� �����������ɵ�ʱ��
		if( vramFullFlag == FALSE ){
			// DirectDraw�ä�BITMAP���{���뤿��Υ��`�ե��������� ��BITMAP���洢�ı�������
			if( ( SurfaceInfo[ i ].lpSurface = CreateSurface( SurfaceSizeX, SurfaceSizeY, DEF_COLORKEY, DDSCAPS_VIDEOMEMORY )) == NULL ){
				//MessageBox( hWnd ,"�֣ң��ͥ��`�ե��������ɤ�ʧ�����ޤ�����" ,"�_�J",MB_OK | MB_ICONSTOP );
				vramFullFlag = TRUE;
			}else{
				// VRAM �����ɤ������`�ե��������������
				VramSurfaceCnt++;
			}
		}
			
		// SYSTEMRAM�˥��`�ե��������ɤ���Ȥ�  ������������ʱ��
		if( vramFullFlag == TRUE ){
			if( ( SurfaceInfo[ i ].lpSurface = CreateSurface( SurfaceSizeX, SurfaceSizeY, DEF_COLORKEY, DDSCAPS_SYSTEMMEMORY )) == NULL ){
				MessageBox( hWnd ,"�ӣ٣ӣң��ͥ��`�ե��������ɤ�ʧ�����ޤ�����" ,"�_�J",MB_OK | MB_ICONSTOP );
				return FALSE;
			}else{
				// SYSTEMRAM �����ɤ������`�ե��������������
				SysramSurfaceCnt++;
			}
		}
	}
	
	// ���ɤ������`�ե���������ӛ��
	SurfaceCnt = i;
	
	/* ���`�ե�������ե�������γ��ڻ�  �س���Ϣ�ṹ�ĳ��ڻ�*/
	InitSurfaceInfo();
	/* ���ץ饤�ȥ���ե�������γ��ڻ� */
	InitSpriteInfo();
		
	
	return TRUE;
}

/* ���`�ե�������ե�������γ��ڻ���**************************************/
void InitSurfaceInfo( void )
{
	int i;
	
#ifdef _DEBUG		
	// �F��ʹ�äƤ��륵�`�ե����������ڻ�
	SurfaceUseCnt = 0;
#endif
	
	// ���`�ե�������λ�ó��ڻ�
	SurfaceSearchPoint = 0;
	
	// ���`�ե�����������`�� */
	for( i = 0 ; i < SurfaceCnt ; i++ ){
	
		// ���`�ե�������ե�������γ��ڻ�
		SurfaceInfo[ i ].bmpNo = -1;
		SurfaceInfo[ i ].date = 0;
		SurfaceInfo[ i ].pNext = NULL;
	}
}	

/* �ӿ� �������ʼ�� ������γ��ڻ���**************************************/
void InitSpriteInfo( void )
{
	int i;
	
	// �£ͣФ���������`�� */
	for( i = 0 ; i < MAX_GRAPHICS ; i++ ){
		// VRAM �ˤ��ʤ����Ȥˤ���
		SpriteInfo[ i ].lpSurfaceInfo = NULL;
	}
}	

// �£ͣФ���`�ɤ��� ***��Ӳ�̼���******************************************************/
BOOL LoadBmp( int bmpNo )
{
	// �֣ң��ͤˤ��ʤ��Ȥ��ϥϩ`�ɥǥ�����������`�ɤ���  û��Ӳ�̴���Ƶ����
	if( SpriteInfo[ bmpNo ].lpSurfaceInfo == NULL )
	{
		// �£ͣз��Ť��饤��`���ǩ`���򷵤�( Realbin �����i���z�� )
		if( realGetImage( 	bmpNo, 
							( unsigned char **)&pRealBinBits, 
							&RealBinWidth, 
							&RealBinHeight ) == FALSE ){
							
			//MessageBox( hWnd, "����`���ǩ`�����i���z�ߤ�ʧ�����ޤ�����", "�_�J", MB_OK );
			return FALSE;
		}
		// �£ͣФΥ�������ӛ��
		SpriteInfo[ bmpNo ].width = RealBinWidth;
		SpriteInfo[ bmpNo ].height = RealBinHeight;
		// �£ͣФ򥵩`�ե����ظ�굱�� 
		AllocateBmpToSurface( bmpNo );
	}
	return TRUE;
}

// �£ͣФ���`�ɤ��루����ե��å����ŉ�Q�⤹�룩 **����ƽ�����任�� ��Ӳ��*************************/
BOOL LoadBmp2( int bmpNo )
{
	// RIAL.BIN�����Ťˤ���
	realGetNo( bmpNo , (U4 *)&bmpNo );
	// �֣ң��ͤˤ��ʤ��Ȥ��ϥϩ`�ɥǥ�����������`�ɤ���
	if( SpriteInfo[ bmpNo ].lpSurfaceInfo == NULL ){
		// �£ͣз��Ť��饤��`���ǩ`���򷵤�( Realbin �����i���z�� )
		//�ŵ�ͼ�����ݷ��أ�Realbin��ȡ����
		if( realGetImage( 	bmpNo, 
							( unsigned char **)&pRealBinBits, 
							&RealBinWidth, 
							&RealBinHeight ) == FALSE ){
							
			//MessageBox( hWnd, "����`���ǩ`�����i���z�ߤ�ʧ�����ޤ�����", "�_�J", MB_OK );
			return FALSE;
		}
		// �£ͣФΥ�������ӛ��
		SpriteInfo[ bmpNo ].width = RealBinWidth;
		SpriteInfo[ bmpNo ].height = RealBinHeight;
		// �£ͣФ򥵩`�ե����ظ�굱�� 
		AllocateBmpToSurface( bmpNo );
	}
	return TRUE;
}


/* �ӥåȥޥåפ��i���z�� *****************************************************/
void OpenBmp( void )
{
	int i;
   	char *fileName[]=
	/* 0 */	  { "data\\spr\\��.bmp","data\\spr\\����.bmp","data\\spr\\������.bmp","data\\spr\\����.bmp","data\\spr\\������.bmp",
	/* 5 */		"data\\spr\\������.bmp","data\\spr\\����.bmp","data\\spr\\����.bmp","data\\spr\\������.bmp","data\\spr\\����.bmp",
	/*10 */		"data\\spr\\������.bmp","data\\spr\\����.bmp","data\\spr\\��.bmp","data\\spr\\��.bmp","data\\spr\\��.bmp",
	/*15 */		"data\\spr\\�ͣ�.bmp","data\\spr\\�ף�.bmp","data\\spr\\�ӣ�.bmp","data\\spr\\�ͣ�.bmp","data\\spr\\�ף�.bmp",
	/*20 */		"data\\spr\\�ӣ�.bmp","data\\spr\\���k��.bmp","data\\spr\\���k��.bmp","data\\spr\\���k��.bmp", "data\\spr\\�ԏ�.bmp",
	/*25 */		"data\\spr\\�ԏ���.bmp","data\\spr\\����.bmp","data\\spr\\������.bmp","data\\spr\\��ɫ��.bmp","data\\spr\\��ɫ��.bmp",
	/*30 */		"data\\spr\\�L�Έ�.bmp","data\\spr\\pl01.bmp","data\\spr\\�ե����.bmp","data\\spr\\st\\ken.bmp" };

	//BITMAP�ե������չ�_���ӥ���`�_�� �ļ���չ����ȷ�����洢��
	//lpBmp[ 0 ] = LoadDirectDrawBitmap( fileName[ 0 ] ); 
	//if( lpBmp[ 0 ] == NULL ) MessageBox( hWnd, "�£ͣФΥ��`�ץ��ʧ�����ޤ�����", "�_�J", MB_OK );
	
	//if( !WindowMode ){
		// �����ƥ�ѥ�åȳ��ڻ�
	//	InitPalette( lpBmp[ 0 ] );
	//}
	
	// �����������`�ե��������� */
	for( i = 0 ; i < SURACE_INFO_MAX ; i++ ){
	
		// ���`�ե�������ե�������γ��ڻ�
		SurfaceInfo[ i ].bmpNo = -1;
		SurfaceInfo[ i ].pNext = NULL;
		
		// DirectDraw�ä�BITMAP���{���뤿��Υ��`�ե���������
		if( ( SurfaceInfo[ i ].lpSurface = CreateSurface( SURFACE_WIDTH, SURFACE_HEIGHT, DEF_COLORKEY, 0 )) == NULL ){
			MessageBox( hWnd ,"���`�ե����γ��ڻ���ʧ�����ޤ�����" ,"�_�J",MB_OK | MB_ICONSTOP );
			break;
			//DestroyWindow( hWnd );
		}
		
		// ���`�ե�������
		SurfaceCnt++;
		
		// BITMAP�򥵩`�ե���(����[0,0])���}д
		//DrawBitmapToSurface( lpSurface[ i ], 0, 0 ,lpBmp[ 0 ] );
		//SetRect( &Rect, 0, 0, lpBmp[ 0 ]->bmiHeader.biWidth, lpBmp[ 0 ]->bmiHeader.biHeight ); 
	}
}

