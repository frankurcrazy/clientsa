#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<direct.h>
#include<math.h>
//ս����ͼ


#include"system.h"
#include"loadrealbin.h"
#include"loadsprbin.h"
#include"anim_tbl.h"
#include"login.h"
#include"menu.h"
#include"map.h"
#include"lssproto_cli.h"
#include"pc.h"
#include"netmain.h"
#include"handletime.h"
#include"character.h"
#include"ime_sa.h"
#include "t_music.h"
#include "netproc.h"

#if 0

#include"netproc.h"
#include"character.h"
#include "battleProc.h"

#endif

#include "field.h"

// �ѥͥ�?�ܥ���ȫ��
short drawFieldButtonFlag = 1;
int fieldBtnHitId[FIELD_FUNC_END];	// �ޥ����Ȥε������ж���
short drawTimeAnimeFlag = 0;


// ����åȥܥ���
short menuBtn = 0;
short menuBtnFocus = 0;
int menuBtnGraNo[] =
{
	CG_FIELD_MENU_BTN_OFF,
	CG_FIELD_MENU_BTN_ON
};

// ���̥ܥ���
short cardBtn = 0;
short cardBtnFocus = 0;
int cardBtnGraNo[] =
{
	CG_FIELD_CARD_BTN_OFF,
	CG_FIELD_CARD_BTN_ON
};

// ���g�ܥ���
short partyBtn = 0;
short partyBtnFocus = 0;
short partyBtnEnableFlag = 1;
int partyBtnGraNo[] =
{
	CG_FIELD_GROUP_BTN_OFF,
	CG_FIELD_GROUP_BTN_ON
};

// ��`�����
short mailLamp = 0;
short mailLampDrawFlag = 0;
unsigned int mailLampFlashTime = 0;

// �Α�ܥ���
short joinBattleBtn = 0;
short joinBattleBtnFocus = 0;
short joinBattleBtnEnableFlag = 1;
short joinBattleHelpMsgFlag = 1;
int joinBattleBtnGraNo[] =
{
	CG_FIELD_JOIN_BTL_BTN_OFF,
	CG_FIELD_JOIN_BTL_BTN_ON
};

// ����ܥ���
short duelBtn = 0;
short duelBtnFocus = 0;
int duelBtnGraNo[] =
{
	CG_FIELD_DUEL_BTN_OFF,
	CG_FIELD_DUEL_BTN_ON
};

// ���������ܥ���
short actBtn = 0;
short actBtnForcus = 0;
int actBtnUpTime;
int nowActMode;
int actBtnGraNo[] =
{
	CG_FIELD_ACT_BTN_OFF,
	CG_FIELD_ACT_BTN_ON
};
short charActionAnimeChangeProcNo;
short charActionAnimeChangeWindowFlag;


// ���g?�Α�?����ϣ˥����å��ե饰
short etcSwitchProcNo;
short etcSwitchChangeFlag = 0;


// ��?ҹ���˥�
int amPmAnimeTime;
int amPmAnimeX;
int amPmAnimeGraNoIndex0, amPmAnimeGraNoIndex1;
int amPmAnimeGraNo[] =
{
	CG_FIELD_AM_PM_01,
	CG_FIELD_AM_PM_02,
	CG_FIELD_AM_PM_03,
	CG_FIELD_AM_PM_00
};


// ���`�ФغΤ��Υץ��ȥ�����ͤä�
short etcSendFlag = 0;

// ��˥�`�v�B
short nowFieldMenuOpenFlag = 0;		// ��˥�`�����Υե�`����_���Ƥ��뤫


// �ե��`��ɥܥ����Ѻ���ƥץ��ȥ�����ͤä��r�g
unsigned int fieldBtnPushTime = 0;

// ��`�וr�ˬF�ڤ�λ�ä��ʾ
unsigned int fieldInfoTime = 0;


// �ץ��ȥ�����
void initCharActionAnimeChange( void );
int charActionAnimeChange( void );
void initEtcSwitch( void );
int  etcSwitch( void );



struct tm *serverAliveTime;
time_t serverAliveLongTime;



// �ե��`����ϤǤ΄I���γ��ڻ�
void initFieldProc( void )
{
	int i;

	drawFieldButtonFlag = 1;

	menuBtn = 0;
	cardBtn = 0;
	partyBtn = 0;
// initPcAll()��ζ�i���{�٤��O�����Ƥ�ΤǤ����ǤϺΤ⤷�ʤ�
//	mailLamp = 0;

	mailLampDrawFlag = 0;
	mailLampFlashTime = 0;

	joinBattleBtn = 0;
	duelBtn = 0;
	actBtn = 0;
	actBtnForcus = 0;

	nowActMode = 0;

#if 0
	drawTimeAnimeFlag = 0;
#else
	drawTimeAnimeFlag = 1;
#endif

	for( i = 0; i < FIELD_FUNC_END; i++ )
	{
		fieldBtnHitId[i] = -2;
	}

	etcSendFlag = 0;
	etcSwitchChangeFlag = 0;

	fieldInfoTime = 0;

	partyBtnEnableFlag = 1;
	joinBattleBtnEnableFlag = 1;
	joinBattleHelpMsgFlag = 1;
}


// �ե��`��ɄI���Υꥻ�å�
void resetFieldProc( void )
{
	actBtn = 0;
	menuBtn = 0;
	fieldInfoTime = 0;
}


