/************************/
/*	ohta.c				*/
/************************/
#include "system.h"
#include "ohta.h"
#include "loadsprbin.h"
#include "t_music.h"

ACTION *MakeJiki( void );
void Jiki( ACTION *pAct );

ACTION *MakeJikiTama( int x, int y, int dx, int dy, int bmpNo );
void JikiTama( ACTION *pAct );

ACTION *MakeEnemy( void );
void Enemy( ACTION *pAct );

ACTION *MakeEnemyTama( ACTION *pOya );
void EnemyTama( ACTION *pAct );

ACTION *MakeBg( void );
void Bg( ACTION *pAct );

ACTION *MakeKen( void );
void Ken( ACTION *pAct );

ACTION *MakeGameOver( void );
void GameOver( ACTION *pAct );

/* 爆kデ`タ */
int bomData[] = { 21, 22, 23, 23, 22, 21 };

/* 自Cのポインタ */
ACTION *pJiki;


/* 攻膜畏N */
enum{
	TAMA_NORMAL,	/* ノ`マル */
	TAMA_TWIN,		/* ツイン */
	TAMA_3WAY,		/* ３方向 */
	TAMA_DIR,		/* 全方向 */
	TAMA_END		/* Kわり */
};
	
/********************************************/
/*	自C									*/
/********************************************/

/* 自C予造体 */
typedef struct{
	int cnt,cnt2,cnt3;	/* 氓Ε螗 */
	int tamaDir;		/* 畏较 */
	int tamaDx;			/* 危分 */
	int attackKind;		/* 攻膜畏N */
	STR_BUFFER strTestBuffer;	/* テスト文字列 */
}JIKI;

static No = 36000;


/* 自CI理 *******************************************************************/
void Jiki( ACTION *pAct )
{
	JIKI *pYobi = ( JIKI *)pAct->pYobi;
	//int i;
	
	/* 攻氖埭堡r */
	if( pAct->damage > 0 ){
		pAct->hp -= pAct->damage;
		if( pAct->hp <= 0 ){ 
			pAct->state = ACT_STATE_DEAD;
			pYobi->cnt  = 0;
			pYobi->cnt2 = 0;
			pYobi->cnt3 = 0;
		}
		pAct->damage = 0;
	}
	// 取出す座
//	rcX = Rnd( 0,608 );
//	rcY = Rnd( 0,400 );
//	SetRect( &Rc[ pAct->bmpNo ], rcX, rcY, rcX + 32, rcY + 32 ); 
	
	// マウス移
	//pAct->x = mouse.nowPoint.x;
	//pAct->y = mouse.nowPoint.y;

	// マウスでk射
	if( pYobi->cnt3 <= 0 ){
		// k射
		//if( mouse.state == MOUSE_LEFT_CRICK ){
		if( mouse.onceState & MOUSE_LEFT_CRICK ){
			// 入力フォ`カス取得
			//GetKeyInputFocus( &pYobi->strTestBuffer );
			/* 鞒 */
			//MakeJikiTama( pAct->x , pAct->y -16, 0, -8, Rnd( 0, BMP_FILE - 1 ) );
			//MakeJikiTama( pAct->x , pAct->y -16, 0, -8, 1034 );
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, 25 );
			
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 0, BMP_FILE - 2 ) );
			
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 31554, 31557 ) ); // ＴＧＳ
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 31000, 31069 ) ); // モンスタ`
//			MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 30100, 30135 ) ); // ＰＣ
			//MakeJikiTama( pAct->x + 10, pAct->y -16, 0, -8, Rnd( 0, BMP_FILE - 1 ) );
			//MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 29 );
			//MakeJikiTama( pAct->x - 10, pAct->y -16, 0, -8, 29 );
			//MakeJikiTama( pAct->x + 10, pAct->y -16, 0, -8, 29 );
			pYobi->cnt3 = 5;
