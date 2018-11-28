/************************/
/*	menu.h				*/
/************************/
#ifndef _MENU_H_
#define _MENU_H_

#include "pc.h"
#include "chat.h"


#define MAIL_STR_LEN 140		// ��`�����֤��L��
#define MAIL_DATE_STR_LEN 20	// �ո��������Ф��L��
#define MAIL_MAX_HISTORY 20		// �Ěs�μ���
#define MAX_CHAT_REGISTY_STR		8		// ����åȤ����ֵ��h����

// �����ƥ�Хåե��`������
typedef struct{
	int 	x, y;			// ��ʾ����
	int 	defX, defY;		// �ǥե��������
	int 	bmpNo;			// ���񷬺�
	int 	dispPrio;		// ��ʾ����
	BOOL	dragFlag;		// �ɥ�å��ե饰
	BOOL	mixFlag;		// �ϳɥե饰
}ITEM_BUFFER;

// ��`����Ěs������
typedef struct{
	char 	str[ MAIL_MAX_HISTORY ][ MAIL_STR_LEN + 1 ];			// ��`����Ěs������
	char 	dateStr[ MAIL_MAX_HISTORY ][ MAIL_DATE_STR_LEN + 1 ];	// �ո���������
	int 	noReadFlag[ MAIL_MAX_HISTORY ];							// δ�i�ե饰�����ťڥåȤΥ���ե��å����ţ�
	int 	petLevel[ MAIL_MAX_HISTORY ];							// ���ťڥåȤΥ���ե��å�����
	char 	petName[ MAIL_MAX_HISTORY ][ PET_NAME_LEN + 1 ];		// �ڥåȤ���ǰ
	int 	itemGraNo[ MAIL_MAX_HISTORY ];							// �줤�������ƥ�Υ���ե��å�����
	int 	newHistoryNo;											// �����Ěs�η���
}MAIL_HISTORY;

// ������ɥ���ʾ�����������䘋����
typedef struct{
	int sizeX,sizeY;	// ������
	int titleNo;		// �����ȥ뷬��
	int titleX, titleY;	// �����ȥ������
	int mx, my;			// ��������
	int nowX, nowY;		// �����ФΥ�����
	int cnt;			// ��������
	int wndType;		// ������ɥ��ηN�
	int hitFlag;		// �������ж��Ф�o��
	int yesDispNo;		// �Ϥ��ε������ж�����
	int noDispNo;		// �������ε������ж�����
	int boxDispPrio;	// ����ܥå����α�ʾ�����λ
	int yesNoResult;	// YesNo�νY���������������������Ϥ���
}WINDOW_DISP;

#define MAX_PET_KIND 	124		// �ڥåȤηN�
#define MAX_PET_TBL 	181		// �ڥåȥƩ`�֥�Υ�����
#define MAX_PET_PAGE_SIZE 	8	// ���ک`���˱�ʾ������ڥåȤ�����

// ����Х�������
typedef struct{
	char name[ PET_NAME_LEN + 1 ];	// �ڥåȤ���ǰ
	char freeName[ PET_NAME_LEN + 1 ];	// �ڥåȤΤ�����
	int faceGraNo;					// �ڥåȤ����ե��å�����
	int level;						// ��٥�
	int maxHp;						// ���ȣ�
	int str;						// ������
	int quick;						// ���礵
	int def;						// ������
	int earth, water, fire, wind;	// ��ˮ���L
	int flag;						// ���h�ե饰������Ҋ�����Ȥʤ� ����Ҋ�� ����д���ä�����Ҏ�� �������h�g�� ��
}PET_ALBUM;

// ����Х��åڥåȥǩ`���Ʃ`�֥똋����
typedef struct{
	int albumNo;					// ����Х�ΤȤ���������
	char name[ PET_NAME_LEN + 1 ];	// �ڥåȤ���ǰ
	int faceGraNo;					// �ڥåȤ����ե��å�����
}PET_ALBUM_TBL;


