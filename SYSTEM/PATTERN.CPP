/************************/
/*	pattern.cpp		图案	*/
/************************/

/*	ENT	:   ACTION *a0 , 
		:	アニメ`ション速度（０はｓｐｒ．ｂｉｎの速度），
		:	ル`プフラグ（ＡＮＭ＿ＬＯＯＰかＡＮＭ＿ＮＯ＿ＬＯＯＰ）

	RET	:   ０：ル`プしなかった
		:   １：ル`プした
		ENT /:行动* a0。:动画速度（0spr. bin的速度），
		:循环转折点（ANM＿＿＿LOOP LOOP吗ANM NO）退赛:0 :循环了:1：循环的
*/
#include 	"system.h"
#include	"loadrealbin.h"
#include	"loadsprbin.h"
#include	"anim_tbl.h"
#include	"t_music.h"

void StockDispBuffer2( int x, int y, UCHAR dispPrio, int bmpNo );

/*----------　アニメ`ションI理　-动画处理---------*/
int pattern( ACTION *a0, int anim_spd, int loop_flg )
{
	ANIMLIST	*ptAnimlist;
	FRAMELIST	*ptFramelist;
	int i;
	short dx,dy;
	int BmpNo;
	U4 chrNo;

	/*新しいアニメ`ションか幼鳏颏胜*/
	if(ATR_CHR_ACT_OLD(a0) != ATR_CHR_ACT(a0) ||
	ATR_CHR_ANG(a0) != ATR_CHR_ANG_OLD(a0) ||
	ATR_CHR_NO(a0) != ATR_CHR_NO_OLD(a0)){
		ATR_CHR_NO_OLD(a0) = ATR_CHR_NO(a0);		/*新しいアニメ`ションセット*/
		ATR_CHR_ACT_OLD(a0) = ATR_CHR_ACT(a0);		/*新しい幼鳐互氓*/
		ATR_CHR_ANG_OLD(a0) = ATR_CHR_ANG(a0);		/*新しい向きセット*/
		ATR_CHR_TIM(a0) = 0;		/*アニメ`ションのrgクリア*/
		ATR_CHR_CNT(a0) = 0;		/*パタ`ンのクリア*/
	}
	if(ATR_CHR_TIM(a0)){		/*パタ`ン涓rgでなければ*/
		ATR_CHR_TIM(a0)--;
		return 0;		/*アニメ`ションはAく*/
	}
//	ATR_CHR_TIM(a0)=2;		//１枚２割りzみ

	if(ATR_CHR_NO(a0) <= CG_INVISIBLE)
	{
		ATR_PAT_NO(a0) = 0;	//画像のない番号を指定してえなくする
		return 0;			//予s番号ならiける
	}

	if(ATR_CHR_NO(a0) < SPRSTART){		//oいなら
	 	realGetNo( ATR_CHR_NO(a0) , (U4 *)&BmpNo );
		realGetPos( BmpNo  , &dx, &dy);
		ATR_PAT_NO(a0) = BmpNo;			//ＢＭＰ番号セット
		ATR_CHR_H_POS(a0) = dx;		//Ｘ座衰榨互氓去互氓
		ATR_CHR_V_POS(a0) = dy;		//Ｙ座衰榨互氓去互氓
		ATR_CHR_TIM(a0)=0x7fffffff;
		return 0;
	}

	if(ATR_CHR_NO(a0) > SPRSTART + mxSPRITE) return 0;	// 猡胜iける

	chrNo = ATR_CHR_NO(a0) - SPRSTART;
	ptAnimlist = SpriteData[chrNo].ptAnimlist;
	if( SpriteData[chrNo].animSize == 0 ) return 0;		// アニメ`ションデ`タがoいならiける

	//アニメ`ションのNのO定
	for(i=0 ; i < SpriteData[chrNo].animSize; i++){
		if( ATR_CHR_ANG(a0) == ptAnimlist[i].dir && ptAnimlist[i].no == ATR_CHR_ACT(a0) ){
			break;
		}
	}
	if( i >= SpriteData[chrNo].animSize )
		i = 0;
	ptFramelist = ptAnimlist[i].ptFramelist;

	if(anim_spd){		//アニメ`ション速度指定なら
		ATR_CHR_TIM(a0) = anim_spd;		//１枚の表示rgセット
	} else {
		ATR_CHR_TIM(a0) = ptAnimlist[i].dtAnim;		//１枚の表示rgセット
	}

//	for( i = 0; ptFramelist[i].ptBmplist != NULL; i++ );
//	if(ATR_CHR_CNT(a0) == i)		//アニメ`ションル`プなら
//		ATR_CHR_CNT(a0) = 0;		//先^に

	if((U4)ATR_CHR_CNT(a0) >= ptAnimlist[i].frameCnt){	//アニメ`ションル`プなら
		if(loop_flg == ANM_NO_LOOP){		//ル`プoしか
			ATR_CHR_TIM(a0) = 255;
			return 1;		//アニメ`ションK了
		} else {
			ATR_CHR_CNT(a0) = 0;		//先^に
		}
	}

	//抗粼偕胜
	if( ptFramelist[ATR_CHR_CNT(a0)].SoundNo != 0 ){
		if(ptFramelist[ATR_CHR_CNT(a0)].SoundNo < 10000){
			play_se( ptFramelist[ATR_CHR_CNT(a0)].SoundNo, ATR_H_POS(a0), ATR_V_POS(a0) );
		} else {
			ATR_HIT(a0) = ptFramelist[ATR_CHR_CNT(a0)].SoundNo;
		}
	}

	ATR_PAT_NO(a0) = ptFramelist[ATR_CHR_CNT(a0)].BmpNo;			//ＢＭＰ番号セット
	realGetPos(ptFramelist[ATR_CHR_CNT(a0)].BmpNo, &dx, &dy);		//アダ`ンビンのＸＹ座巳・瓿訾
	ATR_CHR_H_POS(a0) = ptFramelist[ATR_CHR_CNT(a0)].PosX + dx;		//Ｘ座衰榨互氓去互氓
	ATR_CHR_V_POS(a0) = ptFramelist[ATR_CHR_CNT(a0)].PosY + dy;		//Ｙ座衰榨互氓去互氓
	ATR_CHR_CNT(a0)++;		//カウンタ`更新
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

	/*新しいアニメ`ションか幼鳏颏胜*/
	if(ATR_CHR_ACT_OLD(a0) != ATR_CHR_ACT(a0) ||
	ATR_CHR_ANG(a0) != ATR_CHR_ANG_OLD(a0) ||
	ATR_CHR_NO(a0) != ATR_CHR_NO_OLD(a0)){
		ATR_CHR_NO_OLD(a0) = ATR_CHR_NO(a0);		/*新しいアニメ`ションセット*/
		ATR_CHR_ACT_OLD(a0) = ATR_CHR_ACT(a0);		/*新しい幼鳐互氓*/
		ATR_CHR_ANG_OLD(a0) = ATR_CHR_ANG(a0);		/*新しい向きセット*/
		ATR_CHR_TIM(a0) = 0;		/*アニメ`ションのrgクリア*/
		ATR_CHR_CNT(a0) = 0;		/*パタ`ンのクリア*/
	}
	if(ATR_CHR_TIM(a0)){		/*パタ`ン涓rgでなければ*/
		ATR_CHR_TIM(a0)--;
		return 0;		/*アニメ`ションはAく*/
	}
//	ATR_CHR_TIM(a0)=2;		//１枚２割りzみ
	if (ATR_CHR_NO(a0) >= mxSPRITE || ATR_CHR_NO(a0) <= CG_INVISIBLE) return 0;		//oいならiける
	ptAnimlist = SpriteData[ATR_CHR_NO(a0)].ptAnimlist;		//？？？アドレスセット
	if (ptAnimlist == NULL){		//oいなら
	 	realGetNo( ATR_CHR_NO(a0) , (U4 *)&BmpNo );
		realGetPos( BmpNo  , &dx, &dy);
		ATR_PAT_NO(a0) = BmpNo;			//ＢＭＰ番号セット
		ATR_CHR_H_POS(a0) = dx;		//Ｘ座衰榨互氓去互氓
		ATR_CHR_V_POS(a0) = dy;		//Ｙ座衰榨互氓去互氓
		ATR_CHR_TIM(a0)=0x7fffffff;
		return 0;
	}

	//アニメ`ションのNのO定
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

	//抗粼偕胜
	if( ptFramelist[ATR_CHR_CNT(a0)].SoundNo != 0 ){
		play_se( ptFramelist[ATR_CHR_CNT(a0)].SoundNo, ATR_H_POS(a0), ATR_V_POS(a0) );
	}

	if(anim_spd){		//アニメ`ション速度指定なら
		ATR_CHR_TIM(a0) = anim_spd;		//１枚の表示rgセット
	} else {
		ATR_CHR_TIM(a0) = ptAnimlist[i].frameCnt;		//１枚の表示rgセット
	}

//	for( i = 0; ptFramelist[i].ptBmplist != NULL; i++ );
//	if(ATR_CHR_CNT(a0) == i)		//アニメ`ションル`プなら
//		ATR_CHR_CNT(a0) = 0;		//先^に

	if(ptFramelist[ATR_CHR_CNT(a0)].ptBmplist == NULL){		//アニメ`ションル`プなら
		if(loop_flg == ANM_NO_LOOP){		//ル`プoしか
			ATR_CHR_TIM(a0) = 255;
			return 1;		//アニメ`ションK了
		} else {
			ATR_CHR_CNT(a0) = 0;		//先^に
		}
	}
	ptBmplist = ptFramelist[ATR_CHR_CNT(a0)].ptBmplist;		//今回の
#if 0
	for (i=0 ; ptBmplist[i].ptBmppos != NULL ; i++ ){		//枚数分表示
		if(!i){
			ATR_PAT_NO(a0) = ptBmplist[i].ptBmppos->BmpNo;			//ＢＭＰ番号セット
			realGetPos(ptBmplist[i].ptBmppos->BmpNo, &dx, &dy);		//アダ`ンビンのＸＹ座巳・瓿訾
			ATR_CHR_H_POS(a0) = ptBmplist[i].PosX + dx;		//Ｘ座衰榨互氓去互氓
			ATR_CHR_V_POS(a0) = ptBmplist[i].PosY + dy;		//Ｙ座衰榨互氓去互氓
		} else {
			realGetPos(ptBmplist[i].ptBmppos->BmpNo, &dx, &dy);		//アダ`ンビンのＸＹ座巳・瓿訾
			StockDispBuffer2( ATR_H_POS(a0) + ptBmplist[i].PosX + dx,
			ATR_V_POS(a0) + ptBmplist[i].PosY + dy,
			a0->dispPrio,
			ptBmplist[i].ptBmppos->BmpNo );
		}
	}

#else
	ATR_PAT_NO(a0) = ptBmplist[0].ptBmppos->BmpNo;			//ＢＭＰ番号セット
	realGetPos(ptBmplist[0].ptBmppos->BmpNo, &dx, &dy);		//アダ`ンビンのＸＹ座巳・瓿訾
	ATR_CHR_H_POS(a0) = ptBmplist[0].PosX + dx;		//Ｘ座衰榨互氓去互氓
	ATR_CHR_V_POS(a0) = ptBmplist[0].PosY + dy;		//Ｙ座衰榨互氓去互氓
#endif
	ATR_CHR_CNT(a0)++;		//カウンタ`更新
	ATR_CHR_TIM(a0)--;		//
	return 0;

#if 0	//未使用//////////////////////////////////////////////////////
	// １フレ`ムアニメ`ション枚数を算
	for( i = 0; ptFramelist[i].ptBmplist != NULL; i++ );
	animSize = i;

	//F在のフレ`ム位置を取得
	if (ptsprite->ctAnim < 1){		//１未亥猢`ド
		ptBmplist = ptFramelist[(int)(ptsprite->ctAnim * animSize)].ptBmplist;
	}else{
		ptBmplist = ptFramelist[(int)(ptsprite->ctAnim)                        ].ptBmplist;
	}

	//1フレ`ム内の
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
