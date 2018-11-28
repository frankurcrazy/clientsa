/************************/
/*	pattern.cpp		ͼ��	*/
/************************/

/*	ENT	:   ACTION *a0 , 
		:	���˥�`������ٶȣ����ϣ��򣮣�����ٶȣ���
		:	��`�ץե饰�����Σͣߣ̣ϣϣФ����ΣͣߣΣϣߣ̣ϣϣУ�

	RET	:   ������`�פ��ʤ��ä�
		:   ������`�פ���
		ENT /:�ж�* a0��:�����ٶȣ�0spr. bin���ٶȣ���
		:ѭ��ת�۵㣨ANM�ߣߣ�LOOP LOOP��ANM NO������:0 :ѭ����:1��ѭ����
*/
#include 	"system.h"
#include	"loadrealbin.h"
#include	"loadsprbin.h"
#include	"anim_tbl.h"
#include	"t_music.h"

void StockDispBuffer2( int x, int y, UCHAR dispPrio, int bmpNo );

/*----------�����˥�`�����I����-��������---------*/
int pattern( ACTION *a0, int anim_spd, int loop_flg )
{
	ANIMLIST	*ptAnimlist;
	FRAMELIST	*ptFramelist;
	int i;
	short dx,dy;
	int BmpNo;
	U4 chrNo;

	/*�¤������˥�`����󤫄������򤭤ʤ�*/
	if(ATR_CHR_ACT_OLD(a0) != ATR_CHR_ACT(a0) ||
	ATR_CHR_ANG(a0) != ATR_CHR_ANG_OLD(a0) ||
	ATR_CHR_NO(a0) != ATR_CHR_NO_OLD(a0)){
		ATR_CHR_NO_OLD(a0) = ATR_CHR_NO(a0);		/*�¤������˥�`����󥻥å�*/
		ATR_CHR_ACT_OLD(a0) = ATR_CHR_ACT(a0);		/*�¤����������å�*/
		ATR_CHR_ANG_OLD(a0) = ATR_CHR_ANG(a0);		/*�¤����򤭥��å�*/
		ATR_CHR_TIM(a0) = 0;		/*���˥�`�����Εr�g���ꥢ*/
		ATR_CHR_CNT(a0) = 0;		/*�ѥ��`��Υ��ꥢ*/
	}
	if(ATR_CHR_TIM(a0)){		/*�ѥ��`�����Εr�g�Ǥʤ����*/
		ATR_CHR_TIM(a0)--;
		return 0;		/*���˥�`�����ϾA��*/
	}
//	ATR_CHR_TIM(a0)=2;		//��ö������z��

	if(ATR_CHR_NO(a0) <= CG_INVISIBLE)
	{
		ATR_PAT_NO(a0) = 0;	//����Τʤ����Ť�ָ������Ҋ���ʤ�����
		return 0;			//��s���Ťʤ�i����
	}

	if(ATR_CHR_NO(a0) < SPRSTART){		//�o���ʤ�
	 	realGetNo( ATR_CHR_NO(a0) , (U4 *)&BmpNo );
		realGetPos( BmpNo  , &dx, &dy);
		ATR_PAT_NO(a0) = BmpNo;			//�£ͣз��ť��å�
		ATR_CHR_H_POS(a0) = dx;		//�����˥��ե��åȥ��å�
		ATR_CHR_V_POS(a0) = dy;		//�����˥��ե��åȥ��å�
		ATR_CHR_TIM(a0)=0x7fffffff;
		return 0;
	}

	if(ATR_CHR_NO(a0) > SPRSTART + mxSPRITE) return 0;	// Ҏ������ʤ�i����

	chrNo = ATR_CHR_NO(a0) - SPRSTART;
	ptAnimlist = SpriteData[chrNo].ptAnimlist;
	if( SpriteData[chrNo].animSize == 0 ) return 0;		// ���˥�`�����ǩ`�����o���ʤ�i����

	//���˥�`�����ηN��O��
	for(i=0 ; i < SpriteData[chrNo].animSize; i++){
		if( ATR_CHR_ANG(a0) == ptAnimlist[i].dir && ptAnimlist[i].no == ATR_CHR_ACT(a0) ){
			break;
		}
	}
	if( i >= SpriteData[chrNo].animSize )
		i = 0;
	ptFramelist = ptAnimlist[i].ptFramelist;

	if(anim_spd){		//���˥�`������ٶ�ָ���ʤ�
		ATR_CHR_TIM(a0) = anim_spd;		//��ö�α�ʾ�r�g���å�
	} else {
		ATR_CHR_TIM(a0) = ptAnimlist[i].dtAnim;		//��ö�α�ʾ�r�g���å�
	}

//	for( i = 0; ptFramelist[i].ptBmplist != NULL; i++ );
//	if(ATR_CHR_CNT(a0) == i)		//���˥�`������`�פʤ�
//		ATR_CHR_CNT(a0) = 0;		//���^�ˑ���

	if((U4)ATR_CHR_CNT(a0) >= ptAnimlist[i].frameCnt){	//���˥�`������`�פʤ�
		if(loop_flg == ANM_NO_LOOP){		//��`�ןo����
			ATR_CHR_TIM(a0) = 255;
			return 1;		//���˥�`�����K��
		} else {
			ATR_CHR_CNT(a0) = 0;		//���^�ˑ���
		}
	}

	//�����������ʤ�
	if( ptFramelist[ATR_CHR_CNT(a0)].SoundNo != 0 ){
		if(ptFramelist[ATR_CHR_CNT(a0)].SoundNo < 10000){
			play_se( ptFramelist[ATR_CHR_CNT(a0)].SoundNo, ATR_H_POS(a0), ATR_V_POS(a0) );
		} else {
			ATR_HIT(a0) = ptFramelist[ATR_CHR_CNT(a0)].SoundNo;
		}
	}

	ATR_PAT_NO(a0) = ptFramelist[ATR_CHR_CNT(a0)].BmpNo;			//�£ͣз��ť��å�
	realGetPos(ptFramelist[ATR_CHR_CNT(a0)].BmpNo, &dx, &dy);		//�����`��ӥ�Σأ�����ȡ�����
	ATR_CHR_H_POS(a0) = ptFramelist[ATR_CHR_CNT(a0)].PosX + dx;		//�����˥��ե��åȥ��å�
	ATR_CHR_V_POS(a0) = ptFramelist[ATR_CHR_CNT(a0)].PosY + dy;		//�����˥��ե��åȥ��å�
	ATR_CHR_CNT(a0)++;		//�����󥿩`����
	ATR_CHR_TIM(a0)--;		//
	return 0;
}