// �ե��`����ϤǤ΄I��
void fieldProc( void )
{
	int selId;
	int pushId;
	int focusId;
	BOOL walkFlag = FALSE;

	// ���������ؤ�
	rand2();

#ifdef _DEBUG
#if 0
	if( joy_trg[1] & JOY_CTRL_YEN )
	{
		int i;
		int graNo;
		static mode = 0;
		int dx, dy;
		short dir;

		graNo = -1;
		for( i = 0; i < MAX_PET; i++ )
		{
			if( pet[i].useFlag )
			{
				graNo = pet[i].graNo;
				break;
			}
		}
		if( graNo >= 0 && pc.ptAct != NULL )
		{
			if( mode == 0 )
			{
				createPetAction( graNo, nowGx, nowGy, pc.ptAct->anim_ang, 0, (rand() % 8), -1 );
			}
			else
			if( mode == 1 )
			{
				getRouteData( pc.ptAct->anim_ang, &dx, &dy );
				dir = pc.ptAct->anim_ang+4;
				ajustClientDir( &dir );
				createPetAction( graNo, nowGx+dx, nowGy+dy, dir, 1, 0, -1 );
			}
			else
			if( mode == 2 )
			{
				getRouteData( pc.ptAct->anim_ang, &dx, &dy );
				dir = pc.ptAct->anim_ang+4;
				ajustClientDir( &dir );
				createPetAction( graNo, nowGx+dx, nowGy+dy, dir, 2, 0, -1 );
			}
			else
			if( mode == 3 )
			{
				getRouteData( pc.ptAct->anim_ang, &dx, &dy );
				dir = pc.ptAct->anim_ang+4;
				ajustClientDir( &dir );
				createPetAction( graNo, nowGx+dx, nowGy+dy, dir, 3, 0, -1 );
			}
			mode++;
			if( mode > 3 )
				mode = 0;
		}
	}
#endif
#endif


	// ������������Υ���`�ȥ��åȥ��`�΄I��
	actionShortCutKeyProc();


	// ��˥�`�Υե饰��䤨�褦�Ȥ����B�����Ƥ����ä����ä��Τ�
	// �����r�g���^���Ƥ����ͤ�
	if( etcSwitchChangeFlag
	 && eventWarpSendFlag == 0
	 && eventEnemySendFlag == 0
	 && sendEnFlag == 0 )
	{
		if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// �B������
		{
			lssproto_FS_send( sockfd, pc.etcFlag );
			fieldBtnPushTime = GetTickCount();
			etcSwitchChangeFlag = 0;
		}
	}


	// ���Υ�˥�`�����Ƥ�Ȥ��ϱ�ʾ���ʤ�
	if( CheckMenuFlag()
	 || windowTypeWN > -1
	 || actBtn == 1
	 || menuBtn == 1 )
	{
		drawFieldButtonFlag = 0;
	}
	else
	{
		drawFieldButtonFlag = 1;
	}



	// ��ե��`��ɄI���Ǥʤ󤫤Υ�����ɥ������Ƥ뤫�����å�
	nowFieldMenuOpenFlag = 0;
	// ���`�Хꥯ�����Ț��å�����ɥ����_���Ƥ�
	if( windowTypeWN != -1 )
	{
		nowFieldMenuOpenFlag = 1;
	}
	// �����������������ɥ����_���Ƥ�
	if( actBtn == 1 )
	{
		nowFieldMenuOpenFlag = 1;
	}
	// ���g?�Α�?����ϣ˥����å�������ɥ����_���Ƥ�
	if( menuBtn == 1 )
	{
		nowFieldMenuOpenFlag = 1;
	}



	// �ӣ��r�g�ˤ�볯?��?ҹ���˥�`�����I�����ӣ��r�g1�դ�1024��
	amPmAnimeTime = (SaTime.hour+832)%1024;
	amPmAnimeGraNoIndex0 = amPmAnimeTime/256;
	amPmAnimeGraNoIndex1 = (amPmAnimeGraNoIndex0+1)%4;
	amPmAnimeX = (amPmAnimeTime%256)/4;


	// �Уä��ƄӤ��Ƥ��뤫�����å�
	if( nowVx != 0 || nowVy != 0 )
		walkFlag = TRUE;

	// ���`�Ф��ͤä��ץ��ȥ���η��¤����äƤ�����ե饰��ꥻ�å�
	if( etcSendFlag )
	{
		if( prSendFlag == 0
		 && jbSendFlag == 0
		 && duelSendFlag == 0 )
		{
			etcSendFlag = 0;
		}
		else
		{
			etcSendFlag = 1;
		}
	}

	// �Τ餫�Υץ��ȥ���򥵩`�Ф��ͤäƤ���r��
	// ���ΙC�ܤ�g�ФǤ��ʤ�����
	// �i���Ƥ�Ȥ���
	if( etcSendFlag == 0 && walkFlag == FALSE )
	{
		// ���N�ܥ���Ѻ���줿�������å�
		selId = selGraId( fieldBtnHitId, sizeof( fieldBtnHitId )/sizeof( int ) );
		pushId = pushGraId( fieldBtnHitId, sizeof( fieldBtnHitId )/sizeof( int ) );
		focusId = focusGraId( fieldBtnHitId, sizeof( fieldBtnHitId )/sizeof( int ) );
	}
	else
	{
		selId = -1;
	}

	// �ض��Υե����Ǥ����g�ܥ����ʹ�äǤ��ʤ�����
	if( nowFloor == 31706
	 || nowFloor == 10204
	 || (10601 <= nowFloor && nowFloor <= 10605 )
	 || nowFloor == 10919 || nowFloor == 10920
	 || nowFloor == 20711 || nowFloor == 20712
	 || nowFloor == 1008 || nowFloor == 1021
	 || nowFloor == 3008 || nowFloor == 3021 )
	{
		partyBtnEnableFlag = 0;
	}
	else
	{
		partyBtnEnableFlag = 1;
	}

	// �ض��Υե����ǤϲΑ�ܥ�����Q��ܥ���Ȥ��ƒQ����Τ��h����������
	if( nowFloor < 10000 && (nowFloor/1000 > 0) && (nowFloor%1000 == 7) )
	{
		joinBattleHelpMsgFlag = 0;	// �Α��h��
		joinBattleBtnEnableFlag = 1;
	}
	else
	if( nowFloor == 1008 || nowFloor == 1021
	 || nowFloor == 3008 || nowFloor == 3021
	 || nowFloor == 10919 || nowFloor == 10920
	 || nowFloor == 20711 || nowFloor == 20712
	 || nowFloor == 10204
	)
	{
		joinBattleHelpMsgFlag = 2;	// �Α�Ǥ��ʤ��h����
		joinBattleBtnEnableFlag = 0;
	}
	else
	{
		joinBattleHelpMsgFlag = 1;	// �Q���h��
		joinBattleBtnEnableFlag = 1;
	}





	// ��˥�`�ܥ���I��
	if( selId == FIELD_FUNC_MENU
	 || (joy_trg[1] & JOY_CTRL_Q) )
	{
		if( menuBtn == 0 )
		{
			// �����������������ɥ����_���Ƥ��뤫�⤷��ʤ��Τ��]����
			// ���_���Ƥʤ��r�i��Ǥ↖�}�o����
			closeCharActionAnimeChange();

			// ���Υ�����ɥ������Ƥ�������
			if( CheckMenuFlag() )
			{
				InitMenu2();
			}

			initEtcSwitch();
			menuBtn = 1;
			play_se( 202, 320, 240 );	// ������ɥ��_����
		}
		else
		{
			closeEtcSwitch();
		}
	}
	if( menuBtn == 1 )
	{
		if( etcSwitch() )
		{
			menuBtn = 0;
		}
	}
	if( focusId == FIELD_FUNC_MENU )
	{
		menuBtnFocus = 1;		// �ܥ���˥ե��`���������äƤ���
	}
	else
	{
		menuBtnFocus = 0;
	}


	// ���̥ܥ���I��
	if( pushId == FIELD_FUNC_CARD )
	{
		cardBtn = 1;

		if( selId == FIELD_FUNC_CARD )
		{
			int dx, dy;
			int flag;

			// �Է֤�����˥���餬���뤫�{�٤�
			getRouteData( pc.dir, &dx, &dy );
			flag = checkCharObjPoint( nowGx+dx, nowGy+dy, CHAROBJ_TYPE_USER_NPC );
			// �ѩ`�ƥ�����äƤʤ��r������˥���餬����
			if( partyModeFlag == 0 && flag == TRUE
			 && eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// �B������
				{
					// ���̽��Q
					lssproto_AAB_send( sockfd, nowGx, nowGy );
					fieldBtnPushTime = GetTickCount();
				}
			}
			play_se( 217, 320, 240 );	// ����å���
		}
	}
	else
	{
		cardBtn = 0;
	}
	if( focusId == FIELD_FUNC_CARD )
	{
		cardBtnFocus = 1;		// �ܥ���˥ե��`���������äƤ���
	}
	else
	{
		cardBtnFocus = 0;
	}


	// ���g�ܥ���I��
	if( pushId == FIELD_FUNC_PARTY )
	{
		partyBtn = 1;
		if( partyBtnEnableFlag == 0 )
		{
			// �ض��ΥޥåפǤ����g�ˤʤ�ʤ�
			partyBtn = 0;
			play_se( 220, 320, 240 );	// �Σ���
		}
		else
		if( selId == FIELD_FUNC_PARTY )
		{

			// �ѩ`�ƥ�����äƤʤ��r�΄���
			if( partyModeFlag == 0 )
			{
				int dx, dy;
				int flag;

				// ����ˑ��L�򤷤Ƥ��ʤ�����餬���뤫�����å�
				getRouteData( pc.dir, &dx, &dy );
				flag = checkCharObjPointNotStatus( nowGx+dx, nowGy+dy,
					CHAROBJ_TYPE_USER_NPC, CHR_STATUS_BATTLE );
				// ����˥���餬����Τ����gҪ��
				if( flag == 1
				 && eventWarpSendFlag == 0
				 && eventEnemySendFlag == 0
				 && sendEnFlag == 0 )
				{
					if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// �B������
					{
						// �l�����g�ˤ�ʤäƤʤ��r���Է֤���`���`�Ǥ⣩�����gҪ��
						lssproto_PR_send( sockfd, nowGx, nowGy, 1 );
						prSendMode = 1;
						prSendFlag = 1;
						etcSendFlag = 1;
						fieldBtnPushTime = GetTickCount();
					}
				}
			}
			else
			// �ѩ`�ƥ��Εr�΄���
			{
				if( eventWarpSendFlag == 0
				 && eventEnemySendFlag == 0
				 && sendEnFlag == 0 )
				{
					if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// �B������
					{
						// �l�������g�Εr�����
						lssproto_PR_send( sockfd, nowGx, nowGy, 0 );
						prSendMode = 0;
						prSendFlag = 1;
						etcSendFlag = 1;
						fieldBtnPushTime = GetTickCount();
					}
				}
			}
			play_se( 217, 320, 240 );	// ����å���
		}
	}
	else
	{
		partyBtn = 0;
	}
	if( focusId == FIELD_FUNC_PARTY )
	{
		partyBtnFocus = 1;		// �ܥ���˥ե��`���������äƤ���
	}
	else
	{
		partyBtnFocus = 0;
	}


	// �Α�ܥ���I��
	if( pushId == FIELD_FUNC_JOIN_BATTLE )
	{
		joinBattleBtn = 1;
		if( joinBattleBtnEnableFlag == 0 )
		{
			// �ض��ΥޥåפǤϲΑ�Ǥ��ʤ�
			joinBattleBtn = 0;
			play_se( 220, 320, 240 );	// �Σ���
		}
		else
		if( selId == FIELD_FUNC_JOIN_BATTLE )
		{
			int dx, dy;
			int flag;

			// �ѩ`�ƥ�����äƤʤ��r
			// �ޤ��ϡ���`���Εr
			// ����ˑ��L�򤷤Ƥ��륭��餬���뤫�����å�
			getRouteData( pc.dir, &dx, &dy );
			flag = checkCharObjPointStatus( nowGx+dx, nowGy+dy,
				CHAROBJ_TYPE_USER_NPC, CHR_STATUS_BATTLE );
			// ����ˑ��L�򤷤Ƥ��륭��餬����
			if( (partyModeFlag == 0
			 || (partyModeFlag == 1 && (pc.status & CHR_STATUS_LEADER) != 0))
			 && flag == 2
			 && eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// �B������
				{
					// ���L�Ф����֤˲Α���ͤ�
					lssproto_JB_send( sockfd, nowGx, nowGy );
					jbSendFlag = 1;
					etcSendFlag = 1;
					fieldBtnPushTime = GetTickCount();
				}
			}
			play_se( 217, 320, 240 );	// ����å���
		}
	}
	else
	{
		joinBattleBtn = 0;
	}
	if( focusId == FIELD_FUNC_JOIN_BATTLE )
	{
		joinBattleBtnFocus = 1;		// �ܥ���˥ե��`���������äƤ���
	}
	else
	{
		joinBattleBtnFocus = 0;
	}


	// ����ܥ���I��
	if( pushId == FIELD_FUNC_DUEL )
	{
		duelBtn = 1;

		if( selId == FIELD_FUNC_DUEL )
		{
			int dx, dy;
			int flag;

			// �ѩ`�ƥ�����äƤʤ��r
			// �ޤ��ϡ���`���Εr
			// ����˥���餬���뤫�����å�
			getRouteData( pc.dir, &dx, &dy );
			flag = checkCharObjPoint( nowGx+dx, nowGy+dy, CHAROBJ_TYPE_USER_NPC );
			// ����˥���餬����Τǌ����ꤷ�z��
			if( (partyModeFlag == 0
			 || (partyModeFlag == 1 && (pc.status & CHR_STATUS_LEADER) != 0))
			 && flag == TRUE
			 && eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// �B������
				{
					lssproto_DU_send( sockfd, nowGx, nowGy );
					duelSendFlag = 1;
					etcSendFlag = 1;
					fieldBtnPushTime = GetTickCount();
				}
			}
			play_se( 217, 320, 240 );	// ����å���
		}
	}
	else
	{
		duelBtn = 0;
	}
	if( focusId == FIELD_FUNC_DUEL )
	{
		duelBtnFocus = 1;	// �ܥ���˥ե��`���������äƤ���
	}
	else
	{
		duelBtnFocus = 0;
	}

	// ���������ܥ���I��
	if( selId == FIELD_FUNC_ACT
	 || (joy_trg[1] & JOY_CTRL_W) )
	{
		if( actBtn == 0 )
		{
			// ���g?�Α�?����ϣ˥����å��ե饰���_���Ƥ��뤫�⤷��ʤ��Τ��]����
			// ���_���Ƥʤ��r�i��Ǥ↖�}�o����
			closeEtcSwitch();

			// ���Υ�����ɥ������Ƥ�������
			if( CheckMenuFlag() )
			{
				InitMenu2();
			}

			initCharActionAnimeChange();
			actBtn = 1;
			play_se( 202, 320, 240 );	// ������ɥ��_����
		}
		else
		{
			closeCharActionAnimeChange();
		}
	}
	if( actBtn == 1 )
	{
		if( charActionAnimeChange() )
		{
			actBtn = 0;
		}
	}
	if( focusId == FIELD_FUNC_ACT )
	{
		actBtnForcus = 1;	// �ܥ���˥ե��`���������äƤ���
	}
	else
	{
		actBtnForcus = 0;
	}


	// ��`����ׄI��
	if( mailLamp )
	{
		// ���ڻ�
		if( mailLampFlashTime == 0 )
		{
			mailLampDrawFlag = 1;
			mailLampFlashTime = GetTickCount();
		}
		else
		if( mailLampFlashTime+FIELD_MAIL_LAMP_FLASH_TIME < GetTickCount() )
		{
			mailLampDrawFlag++;
			mailLampDrawFlag &= 1;
			mailLampFlashTime = GetTickCount();
		}
	}
	else
	{
		mailLampDrawFlag = 0;
		mailLampFlashTime = 0;
	}
}


