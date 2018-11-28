/************************/
/*	battleMenu.h		*/
/************************/
#ifndef _BATTLE_MENU_H_
#define _BATTLE_MENU_H_

// BattleBpFlag�Υӥå����
#define BATTLE_BP_JOIN				( 1 << 0 )		// �Α�
#define BATTLE_BP_PLAYER_MENU_NON	( 1 << 1 )		// �ץ쥤��`��˥�`�ʤ�
#define BATTLE_BP_BOOMERANG			( 1 << 2 )		// �֩`����
#define BATTLE_BP_PET_MENU_NON		( 1 << 3 )		// �ڥåȥ�˥�`�ʤ�
#define BATTLE_BP_ENEMY_SURPRISAL	( 1 << 4 )		// ���������Ĥ����r������Ĥ��줿�r��
#define BATTLE_BP_PLAYER_SURPRISAL	( 1 << 5 )		// �ץ쥤��`��������Ĥ����r

// �Хȥ륳�ޥ�ɥ�����
#define BATTLE_BUF_SIZE	4
#define BATTLE_COMMAND_SIZE			4096
// �Хȥ륳�ޥ��������
extern char BattleCmd[ BATTLE_COMMAND_SIZE ];
extern char BattleCmdBak[ BATTLE_BUF_SIZE ][ BATTLE_COMMAND_SIZE ];
// �Хȥ륳�ޥ�������Хݥ��󥿩`
extern int BattleCmdReadPointer;
extern int BattleCmdWritePointer;
// �Хȥ륳�ޥ�������ХХå����å�
//extern char BattleCmdBak[];
// �Хȥ�״�B������
extern char BattleStatus[ BATTLE_COMMAND_SIZE ];
extern char BattleStatusBak[ BATTLE_BUF_SIZE ][ BATTLE_COMMAND_SIZE ];
// �Хȥ�״�B�����Хݥ��󥿩`
extern int BattleStatusReadPointer;
extern int BattleStatusWritePointer;
// �Хȥ�״�B�����ХХå����å�
//extern char BattleStatusBak[];
// �Хȥ�״�B�����ХХå����å�
//extern char BattleStatusBak2[];
// �Хȥ��˥�`�ե饰
extern int battleMenuFlag2;
// �Է֤����з���
extern int BattleMyNo;
// ���L�r���Է֤Σͣ�
extern int BattleMyMp;
// ���L���ƽK�˥ե饰
extern int BattleEscFlag;
// �ڥåȥ�����󥫥��󥿩`
extern int BattlePetStMenCnt;
// �Α�ڥåȷ��ťХå����å�
extern int battlePetNoBak;
// �Α�ڥåȷ��ťХå����åף�
extern int battlePetNoBak2;

// �£Хե饰
extern int BattleBpFlag;
// ���ޥ�������g�ߥե饰
extern int BattleAnimFlag;
// ���`�����ťե饰
extern BOOL BattleTurnReceiveFlag;
// �F�ڤΥ��饤����ȥ��`�󷬺�
extern int BattleCliTurnNo;
// �F�ڤΥ��`�Щ`���`�󷬺�
extern int BattleSvTurnNo;

// ���L�Y��������ɥ��ե饰
extern BOOL BattleResultWndFlag;

// �Хȥ��˥�`�I�� *********************************************************/
void BattleMenuProc( void );
// �ܥ���ե饰ȫ���ڻ���*****************************************************/
void ClearBattleButton( void );
// �Хȥ��˥�`���ڻ� ********************************************************/
void InitBattleMenu( void );
// ���ޥ�������g�ߥե饰�Υ����å� ********************************************/
void CheckBattleAnimFlag( void );
// һ���ä������ä������å� ********************************************/
int CheckBattle1P2P( void );
// �;�����`���`��ʾ ***********************************************************/
void HpMeterDisp( int no );

#endif

