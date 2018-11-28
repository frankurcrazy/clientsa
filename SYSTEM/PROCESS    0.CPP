/************************/
/*	process.c		������Ҫ��������	*/
/************************/
#include "system.h"
#include "../ohta/ohta.h"
#include "../dwaf/dwafTestProc.h"
#include "ime_sa.h"
#include "menu.h"
#include "pc.h"
#include "character.h"
#include "login.h"
#include "netproc.h"
#include "savedata.h"
#include "testView.h"
#include "battleProc.h"
#include "produce.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "battleMenu.h"
#include "t_music.h"
#include "field.h"
#include "handletime.h"

/* ��������� *******************************************************************/
extern void kakushi_command(void);
/* ���̺� */
UINT ProcNo;
/* �ӽ��� */
UINT SubProcNo;

int palNo;//�ֵ�
int oldPalNo;

/* �v���Z�X���� ********************************************************************/
void Process( void )
{
	/* �v���Z�X���� */
	switch( ProcNo ){
	
		case PROC_OPENNING:
		
			break;
			
		case PROC_INIT: 	/* �������v���Z�X */
		
			InitProc();
			
			break;
			

		// ����֤�����봦��
		case PROC_ID_PASSWORD:
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			idPasswordProc();
			RunAction();			// ִ�ж���
			StockTaskDispBuffer();	// ��ʾ���ݻ�����������
			FlashKeyboardCursor();	// ���������˸����/ /
			ImeProc();				// ���뷨�йصĴ���
			break;

		// ����Ͳ˵�ѡ��
		case PROC_TITLE_MENU:
			// /�ı����˻�����
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			titleProc();
			TitleProduce();			// ְ��/ 
			RunAction();			// /ִ�ж���
			StockTaskDispBuffer();	// ��ʾ���ݻ�����������
			kakushi_command();		//���ص������
			break;


		// ����ѡ��
		case PROC_CHAR_SELECT:		// ����ѡ��
			// ���ĺ����Ⱦ����
			BackBufferDrawType = DRAW_BACK_NORMAL;// ��׼
			selectCharacterProc();
			break;

		// ��������
		case PROC_CHAR_MAKE:
			// ��Ⱦ����
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			makeCharacterProc();
			break;

		// ��ʼ��½ѡ��������
		case PROC_CHAR_LOGIN_START:
			// ת������
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			// charLoginStart();�����δ������ϵ����ݱ��ݻ�����
			//��ʼ���Ա�����
			initMap();		// ��ͼ
			initPcAll();	// PC�ĳ�ʼ����Ϣ�����
			initCharObj();	// ��ʼ�����������
			initOpenServerWindow();	// ��ʼ���Ĵ�������ʾ����������·
			initFieldProc();		// ���ڳ�ʼ��
			initMapEffect();		// ��ͼ��ʼ����Ӱ��
			EncountFlag = FALSE;
			logOutFlag = FALSE;
			InitOhtaParam();	// ��������
			
			ChangeProc( PROC_CHAR_LOGIN );
			// �a�f�l�t�F�[�h�A�E�g�J�n
			fade_out_bgm();//�˳�GM��
			break;

		case PROC_CHAR_LOGIN:
			// �o�b�N�o�b�t�@�[�`����@�ύX
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLoginProc();
			break;

		// ���O�A�E�g
		case PROC_CHAR_LOGOUT:
			// ��־���
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLogoutProc();
			break;


		case PROC_GAME:     /*���̵���Ϸ */
		
			GameProc();
			break;

		case PROC_DISCONNECT_SERVER://�Ͽ���Ϸ��
			switch( SubProcNo )
			{
				case 0:
					// ������
					// PC���Z�b�g  / /��ʼ��/ /���Ը�λ
					resetPc();
					// ���������
					initCharObj();
					// ȡ������
					DeathAllAction();


					//  /��ʼ������
					ProduceInitFlag = TRUE;
					// ���Ʊ�������ͼ��
					CopyBackBuffer();
					// ��Ⱦ����
					BackBufferDrawType = DRAW_BACK_PRODUCE; 
					// ��������
					fade_out_bgm();
					// ��¼��ǰʱ��
					NowTime = GetTickCount();
					SubProcNo++;

					break;

				case 1:
					// ����
					if( DrawProduce( PRODUCE_DOWN_ACCELE ) == TRUE )
					{
						BackBufferDrawType = DRAW_BACK_NORMAL; 
						SubProcNo++;
					}
					break;

				case 2:
					// ����
					break;
			}
			// �Ͽ�����Windows ����
			if( disconnectServer() )
			{
				// ��ʼ������
				cleanupNetwork();
				// �ָ�Ĭ�ϵ�ɫ��
				PaletteChange( DEF_PAL, 0 );
				// �����⻭��
				ChangeProc( PROC_TITLE_MENU );
				// /�����ж�
				DeathAllAction();
				disconnectServerFlag = FALSE;
				oldDisconnectServerFlag = FALSE;
				break;
			}
			RunAction();			// �A�N�V�������点��
			StockTaskDispBuffer();	// �^�X�N�\���f�[�^���o�b�t�@�ɗ��߂�
			break;


		case PROC_BATTLE:     /* �o�g���v���Z�X */
		
			BattleProc();
			break;
			
#ifdef _DEBUG		
		//case PROC_OHTA_TEST:     /* ���c�e�X�g�v���Z�X */
		
		//	OhtaTestProc();
		//	break;
			
		//case PROC_TAKE_TEST:     /* �|������e�X�g�v���Z�X */
		
		//	TakeTestProc();
		//	break;

		//case PROC_DWAF_TEST:	// DWAF�e�X�g
		//	dwafTestProc();
		//	break;

		case PROC_SPR_VIEW:		// ��ʼ��֤
			SprViewProc();
			break;
			
		case PROC_ANIM_VIEW:	// ������֤
			AnimViewProc();
			break;
			
		case PROC_SE_TEST:	//  SE��������
			SeTestProc();
			break;
#endif
		case PROC_ENDING:
			
			break;
			
	}
}