#if 0
			for( i = 0; i < (int)256; i++ ){
				// 明るくする
				Palette[i].peRed 	 *= 0.95;
				Palette[i].peGreen *= 0.95;
				Palette[i].peBlue  *= 0.95;
			}
			// パレットの中身をO定
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
#endif
		}
		// 真中ボタン
		if( GetAsyncKeyState( VK_MBUTTON ) ){
			//MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 29 );
			//MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 0, BMP_FILE - 1 ) );
	//		MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, 32 );
			pYobi->cnt3 = 5;
			
		}
		// k射
		//if( mouse.onceState & MOUSE_RIGHT_CRICK ){
		if( mouse.state & MOUSE_RIGHT_CRICK ){
			// 入力フォ`カス取得
			//GetKeyInputFocus( &MyChatBuffer );
		//if( GetAsyncKeyState( VK_RBUTTON ) 	& 0x8000 ){
		//	MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 29 );
		// サ`フェスがあったら
		//if( lpSurface[ 0 ] != NULL ){
		//	// リリ`ス
		//	lpSurface[ 0 ]->Release( );
		//	lpSurface[ 0 ] = NULL;
		//}
		//	MakeBg();
#if 0		
			MakeJikiTama( pAct->x - 10, pAct->y -16, -6, -8, Rnd( 0, BMP_FILE - 2 ) );
			MakeJikiTama( pAct->x -  5, pAct->y -16, -3, -8, Rnd( 0, BMP_FILE - 2 ) );
			MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, Rnd( 0, BMP_FILE - 2 ) );
			MakeJikiTama( pAct->x +  5, pAct->y -16,  3, -8, Rnd( 0, BMP_FILE - 2 ) );
			MakeJikiTama( pAct->x + 10, pAct->y -16,  6, -8, Rnd( 0, BMP_FILE - 2 ) );
#else
	//		MakeJikiTama( pAct->x - 10, pAct->y -16, -6, -8, No );
	//		MakeJikiTama( pAct->x -  5, pAct->y -16, -3, -8, No );
	//		MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, No );
	//		MakeJikiTama( pAct->x +  5, pAct->y -16,  3, -8, No );
	//		MakeJikiTama( pAct->x + 10, pAct->y -16,  6, -8, No );
#endif		
			pYobi->cnt3 = 5;
#if 0
			for( i = 0; i < (int)256; i++ ){
				// 暗くする
				Palette[i].peRed  *= 1.05;
				Palette[i].peGreen*= 1.05;
				Palette[i].peBlue *= 1.05;
			}
			//パレットの中身をO定
			lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette );
#endif
		}
		
	}else pYobi->cnt3--;

	/* 状Bで分岐 */
	switch( pAct->state ){
		/* 通常r */
		case ACT_STATE_ACT:
		
			/* 趣蔚堡郡昱卸 */
			//Hit( pAct, PRIO_ENEMY_TAMA );
			/* 长趣蔚堡郡昱卸 */
			//Hit( pAct, PRIO_ENEMY );

			/* 移 */
			if( joy_con[ 0 ] & JOY_UP ){	/* 上 */
				pAct->y -= 2;
				if( pAct->y < 0 ) pAct->y = 0;
			}
			if( joy_con[ 0 ] & JOY_DOWN ){	/* 下 */
				pAct->y += 2;
				if( pAct->y > 448 ) pAct->y = 448;
			}
			if( joy_con[ 0 ] & JOY_LEFT ){	/* 左 */
				pAct->x -= 2;
				if( pAct->x < 0 ) pAct->x = 0;
			}
			if( joy_con[ 0 ] & JOY_RIGHT ){	/* 右 */
				pAct->x += 2;
				if( pAct->x > 608 ) pAct->x = 608;
			}
			
			// 匙鞒
			//if( joy_con[ 0 ] & JOY_B )	MakeEnemy();
			
			/* k射 */
			if( pYobi->cnt3 <= 0 ){ /* ΕДぅ */
				if( joy_con[ 0 ] & JOY_A ){
					
					/*DeathAllAction();*/
					/* 畏Nで分岐 */
					switch( pYobi->attackKind ){
						
						case TAMA_NORMAL:
						
							/* 鞒 */
							MakeJikiTama( pAct->x, pAct->y -16, 0, -8, 28 );
							
							break;
							
						case TAMA_TWIN:
						
							/* 鞒 */
							MakeJikiTama( pAct->x - 10, pAct->y -16, 0, -8, 29 );
							MakeJikiTama( pAct->x + 10, pAct->y -16, 0, -8, 29 );
							
							break;
							
						case TAMA_3WAY:
						
							MakeJikiTama( pAct->x - 10, pAct->y -16, -4, -8, 29 );
							MakeJikiTama( pAct->x, 	   	pAct->y -16,  0, -8, 29 );
							MakeJikiTama( pAct->x + 10, pAct->y -16,  4, -8, 29 );
							
							break;
							
						case TAMA_DIR:
						
							MakeJikiTama( pAct->x, pAct->y -16,  pYobi->tamaDx, -8, 29 );
							MakeJikiTama( pAct->x, pAct->y -16, -pYobi->tamaDx, -8, 29 );
							/* 较 */
							pYobi->tamaDx += pYobi->tamaDir;
							if(	pYobi->tamaDx <= -10 || 10 <= pYobi->tamaDx ){
								pYobi->tamaDir *= -1;
							}
							break;
					}
					pYobi->cnt3 = 2; /*  ΕДぅ */
					//pYobi->cnt3 = 0; /*  ΕДぅ */
				}
			}else{
				if( pYobi->cnt3 > 0 ) pYobi->cnt3--;
			}
			
			/* 点 */
/*			if( ( int )pAct->x % 2 ) pAct->atr |= ACT_ATR_HIDE;
			else pAct->atr &= ~ACT_ATR_HIDE;
*/		
			/* 匙鞒 */
			pYobi->cnt++;
			if( pYobi->cnt >= 1 ){
				//MakeEnemy();
				pYobi->cnt2++;
				if( pYobi->cnt2 >= 5 ){
					pYobi->cnt2 = 0;
					/* 涓 */
					//pYobi->attackKind++;
					if( pYobi->attackKind >= TAMA_END ){
						pYobi->attackKind = 0;
					}
				}
				pYobi->cnt = 0;
			}
			
			break;
	
		/* 死亡r */	
		case ACT_STATE_DEAD:
		
			if( pYobi->cnt == 0 ){ 
				pAct->bmpNo = bomData[ 0 ];
				pYobi->cnt = 1;
			}
			/* 爆kアニメ`ション */
			pYobi->cnt2++;
			if( pYobi->cnt2 >= 5 ){
				pAct->bmpNo = bomData[ pYobi->cnt ];
				pYobi->cnt++;
				pYobi->cnt2 = 0;
				/* Kわったら死ぬ */
				if( pYobi->cnt == 6 ){
					DeathAction( pAct );
					MakeGameOver( );
				}
			}
			
			break;
	}
	
	// 表示デ`タをバッファに溜める
	//StockDispBuffer( pAct->x + 32, pAct->y, pAct->dispPrio, pAct->anim_chr_no, 0 );
	//StockDispBuffer( pAct->x - 32, pAct->y, pAct->dispPrio, pAct->anim_chr_no, 0 );
	{
		char szMoji[ 256 ];
		wsprintf( szMoji,"BmpNo = %d",No );
	//	StockFontBuffer( pAct->x, pAct->y, FONT_PRIO_FRONT, 2, szMoji, 0 );
	}
	
	// アニメ`ション
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
	// テスト文字列表示
	//StockFontBuffer( 320, 200, FONT_PRIO_FRONT, pYobi->strTestBuffer.color, pYobi->strTestBuffer.buffer, 0 );
	StockFontBuffer2( &pYobi->strTestBuffer );
}

/********************************************/
/*	自C									*/
/********************************************/

