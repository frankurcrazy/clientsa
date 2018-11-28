/************************/
/*	battleProc.cpp		*/
/************************/
#include "system.h"
#include "ime_sa.h"
#include "map.h"
#include "menu.h"
#include "battlemap.h"
#include "battleProc.h"
#include "battleMenu.h"
#include "battleMap.h"
#include "produce.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "t_music.h"
#include "pc.h"
#include "character.h"
#include "handletime.h"
#include "savedata.h"

extern	ACTION* oft_test();
extern void damage_num( ACTION *a0 );
extern void WindowDisp( ACTION *a0 );
extern int piyo_point;
extern void set_bc( void );
extern void disp_kanji(ACTION *a0);

int flash_pal_cnt;
PALETTEENTRY	Palette2[256];	// �ѥ�åȘ�����

// ���󥫥���ȥե饰
BOOL EncountFlag = FALSE;
// �ǥ奨��ե饰
BOOL DuelFlag = FALSE;
// �إ�ןo���Εr
BOOL NoHelpFlag = FALSE;
// ���󥫥���ȥ��եե饰
BOOL EncountOffFlag = FALSE;

// һ���ä������ä��ե饰
BOOL Battle1P2PFlag;

// ��������ʾ������ɥ�
ACTION* pActSurprisalWnd;
ACTION* pActAudienceExitWnd;

// ȫ������å��I��
extern int check_all_dead( void );

/* ���L���� *********************************************************/
void battle_quake( void )
{
	switch(quake_vct_no){
	case 0:		//�I��o��
		break;
	case 1:		//�����_ʼ
		quake_flg = 60;
		quake_vct_no = 2;
		break;
	case 2:		//������
		if(quake_flg >= 16){
			piyo_point += 16;
		} else {
			piyo_point += quake_flg;
		}
		piyo_point &= 63;
		if(!--quake_flg){		//����K�ˤʤ�
			quake_vct_no = 0;
		}
		break;
	}
}


// ��������ʾ�I�� **************************************************************/
void SurprisalDisp( void ){
	
	// ����Ĥ����r���Ĥ��줿�r
	if( BattleBpFlag & BATTLE_BP_ENEMY_SURPRISAL || BattleBpFlag & BATTLE_BP_PLAYER_SURPRISAL ){
		// ������ɥ��o���ä���
		if( pActSurprisalWnd == NULL ){
			// ������ɥ���ʾ����������
			pActSurprisalWnd = MakeWindowDisp( 320 - 96, 240 - 48, 3, 2, NULL, 1 );
			// ������ɥ��_����
			play_se( 202, 320, 240 );
		}
		// ������ɥ����������äƤ�����
		if( pActSurprisalWnd->hp > 0 ){
			// ���������Ĥ����r������Ĥ��줿�r��
			if( BattleBpFlag & BATTLE_BP_ENEMY_SURPRISAL ){
				StockFontBuffer( pActSurprisalWnd->x + 38, pActSurprisalWnd->y + 40, FONT_PRIO_FRONT, 0, 	"�����Ĥ��줿", 0 );
			}
			// �ץ쥤��`�������Ĥ����r
			if( BattleBpFlag & BATTLE_BP_PLAYER_SURPRISAL ){
				StockFontBuffer( pActSurprisalWnd->x + 38, pActSurprisalWnd->y + 40, FONT_PRIO_FRONT, 0, 	" �����Ĥ��� ", 0 );
			}
		}
	}
}

// �Хȥ�ץ����� ���֥ץ���������
enum{
	BATTLE_SUBPROC_INIT,				// �������ڻ�
	BATTLE_SUBPROC_IN_PRODUCE,			// �����Хȥ륤���ݳ�
	BATTLE_SUBPROC_RECEIVE_BC,			// �����£����Ŵ���
	BATTLE_SUBPROC_CHAR_IN,				// �����ǈ��ݳ�
	BATTLE_SUBPROC_CMD_INPUT,			// �������ޥ������
	BATTLE_SUBPROC_RECEIVE_MOVIE,		// ������`�ө`���Ŵ���
	BATTLE_SUBPROC_MOVIE,				// ������`�ө`�I��
	BATTLE_SUBPROC_OUT_PRODUCE_INIT,	// �����K���ݳ����ڻ�
	BATTLE_SUBPROC_OUT_PRODUCE,			// �����K���ݳ�
};
	