// �ե��`����ϤǤ΄I����ӳ�����軭
void drawField( void )
{
	int leftUpPanelX = 0;		// ���ϥܥ����̨��������
	int leftUpPanelY = 0;
	int rightUpPanelX = 504;	// ���ϥܥ����̨��������
	int rightUpPanelY = 0;
	int i;


#if 0
#ifdef _DEBUG_MSG

	char msg[256];

	// ���`�Ф���Echo�����Ť��Фä��r�Εr�g���ʾ
	//  ͨ�����`��Υץ��ȥ��뤬���Ť���Ƥ����g�ϸ��¤���ʤ�
	sprintf( msg, "%s Server Alive -> %02d/%02d/%02d %02d:%02d:%02d",
		selectServerName2[selectServerIndex],
		(serverAliveTime->tm_year % 100), serverAliveTime->tm_mon+1, serverAliveTime->tm_mday,
		serverAliveTime->tm_hour, serverAliveTime->tm_min, serverAliveTime->tm_sec );
	StockFontBuffer( 108, 8, FONT_PRIO_FRONT, 0, msg, 0 );

#endif
#endif


	if( drawFieldButtonFlag )
	{
		// ���ϥܥ����̨��
		StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
			DISP_PRIO_MENU, CG_FIELD_MENU_LEFT, 0 );

		// ���ϥ�˥�`�ܥ���
		fieldBtnHitId[FIELD_FUNC_MENU] =
			StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
				DISP_PRIO_IME3, menuBtnGraNo[menuBtn], 2 );
		if( menuBtnFocus )
		{
			// ��˥�`�ܥ����h��
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"���N�O�����Ф��ޤ���", 0 );
		}

		// �������̥ܥ���
		fieldBtnHitId[FIELD_FUNC_CARD] =
			StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
				DISP_PRIO_IME3, cardBtnGraNo[cardBtn], 2 );
		if( cardBtnFocus )
		{
			// ���̥ܥ����h��
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"���Υ���饯���`�����̽��Q�򤷤ޤ���", 0 );
		}

		// �������g�ܥ���
		fieldBtnHitId[FIELD_FUNC_PARTY] =
			StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
				DISP_PRIO_IME3, partyBtnGraNo[partyBtn], 2 );
		if( partyBtnFocus )
		{
#if 0
			// ���g�ܥ����h��
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"���Υ���饯���`�����g�����ޤ���", 0 );
#else
			// ���g�ܥ����h��
			// �ض��ΥޥåפǤ����g�ܥ����ʹ���ʤ����뤿��
			if( partyBtnEnableFlag == 0 )
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"�����Ǥ����g������¤������ޤ���", 0 );
			}
			else
			{
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"���Υ���饯���`�����g�����ޤ���", 0 );
			}