/* �v���Z�X�ύX���� ***********************************************************/
void ChangeProc( int procNo )
{
	// �v���Z�X�ԍ����L��
	ProcNo = procNo;
	// �T�u�v���Z�X�ԍ��̏�����
	SubProcNo = 0;
}
/* �v���Z�X�ύX���� ***********************************************************/
void ChangeProc( int procNo, int subProcNo )
{
	// �v���Z�X�ԍ����L��
	ProcNo = procNo;
	// �T�u�v���Z�X�ԍ��̏�����
	SubProcNo = subProcNo;
}

/* �Q�[���v���Z�X���� ********************************************************************/
void GameProc( void )
{
	static int now_bgm;
	static BOOL produceFlag;	// ���o�t���O

	/* �T�u�v���Z�X�ԍ��ŕ��� */
	switch( SubProcNo ){
	
		case 0:
#ifdef _DEBUG
			// �I�t���C�����[�h�̎��̃}�b�v������
			if( offlineFlag )
			{
				initMap();		// �}�b�v������
			}
#endif

			//InitIme();		// �h�l�d�֘A�̏�����
			ClrImeCompositionString(); // �E�B���h�E�Y�̂h�l�d�o�b�t�@�[���N���A����
			InitChat();		/* �`���b�g�֘A������ */
			// ���o�ԍ������_���i�����̂݁j
			//ProduceNo = Rnd( 0, PRODUCE_END - 1 ) & 0xfffffffe;
			produceFlag = FALSE;	// ���o�t���O�n�e�e
			EncountFlag = FALSE;	// �G���J�E���g�t���O�n�e�e
			//�t�B�[���h�a�f�l�Đ��J�n
//			play_bgm(0);
			// �A���o���̓ǂݍ���
			LoadAlbum();
			
			SubProcNo++;

		// ���O�C�����o
		case 150:
			SubProcNo = 100;

		case 100:
			initPc();				// �v���C���[�L�����쐬
			resetFieldProc();		// �t�B�[���h�����̃��Z�b�g
			restorePtActCharObjAll();	// �L�����Ǘ��e�[�u���̃A�N�V��������
			initOpenServerWindow();	// �T�[�o�̎w���ŕ\������E�B���h�E�̏�����
			InitMenu();		// ���j���[�֘A�̏�����
//			BattleStatusBak[ 0 ] = NULL;// �o�g����ԏ�����
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// ���̓t�H�[�J�X�擾
			GetKeyInputFocus( &MyChatBuffer );
			SubProcNo++;

		case 101:
			if( loginFlag )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				break;
			}
			SubProcNo++;

		case 102:
			// ���o�摜�̍쐬
			// �o�b�t�@������
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// �L�����E�p�[�c�̗D�揇�ʌ��菈���̏�����
			RunAction();			// �A�N�V�������点��
			StockTaskDispBuffer();	// �^�X�N�\���f�[�^���o�b�t�@�ɗ��߂�
			stop_bgm();				//�a�f�l��~
			drawMap();				//	�}�b�v�\��
			MenuProc();				// ���j���[����
			// ��s�C���t�H���h��Ԃ��i���F�j
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// �`���b�g����
			ChatBufferToFontBuffer(); // �`���b�g�o�b�t�@�[���t�H���g�o�b�t�@�ɗ��߂�
			ImeProc();				// �h�l�d�֘A����
			SortDispBuffer(); 	// �\���o�b�t�@�\�[�g
			// �o�b�N�T�[�t�F�X�����ŃN���A�[
			ClearBackSurface();	
			// �a�l�o���o�b�N�T�[�t�F�X�ɃZ�b�g
			PutBmp();	
			// �o�b�N�T�[�t�F�X����o�g���T�[�t�F�X�փR�s�[
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// �o�b�t�@������
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

			play_bgm( map_bgm_no );

			// ���݂̎��Ԃ��L��
			NowTime = GetTickCount();
			// �o�b�N�o�b�t�@�[�`����@�ύX
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// ���o��
			ProduceInitFlag = TRUE;
			DrawProduce( PRODUCE_BRAN_BIG );
			SubProcNo++;
			break;

		case 103:
			// ���o��
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_BRAN_BIG ) == TRUE )
			{
				SubProcNo = 3;
				produceFlag = TRUE;
				fieldInfoTime = GetTickCount();
			}
			break;


		// ���[�v���o
		case 200:
			// �a�f�l�t�F�[�h�A�E�g�J�n