#if 0
int pattern( ACTION *a0, int anim_spd, int loop_flg )
{
	ANIMLIST	*ptAnimlist;
	FRAMELIST	*ptFramelist;
	BMPLIST		*ptBmplist;
	int i;
	short dx,dy;
	int BmpNo;

	/*�¤������˥�`����󤫄������򤭤ʤ�*/
	if(ATR_CHR_ACT_OLD(a0) != ATR_CHR_ACT(a0) ||
	ATR_CHR_ANG(a0) != ATR_CHR_ANG_OLD(a0) ||
	ATR_CHR_NO(a0) != ATR_CHR_NO_OLD(a0)){
		ATR_CHR_NO_OLD(a0) = ATR_CHR_NO(a0);		/*�¤������˥�`����󥻥å�*/
		ATR_CHR_ACT_OLD(a0) = ATR_CHR_ACT(a0);		/*�¤����������å�*/
		ATR_CHR_ANG_OLD(a0) = ATR_CHR_ANG(a0);		/*�¤����򤭥��å�*/
		ATR_CHR_TIM(a0) = 0;		/*���˥�`�����Εr�g���ꥢ*/
		ATR_CHR_CNT(a0) = 0;		/*�ѥ��`��Υ��ꥢ*/
	}
	if(ATR_CHR_TIM(a0)){		/*�ѥ��`�����Εr�g�Ǥʤ����*/
		ATR_CHR_TIM(a0)--;
		return 0;		/*���˥�`�����ϾA��*/
	}
//	ATR_CHR_TIM(a0)=2;		//��ö������z��
	if (ATR_CHR_NO(a0) >= mxSPRITE || ATR_CHR_NO(a0) <= CG_INVISIBLE) return 0;		//�o���ʤ�i����
	ptAnimlist = SpriteData[ATR_CHR_NO(a0)].ptAnimlist;		//���������ɥ쥹���å�
	if (ptAnimlist == NULL){		//�o���ʤ�
	 	realGetNo( ATR_CHR_NO(a0) , (U4 *)&BmpNo );
		realGetPos( BmpNo  , &dx, &dy);
		ATR_PAT_NO(a0) = BmpNo;			//�£ͣз��ť��å�
		ATR_CHR_H_POS(a0) = dx;		//�����˥��ե��åȥ��å�
		ATR_CHR_V_POS(a0) = dy;		//�����˥��ե��åȥ��å�
		ATR_CHR_TIM(a0)=0x7fffffff;
		return 0;
	}

	//���˥�`�����ηN��O��
	for (i=0 ;  ; i++){
		if ( ptAnimlist[i].ptFramelist == NULL ){
			if (i==0) return 0;
			i = 0;
			break;
		}
		if ( ATR_CHR_ANG(a0) == ptAnimlist[i].dir && ptAnimlist[i].id == ATR_CHR_ACT(a0) ){
			break;
		}
	}
	ptFramelist = ptAnimlist[i].ptFramelist;

	//�����������ʤ�
	if( ptFramelist[ATR_CHR_CNT(a0)].SoundNo != 0 ){
		play_se( ptFramelist[ATR_CHR_CNT(a0)].SoundNo, ATR_H_POS(a0), ATR_V_POS(a0) );
	}

	if(anim_spd){		//���˥�`������ٶ�ָ���ʤ�
		ATR_CHR_TIM(a0) = anim_spd;		//��ö�α�ʾ�r�g���å�
	} else {
		ATR_CHR_TIM(a0) = ptAnimlist[i].frameCnt;		//��ö�α�ʾ�r�g���å�
	}

//	for( i = 0; ptFramelist[i].ptBmplist != NULL; i++ );
//	if(ATR_CHR_CNT(a0) == i)		//���˥�`������`�פʤ�
//		ATR_CHR_CNT(a0) = 0;		//���^�ˑ���

	if(ptFramelist[ATR_CHR_CNT(a0)].ptBmplist == NULL){		//���˥�`������`�פʤ�
		if(loop_flg == ANM_NO_LOOP){		//��`�ןo����
			ATR_CHR_TIM(a0) = 255;
			return 1;		//���˥�`�����K��
		} else {
			ATR_CHR_CNT(a0) = 0;		//���^�ˑ���
		}
	}
	ptBmplist = ptFramelist[ATR_CHR_CNT(a0)].ptBmplist;		//��ؤ�
#if 0
	for (i=0 ; ptBmplist[i].ptBmppos != NULL ; i++ ){		//ö���ֱ�ʾ
		if(!i){
			ATR_PAT_NO(a0) = ptBmplist[i].ptBmppos->BmpNo;			//�£ͣз��ť��å�
			realGetPos(ptBmplist[i].ptBmppos->BmpNo, &dx, &dy);		//�����`��ӥ�Σأ�����ȡ�����
			ATR_CHR_H_POS(a0) = ptBmplist[i].PosX + dx;		//�����˥��ե��åȥ��å�
			ATR_CHR_V_POS(a0) = ptBmplist[i].PosY + dy;		//�����˥��ե��åȥ��å�
		} else {
			realGetPos(ptBmplist[i].ptBmppos->BmpNo, &dx, &dy);		//�����`��ӥ�Σأ�����ȡ�����
			StockDispBuffer2( ATR_H_POS(a0) + ptBmplist[i].PosX + dx,
			ATR_V_POS(a0) + ptBmplist[i].PosY + dy,
			a0->dispPrio,
			ptBmplist[i].ptBmppos->BmpNo );
		}
	}

#else
	ATR_PAT_NO(a0) = ptBmplist[0].ptBmppos->BmpNo;			//�£ͣз��ť��å�
	realGetPos(ptBmplist[0].ptBmppos->BmpNo, &dx, &dy);		//�����`��ӥ�Σأ�����ȡ�����
	ATR_CHR_H_POS(a0) = ptBmplist[0].PosX + dx;		//�����˥��ե��åȥ��å�
	ATR_CHR_V_POS(a0) = ptBmplist[0].PosY + dy;		//�����˥��ե��åȥ��å�
#endif
	ATR_CHR_CNT(a0)++;		//�����󥿩`����
	ATR_CHR_TIM(a0)--;		//
	return 0;

#if 0	//δʹ��//////////////////////////////////////////////////////
	// ���ե�`�ॢ�˥�`�����ö����Ӌ��
	for( i = 0; ptFramelist[i].ptBmplist != NULL; i++ );
	animSize = i;

	//�F�ڤΥե�`��λ�ä�ȡ��
	if (ptsprite->ctAnim < 1){		//��δ����`��
		ptBmplist = ptFramelist[(int)(ptsprite->ctAnim * animSize)].ptBmplist;
	}else{
		ptBmplist = ptFramelist[(int)(ptsprite->ctAnim)                        ].ptBmplist;
	}

	//1�ե�`���ڤ�
	for (i=0 ; ptBmplist[i].ptBmppos != NULL ; i++ ){
		realGetPos(ptBmplist[i].ptBmppos->BmpNo, &dx, &dy);
		drawImage( ptBmplist[i].ptBmppos->BmpNo , 
				  x + ptBmplist[i].ptBmppos->PosX + ptBmplist[i].PosX + dx ,
				  y + ptBmplist[i].ptBmppos->PosY + ptBmplist[i].PosY + dy);
	}
#endif	/////////////////////////////////////////////////////////////

}
#endif