/* 自C鞒 *********************************************************************/
ACTION *MakeJikiTama( int x, int y, int dx, int dy, int bmpNo )
{
	ACTION *pAct;
	
	/* アクションリストに登h */
	pAct = GetAction( PRIO_JIKI_TAMA, 0 );
	if( pAct == NULL ) return NULL;
	
	/* g行v数 */
	pAct->func = JikiTama;
	// anim_tbl.h の番号
	pAct->anim_chr_no = bmpNo; // イノシシ
	// 幼鞣
	//pAct->anim_no = ANIM_ATTACK;
	pAct->anim_no = Rnd( ANIM_ATTACK, ANIM_WALK );
	//pAct->anim_no = ANIM_WALK;
	// アニメ`ション向き( ０～７ )( 下が０で右回り )
	pAct->anim_ang = Rnd( 0, 7 );
	///* スプライト番号 */
	//pAct->bmpNo = bmpNo;
	/* 表示先度 */
	pAct->dispPrio = 2;
	/* ＨＰ */
	pAct->hp = 10000;
	/* 初期位置 */
	pAct->x = x - dx;
	pAct->y = y - dy;
	/* 分 */
	pAct->dx = dx / 2;
	pAct->dy = dy / 2;
	
	return pAct;
}

/* 自CI理 *********************************************************************/
void JikiTama( ACTION *pAct )
{

	/* 攻氖埭堡r */
	if( pAct->damage > 0 ){
		pAct->hp -= pAct->damage;
		if( pAct->hp <= 0 ){ 
			pAct->state = ACT_STATE_DEAD;
		}
		pAct->damage = 0;
	}

	/* 状Bで分岐 */
	switch( pAct->state ){
		/* 通常r */
		case ACT_STATE_ACT:

			/* 长趣蔚堡郡昱卸 */
			//Hit( pAct, PRIO_ENEMY );

			/* 移 */
			pAct->x += pAct->dx;
			pAct->y += pAct->dy;
	
			/* リミット */
			if(	pAct->y <= -32 ){ 
				pAct->y = -32;
				MakeEnemyTama( pAct );
				DeathAction( pAct );
			}else
			if( 480 <= pAct->y ){ 
				pAct->y = 480;
				DeathAction( pAct );
			}
			
			if(	pAct->x <= -32 ){
				pAct->x = -32;
				DeathAction( pAct );
			}else
			if( 640 <= pAct->x ){ 
				pAct->x = 640;
				DeathAction( pAct );
			}

			break;
		
		/* 死亡r */	
		case ACT_STATE_DEAD:
		
			DeathAction( pAct );
			
			break;
	}
	
	
	// アニメ`ション
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );
}


/********************************************/
/*											*/
/********************************************/

/* 秤造体 */
typedef struct{
	int cnt,cnt2,cnt3,animCnt;	/* 氓Ε螗 */
}ENEMY;

/* 匙鞒 *********************************************************************/
ACTION *MakeEnemy( void )
{
	ACTION *pAct;
	ENEMY *pYobi;
	short bmpTbl[] = { 1, 3, 5, 7, 9, 29 };
	
	/* アクションリストに登h */
	pAct = GetAction( PRIO_ENEMY, sizeof( ENEMY ) );
	if( pAct == NULL ) return NULL;
	
	/* 予造体のアドレス */
	pYobi = ( ENEMY *)pAct->pYobi;
	
	/* g行v数 */
	pAct->func = Enemy;
	/* スプライト番号 */
	pAct->bmpNo = bmpTbl[ ( int )Rnd( 0, 4 ) ];
	//pAct->bmpNo = Rnd( 1, 10 );
	/* 表示先度 */
	pAct->dispPrio = 1;
	/* ＨＰ */
    pAct->hp = 10;//Rnd( 10, 50 );
	/* 初期位置ランダム */
//	pAct->x = Rnd( 0, 608 );
	pAct->x = Rnd( 0, 640 );
	ATR_CRS(pAct) = 16;
	ATR_SPD(pAct) = Rnd( 28, 36 );
	pAct->dx = 0;

	pAct->y = 0;
	/* 分ランダム */
	pAct->dx = Rnd( -3, 3 );
	pAct->dy = Rnd( 1, 3 );
	
	return pAct;
}

