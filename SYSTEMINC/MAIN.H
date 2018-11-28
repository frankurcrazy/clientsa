// main.cpp �إå��ե�����

#ifndef _MAIN_H_
#define _MAIN_H_

//---------------------------------------------------------------------------//
// ��Ҫ ���ޥ�����define)����                                                //
//---------------------------------------------------------------------------//
//������������
#define DEF_APPNAME		"ħ��" // "�ӣ���������" //�����ȥ�Щ`����ǰ   ����û�ô���ģʽ�� 
#define DEF_APPSIZEX	640								//���ɤ���WINDOWS�κ��640	
#define DEF_APPSIZEY	480							//���ɤ���WINDOWS�οk��480
//#define DEF_APPSIZEX	320								//���ɤ���WINDOWS�κ��
//#define DEF_APPSIZEY	240								//���ɤ���WINDOWS�οk��

#define SCREEN_WIDTH_CENTER			DEF_APPSIZEX/2
#define SCREEN_HEIGHT_CENTER		DEF_APPSIZEY/2

//---------------------------------------------------------------------------//
// ��Ҫ ���v���ץ��ȥ���������                                               //
//---------------------------------------------------------------------------//

int PASCAL WinMain( HINSTANCE 		hInstance, 	HINSTANCE 		hPrevInstance, 	LPSTR lpCmdLine, 	int nCmdShow );	// ���ڻ��v��

LRESULT CALLBACK PASCAL WindMsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );				//�ᥤ��ץ����`����`
BOOL SystemTask( void );																		// ��å��`����`���v��

//#define P( a ) 	printf( #a "\n")
//#define Pd( a ) printf( #a " = %d\n",a )
//#define Pf( a ) printf( #a " = %f\n",a )
//#define Pp( a ) printf( #a " = %p\n",a )

// �����`�Х����
extern HWND hWnd;		// ������ɥ��ϥ�ɥ�
extern BOOL WindowMode;		// ������ɥ���`��
extern int ResoMode;
extern int LowResoCmdFlag;		// ����ȉ�����ޥ�ɥե饰
extern int	CmdShow;	// WinMain�v���������򥰥��`�Х�ˤ���
extern LPSTR CmdLine; 	// WinMain�v���Υ��ޥ�ɥ饤�������򥰥��`�Х�ˤ���
extern HANDLE hMutex;	// ͬ�r���ӥ����å����֥������ȤΥϥ�ɥ�

extern char realBinName[];
extern char adrnBinName[];
extern char sprBinName[];
extern char sprAdrnBinName[];

#ifdef _DEBUG
extern BOOL offlineFlag;
extern char DebugKey0[ 256 ];		// �ǥХå����`��
extern char DebugKey1[ 256 ];		// �ǥХå����`��
extern char DebugKey2[ 256 ];		// �ǥХå����`��
#endif

#endif
