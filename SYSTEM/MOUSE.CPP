/************************/
/*	mouse.c				*/
/************************/
#include "system.h"
#include "loadrealbin.h"
#include "anim_tbl.h"
#include "pc.h"
#include "menu.h"
#include "battleMenu.h"

// マウスの当たり判定サイズ
#define MOUSE_HIT_SIZE_X 48
#define MOUSE_HIT_SIZE_Y 48

// マウスのオ`トリピ`トのg隔
#define MOUSE_AUTO_REPEATE_TIME 100

// マウス造体
MOUSE mouse;
// マウスカ`ソルに当たったフォント番号（フォント用）
int	HitFontNo;
// マウスカ`ソルに当たった表示番号（ＢＭＰ用）
int HitDispNo;
// 一行インフォ文字列
char OneLineInfoStr[ 256 ];

// 表示ボックスの色
int BoxColor;	


// マウス情螭纬跗诨 ////////////////////////////////////////////////////////
void MouseInit( void )
{
	memset( &mouse, 0, sizeof( MOUSE ) );
	mouse.itemNo = -1; // アイテム番号初期化
}

// F在のマウスの位置をする //////////////////////////////////////////////
void MouseNowPoint( int x, int y )
{
	// 座摔颔互氓
	//３２０＊２４０ドットモ`ドのr
	if(ResoMode == 1){
		mouse.nowPoint.x = x * 2;
		mouse.nowPoint.y = y * 2;
	} else {
		mouse.nowPoint.x = x;
		mouse.nowPoint.y = y;
	}
}

// マウスの左ボタンが押されたrのマウスの位置をする //////////////////////
void MouseCrickLeftDownPoint( int x, int y )
{
	// 状Bをセット
	mouse.state |= MOUSE_LEFT_CRICK;
	mouse.onceState |= MOUSE_LEFT_CRICK;
	mouse.autoState |= MOUSE_LEFT_CRICK;
	// 座摔颔互氓
	mouse.crickLeftDownPoint.x = x;
	mouse.crickLeftDownPoint.y = y;
	// ボタンが押されてからのU^rgをO定する前
	mouse.beforeLeftPushTime = GetTickCount();
	mouse.leftPushTime = 0;
}

// マウスの左ボタンxされたrのマウスの位置をする ////////////////////////
void MouseCrickLeftUpPoint( int x, int y )
{
	// 状Bをセット
	mouse.state &= (~MOUSE_LEFT_CRICK);
	mouse.onceState |= MOUSE_LEFT_CRICK_UP;
	mouse.autoState &= (~MOUSE_LEFT_CRICK);
	// 座摔颔互氓
	mouse.crickLeftUpPoint.x = x;
	mouse.crickLeftUpPoint.y = y;
	// ボタンがxされたらrgをリセット
	mouse.beforeLeftPushTime = 0;
	mouse.leftPushTime = 0;
}

// 左ダブルクリックされたrのマウスの位置をする //////////////////////////
void MouseDblCrickLeftUpPoint( int x, int y )
{
	// 状Bをセット
	//mouse.onceState |= ( MOUSE_LEFT_DBL_CRICK | MOUSE_LEFT_CRICK );
	mouse.state |= MOUSE_LEFT_CRICK;
	mouse.onceState |= MOUSE_LEFT_DBL_CRICK;
	mouse.onceState |= MOUSE_LEFT_CRICK;
	mouse.autoState |= MOUSE_LEFT_CRICK;
	// 座摔颔互氓
	mouse.crickLeftDownPoint.x = x;
	mouse.crickLeftDownPoint.y = y;
	// ボタンが押されてからのU^rgをO定する前
	mouse.beforeLeftPushTime = GetTickCount();
	mouse.leftPushTime = 0;
}