#endif
		}


		// ���ϥ�`�����
		if( mailLampDrawFlag )
			StockDispBuffer( leftUpPanelX+52, leftUpPanelY+28,
				DISP_PRIO_IME3, CG_FIELD_MAIL_LAMP, 0 );


		// ���ϥܥ����̨��
		StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
			DISP_PRIO_MENU, CG_FIELD_MENU_RIGHT, 0 );

		// ���ϲΑ�ܥ���
		fieldBtnHitId[FIELD_FUNC_JOIN_BATTLE] =
			StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
				DISP_PRIO_IME3, joinBattleBtnGraNo[joinBattleBtn], 2 );
		if( joinBattleBtnFocus )
		{
			if( joinBattleHelpMsgFlag == 1 )
			{
				// �Α�ܥ����h��
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"���L�ФΥ���饯���`���������Ȥ��ƲΑ餷�ޤ���", 0 );
			}
			else
			if( joinBattleHelpMsgFlag == 2 )
			{
				// �Α�ܥ����h��
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"�����ǤϲΑ�Ǥ��ޤ���", 0 );
			}
			else
			{
				// �Q��ܥ����h��
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"�ǥ奨����Q�餷�ޤ���", 0 );
			}
		}

		// ���ό���ܥ���
		fieldBtnHitId[FIELD_FUNC_DUEL] =
			StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
				DISP_PRIO_IME3, duelBtnGraNo[duelBtn], 2 );
		if( duelBtnFocus )
		{
			// �ǥ奨��ܥ����h��
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"���Υץ쥤��`����饯���`�˥ǥ奨��(����)���ꤷ�z�ߤޤ���", 0 );
		}

		// ���ϥ��������ܥ���
		fieldBtnHitId[FIELD_FUNC_ACT] =
			StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
				DISP_PRIO_IME3, actBtnGraNo[actBtn], 2 );
		if( actBtnForcus )
		{
			// ���������ܥ����h��
			StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
				"����饯���`��ɫ���ʥ��������򤵤��ޤ���", 0 );
		}

		// �r�g���˥�
		if( drawTimeAnimeFlag )
		{
			StockDispBuffer( rightUpPanelX+104-amPmAnimeX, rightUpPanelY+21,
				DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex0], 0 );
			StockDispBuffer( rightUpPanelX+104+64-amPmAnimeX, rightUpPanelY+21,
				DISP_PRIO_MENU, amPmAnimeGraNo[amPmAnimeGraNoIndex1], 0 );
		}
		else
		{
			// �r�g���˥���ʾ���ʤ��r�����󥸥��Ȥ����ϥХå���������
			StockDispBuffer( rightUpPanelX+68, rightUpPanelY+32,
				DISP_PRIO_MENU, CG_FIELD_MENU_RIGHT_BACK, 0 );
		}

	}
	else
	{
		for( i = 0; i < FIELD_FUNC_END; i++ )
		{
			fieldBtnHitId[i] = -2;
		}
	}
}