/* I理 *********************************************************************/
void enemy_missile( ACTION *a0 )
{
	gemini(a0);		//移
	//画面外なら
	if(ATR_H_POS(a0)>=640 || ATR_H_POS(a0)<-32 || ATR_V_POS(a0)>480 || ATR_V_POS(a0)<-32)
		DeathAction( a0 );		//K了
}

/* I理 *********************************************************************/
void Enemy( ACTION *pAct )
{
	ENEMY *pYobi = ( ENEMY *)pAct->pYobi;

	/* 攻氖埭堡r */
	if( pAct->damage > 0 ){
		pAct->hp -= pAct->damage;
		if( pAct->hp <= 0 ){ 
			pAct->state = ACT_STATE_DEAD;
			pYobi->cnt  = 0;
			pYobi->cnt2 = 0;
			pYobi->cnt3 = 0;
		}
		pAct->damage = 0;
	}

	/* 状Bで分岐 */
	switch( pAct->state ){
		/* 通常r */
		case ACT_STATE_ACT:
		
			/* 移 */
//			pAct->x += pAct->dx;
//			pAct->y += pAct->dy;

			/* リミットチェック */
//			if( pAct->x < 0 ){ 
//				pAct->x = 0;
//				pAct->dx *= -1;
//			}
//			else
//			if( pAct->x > 608 ){ 
//				pAct->x = 608;
//				pAct->dx *= -1;
//			}
//			if( pAct->y > 480 ){ 
//				pAct->y = -32;
//			}

			pAct->dx++;		//２割りzみに１回スピ`ド涓工
			pAct->dx&=1;	//
			gemini(pAct);		//移
			if(ATR_CRS(pAct)==0){		//上移又肖胜
				if( pAct->y < -32 ){			//上端なら
					ATR_CRS(pAct)=16;			//下移娱_始
					ATR_SPD(pAct) = Rnd( 28, 36 );
					pAct->x = Rnd( 0, 640 );	//
				} else {
					if(pAct->dx)
						ATR_SPD(pAct)++;		//スピ`ドアップ
				}
			} else {
				if(pAct->dx){
					ATR_SPD(pAct)--;		//スピ`ドダウン
					if(!ATR_SPD(pAct)){		//停止したなら
						ATR_CRS(pAct)=0;		//上移娱_始
						ACTION *a1;		//ミサイルk射
						int d1,d2;
						/* アクションリストに登h */
						a1 = GetAction( PRIO_ENEMY_TAMA, 0 );
						if( a1 == NULL ) break;
						/* g行v数 */
						a1->func = enemy_missile;
						/* スプライト番号 */
						a1->bmpNo = No++;
						/* 表示先度 */
						a1->dispPrio = 3;
						/* 初期位置 */
						ATR_H_POS(a1) = ATR_H_POS(pAct)+16;
						ATR_V_POS(a1) = ATR_V_POS(pAct)+16;
						d1 = pJiki->x;			//自Cの方向セット
						d2 = pJiki->y;			//
						radar(a1, &d1, &d2);	//
						ATR_CRS(a1)=d1;			//
						ATR_SPD(a1)=16;			//移铀俣
						play_se( 2, ATR_H_POS(a1), ATR_V_POS(a1) );		//ミサイルk射音
					}
				}
			}
			// アニメ`ション
/*			pYobi->cnt3 += pAct->dy;
			if( pYobi->cnt3 >= 20 ){ 
				pYobi->cnt3 = 0;
				pYobi->animCnt = pYobi->animCnt * ( -2 ) + 1;
				pAct->bmpNo += pYobi->animCnt;
			}
*/
/*			
			pAct->cnt++;
			if( pAct->cnt >= 300 ){
				pAct->state = ACT_STATE_DEAD;
				pAct->cnt = 0;
			}
*/			
			//if( Rnd( 0, 300 ) == 0 ) MakeEnemyTama( pAct );
			//MakeEnemyTama( pAct );
			
			break;
		
		/* 死亡r */	
		case ACT_STATE_DEAD:
		
			if( pYobi->cnt == 0 ){ 
				pAct->bmpNo = bomData[ 0 ];
				pYobi->cnt = 1;
			}
			/* 爆kアニメ`ション */
			pYobi->cnt2++;
			if( pYobi->cnt2 >= 5 ){
				pAct->bmpNo = bomData[ pYobi->cnt ];
				pYobi->cnt++;
				pYobi->cnt2 = 0;
				/* Kわったら死ぬ */
				if( pYobi->cnt == 6 ){
					DeathAction( pAct );
				}
			}
			
			break;
	}

}