// マウスの右ボタン押されたrのマウスの位置をする ////////////////////////
void MouseCrickRightDownPoint( int x, int y )
{
	// 状Bをセット
	mouse.state |= MOUSE_RIGHT_CRICK;
	mouse.onceState |= MOUSE_RIGHT_CRICK;
	mouse.autoState |= MOUSE_RIGHT_CRICK;
	// 座摔颔互氓
	mouse.crickRightDownPoint.x = x;
	mouse.crickRightDownPoint.y = y;
	// ボタンが押されてからのU^rgをO定する前
	mouse.beforeRightPushTime = GetTickCount();
	mouse.rightPushTime = 0;
}

// マウスの右ボタンxされたrのマウスの位置をする ////////////////////////
void MouseCrickRightUpPoint( int x, int y )
{
	// 状Bをセット
	mouse.state &= (~MOUSE_RIGHT_CRICK);
	mouse.onceState |= MOUSE_RIGHT_CRICK_UP;
	mouse.autoState &= (~MOUSE_RIGHT_CRICK);
	// 座摔颔互氓
	mouse.crickRightUpPoint.x = x;
	mouse.crickRightUpPoint.y = y;
	// ボタンがxされたらrgをリセット
	mouse.beforeRightPushTime = 0;
	mouse.rightPushTime = 0;
}

// 右ダブルクリックされたrのマウスの位置をする //////////////////////////
void MouseDblCrickRightUpPoint( int x, int y )
{
	// 状Bをセット
	mouse.state |= MOUSE_RIGHT_CRICK;
	mouse.onceState |= MOUSE_RIGHT_DBL_CRICK;
	mouse.onceState |= MOUSE_RIGHT_CRICK;
	mouse.autoState |= MOUSE_RIGHT_CRICK;
	// 座摔颔互氓
	mouse.crickRightDownPoint.x = x;
	mouse.crickRightDownPoint.y = y;
	// ボタンが押されてからのU^rgをO定する前
	mouse.beforeRightPushTime = GetTickCount();
	mouse.rightPushTime = 0;
}