// ��˥�`�_���Ƥ뤫�ɤ���
BOOL checkFieldMenuFlag( void )
{
	return nowFieldMenuOpenFlag;
}




// ����饢���������������ɥ�
// ���ڻ�
void initCharActionAnimeChange( void )
{
	charActionAnimeChangeProcNo = 0;
}


// �I����K��餻��
void closeCharActionAnimeChange( void )
{
	charActionAnimeChangeProcNo = 1000;
}


// ������������I��
//  ���ꂎ: 0 ... �I����
//          !0 ... �I������
int charActionAnimeChange( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	static int btnId[13];
	int id = 0;
	int focusId = 0;
	char *msg[] =
	{
		" ���� ",
			" վ�� ",
			" ����",
			" �߶� ",
			" ��ͷ",
			" �ε� ",
			" ���� ",
			" ���� ",
			" ���� ",
			" ���� ",
			" ���� ",
			" ����",
			" Ͷ�� "
	};
	char *msg2[] =
	{
		"����(Ctrl+1)",
			"վ��(Ctrl+7)",
			"����(Ctrl+2)",
			"�߶�(Ctrl+8)",
			"��ͷ(Ctrl+3)",
			"�ε�(Ctrl+9)",
			"����(Ctrl+4)",
			"����(Ctrl+0)",
			"����(Ctrl+5)",
			"����(Ctrl+-)",
			"Ͷ��(Ctrl+6)",
			"����(Ctrl+^)",
			"Ͷ��(Ctrl+\\)"
	};
	/*
	char *msg[] =
	{
		" ������ ",
		" ������ ",
		"�֤����",
		" �i���� ",
		"���ʤ���",
		" ����� ",
		" ϲ���� ",
		" ������ ",
		" ŭ���� ",
		" ���`�� ",
		" ������ ",
		"����`��",
		" Ͷ���� "
	};
	char *msg2[] =
	{
		"���륢�������򤷤ޤ���(Ctrl+1)",
		"���ĥ��������򤷤ޤ���(Ctrl+7)",
		"�֤���륢�������򤷤ޤ���(Ctrl+2)",
		"�i�����������򤷤ޤ���(Ctrl+8)",
		"���ʤ������������򤷤ޤ���(Ctrl+3)",
		"����륢�������򤷤ޤ���(Ctrl+9)",
		"ϲ�֥��������򤷤ޤ���(Ctrl+4)",
		"���ĥ��������򤷤ޤ���(Ctrl+0)",
		"ŭ�륢�������򤷤ޤ���(Ctrl+5)",
		"���`�ɥ��������򤷤ޤ���(Ctrl+-)",
		"�����ॢ�������򤷤ޤ���(Ctrl+6)",
		"����`�����������򤷤ޤ���(Ctrl+^)",
		"Ͷ���륢�������򤷤ޤ���(Ctrl+\\)"
	};*/
	int i, j;
	int x1, y1, x2, y2;
	int chgTbl[] =
	{
	   5,	//  ������ 
	   3,	//  ������ 
	   6,	// �֤����
	   4,	//  �i���� 
	  11,	// ���ʤ���
	   2,	//  ����� 
	   7,	//  ϲ���� 
	   0,	//  ������ 
	   8,	//  ŭ���� 
	  10,	//  ���`�� 
	   9,	//  ������ 
	   1,	// ����`��
	  12	// Ͷ����
	};


	if( charActionAnimeChangeProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ������ɥ�����
		w = 3;
		h = 6;
		x = 440;
		//y = 72;
		y = 16;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		charActionAnimeChangeProcNo++;
	}

	if( ptActMenuWin != NULL )
	{
		id = -1;
		focusId = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// �ܥ�����x�k�ж�
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
			focusId = focusGraId( btnId, sizeof( btnId )/sizeof( int ) );

			for( i = 0; i < 6; i++ )
			{
				for( j = 0; j < 2; j++ )
				{
					x1 = x+84*j+18;
					y1 = y+i*30+21;
					x2 = x1 + 73;
					y2 = y1 + 22;
					if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
					{
						if( mouse.onceState & MOUSE_LEFT_CRICK )
						{
							id = i*2+j+1;
						}
						focusId = i*2+j+1;
					}
				}
			}

			i = 6;
			j = 0;
			x1 = x+42+18;
			y1 = y+i*30+21;
			x2 = x1 + 73;
			y2 = y1 + 22;
			if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
			{
				if( mouse.onceState & MOUSE_LEFT_CRICK )
				{
					id = i*2+j+1;
				}
				focusId = i*2+j+1;
			}
		}

		// ���Υ�˥�`������ΤǤ��ä��Υ�����ɥ����]����
		if( CheckMenuFlag() )
		{
			id = 100;
		}
		else
		// ESC��Ѻ���줿�ΤǽK��
		if( (joy_trg[ 0 ] & JOY_ESC) && ImeBufferBak2 == NULL )
		{
			id = 100;
			play_se( 203, 320, 240 );	// ������ɥ��]������
		}
		else
		// �K���v�������Ф줿�ΤǽK��
		if( charActionAnimeChangeProcNo == 1000 )
		{
			id = 100;
		}

		// ����󥻥�
		if( id == 0 )
		{
			play_se( 203, 320, 240 );	// ������ɥ��]������
		}
		else
		if( 1 <= id && id <= 13 )
		{
			id--;
//			if( id >= 3 )
//				id++;
			if( eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// �B������
				{
					lssproto_AC_send( sockfd, nowGx, nowGy, chgTbl[id] );
					setPcAction( chgTbl[id] );
					fieldBtnPushTime = GetTickCount();
				}
			}
			// ��˥�`���Ŀ���x��Ǥ⥦����ɥ����]���ʤ��褦�ˤ���
			id = -1;
			play_se( 217, 320, 240 );	// ����å���
		}

		if( id >= 0 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return 1;
		}

		if( ptActMenuWin->hp >= 1 )
		{
			btnId[0] = StockDispBuffer( x+96, y+258,
					DISP_PRIO_MENU, CG_CLOSE_BTN, 2 );
			if( focusId == 0 )
			{
				// ��˥�`�Ŀ�h��
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"��������󥦥���ɥ����]���ޤ���", 0 );
			}

			for( i = 0; i < 6; i++ )
			{
				for( j = 0; j < 2; j++ )
				{
					StockFontBuffer( x+84*j+20, y+i*30+24,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i*2+j], 0 );
					if( i*2+j == focusId-1 )
					{
						// ��˥�`�Ŀ�h��
						StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
							msg2[i*2+j], 0 );
					}
				}
			}
			i = 6;
			j = 0;
			StockFontBuffer( x+42+20, y+i*30+24,
				FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i*2+j], 0 );
			if( i*2+j == focusId-1 )
			{
				// ��˥�`�Ŀ�h��
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					msg2[i*2+j], 0 );
			}
		}
	}

	return 0;
}