int xxx;
/* �Хȥ�ץ������I�� ********************************************************************/
void BattleProc( void )
{
	int i;
	//static no = Rnd( 0, BATTLE_MAP_FILES - 1 );	// �����������Хȥ�ޥå�
	s_timer++;	// �ե�`�ॿ���ީ`


	xxx = 0;
	
	/* ���֥ץ��������ŤǷ�� */
	switch( SubProcNo ){
	
		case BATTLE_SUBPROC_INIT:				// ���ڻ�
		
			// ���������Ĩ��
			DeathAllAction();
			
			slow_flg = 0;
			action_inf = 0;
			/* �ԙC ���������� */
			oft_test();
			
			// ��˥�`�v�B�γ��ڻ�
			InitMenu();
			// �Хȥ��˥�`���ڻ�
			InitBattleMenu();
			// ������������ɳ��ڻ�
			//DeathAction( pActSurprisalWnd );
			pActSurprisalWnd = NULL;
			pActAudienceExitWnd = NULL;
			// �ץ��ǥ�`�����ڻ�
			ProduceInitFlag = TRUE;
			/* ����å��v�B���ڻ� */
			//InitChat();
			// �����ե��`����ȡ��
			GetKeyInputFocus( &MyChatBuffer );
			
			// �ݳ����������
			// �Хåե����ڻ�
			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			// �Хȥ�ޥå��i���z�ߤȥХȥ륵�`�ե����λ������� */
			ReadBattleMap( BattleMapNo );
			//MenuProc();				// ��˥�`�I��
			//PaletteProc();			// �ѥ�åȄI��
			ChatProc();				// ����åȄI��
			ChatBufferToFontBuffer(); // ����åȥХåե��`��ե���ȥХåե�������
			//ImeProc();				// �ɣͣ��v�B�I��
			//SortDispBuffer(); 	// ��ʾ�Хåե����`��
			// �Хå����`�ե������\�ǥ��ꥢ�`
			ClearBackSurface();	
			// �£ͣФ�Хå����`�ե����˥��å�
			PutBmp();	
			// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// �Хåե����ڻ�
			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			//�̶������ǥ奨��ʤ�
			if(DuelFlag == TRUE || eventEnemyFlag == 1){
				//�ܥ����L�£ǣ�����
				play_bgm( 6 );
			} else {
				//ͨ�����L�£ǣ�����
				play_bgm( 5 );
			}
			// �F�ڤΕr�g��ӛ��
			NowTime = GetTickCount();
			// �Хå��Хåե��`�軭�������
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// �ݳ���
			DrawProduce( PRODUCE_HAGARE_OCHI_IN );
			
			// ����o������
			MenuProc();				// ��˥�`�I��
			ImeProc();				// �ɣͣ��v�B�I��
			
			// ��˥�`�ե饰�ϣ� 
			battleMenuFlag2 = TRUE;
			
			// ���ʤ��ڥåȥ����å�
			for( i = 0 ; i < 5 ; i++ ){
				// ���Ιڤ˥ڥåȤ����ʤ��r
				if( pet[ i ].useFlag == FALSE ){
					// �Хȥ����
					if( i == pc.battlePetNo ) pc.battlePetNo = -1;
					// ��`��Εr
					if( i == pc.mailPetNo ) pc.mailPetNo = -1;
					// �Ȥꤢ������܊�ˤ���
					pc.selectPetNo[ i ] = FALSE;
					
				}
#if 0
				// �Хȥ�ΤȤ����ĥ�`��Εr���饰�Ǥʤ�Ȥ������룩
				if( pc.battlePetNo == i && pc.mailPetNo == i ){
					// ��`��ʤ��ˤ���
					pc.mailPetNo = -1;
					// �Ȥꤢ�������C�ˤ���
					pc.selectPetNo[ i ] = TRUE;
				}
#endif
			}
			// �ѥ��`�����`��
			saveUserSetting();
			
			// �ץ��������ťץ饹
			SubProcNo++;
			break;
			
		case BATTLE_SUBPROC_IN_PRODUCE:			// �Хȥ륤���ݳ�
		
			// �ݳ���
			if( DrawProduce( PRODUCE_HAGARE_OCHI_IN ) == TRUE ){
				// �Хåե����ڻ�
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// �Хȥ�ޥå��i���z�ߤȥХȥ륵�`�ե����λ������� */
				ReadBattleMap( BattleMapNo );
				//no++;
				//if( no >= BATTLE_MAP_FILES ) no = 0;
				//no = Rnd( 0, BATTLE_MAP_FILES - 1 );
				// �Хå����`�ե������\�ǥ��ꥢ�`
				ClearBackSurface();	
				//SortDispBuffer(); 	// ��ʾ�Хåե����`��
				// �£ͣФ�Хå����`�ե����˥��å�
				PutBmp();
				// �Хȥ륵�`�ե�������ڻ�
				//ClearSurface( lpBattleSurface );
				// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
				lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
				// �Хåե����ڻ�
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// �F�ڤΕr�g��ӛ��
				NowTime = GetTickCount();
				
				// �Хå��Хåե��`�軭�������
				BackBufferDrawType = DRAW_BACK_BATTLE; 
				
				// �Хå��Хåե��`���褯
				/* ����åȄI�� */
				ChatProc();
				// ����åȥХåե��`��ե���ȥХåե�������
				ChatBufferToFontBuffer(); 
				// ���`�ܩ`�ɥ��`������I��
				FlashKeyboardCursor();
				// ��˥�`�I��
				MenuProc();
				// �ɣͣ��v�B�I��
				ImeProc();
				// �ѥ�åȄI��
				//PaletteProc();
				
				SubProcNo++;
			}else{
				MenuProc();	// ��˥�`�I��
				ImeProc();	// �ɣͣ��v�B�I��
			}
		
			break;
			
		case BATTLE_SUBPROC_RECEIVE_BC:			// �£����Ŵ���
		
#ifdef _DEBUG
			if(offlineFlag == TRUE){
				strcpy(BattleStatus,"BC|3|"
				"0|ishikawa|title|18768|1|20|20|0|"
				"1|ishikawa|title|18768|1|20|20|0|"
				"2|ishikawa|title|18768|1|20|20|0|"
				"3|ishikawa|title|18768|1|20|20|0|"
				"4|ishikawa|title|18768|1|20|20|0|"
				"5|beron|title|18813|1|20|20|0|"
				"6|beron|title|18813|1|20|20|0|"
				"7|beron|title|18813|1|20|20|0|"
				"8|beron|title|18813|1|20|20|0|"
				"9|beron|title|18813|1|20|20|0|"
				"A|ishikawa|title|18813|1|20|20|0|"
				"B|ishikawa|title|18813|1|20|20|0|"
				"C|ishikawa|title|18813|1|20|20|0|"
				"D|ishikawa|title|18813|1|20|20|0|"
				"E|ishikawa|title|18813|1|20|20|0|"
				"F|beron|title|187BA|1|20|20|0|"
				"10|beron|title|187BA|1|20|20|0|"
				"11|beron|title|187BA|1|20|20|0|"
				"12|beron|title|187BA|1|20|20|0|"
				"13|beron|title|187BA|1|20|20|0|"
				);

	strcpy(BattleStatus,"BC|0|0|����犤�ʮ����||186F7|1F|B6|B6|5|F|���L�Υ����Ԥ�||18786|20|11D|11D|1|10|����||187B2|1D|F8|F8|1|");
//	strcpy(BattleStatus,"BC|0|0|�����å�|Ŀ�˥��󥾵�|186DF|E|2A8|2BC|4|F|���å���||187B6|F|7D|7D|0|10|���å���||187B6|10|1|91|0|");

				// �£��O��
				set_bc();
				SubProcNo++;
				break;
			}
#endif
			
			/* ����åȄI�� */
			ChatProc();
			// ����åȥХåե��`��ե���ȥХåե�������
			ChatBufferToFontBuffer(); 
			// ���`�ܩ`�ɥ��`������I��
			FlashKeyboardCursor();
			// ��˥�`�I��
			MenuProc();
			// �ɣͣ��v�B�I��
			ImeProc();
			// �r�g���I���ȥѥ�åȥ����󥸄I��
			TimeZoneProc();
			
			// �Α�Εr
			if( BattleBpFlag & BATTLE_BP_JOIN ){
				// һ�Х���ե�
				strcpy( OneLineInfoStr,"���`��K�˴�����");
			}
				
			// �����`����Խ��ϥ���������ߤ餻��
			if( EncountFlag == FALSE ){
				// �Υ饹���`�������`��
				if( !( s_timer & 7 ) ){
					piyo_point++;
					piyo_point &= 63;
				}
				/* ����������ߤ餻�� */
				RunAction();
				// ��������ʾ�ǩ`����Хåե�������
				StockTaskDispBuffer();
			}
			
			// ���`�Щ`�Ώ�����
#if 0
			if( BattleStatusBak[ 0 ] != NULL ){
				strcpy( BattleStatus, BattleStatusBak );
				BattleStatusBak[ 0 ] = NULL;
				// ���󥫥���ȥե饰���ڻ�
				EncountFlag = FALSE;
				// �£��O��
				set_bc();
				// һ���ä������ä������å�
				Battle1P2PFlag = CheckBattle1P2P();
				SubProcNo++;

				//�Q���`�ɤʤ�
//				if(xxx == 0 && BattleMyNo >= 20){
//					BattleCmdBak[ 0 ] = NULL;
//					xxx = 1;
//				}
#else
			if( BattleStatusReadPointer != BattleStatusWritePointer ){
				strcpy( BattleStatus, BattleStatusBak[ BattleStatusReadPointer ] );
				BattleStatusReadPointer = ( BattleStatusReadPointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
				// ���󥫥���ȥե饰���ڻ�
				EncountFlag = FALSE;
				// �£��O��
				set_bc();
				// ���L�Ф���Ҏ�ڥåȥ����å�
				CheckBattleNewPet();
				// �Q���`�ɤǤʤ��r
				if( BattleMyNo < 20 ){
					// �ͣФ��O��
					p_party[ BattleMyNo ]->mp = BattleMyMp;
				}
				
				// һ���ä������ä������å�
				Battle1P2PFlag = CheckBattle1P2P();
				SubProcNo++;

				//�Q���`�ɤʤ�
//				if(xxx == 0 && BattleMyNo >= 20){
//					BattleCmdBak[ 0 ] = NULL;
//					xxx = 1;
//				}
#endif

			}
			// �Q��r���ҥ���å����줿�r
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// �Q��K�˥ץ��ȥ����ͤ�
			//	lssproto_B_send( sockfd, "U" );
			//}
			// �Q��Εr��������ɥ��α�ʾ
			if( BattleMyNo >= 20 ){
				// ������ɥ��o���ä���
				if( pActAudienceExitWnd == NULL ){
					// ������ɥ���ʾ����������
					pActAudienceExitWnd = MakeWindowDisp( 444, 4, 3, 2, NULL, 1 );
					// ������ɥ��_����
					play_se( 202, 320, 240 );
				}
			}
			
			break;
			
		case BATTLE_SUBPROC_CHAR_IN:			// �ǈ��ݳ�
			
			// �Υ饹���`�������`��
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			// ��������ʾ�I��
			SurprisalDisp();
			
			// ���ޥ�������g�ߥե饰�Υ����å�
			CheckBattleAnimFlag();
			/* ����������ߤ餻�� */
			RunAction();
			// ��������ʾ�ǩ`����Хåե�������
			StockTaskDispBuffer();
			/* ����åȄI�� */
			ChatProc();
			// ����åȥХåե��`��ե���ȥХåե�������
			ChatBufferToFontBuffer(); 
			// ���`�ܩ`�ɥ��`������I��
			FlashKeyboardCursor();
			// ��˥�`�I��
			MenuProc();
			// �ɣͣ��v�B�I��
			ImeProc();
			// �r�g���I���ȥѥ�åȥ����󥸄I��
			TimeZoneProc();
			
			// �ǈ��K�ä���
			if( action_inf == 3 ){
				action_inf = 0;
				// ���L�Α�ե饰�ϣƣ�
				BattleBpFlag &= ~BATTLE_BP_JOIN; 
				// ȫ������å��I��
				if( check_all_dead() == 1 ){
					SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;
					break;
				}
				// ������������ɥ����Ƥ�����
				if( pActSurprisalWnd != NULL ){
					// ������ɥ�Ĩ��
					DeathAction( pActSurprisalWnd );
					pActSurprisalWnd = NULL;
					// �������ե饰���ڻ�
					BattleBpFlag &= ~BATTLE_BP_ENEMY_SURPRISAL; 
					BattleBpFlag &= ~BATTLE_BP_PLAYER_SURPRISAL; 
					
				}
				// �ΤΥץ�������
				SubProcNo++;
			}
			//���L�ѳ��ʤ�  ս�����ѵĻ�
			if( action_inf == 2 ){		
				SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;		//���L�K���ݳ���
				action_inf = 0;
				// �Хȥ륳�ޥ�ɳ��ڻ�
				BattleCmd[ 0 ] = NULL;
			}
			// �Q��r���ҥ���å����줿�r
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// �Q��K�˥ץ��ȥ����ͤ�
			//	lssproto_B_send( sockfd, "U" );
			//}
			
			break;
			
		case BATTLE_SUBPROC_CMD_INPUT:			// ���ޥ������
		
#ifdef _DEBUG
			if(offlineFlag == TRUE){
				SubProcNo++;
				break;
			}
#endif
			// �Υ饹���`�������`��
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			
			// �Хȥ��˥�`�I��  
			BattleMenuProc();
			// ���ޥ�������g�ߥե饰�Υ����å�
			CheckBattleAnimFlag();
			
			/* ����������ߤ餻�� */
			RunAction();
			// ��������ʾ�ǩ`����Хåե�������
			StockTaskDispBuffer();
			
			/* ����åȄI�� */
			ChatProc();
			// ����åȥХåե��`��ե���ȥХåե�������
			ChatBufferToFontBuffer(); 
			// ���`�ܩ`�ɥ��`������I��
			FlashKeyboardCursor();
			// ��˥�`�I��
			MenuProc();
			// �ɣͣ��v�B�I��
			ImeProc();
			// �r�g���I���ȥѥ�åȥ����󥸄I��
			TimeZoneProc();
			
			// �Q��r���ҥ���å����줿�r
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// �Q��K�˥ץ��ȥ����ͤ�
			//	lssproto_B_send( sockfd, "U" );
			//}
			
			break;
			
		case BATTLE_SUBPROC_RECEIVE_MOVIE:		// ��`�ө`���Ŵ���
		
#ifdef _DEBUG
			if(offlineFlag == TRUE){
//				strcpy( BattleCmd,"BH|aCr0|f2|d10E|rC|f11|counter156|FF|bg|b|BH|aEr0|f2|dDE|rE|f30|counter0|FF|BH|a"
//				"Dr0|f2|d17E|FF|BE|e0|f1|");

//				strcpy( BattleCmd,"BS jibun_2 flg_0 BS jibun_2 flg_1 pet_186A2");
#if 0
				strcpy( BattleCmd,"BO jibun_0 teki_13 flg_0 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_1 teki_13 flg_8 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_2 teki_13 flg_20 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_3 teki_13 flg_4 damage_4 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				" BO jibun_4 teki_13 flg_0 damage_0 teki_11 flg_0 damage_4 teki_F flg_0 damage_4 teki_10 flg_0 damage_4 teki_12 flg_0 damage_4 FF"
				                 " BO jibun_0 teki_E flg_0 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_1 teki_E flg_8 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_2 teki_E flg_20 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_3 teki_E flg_4 damage_4 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				" BO jibun_4 teki_E flg_0 damage_0 teki_C flg_0 damage_4 teki_A flg_0 damage_4 teki_B flg_0 damage_4 teki_D flg_0 damage_4 FF"
				                 " BO jibun_0 teki_9 flg_0 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_1 teki_9 flg_8 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_2 teki_9 flg_20 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_3 teki_9 flg_4 damage_4 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
				" BO jibun_4 teki_9 flg_0 damage_0 teki_7 flg_0 damage_4 teki_5 flg_0 damage_4 teki_6 flg_0 damage_4 teki_8 flg_0 damage_4 FF"
//				                 " BO jibun_0 teki_4 flg_0 damage_4 teki_2 flg_0 damage_4                       teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_1 teki_4 flg_8 damage_4 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_2 teki_4 flg_20 damage_4 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_3 teki_4 flg_4 damage_4 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
//				" BO jibun_4 teki_4 flg_0 damage_0 teki_2 flg_0 damage_4 teki_0 flg_0 damage_4 teki_1 flg_0 damage_4 teki_3 flg_0 damage_4 FF"
				                 " BO jibun_A teki_8 flg_0 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_B teki_8 flg_8 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_C teki_8 flg_20 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_D teki_8 flg_4 damage_4 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				" BO jibun_E teki_8 flg_0 damage_0 teki_6 flg_0 damage_4 teki_5 flg_0 damage_4 teki_7 flg_0 damage_4 teki_9 flg_0 damage_4 FF"
				                 " BO jibun_A teki_3 flg_0 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_B teki_3 flg_8 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_C teki_3 flg_20 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_D teki_3 flg_4 damage_4 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				" BO jibun_E teki_3 flg_0 damage_0 teki_1 flg_0 damage_4 teki_0 flg_0 damage_4 teki_2 flg_0 damage_4 teki_4 flg_0 damage_4 FF"
				                 " BO jibun_A teki_12 flg_0 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_B teki_12 flg_8 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_C teki_12 flg_20 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_D teki_12 flg_4 damage_4 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF"
				" BO jibun_E teki_12 flg_0 damage_0 teki_10 flg_0 damage_4 teki_F flg_0 damage_4 teki_11 flg_0 damage_4 teki_13 flg_0 damage_4 FF");
#endif
#if 1
				strcpy( BattleCmd,"BY teki_A jibun_0 flg_0 damage_0 jibun_1 flg_0 damage_0 jibun_2 flg_0 damage_0"
				" jibun_3 flg_0 damage_0 jibun_4 flg_0 damage_0 jibun_5 flg_0 damage_0"
				" jibun_6 flg_0 damage_0 jibun_7 flg_0 damage_0 jibun_8 flg_0 damage_0"
				" jibun_9 flg_0 damage_1 FF");

		strcpy( BattleCmd,"bn|f|BH|a0|rF|f20|d0|r0|f10|counter33|FF|BH|a10|r0|f2|d1|FF|");
//		strcpy( BattleCmd,"BH|a0|rF|fC|d18|FF|BH|a10|r0|f20|d0|r10|f31|counter1|FF|bg|f|");
#endif
				BattleMyNo = 1;
				SubProcNo++;
				break;
			}
#endif

			// �Υ饹���`�������`��
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			// ���ޥ�������g�ߥե饰�Υ����å�
			CheckBattleAnimFlag();
			/* ����������ߤ餻�� */
			RunAction();
			// ��������ʾ�ǩ`����Хåե�������
			StockTaskDispBuffer();

			/* ����åȄI�� */
			ChatProc();
			// ����åȥХåե��`��ե���ȥХåե�������
			ChatBufferToFontBuffer(); 
			// ���`�ܩ`�ɥ��`������I��
			FlashKeyboardCursor();
			// ��˥�`�I��
			MenuProc();
			// �ɣͣ��v�B�I��
			ImeProc();
			// �r�g���I���ȥѥ�åȥ����󥸄I��
			TimeZoneProc();
			
			// ���`�Щ`�Ώ�����
#if 0
			if( BattleCmdBak[ 0 ] != NULL ){
				strcpy( BattleCmd, BattleCmdBak );
				BattleCmdBak[ 0 ] = NULL;
				action_inf = 0;
				SubProcNo++;
			}
#else
			if( BattleCmdReadPointer != BattleCmdWritePointer ){
				strcpy( BattleCmd, BattleCmdBak[ BattleCmdReadPointer ] );
				BattleCmdReadPointer = ( BattleCmdReadPointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
				action_inf = 0;
				SubProcNo++;
			}
#endif
			// �����äΕr
			if( Battle1P2PFlag == 2 ){
				// һ�Х���ե�
				strcpy( OneLineInfoStr,"���Υץ쥤��`������������");
			}
			// �Q��Εr
			if( BattleMyNo >= 20 ){
				// �����֥�`��
				for( i = 0 ; i < 20 ; i++ ){
					// �;�����`���`��ʾ
					HpMeterDisp( i );
				}
			}else{
				// ���¥���`�פΕr
				if( BattleMyNo < 10 ){
					for( i  = 0 ; i < 10 ; i++ ) HpMeterDisp( i ); // �;�����`���`��ʾ
				}else 
				// ���¥���`�פΕr
				if( BattleMyNo >= 10 ){ 
					for( i = 10 ; i < 20 ; i++ ) HpMeterDisp( i ); // �;�����`���`��ʾ
				}
			}
			
			// �Q��r���ҥ���å����줿�r
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// �Q��K�˥ץ��ȥ����ͤ�
			//	lssproto_B_send( sockfd, "U" );
			//}
			break;
			
		case BATTLE_SUBPROC_MOVIE:				// ��`�ө`�I��
		
#ifdef _DEBUG
			// �����֥�`��
			//for( i = 0 ; i < 20 ; i++ ){
				// �;�����`���`��ʾ
			//	HpMeterDisp( i );
			//}
			if(offlineFlag == TRUE){
				if(joy_trg[0]&JOY_A){		//�ꥻ�åȤʤ�
					DeathAllAction();
					SubProcNo = 0;
					BattleCmd[0] = NULL;
					break;
				}
			}
#endif
			//�����`�Фʤ飨�����Εr��
			if( slow_flg ){		
				if( !( s_timer & 31 ) ){	// ���ե�`���һ��
					piyo_point++;	// �饹���`�������`���M�ޤ���
					piyo_point &= 63;	// ��ߥåȥ����å�
				}
				if( s_timer & 3 ){		// ���ե�`���һ�أ������Ϥ�����ʣ�������
					damage_dispx();		// �����`�ФΥ���`������ʾ
				}else{
					RunAction();		/* ����������ߤ餻�� */
					battle_quake();		//���L����
				}
			} else { // �����`�Ǥʤ��r
				if( !( s_timer & 7 ) ){	
					piyo_point++;
					piyo_point &= 63;
				}
				RunAction();		/* ����������ߤ餻�� */
				battle_quake();		//���L����
			}
			
			//�����ե�å���ʤ�
			if( flash_vct_no ) die_flash();	//�����ե�å���
			
			// ��������ʾ�ǩ`����Хåե�������
			StockTaskDispBuffer();

			/* ����åȄI�� */
			ChatProc();
			// ����åȥХåե��`��ե���ȥХåե�������
			ChatBufferToFontBuffer(); 
			// ���`�ܩ`�ɥ��`������I��
			FlashKeyboardCursor();
			// ��˥�`�I��
			MenuProc();
			// �ɣͣ��v�B�I��
			ImeProc();
			// �r�g���I���ȥѥ�åȥ����󥸄I��
			TimeZoneProc();
			//���`��K�ˤʤ�
			if( action_inf == 1 ){		
				//if( BattleBpFlag & BATTLE_BP_JOIN ){	// �Α�Εr
				//	BattleBpFlag &= ~BATTLE_BP_JOIN;
				//	SubProcNo = BATTLE_SUBPROC_CMD_INPUT;		//��˥�`�ˑ���
				//}else{
					SubProcNo = BATTLE_SUBPROC_RECEIVE_BC;		//�£äˑ���
					BattleCliTurnNo++;							// ���饤����ȥ��`���M���
				//}
#ifdef _DEBUG
				if(offlineFlag == TRUE){
					SubProcNo = BATTLE_SUBPROC_RECEIVE_BC;		//��˥�`�ˑ���ʤ�
				}
				
#endif
				action_inf = 0;
				// �Хȥ륳�ޥ�ɳ��ڻ�
				BattleCmd[ 0 ] = NULL;
				// ��˥�`�ե饰�ϣ� 
				battleMenuFlag2 = TRUE;
			}
			
			//���L�ѳ��ʤ�
			if( action_inf == 2 ){		
				SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;		//���L�K���ݳ���
				action_inf = 0;
				// �Хȥ륳�ޥ�ɳ��ڻ�
				BattleCmd[ 0 ] = NULL;
			}
			
			// �Q��r���ҥ���å����줿�r
			//if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
				// �Q��K�˥ץ��ȥ����ͤ�
			//	lssproto_B_send( sockfd, "U" );
			//}
			break;
			
		case BATTLE_SUBPROC_OUT_PRODUCE_INIT:	// �K���ݳ����ڻ�
			
			//�£ǣͥե��`�ɥ�����
			fade_out_bgm();
			// �Хȥ륵�`�ե����λ������� 
			CopyBackBuffer();
			// �Хå��Хåե��`�軭�������
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
			break;
			
		case BATTLE_SUBPROC_OUT_PRODUCE:		// �K���ݳ�
			
			// �ݳ���
			if( DrawProduce( PRODUCE_4WAY_OUT ) == TRUE ){
				// �ץ�����������
				ChangeProc( PROC_GAME, 1 );
				// ���������ȫĨ��
				DeathAllAction();
				// ���������Ʃ`�֥�Υ���������Ĩ��
				clearPtActCharObj();
				// �����ᤫ��Σá��ã��ץ��ȥ�����ܤ�������
				encountNowFlag = 0;
				// ���饤����Ȥ�����Υ�`�ө`��Ҋ�K��ä��Ȥ����ͤ롣
				lssproto_EO_send( sockfd, 0 );
				//�ե��`��ɤΣ£ǣ�����
//				play_bgm(map_bgm);		//�ե��`��ɣ£ǣ�����
			//	SubProcNo++;
				// ���L�Y��������ɥ������r���Ӥ����r���������åȤΕr�����ʤ�
				if( battleResultMsg.useFlag >= 1 ){ 
					BattleResultWndFlag = battleResultMsg.useFlag;	// ��˥�`�ե饰�ϣ�
					battleResultMsg.useFlag = FALSE; // ������Υե饰���ڻ�
				}
				return;
			}
			break;
			
		case 12:	// �ݳ�
			break;
	}
	
	// �ץ��ǥ�`���ФǤʤ��Ȥ�  �ݳ���û�е�ʱ��
	if( BackBufferDrawType != DRAW_BACK_PRODUCE ){ 
		// ���L���ƽK�ˤΕr
		if( BattleEscFlag == TRUE ){	
			// �K�˥ץ�������
			SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;
			// �ե饰���ڻ�
			BattleEscFlag = FALSE;
		}
		// �Q��r���ҥ���å����줿�r
		if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
			// �Q��K�˥ץ��ȥ����ͤ�
			lssproto_B_send( sockfd, "U" );
		}
		// ������ɥ�����r
		if( pActAudienceExitWnd != NULL ){
			// �Q��K�����ֱ�ʾ������ɥ������������äƤ�����
			if( pActAudienceExitWnd->hp > 0 ){
				// ���֤��ʾ
				StockFontBuffer( pActAudienceExitWnd->x + 30, pActAudienceExitWnd->y + 28, FONT_PRIO_FRONT, 0, 	"�ҥ���å���", 0 );
				StockFontBuffer( pActAudienceExitWnd->x + 30, pActAudienceExitWnd->y + 52, FONT_PRIO_FRONT, 0, 	"�Q���K�ˤ��ޤ�", 0 );
				//StockFontBuffer( pActInfoWnd->x + 38, pActInfoWnd->y + 28, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	"���`���åȤ�", 0 );
				//StockFontBuffer( pActInfoWnd->x + 38, pActInfoWnd->y + 52, FONT_PRIO_FRONT, FONT_PAL_YELLOW, 	"�x�k�����¤���", 0 );
			}
		}
	}
}