//			fade_out_bgm();

			// S�v���g�R����C�� warpEffectProc();���Ă΂��
			// ��ʂ����̂ł����ł͂��Ȃ��ėǂ�
			SubProcNo++;

		case 201:
			// ���o��
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSIN ) == TRUE )
			{
				SubProcNo++;
			}
			drawField();	// �t�B�[���h��ʂɂ��낢��\��
			MenuProc();	// ���j���[����
			ImeProc();	// �h�l�d�֘A����
			break;

		case 202:
			if( !warpEffectStart || !warpEffectOk )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				drawField();	// �t�B�[���h��ʂɂ��낢��\��
				MenuProc();	// ���j���[����
				ImeProc();	// �h�l�d�֘A����
				break;
			}
			warpEffectStart = FALSE;
			warpEffectOk = FALSE;
			SubProcNo++;

		case 203:
			// ���o�摜�̍쐬
			// �o�b�t�@������
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// �L�����E�p�[�c�̗D�揇�ʌ��菈���̏�����
			RunAction();			// �A�N�V�������点��
			StockTaskDispBuffer();	// �^�X�N�\���f�[�^���o�b�t�@�ɗ��߂�
			redrawMap();
			drawMap();				//	�}�b�v�\��

#if 1
			// �G�t�F�N�g�̃N���A
			if( (mapEffectRainLevel == 0 && oldMapEffectRainLevel != 0 )
			 || (mapEffectSnowLevel == 0 && oldMapEffectSnowLevel != 0 ) )
			{
				initMapEffect();
			}