// ���g?�Α�?����ϣ˥����å�������ɥ�
// ���ڻ�
void initEtcSwitch( void )
{
	etcSwitchProcNo = 0;
}


// �I����K��餻��
void closeEtcSwitch( void )
{
	etcSwitchProcNo = 1000;
}


// ���g?�Α�?����ϣ˥����å��I��
//  ���ꂎ: 0 ... �I����
//          !0 ... �I������
/// /��飿��ս����սOK���ش���/ /����ֵ:0...������/ /0...������ϣ�
int etcSwitch( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	static int btnId[5];
	int id = 0;
	int focusId = 0;
	//�����޸İ�ť
	char *msg[] =
		//�����ţ�",����",������������Ƭ��",�����죺��
	{
		"��ӣ�",
		"����",
		"��Ƭ",
		"����"
	};
	char *msg2[] =
	{
		" �Ρ���",
		" �٣ţ�"
	};
	char *msg3[] =
	{
		"ȫ��",
		"����"
	};
	char *msg4[] =
	{
		"������ɫ���趨�Ƿ��ܳ�Ϊͬ��",
		"������ɫ�Ķ�ս�������ܵ��趨",
		"������Ƭ�Ƿ���ܵ��趨",
		"������Ϣ�ķ�Χ�趨"
	};
	int mask[] =
	{
		PC_ETCFLAG_PARTY,
		PC_ETCFLAG_DUEL,
		PC_ETCFLAG_MAIL,
		PC_ETCFLAG_CHAT_MODE
	};
	int i;
	int x1, y1, x2, y2;

	if( etcSwitchProcNo == 0 )
	{
		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ������ɥ�����
		w = 3;
		h = 4;
		x = 16;
		//x = 440;
		//y = 72;
		y = 16;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );

		etcSwitchProcNo++;
	}

	if( ptActMenuWin != NULL )
	{
		id = -1;
		focusId = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// �ܥ�����x�k�ж�
			id = selGraId( btnId, sizeof( btnId )/sizeof( int ) );
			focusId = focusGraId( btnId, sizeof( btnId )/sizeof( int ) );

			for( i = 0; i < sizeof( msg )/sizeof( char * ); i++ )
			{
				x1 = x+18;
				y1 = y+i*30+23;
				x2 = x1 + 157;
				y2 = y1 + 22;
				if( MakeHitBox( x1, y1, x2, y2, DISP_PRIO_BOX2 ) )
				{
					if( mouse.onceState & MOUSE_LEFT_CRICK )
					{
						id = i+1;
					}
					focusId = i+1;
				}
			}
		}

		// ���Υ�˥�`������ΤǤ��ä��Υ�����ɥ����]����
		if( CheckMenuFlag() )
		{
			id = 100;
		}
		else
		// ESC��Ѻ���줿�ΤǽK��
		if( ((joy_trg[ 0 ] & JOY_ESC) && ImeBufferBak2 == NULL) )
		{
			id = 100;
			play_se( 203, 320, 240 );	// ������ɥ��]������
		}
		else
		// �K���v�������Ф줿�ΤǽK��
		if( etcSwitchProcNo == 1000 )
		{
			id = 100;
		}

		// ����󥻥�
		if( id == 0 )
		{
			play_se( 203, 320, 240 );	// ������ɥ��]������
		}
		else
		if( 1 <= id && id <= sizeof( msg )/sizeof( char * ) )
		{
			id--;
			// �ӥåɤ��Ϥ��äƤ����¤���
			if( pc.etcFlag & mask[id] )
			{
				pc.etcFlag &= ~mask[id];
			}
			else
			// �¤��äƤ����Ϥ���
			{
				pc.etcFlag |= mask[id];
			}
			if( eventWarpSendFlag == 0
			 && eventEnemySendFlag == 0
			 && sendEnFlag == 0 )
			{
				if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// �B������
				{
					lssproto_FS_send( sockfd, pc.etcFlag );
					fieldBtnPushTime = GetTickCount();
				}
				else
				{
					// �ե饰��䤨�褦�Ȥ������B�����Ƥ����ä����ä��Τ�
					// ����ͤ�
					etcSwitchChangeFlag = 1;
				}
			}
			// ��˥�`���Ŀ���x��Ǥ⥦����ɥ����]���ʤ��褦�ˤ���
			id = -1;
			play_se( 217, 320, 240 );	// ����å���
		}

		if( id >= 0 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			return 1;
		}

		if( ptActMenuWin->hp >= 1 )
		{

#if 0
			btnId[0] = StockFontBuffer( x+54, y+154,
//						FONT_PRIO_FRONT, FONT_PAL_WHITE, "����󥻥�", 2 );
						FONT_PRIO_FRONT, FONT_PAL_WHITE, "���]���롡", 2 );
#else
			btnId[0] = StockDispBuffer( x+96, y+162,
					DISP_PRIO_MENU, CG_CLOSE_BTN, 2 );
#endif
			if( focusId == 0 )
			{
				// ��˥�`�Ŀ�h��
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
					"��˥�`������ɥ����]���ޤ���", 0 );
			}

			for( i = 0; i < sizeof( msg )/sizeof( char * ); i++ )
			{
				StockFontBuffer( x+20, y+i*30+26,
					FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i], 0 );
				if( i != 3 )
				{
					StockFontBuffer( x+105, y+i*30+26, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						msg2[((pc.etcFlag & mask[i])?1:0)], 0 );
				}
				else
				{
					StockFontBuffer( x+105, y+i*30+26, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						msg3[((pc.etcFlag & mask[i])?1:0)], 0 );
				}
				if( i == focusId-1 )
				{
					// ��˥�`�Ŀ�h��
					StockFontBuffer( 8, 460, FONT_PRIO_FRONT, FONT_PAL_WHITE,
						msg4[i], 0 );
				}
			}
		}
	}

	return 0;
}