// �������Щ`�ե饰
extern BOOL TaskBarFlag;
// �ȥ���ե饰
extern unsigned int MenuToggleFlag;
// �����ƥ�Хåե��`
extern ITEM_BUFFER ItemBuffer[];
// ���Ʃ`�������åץݥ����
extern int StatusUpPoint;
// �Α�ڥåȉ���ץ��ȥ������ťե饰
extern BOOL BattlePetReceiveFlag;		
// �Хȥ������ФΥڥåȷ���
extern int BattlePetReceivePetNo; 
// ��`����Ěs������
extern MAIL_HISTORY MailHistory[];	
// ����åȤ����ֵ��h�������Хåե��`
extern STR_BUFFER chatRegistryStr[];
// ��`�������������
extern STR_BUFFER MailStr;		
// ������ɥ����������ݥ���
extern ACTION *pActMenuWnd3;
// ����åȤ����ֵ��h�������Хåե��`
extern STR_BUFFER chatRegistryStr[];
// �ץ쥤��`�γƺ���������
extern STR_BUFFER shougouChange;
// �ڥåȤ���ǰ�����������
extern STR_BUFFER petNameChange;
// Ԕ��������ɥ��˱�ʾ����Ƥ���ڥåȤη���
extern int  petStatusNo;

extern int mailHistoryWndPageNo;	// �Ěs������ɥ��Υک`������
extern int mailHistoryWndSelectNo;	// �x�k����Ƥ������̷���	
//extern int mailReceivePetGraNo;		// �ڥåȥ�`�����ŕr�ΥڥåȤΥ���ե��å�����

// ���L����ԄӤǥޥåץ�����ɱ�ʾ����ե饰
extern int MapWmdFlagBak;

// ���L�Y��������ɥ���������ޤǤΥե�`����
#define RESULT_WND_CLOSE_TIME 	300	// ����
// ���L�Y��������ɥ���������ޤǤΥ����ީ`
extern int ResultWndTimer;

// �ϳɳ������ե饰
extern BOOL ItemMixRecvFlag;

// ��˥�`�v�B�γ��ڻ� *******************************************************/
void InitMenu( void );

// ��˥�`�v�B�γ��ڻ��� *******************************************************/
void InitMenu2( void );

// ��˥�`���Ƥ뤫�����å� *******************************************************/
BOOL CheckMenuFlag( void );

// ��˥�`�I�� ***************************************************************/
void MenuProc( void );

// ������ɥ���ʾ���������� ***************************************************/
ACTION *MakeWindowDisp( int x, int y, int sizeX, int sizeY, int titleNo, int wndType );

// �������ж��ܥå������� ******************************************************/
BOOL MakeHitBox( int x1, int y1, int x2, int y2, int dispPrio );

// ��`��ǩ`���α��� ****************************************************************/
BOOL SaveMailHistory( int no );

// ��`��ǩ`�����i���z�� ****************************************************************/
BOOL LoadMailHistory( void );

// ��`��δ�i�����å� ****************************************************************/
BOOL CheckMailNoReadFlag( void );

// ��������r�γ��ڻ���̫��ã� **************************************************/
void InitOhtaParam( void );

// �ּ����˥�`�����Ĩ�� **********************************************************/
void DeathLetterAction( void ); 

// ����åȤε��h���֤α��� ****************************************************************/
BOOL SaveChatRegistyStr( void );

// ����åȤε��h���֤��i���z�� ****************************************************************/
BOOL LoadChatRegistyStr( void );

// �����ФΥ��󥿩`��� *****************************************************/
void CenteringStr( char *inStr, char *outStr, int max );

// ��Ҏ�ڥåȤΥ����å� ****************************************************************/
void CheckNewPet( int sprNo );

// ���L�r����Ҏ�ڥåȥ����å� **********************************************************/
void CheckBattleNewPet( void );

// ����Х���i���z�� ****************************************************************/
BOOL LoadAlbum( void );

// ����Х�α��� ****************************************************************/
BOOL SaveAlbum( int no );

#endif