#endif

			// ��s�C���t�H���h��Ԃ��i���F�j
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// �`���b�g����
			ChatBufferToFontBuffer(); // �`���b�g�o�b�t�@�[���t�H���g�o�b�t�@�ɗ��߂�
			SortDispBuffer(); 	// �\���o�b�t�@�\�[�g
			// �o�b�N�T�[�t�F�X�����ŃN���A�[
			ClearBackSurface();	
#if 1
			if( (mapEffectRainLevel != 0 && oldMapEffectRainLevel == 0 )
			 || (mapEffectSnowLevel != 0 && oldMapEffectSnowLevel == 0 ) )
			{
				mapEffectProc2( 80 );		// �}�b�v�G�t�F�N�g�i�J�E�ᓙ�j
			}
#endif
			// �a�l�o���o�b�N�T�[�t�F�X�ɃZ�b�g
			PutBmp();	
			// �o�b�N�T�[�t�F�X����o�g���T�[�t�F�X�փR�s�[
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// �o�b�t�@������
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

//			if( map_bgm_no != now_bgm )
//			{
//				stop_bgm();				//�a�f�l��~
//				play_bgm( map_bgm_no );
//			}

			// ���݂̎��Ԃ��L��
			NowTime = GetTickCount();

			ProduceInitFlag = TRUE;

			SubProcNo++;

		case 204:
			// ���o��
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSOUT ) == TRUE )
			{
				SubProcNo = 3;
				fieldInfoTime = GetTickCount();
			}
			drawField();	// �t�B�[���h��ʂɂ��낢��\��
			MenuProc();	// ���j���[����
			ImeProc();	// �h�l�d�֘A����
			break;


		case 1:
			initPc();				// �v���C���[�L�����쐬
			resetFieldProc();		// �t�B�[���h�����̃��Z�b�g
			restorePtActCharObjAll();	// �L�����Ǘ��e�[�u���̃A�N�V��������
			initOpenServerWindow();	// �T�[�o�̎w���ŕ\������E�B���h�E�̏�����
			InitMenu();		// ���j���[�֘A�̏�����
//			BattleStatusBak[ 0 ] = NULL;// �o�g����ԏ�����
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// ���̓t�H�[�J�X�擾
			GetKeyInputFocus( &MyChatBuffer );
			
			// ���o���鎞
			if( produceFlag == TRUE ){
			// �퓬���ʃE�B���h�E�t���O�n�m
			//if( BattleResultWndFlag == TRUE ){

				// ���o�摜�̍쐬
				// �o�b�t�@������
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				initCharPartsPrio();	// �L�����E�p�[�c�̗D�揇�ʌ��菈���̏�����
				RunAction();			// �A�N�V�������点��
				StockTaskDispBuffer();	// �^�X�N�\���f�[�^���o�b�t�@�ɗ��߂�
				stop_bgm();				//�a�f�l��~
				drawMap();				//	�}�b�v�\��
				//�a�f�l���Ȃ����
				if(!draw_map_bgm_flg){
					//�������̂a�f�l�Đ�
					play_bgm(map_bgm_no = now_bgm);
					draw_map_bgm_flg = 1;
				}
				MenuProc();				// ���j���[����
				// ��s�C���t�H���h��Ԃ��i���F�j
				//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
				ChatProc();				// �`���b�g����
				ChatBufferToFontBuffer(); // �`���b�g�o�b�t�@�[���t�H���g�o�b�t�@�ɗ��߂�
				ImeProc();				// �h�l�d�֘A����
				SortDispBuffer(); 	// �\���o�b�t�@�\�[�g
				// �o�b�N�T�[�t�F�X�����ŃN���A�[
				ClearBackSurface();	
				// �a�l�o���o�b�N�T�[�t�F�X�ɃZ�b�g
				PutBmp();	
				// �o�b�N�T�[�t�F�X����o�g���T�[�t�F�X�փR�s�[
				lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
				// �o�b�t�@������
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// ���݂̎��Ԃ��L��
				NowTime = GetTickCount();
				// �o�b�N�o�b�t�@�[�`����@�ύX
				BackBufferDrawType = DRAW_BACK_PRODUCE; 
				// ���o��
				DrawProduce( PRODUCE_4WAY_IN );
			}else{
				// ���o�t���O�n�m
				produceFlag = TRUE;
				SubProcNo++;
			}
			SubProcNo++;
			
			break;
		
		case 2:	// �t�B�[���h�C�����o
			
			// ���o��
			if( DrawProduce( PRODUCE_4WAY_IN ) == TRUE ){
				SubProcNo++;
				warpEffectStart = FALSE;
				warpEffectOk = FALSE;
			}
			break;
			
		case 3:
			// �o�b�N�o�b�t�@�[�`����@�ύX
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			
			paletteProc();
			
			initItemOverlapCheck();	// �A�C�e���d�Ȃ�`�F�b�N�����̏�����
			drawGrid();				// �O���b�h�J�[�\���\��
			fieldProc();			// �t�B�[���h�֘A����
			moveProc();				// �ړ�����
			charObjProc();			// �L�����Ǘ�����
			initCharPartsPrio();	// �L�����E�p�[�c�̗D�揇�ʌ��菈���̏�����
			addressBookProc();		// �A�h���X�u�b�N����
			openServerWindowProc();	// �T�[�o�����E�B���h�E
			drawFieldInfoWin();		// ���[�v��̏ꏊ���
			
			/* �A�N�V�������点�� */
			RunAction();
			// �^�X�N�\���f�[�^���o�b�t�@�ɗ��߂�
			// �w��͈͂̓X�g�b�N���Ȃ�
			StockTaskDispBuffer();

			mapEffectProc();		// �}�b�v�G�t�F�N�g�i�J�E�ᓙ�j

			drawMap();		// �}�b�v�\��
			drawField();	// �t�B�[���h��ʂɂ��낢��\��

			/* �`���b�g���� */
			ChatProc();
			// �`���b�g�o�b�t�@�[���t�H���g�o�b�t�@�ɗ��߂�
			ChatBufferToFontBuffer(); 
			// �L�[�{�[�h�J�[�\���_�ŏ���
			FlashKeyboardCursor();
			// ���j���[����
			MenuProc();
			// �h�l�d�֘A����
			ImeProc();		
			// ���ԑя����ƃp���b�g�`�F���W����
			TimeZoneProc();
			
#ifdef _DEBUG
			// �G���J�E���g�{�^��
			if( joy_trg[ 0 ] & JOY_P_DOWN ){
				if( pc.ptAct != NULL ){
					resetMap();					// �}�b�v�������Z�b�g
					// �G���J�E���g���M
					lssproto_EN_send( sockfd, pc.ptAct->gx, pc.ptAct->gy );
					eventEnemyFlag = 0;
				}
			}
