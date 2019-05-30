#ifndef _LOGIN_H_
#define _LOGIN_H_


enum
{
	WINDOW_MESSAGETYPE_MESSAGE,					// 只有消息
	WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,		// 消息
	WINDOW_MESSAGETYPE_SELECT,					// 选择器
	WINDOW_MESSAGETYPE_PETSELECT,				// 宠物
	WINDOW_MESSAGETYPE_PARTYSELECT,				// 家族？？ 
	WINDOW_MESSAGETYPE_PETANDPARTYSELECT,		// 家族宠
	WINDOW_MESSAGETYPE_ITEMSHOPMENU,			// 菜单 物品商店
	WINDOW_MWSSAGETYPE_ITEMSHOPMAIN,			// 这个店  当前商店
	WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN,		// 商店橱窗中的专业项目的主要购买主窗口
	WINDOW_MESSAGETYPE_PETSKILLSHOP,			// 窗口店宠物技能
	WINDOW_MESSAGETYPE_WIDEMESSAGE,				// 只有信息窗口（大）
	WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT	// 线路输入和信息（大）
};

#define	WINDOW_BUTTONTYPE_NONE		(0)
#define	WINDOW_BUTTONTYPE_OK		(1 << 0)
#define	WINDOW_BUTTONTYPE_CANCEL	(1 << 1)
#define	WINDOW_BUTTONTYPE_YES		(1 << 2)
#define	WINDOW_BUTTONTYPE_NO		(1 << 3)
#define	WINDOW_BUTTONTYPE_PRE		(1 << 4)
#define	WINDOW_BUTTONTYPE_NEXT		(1 << 5)

extern char selectServerName[][64];
extern char selectServerName2[][64];

extern BOOL logOutFlag;

extern int windowTypeWN;
extern short wnCloseFlag;


void idPasswordProc( void );

void titleProc( void );
void selectCharacterProc( void );
void characterLoginProc( void );

void makeCharacterProc( void );

void characterLogoutProc( void );


void initOpenServerWindow( void );
void openServerWindow( int, int, int, int, char * );
void openServerWindowProc( void );

int selGraId( int *, int );
int selRepGraId( int *, int );
int pushGraId( int *, int );
int selFontId( int *, int );
int focusGraId( int *, int );
int focusFontId( int *, int );

#endif