// ���`�ФȤνӾA��;�Ф줿
//   ���ꂎ�� 0 ... �I����
//            1 ... "�Ϥ�"�ܥ���Ѻ���줿
int disconnectServer( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	static int btnId[1];
	int id = 0;
	char *msg[] =
	{
		"���`�ФȤνӾA��;�Ф�ޤ�����",
		"�����ȥ�ؑ���ޤ���"
	};
	int i;
	int ret = 0;
	int xx, yy;


	if( ptActMenuWin == NULL )
	{
		// ���Υ�����ɥ������Ƥ�������
		if( CheckMenuFlag() )
		{
			InitMenu2();
		}
		closeEtcSwitch();
		closeCharActionAnimeChange();


		for( i = 0; i < sizeof( btnId )/sizeof( int ); i++ )
		{
			btnId[i] = -2;
		}

		// ������ɥ�����
		w = 5;
		h = 3;
		x = (640 - w*64)/2;
		y = (456 - h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 1 );
	}

	if( ptActMenuWin != NULL )
	{
		id = -1;
		if( ptActMenuWin->hp >= 1 )
		{
			// �ܥ�����x�k�ж�
			id = selFontId( btnId, sizeof( btnId )/sizeof( int ) );
			if( id >= 0 )
			{
				DeathAction( ptActMenuWin );
				ptActMenuWin = NULL;
				ret = 1;
			}

			yy = (h*48)/4;
			for( i = 0; i < sizeof( msg )/sizeof( char * ); i++ )
			{
				xx = (w*64-strlen( msg[i] )/2*17)/2;
				StockFontBuffer( x+xx, y+(i+1)*yy,
					FONT_PRIO_FRONT, FONT_PAL_WHITE, msg[i], 0 );
			}

			xx = (w*64-strlen( "�Ϥ�" )/2*17)/2;
			btnId[0] = StockFontBuffer( x+xx, y+3*yy,
						FONT_PRIO_FRONT, FONT_PAL_YELLOW, "�Ϥ�", 2 );
		}
	}

	return ret;
}