/********************************************/
/*										*/
/********************************************/


/* ゲ`ムオ`バ`I理 *******************************************************************/
void GameOver( ACTION *pAct )
{
	//char szMoji[128];
	
	// GAME OVER 表示
	
}

/* ＢＧ作成 *********************************************************************/
ACTION *MakeBg( void )
{
	ACTION *pAct;
	
	/* アクションリストに登h */
	pAct = GetAction( PRIO_BG, NULL );
	if( pAct == NULL ) return NULL;
	
	/* g行v数 */
	pAct->func = Bg;
	
	/* スプライト番号 */
	//pAct->bmpNo = 30;
//	pAct->bmpNo = 44;
	pAct->anim_chr_no = 9376;
	/* 表示先度 */
	pAct->dispPrio = 0;
	/* 初期位置 */
	pAct->x = 320;
	pAct->y = 240;

	// 取出す座
	//pYobi->rcX = 0;
	//pYobi->rcY = 480;
	//SetRect( &Rc[ pAct->bmpNo ], pYobi->rcX, pYobi->rcY, pYobi->rcX + 640, pYobi->rcY + 480 ); 

	return pAct;
}
	
/* ＢＧI理 *********************************************************************/
void Bg( ACTION *pAct )
{
	// 取出す座
	// 取出す座
	//pAct->y++;
	//if( pAct->y >= 240 ) pAct->y = -240; 
	pattern( pAct, ANM_NOMAL_SPD, ANM_LOOP );		//アニメ`ションI理
	//StockDispBuffer( pAct->x, pAct->y+480, 0, pAct->anim_chr_no, 0 );
	//StockDispBuffer( pAct->x, pAct->y, 0, pAct->anim_chr_no, 0 );
	//StockDispBuffer( pAct->x, pAct->y+480, 0, pAct->anim_chr_no, 0);
}


/* ケン作成 *********************************************************************/
ACTION *MakeKen( void )
{
	ACTION *pAct;
	
	/* アクションリストに登h */
	pAct = GetAction( PRIO_BG, NULL );
	if( pAct == NULL ) return NULL;
	
	/* g行v数 */
	pAct->func = Ken;
	
	/* スプライト番号 */
	pAct->bmpNo = 33;
	/* 表示先度 */
	pAct->dispPrio = 100;
	/* 初期位置 */
	pAct->x = 100;
	pAct->y = 100;
	
	// 取出す座
	//pYobi->rcX = 0;
	//pYobi->rcY = 480;
	//SetRect( &Rc[ pAct->bmpNo ], pYobi->rcX, pYobi->rcY, pYobi->rcX + 640, pYobi->rcY + 480 ); 

	return pAct;
}
	
/* ケンI理 *********************************************************************/
void Ken( ACTION *pAct )
{
	int anim[] = { 	0,0,0,0,
					1,1,1,1,
					2,2,2,2,
					3,3,3,3,
					2,2,2,2,
					1,1,1,1 
					};
}