#endif
			// �G���J�E���g������
			if( EncountFlag == TRUE ){
				resetPc();				// PC���Z�b�g
				resetCharObj();			// �L�����Ǘ��e�[�u�����Z�b�g
				resetMap();				// �}�b�v�������Z�b�g
				clearPtActPartyParam();	// ���ԏ��̃A�N�V�����|�C���^������NULL�ɂ���
				fieldInfoTime = 0;		// �ꏊ��񂪏o�Ă������
				drawFieldInfoWin();		// ���[�v��̏ꏊ���
				nowEncountPercentage = minEncountPercentage;// �G���J�E���g�����ŏ��ɖ߂�
				sendEnFlag = 0;
				encountNowFlag = 1;
				eventEnemySendFlag = 0;
				duelSendFlag = 0;
				jbSendFlag = 0;
				// �}�b�v�E�B���h�E�t���O�o�b�N�A�b�v
				if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
				// ������܂ł̎��ԃ^�C�}�[
				ResultWndTimer = RESULT_WND_CLOSE_TIME;
				InitMenu();			// ���j���[�֘A�̏�����
				BattleCmd[ 0 ] = NULL;		// �o�g���R�}���h������
//				BattleCmdBak[ 0 ] = NULL;	// �o�g���R�}���h������
//				BattleCmdReadPointer = BattleCmdWritePointer =0;
				BattleStatus[ 0 ] = NULL;	// �o�g����ԏ�����
				//BattleStatusBak[ 0 ] = NULL;// �o�g����ԏ�����
//				BattleStatusReadPointer = BattleStatusWritePointer =0;
				BattleTurnReceiveFlag = TRUE;	// �^�[����M�t���O�n�m
				// �G���J�E���g��
				play_se( 215, 320, 240 );
				// ���݂̂a�f�l�ۑ�
				now_bgm = t_music_bgm_no;
				// �a�f�l��~
				stop_bgm();
				SubProcNo++;
			}
			break;
			
		case 4:	// �o�b�N�o�b�t�@�[�����o�p�T�[�t�F�X�ɃR�s�[
			// �o�g���T�[�t�F�X�̉摜�쐬 
			CopyBackBuffer();
			// �o�b�N�o�b�t�@�[�`����@�ύX
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
			
			break;
			
		case 5:	// �G���J�E���g���o
		
			// ���o��
			if( DrawProduce( PRODUCE_HAGARE_OCHI_OUT ) == TRUE ){
			//if( GameState == GAME_ENCOUNT_TO_BATTLE ){ 
				// �v���Z�X�`�F���W
				ChangeProc( PROC_BATTLE );
			}
			MenuProc();	// ���j���[����
			ImeProc();	// �h�l�d�֘A����
			
			break;
	}
}


// ���[�v���o�p
void warpEffectProc( void )
{
	oldMapEffectRainLevel = mapEffectRainLevel;
	oldMapEffectSnowLevel = mapEffectSnowLevel;

	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// �}�b�v�E�B���h�E�t���O�o�b�N�A�b�v
	if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
	InitMenu2();

	fieldProc();			// �t�B�[���h�֘A����
	charObjProc();			// �L�����Ǘ�����
	initCharPartsPrio();	// �L�����E�p�[�c�̗D�揇�ʌ��菈���̏�����
	openServerWindowProc();	// �T�[�o�����E�B���h�E

	fieldInfoTime = 0;		// �ꏊ��񂪏o�Ă������
	drawFieldInfoWin();		// ���[�v��̏ꏊ���


	/* �A�N�V�������点�� */
	RunAction();
	// �^�X�N�\���f�[�^���o�b�t�@�ɗ��߂�
	StockTaskDispBuffer();

	mapEffectProc();		// �}�b�v�G�t�F�N�g�i�J�E�ᓙ�j

	redrawMap();
	drawMap();		// �}�b�v�\��

	/* �`���b�g���� */
	ChatProc();
	// �`���b�g�o�b�t�@�[���t�H���g�o�b�t�@�ɗ��߂�
	ChatBufferToFontBuffer(); 
	// ���ԑя����ƃp���b�g�`�F���W����
//	TimeZoneProc();

	SortDispBuffer(); 	// �\���o�b�t�@�\�[�g

	// �o�b�N�T�[�t�F�X�����ŃN���A�[
	ClearBackSurface();	
	// �a�l�o���o�b�N�T�[�t�F�X�ɃZ�b�g
	PutBmp();	


	// �o�b�N�T�[�t�F�X����o�g���T�[�t�F�X�փR�s�[
	lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );

	// �o�b�t�@������
	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// ���݂̎��Ԃ��L��
	NowTime = GetTickCount();

	ProduceInitFlag = TRUE;
}


void paletteProc( void )
{
	if( palNo == -1 )
	{
		// �p���b�g�`�F���W
		PaletteChange( SaTimeZoneNo, 0 );
		// ���ԑтŃp���b�g�`�F���W���鎞
		TimeZonePalChangeFlag = TRUE;
		palNo = -2;
	}
	else
	if( palNo >= 0 )
	{
		// �Œ�p���b�g�ݒ�
		PaletteChange( palNo, 0 );// �p���b�g�`�F���W
		// ���ԑтŃp���b�g�`�F���W���Ȃ���
		TimeZonePalChangeFlag = FALSE;
		palNo = -2;
	}
}