// ��`���᤽�Έ�������ǰ���ʾ
void drawFieldInfoWin( void )
{
	static ACTION *ptActMenuWin = NULL;
	static int x, y, w, h;
	int xx, yy;

	if( fieldInfoTime == 0 )
	{
		if( ptActMenuWin != NULL )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
		}
		return;
	}

	if( MapWmdFlagBak )
	{
		return;
	}

	if( ptActMenuWin == NULL )
	{
		// ���Υ�����ɥ������Ƥ�������
		if( CheckMenuFlag() )
		{
			// �ޥåץ�����ɥ��ե饰�Хå����å�
			if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
			InitMenu2();
		}
		closeEtcSwitch();
		closeCharActionAnimeChange();

		// ������ɥ�����
		w = 4;
		h = 2;
		x = (640 - w*64)/2;
		y = (456 - h*48)/2;
		ptActMenuWin = MakeWindowDisp( x, y, w, h, NULL, 3 );

		play_se( 202, 320, 240 );	// ������ɥ��_����
	}

	if( ptActMenuWin != NULL )
	{
		if( fieldInfoTime+2000 <= GetTickCount()
		 || CheckMenuFlag()
		 || windowTypeWN > -1
		 || actBtn == 1
		 || menuBtn == 1 )
		{
			DeathAction( ptActMenuWin );
			ptActMenuWin = NULL;
			fieldInfoTime = 0;
			return;
		}
		if( ptActMenuWin->hp >= 1 )
		{

			xx = (w*64-strlen( nowFloorName )/2*17)/2;
			yy = (h*48-16)/2;
			StockFontBuffer( x+xx, y+yy,
				FONT_PRIO_FRONT, FONT_PAL_WHITE, nowFloorName, 0 );
		}
	}
}



// ������������Υ���`�ȥ��åȥ��`�΄I��
void actionShortCutKeyProc( void )
{
	unsigned int key[] =
	{
		JOY_CTRL_0,				// 0
		JOY_CTRL_CIRCUMFLEX,	// 1
		JOY_CTRL_9,				// 2
		JOY_CTRL_7,				// 3
		JOY_CTRL_8,				// 4
		JOY_CTRL_1,				// 5
		JOY_CTRL_2,				// 6
		JOY_CTRL_4,				// 7
		JOY_CTRL_5,				// 8
		JOY_CTRL_6,				// 9
		JOY_CTRL_MINUS,			// 10
		JOY_CTRL_3,				// 11
		JOY_CTRL_YEN			// 12
	};
	int i;
	BOOL pushFlag = FALSE;

	// �Уä��ƄӤ��Ƥ�����Τ⤷�ʤ�
	if( nowVx != 0 || nowVy != 0 )
		return;

	for( i = 0; i < sizeof( key )/sizeof( unsigned int ); i++ )
	{
		if( joy_trg[ 1 ] & key[i] )
		{
			pushFlag = TRUE;
			break;
		}
	}

	if( !pushFlag )
	{
		return;
	}

	if( eventWarpSendFlag == 0
	 && eventEnemySendFlag == 0
	 && sendEnFlag == 0 )
	{
		if( fieldBtnPushTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )// �B������
		{
			lssproto_AC_send( sockfd, nowGx, nowGy, i );
			setPcAction( i );
			fieldBtnPushTime = GetTickCount();
		}
	}
}
