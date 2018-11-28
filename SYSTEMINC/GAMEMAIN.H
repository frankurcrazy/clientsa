// gamemain.cpp �إå��ե�����

#ifndef _GAMEMAIN_H_
#define _GAMEMAIN_H_

// ���`���״�B
enum{
	GAME_LOGIN,					// ��������
	GAME_FIELD,					// �ե��`��ɥޥå�
	GAME_FIELD_TO_ENCOUNT,		// �ե��`��ɥޥåפ��饨�󥫥�����ݳ���
	GAME_ENCOUNT_TO_BATTLE,		// ���󥫥�����ݳ�������L
	GAME_BATTLE				// ���󥫥�����ݳ�������L
};


extern int	GameState;	// ���`���״�B
extern double NowTime;	// �F�ڤΕr�gӛ��

//---------------------------------------------------------------------------//
// �v���ץ��ȥ���������                                                      //
//---------------------------------------------------------------------------//
BOOL GameMain( void );				// ���`���I���ᥤ���v��

// ���`���_ʼ�I��
BOOL InitGame( void );

// ���`��K�˄I��
void EndGame( void );

#ifdef _DEBUG		
// ����ʾ
void InfoDisp( void );
#endif

// �ե�`���`�Ȥ�Ӌ�㣨���ˤ����ɣ�
void DisplayFrameRate( void );

#endif