// マウスI理　///////////////////////////////////////////////////////////////
void MouseProc( void )
{
	static UINT leftPushTimeBak;
	static UINT rightPushTimeBak;
	
	// 状Bを初期化
	mouse.onceState = MOUSE_NO_CRICK;
	// 左ボタンを押してからのU^rgをセット
	if( mouse.beforeLeftPushTime > 0 )
	{
		mouse.leftPushTime = GetTickCount() - mouse.beforeLeftPushTime;
	}
	// 右ボタンを押してからのU^rgをセット
	if( mouse.beforeRightPushTime > 0 )
	{
		mouse.rightPushTime = GetTickCount() - mouse.beforeRightPushTime;
	}
	// オ`トリピ`トI理
	// 左クリック押されているr
	// ０．５秒でリピ`ト_始
	if( mouse.leftPushTime > 500 ){
		// 最初の一回のr
		if( leftPushTimeBak == 0 ){
			mouse.autoState |= MOUSE_LEFT_CRICK; // ビットＯＮ
			leftPushTimeBak = mouse.leftPushTime;	// rgを
		}else
		// rgがきたらRり返し
		if( mouse.leftPushTime - leftPushTimeBak >= MOUSE_AUTO_REPEATE_TIME ){
			mouse.autoState |= MOUSE_LEFT_CRICK; // ビットＯＮ
			leftPushTimeBak = mouse.leftPushTime;	// rgを
		}else{	// すぐにＯＦＦにする
			mouse.autoState &= (~MOUSE_LEFT_CRICK);	// ビットＯＦＦ
		}
	}else{ 
		mouse.autoState &= (~MOUSE_LEFT_CRICK);	// ビットＯＦＦ
		leftPushTimeBak = 0;
	}
	
	// 左クリック押されているr
	// ０．５秒でリピ`ト_始
	if( mouse.rightPushTime > 500 ){
		// 最初の一回のr
		if( rightPushTimeBak == 0 ){
			mouse.autoState |= MOUSE_RIGHT_CRICK; // ビットＯＮ
			rightPushTimeBak = mouse.rightPushTime;	// rgを
		}else
		// rgがきたらRり返し
		if( mouse.rightPushTime - rightPushTimeBak >= MOUSE_AUTO_REPEATE_TIME ){
			mouse.autoState |= MOUSE_RIGHT_CRICK; // ビットＯＮ
			rightPushTimeBak = mouse.rightPushTime;	// rgを
		}else{	// すぐにＯＦＦにする
			mouse.autoState &= (~MOUSE_RIGHT_CRICK);	// ビットＯＦＦ
		}
	}else{ 
		mouse.autoState &= (~MOUSE_RIGHT_CRICK);	// ビットＯＦＦ
		rightPushTimeBak = 0;
	}
	
	
	
	// 右クリック押されているr
	//if( mouse.rightPushTime > 0 ){
	//}else mouse.state &= (~MOUSE_RIGHT_AUTO_CRICK);
	// ウィンドウモ`ドでないr
	//if( !WindowMode ){
	//if( 0 <= mouse.nowPoint.x && mouse.nowPoint.x < 640 &&
	//	0 <= mouse.nowPoint.y && mouse.nowPoint.y < 480 ){
	
#ifndef _DEBUG
	if( mouse.flag == FALSE ){ 
		// マウスカ`ソルの表示（最先表示）
		StockDispBuffer( mouse.nowPoint.x + 16, mouse.nowPoint.y + 16, DISP_PRIO_MOUSE, CG_MOUSE_CURSOR, 0 );
	}
#endif
		
}
// グル`プxkチェック *********************************************************/
void CheckGroupSelect( int no )
{
	int i;
	DISP_INFO 	*pDispInfo;
	DISP_SORT 	*pDispSort;
	/* ＢＭＰ表示バッファから仕鳎ū硎兢先位が高い方から） */
	//for( i = DispBuffer.DispCnt - 1; i >= 0 ; i--, pDispSort-- ){
	for( i = 0 ; i < DispBuffer.DispCnt ; i++ ){
	
		//pDispInfo = DispBuffer.DispInfo[ i ];
		//pDispSort = DispBuffer.DispSort[ i ];
		pDispInfo = DispBuffer.DispInfo + i;
		pDispSort = DispBuffer.DispSort + i;
		
		// フラグが立っていたら
		if( pDispInfo->hitFlag == no ){
			// メニュ`より上だったら
			if( pDispSort->dispPrio >= DISP_PRIO_MENU ){
				// ボックス表示デ`タをバッファに溜める
				StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
									pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
									pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
									DISP_PRIO_BOX2, BoxColor, 0 );
			}else{
				// ボックス表示デ`タをバッファに溜める
				StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
									pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
									pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
									DISP_PRIO_BOX, BoxColor, 0 );
									//pDispSort->dispPrio, 250, 0 );
			}
		}
	}
}

#if 0
// 表示ボックスのカラ`テ`ブル
int BoxColorTbl[] = { 	
						97,97,97,97,97,
						98,98,98,98,98,
						99,99,99,99,99,
						100,100,100,100,100,
						101,101,101,101,101,
						102,102,102,102,102,
						103,103,103,103,103,
						104,104,104,104,104,
						105,105,105,105,105,
						106,106,106,106,106,
						107,107,107,107,107,
						//108,108,108,108,108,
						//109,109,109,109,109,
						//110,110,110,110,110,
						//111,111,111,111,111,
						//110,110,110,110,110,
						//109,109,109,109,109,
						//108,108,108,108,108,
						//107,107,107,107,107,
						106,106,106,106,106,
						105,105,105,105,105,
						104,104,104,104,104,
						103,103,103,103,103,
						102,102,102,102,102,
						101,101,101,101,101,
						100,100,100,100,100,
						99,99,99,99,99,
						98,98,98,98,98,
						};
#endif
#if 1
// 表示ボックスのカラ`テ`ブル
UCHAR BoxColorTbl[] = { 	
						//255, 255, 255, 255, 255,
						//8,8,8,8,8,
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						250,250,250,250,250, 
						15,15,15,15,15,
						2,2,2,2,2, 
						15,15,15,15,15, 
						//250,250,250,250,250, 
						//8,8,8,8,8,
					};
#else
UCHAR BoxColorTbl[] = { 	
						255,255,255,255,255,255,255,255,255,255,
						255,255,255,255,255,255,255,255,255,255,
						7,7,7,7,7,7,7,7,7,7,
						248,248,248,248,248,248,248,248,248,248,
						0,0,0,0,0,0,0,0,0,0,
						248,248,248,248,248,248,248,248,248,248,
						7,7,7,7,7,7,7,7,7,7,
					};
#endif

/* マウスカ`ソルのあたり判定 **************************************************/
void HitMouseCursor( void )
{	
	int i;					// ル`プカウンタ
	int strWidth;			// 文字列のドット数
	int hitFlag = FALSE;	// 当たり判定フラグ
	static int cnt = 0;		// カウンタ`
	int itemNameColor = FONT_PAL_WHITE;	// アイテムの色
	
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt - 1;
	DISP_INFO 	*pDispInfo;
	//char moji[ 256 ];
	
	// 表示ボックスの色涓
	if( cnt >= sizeof( BoxColorTbl ) - 1 ) cnt = 0;
	else cnt++;
	
	// 表示ボックスの色O定
	BoxColor = BoxColorTbl[ cnt ];
	
	
	/* フォント表示バッファから仕鳎ē僵`トされないのでどこからても同じ）*/
	for( i = 0 ; i < FontCnt ; i++ ){
	
		// 当たり判定しないr
		if( FontBuffer[ i ].hitFlag == 0 ) continue;
		
		// 文字列の横幅（ドット数）を{べる
		strWidth = GetStrWidth( FontBuffer[ i ].str );
		// 四角のあたり判定
		if( mouse.nowPoint.x <= FontBuffer[ i ].x + strWidth + 2 &&
			FontBuffer[ i ].x - 2 <= mouse.nowPoint.x &&
			mouse.nowPoint.y <= FontBuffer[ i ].y + FONT_SIZE  + 2 &&
			FontBuffer[ i ].y - 2 <= mouse.nowPoint.y ){
			
			// 当たった配列の番号をえる
			HitFontNo = i;
			
			// ボックスを表示するr
			if( FontBuffer[ i ].hitFlag == 2 ){
				// ボックス表示デ`タをバッファに溜める
				StockBoxDispBuffer( FontBuffer[ i ].x - 3, FontBuffer[ i ].y - 4, 
									FontBuffer[ i ].x + strWidth + 2, 
									FontBuffer[ i ].y + FONT_SIZE + 4, 
									//DISP_PRIO_BOX2, 250, 0 );
									DISP_PRIO_BOX2, BoxColor, 0 );
									
				SortDispBuffer(); 	// 表示バッファソ`ト
			}					
			// 当たった配列の番号を初期化する
			HitDispNo = -1;
			// マウスレベル
			mouse.level = DISP_PRIO_MENU;
			return;		// フォントに当たったrＢＭＰの判定はしない
		}
	}
	// 当たった配列の番号を初期化する
	HitFontNo = -1;
	
	/* ＢＭＰ表示バッファから仕鳎ū硎兢先位が高い方から） */
	for( i = DispBuffer.DispCnt - 1; i >= 0 ; i--, pDispSort-- ){
		// 表示デ`タの入っているアドレスにセット
		pDispInfo = DispBuffer.DispInfo + pDispSort->no;
		
		// 当たり判定しないr
		if( pDispInfo->hitFlag == 0 ) continue;
		
		// 相手のＢＭＰがなかったら@A
		//if( SpriteInfo[ pDispInfo->bmpNo ].lpSurfaceInfo == NULL ) continue;
		// ＶＲＡＭにいないときはハ`ドディスクからロ`ドする
		// ＢＭＰをロ`ドする
		if( LoadBmp( pDispInfo->bmpNo ) == FALSE ) continue;
		
		// アクションでないr
		if( pDispInfo->pAct == NULL ){
			// 四角のあたり判定
			if( mouse.nowPoint.x <= pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width &&
				pDispInfo->x <= mouse.nowPoint.x &&
				mouse.nowPoint.y <= pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height &&
				pDispInfo->y <= mouse.nowPoint.y )
				
				hitFlag = TRUE;
		}else{
			// 四角のあたり判定（アクション用）
			if( mouse.nowPoint.x <= pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width * 0.5 + MOUSE_HIT_SIZE_X * 0.5 &&
				pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width * 0.5 - MOUSE_HIT_SIZE_X * 0.5  <= mouse.nowPoint.x &&
				mouse.nowPoint.y <= pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height &&
				pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height - MOUSE_HIT_SIZE_Y <= mouse.nowPoint.y )
				
				hitFlag = TRUE;
		}
				
		// 当たっていたら
		if( hitFlag == TRUE ){
			// 文字表示バッファに溜める
//			StockFontBuffer( pDispInfo->x, pDispInfo->y, FONT_PRIO_FRONT, 0, "Hit", 1 );
			
			// 当たった配列の番号を
			HitDispNo = pDispSort->no;
			
			// ボックスを表示するr
			if( pDispInfo->hitFlag >= 2 ){
				// メニュ`より上だったら
				if( pDispSort->dispPrio >= DISP_PRIO_YES_NO_WND ){
					// ボックス表示デ`タをバッファに溜める
					StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
										pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
										pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
										//DISP_PRIO_BOX2, 250, 0 );
										DISP_PRIO_BOX3, BoxColor, 0 );
				}else
				// メニュ`より上だったら
				if( pDispSort->dispPrio >= DISP_PRIO_MENU ){
					// ボックス表示デ`タをバッファに溜める
					StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
										pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
										pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
										//DISP_PRIO_BOX2, 250, 0 );
										DISP_PRIO_BOX2, BoxColor, 0 );
				}else{
					// ボックス表示デ`タをバッファに溜める
					StockBoxDispBuffer( pDispInfo->x - 2, pDispInfo->y - 2, 
										pDispInfo->x + SpriteInfo[ pDispInfo->bmpNo ].width + 2, 
										pDispInfo->y + SpriteInfo[ pDispInfo->bmpNo ].height + 2, 
										//DISP_PRIO_BOX, 250, 0 );
										DISP_PRIO_BOX, BoxColor, 0 );
										//pDispSort->dispPrio, 250, 0 );
				}
				
				// グル`プxkチェック（}数xk） 
				if( pDispInfo->hitFlag >= 3 ) CheckGroupSelect( pDispInfo->hitFlag );
				
				SortDispBuffer(); 	// 表示バッファソ`ト
			}
			
			// タスクバ`がないr
			if( TaskBarFlag == FALSE ){
				// アクションポインタがあるr
				if( pDispInfo->pAct != NULL ){
					// 一行インフォ表示ＯＮのr
					if( pDispInfo->pAct->atr & ACT_ATR_INFO ){
						// フィ`ルド上のr
						if( ProcNo == PROC_GAME ){
							// 文字列作成
							//sprintf( OneLineInfoStr,"%s　等级：%d　耐力：%4d/%4d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
							// 一行インフォ表示ＯＮのr
							// プレイヤ`のr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_PC ){
								// 称号あるr
								if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
									sprintf( OneLineInfoStr,"%s [%s] 等级：%d　耐力：%d／%d　气力：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp );
								}else{
									sprintf( OneLineInfoStr,"%s　等级：%d　耐力：%d／%d　气力：%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pc.mp );
								}
							}else
							// 他のプレイヤ`のr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_OTHER_PC ){
								// 称号あるr
								if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
									sprintf( OneLineInfoStr,"%s [%s]　等级：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level );
								}else{
									sprintf( OneLineInfoStr,"%s　等级：%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
								}
							}else
							// ペットのr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_PET ){
								// 称号あるr
								if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
									sprintf( OneLineInfoStr,"%s　等级：%d", pDispInfo->pAct->freeName, pDispInfo->pAct->level );
								}else{
									sprintf( OneLineInfoStr,"%s　等级：%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
								}
							}else
							// アイテムのr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_ITEM ){
								// 名前の色O定
								itemNameColor = pDispInfo->pAct->itemNameColor;
								sprintf( OneLineInfoStr,"%s", pDispInfo->pAct->name );
							}else
							// お金のr
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_GOLD ){
								sprintf( OneLineInfoStr,"%s", pDispInfo->pAct->name );
							}else
							// その他のr（ドア、町の人、看板）
							if( pDispInfo->pAct->atr & ACT_ATR_TYPE_OTHER ){
								sprintf( OneLineInfoStr,"%s", pDispInfo->pAct->name );
							}
						}else
						// 殛Lのr
						if( ProcNo == PROC_BATTLE ){
							// Q楗猢`ドじゃないのr
							if( BattleMyNo < 20 ){
								// 自分ならＨＰとＭＰ表示
								if( ( (ATR_EQU *)pDispInfo->pAct->pYobi )->place_no == BattleMyNo ){
									// 称号あるr
									if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
										//sprintf( OneLineInfoStr,"%s [%s]　等级：%d　耐力：%d／%d　气力：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, BattleMyMp );
										sprintf( OneLineInfoStr,"%s [%s]　等级：%d　耐力：%d／%d　气力：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pDispInfo->pAct->mp );
									}else{
										//sprintf( OneLineInfoStr,"%s　等级：%d　耐力：%d／%d　气力：%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, BattleMyMp );
										sprintf( OneLineInfoStr,"%s　等级：%d　耐力：%d／%d　气力：%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp, pDispInfo->pAct->mp );
									}
								}
								else
								// 味方ならＨＰ表示
								if( ( (ATR_EQU *)p_party[ BattleMyNo ]->pYobi )->group_flg == ( (ATR_EQU *)pDispInfo->pAct->pYobi )->group_flg ){
									// 文字列作成
									//sprintf( OneLineInfoStr,"%s　等级：%d　耐力：%4d/%4d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									// 称号あるr
									if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
										sprintf( OneLineInfoStr,"%s [%s]　等级：%d　耐力：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									}else{
										sprintf( OneLineInfoStr,"%s　等级：%d　耐力：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									}
								}else{
									// 称号あるr
									if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
										sprintf( OneLineInfoStr,"%s [%s]　等级：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level );
									}else{
										sprintf( OneLineInfoStr,"%s　等级：%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
									}
								}
							}else{	// Q楗猢`ドのr
								// 称号あるr
								if( pDispInfo->pAct->freeName[ 0 ] != NULL ){
									//sprintf( OneLineInfoStr,"%s [%s]　等级：%d　耐力：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									sprintf( OneLineInfoStr,"%s [%s]　等级：%d", pDispInfo->pAct->name, pDispInfo->pAct->freeName, pDispInfo->pAct->level );
								}else{
									//sprintf( OneLineInfoStr,"%s　等级：%d　耐力：%d／%d", pDispInfo->pAct->name, pDispInfo->pAct->level, pDispInfo->pAct->hp, pDispInfo->pAct->maxHp );
									sprintf( OneLineInfoStr,"%s　等级：%d", pDispInfo->pAct->name, pDispInfo->pAct->level );
								}
							}
						}
					}
				}
				// 一行インフォ表示
				StockFontBuffer( 8, 460, FONT_PRIO_FRONT, itemNameColor, OneLineInfoStr, 0 );
			}
			// 文字列初期化
			OneLineInfoStr[ 0 ] = NULL;
			
			// マウスレベル
			mouse.level = pDispSort->dispPrio;
			return;
		}
	}
	// マウスレベル
	mouse.level = DISP_PRIO_TILE;
	// 当たった配列の番号を初期化する
	HitDispNo = -1;
	
	// タスクバ`がないr
	if( TaskBarFlag == FALSE ){
		// 一行インフォ表示
		StockFontBuffer( 8, 460, FONT_PRIO_FRONT, itemNameColor, OneLineInfoStr, 0 );
	}
	// 文字列初期化
	OneLineInfoStr[ 0 ] = NULL;
	
}


