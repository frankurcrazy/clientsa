/************************/
/*	sprdisp.c	人物显示		*/
/************************/
#include "system.h"
#include "loadrealbin.h"
#include "loadsprbin.h"
#include "anim_tbl.h"
#include "map.h"

#define STOCK_DISP_BUFFER_NO_BMP	( 1 << 31 ) 	// ＢＭＰでないr
#define STOCK_DISP_BUFFER_LINE		( 1 << 30 ) 	// ライン
#define STOCK_DISP_BUFFER_BOX		( 1 << 29 ) 	// ボックス
#define STOCK_DISP_BUFFER_BOX_FILL	( 1 << 28 )		// ボックスTりつぶし
#define STOCK_DISP_BUFFER_CIRCLE	( 1 << 27 ) 	// 

// 表示バッファ造体
DISP_BUFFER DispBuffer;

// ＢＭＰのイメ`ジデ`タまでのオフセット
int BmpOffBits;
// ＢＭＰiみzみ用ワ`クI域へのポインタ
LPBITMAPINFO lpBmpInfo;
// Realbin iみzみ用ワ`クI域へのポインタ
char *pRealBinBits;
// 今回 Realbin からiみzむＢＭＰのサイズ
int RealBinWidth, RealBinHeight;
// BitBltのr
HBITMAP	hBmp;

// サ`フェスヴィジ`フラグ
int SurfaceBusyFlag = 0;

// ウィンドウポインタ
extern ACTION* pActMenuWnd2;
// トグルフラグ
extern unsigned int MenuToggleFlag;

// ソ`ト比^v数
int SortComp( DISP_SORT *pDisp1, DISP_SORT *pDisp2 );
typedef int CMPFUNC( const void *, const void * );
// 表示バッファ`からボックスデ`タを取り出す
void GetBoxDispBuffer( DISP_INFO *pDispInfo, int bmpNo );

// 表示バッファソ`ト /////表示排序缓冲区//////////////////////////////////////////////////////
void SortDispBuffer( void )
{	
	//クイックソ`ト 快速排序
	qsort( 	DispBuffer.DispSort,	// 造体のアドレス 结构的地址
			DispBuffer.DispCnt,		// 比^する数
			sizeof( DISP_SORT ), 	// 造体のサイズ 结构尺寸
			( CMPFUNC * )SortComp 	// 比^v数へのポインタ 比较函数的指针
		);				
}

// ＢＭＰの描画 ///设定载入- -//////////////////////////////////////////////////////////////
void PutBmp( void )
{
	SURFACE_INFO *lpSurfaceInfo;
	DISP_SORT 	*pDispSort = DispBuffer.DispSort;
	DISP_INFO 	*pDispInfo;
	int i;
	int bmpNo;
	UCHAR putTextFlag = 0; // フォントを描画したかフラグ 字体的标志 字体绘制了转折点
	unsigned char drawMapEffectFlag = 0;
	
	// 溜められた数だけル`プ  积蓄的次数循环
	for( i = 0; i < DispBuffer.DispCnt ; i++, pDispSort++ ){
		// 表示デ`タの入っているアドレスにセット
		pDispInfo = DispBuffer.DispInfo + pDispSort->no;
		bmpNo = pDispInfo->bmpNo; // ＢＭＰ番号
		
		// フォントの表示( BACK )
		if( putTextFlag == 0 ){
			// メニュ`の前だったら
			if( pDispSort->dispPrio >= DISP_PRIO_MENU ){
				// フォントをバックサ`フェスにセット
				PutText( FONT_PRIO_BACK );	
				// フラグＯＮ
				putTextFlag = 1;	
			}
		}
		// フォントの表示( FRONT )
		if( putTextFlag == 1 ){
			// はい、いいえウィンドウの前だったら
			if( pDispSort->dispPrio >= DISP_PRIO_YES_NO_WND ){
				// フォントをバックサ`フェスにセット
				PutText( FONT_PRIO_FRONT );	
				// フラグＯＮ
				putTextFlag = 2;	
			}
		}
		
		// メニュ`Kわったら
		if( pDispSort->dispPrio > DISP_PRIO_MENU ){
			// マップウィンドウあるr
			if( MenuToggleFlag & JOY_CTRL_M && pActMenuWnd2 != NULL ){
				// ウィンドウ出来あがっていたら
				if( pActMenuWnd2->hp > 0 ){
					// オ`トマップ描画
					drawAutoMap( pActMenuWnd2->x, pActMenuWnd2->y );
				}
			}
		}

		// マップのエフェクト
		if( pDispSort->dispPrio >= DISP_PRIO_RESERVE )
		{
			drawMapEffect();
			drawMapEffectFlag = 1;
		}


		// ＢＭＰ以外のr
		if( bmpNo & STOCK_DISP_BUFFER_NO_BMP ){
			// ボックス、またはラインのr
			if( bmpNo & ( STOCK_DISP_BUFFER_BOX | STOCK_DISP_BUFFER_BOX_FILL | STOCK_DISP_BUFFER_LINE ) ){
				// 表示バッファ`からボックスデ`タを取り出す
				GetBoxDispBuffer( pDispInfo, bmpNo );
			}
			continue;
		}
		
		// ＢＭＰのr
		// 当たり判定のみする（非表示）
		if( pDispInfo->pAct != NULL ){
			if( pDispInfo->pAct->atr & ACT_ATR_HIDE2 ) continue;
		}
		// ＶＲＡＭにいないときはハ`ドディスクからロ`ドする
		// ＢＭＰをロ`ドする
		if( LoadBmp( bmpNo ) == FALSE ) continue;
		
		// サ`フェスリストの数だけル`プ
		for(lpSurfaceInfo = SpriteInfo[ bmpNo ].lpSurfaceInfo ;
			lpSurfaceInfo != NULL;
			lpSurfaceInfo = lpSurfaceInfo->pNext ){
				
			// クリッピングI理とオフスクリ`ンサ`フェ`スからバックサ`フェスに高速送
			//３２０＊２４０ドットモ`ドのr
			if(ResoMode == 1){
				//DrawSurfaceFast( 	pDispInfo->x/2 + lpSurfaceInfo->offsetX, 
				//					pDispInfo->y/2 + lpSurfaceInfo->offsetY, 
				//					lpSurfaceInfo->lpSurface );
				if( DrawSurfaceFast( 	pDispInfo->x/2 + lpSurfaceInfo->offsetX, 
									pDispInfo->y/2 + lpSurfaceInfo->offsetY, 
									lpSurfaceInfo->lpSurface ) == DDERR_SURFACEBUSY ) SurfaceBusyFlag = TRUE;
			} else {
				//DrawSurfaceFast( 	pDispInfo->x + lpSurfaceInfo->offsetX, 
				//					pDispInfo->y + lpSurfaceInfo->offsetY, 
				//					lpSurfaceInfo->lpSurface );
				if( DrawSurfaceFast( 	pDispInfo->x + lpSurfaceInfo->offsetX, 
									pDispInfo->y + lpSurfaceInfo->offsetY, 
									lpSurfaceInfo->lpSurface ) == DDERR_SURFACEBUSY ) SurfaceBusyFlag = TRUE;
					
			}
			
			// サ`フェスを使用した日付を
			lpSurfaceInfo->date = SurfaceDate;
		}
	}
	
	// フォント表示もれ防止
	// フォントの表示( BACK )
	if( putTextFlag == 0 ){
		// フォントをバックサ`フェスにセット
		PutText( FONT_PRIO_BACK );	
		// フラグＯＮ
		putTextFlag = 1;	
	}
	// フォントの表示( FRONT )
	if( putTextFlag == 1 ){
		// フォントをバックサ`フェスにセット
		PutText( FONT_PRIO_FRONT );	
	}
	// マップのエフェクト
	if( drawMapEffectFlag == 0 )
	{
		drawMapEffect();
	}
}

//**************************************************************************/
// 	概要：	Rr表示デ`タをバッファに溜める
// 	引数：	UCHAR dispPrio：表示のプライオリティ
//		  	int x, int y：表示座
//			int bmpNo：ＢＭＰ番号
//			int chr_no：キャラクタ`番号
//			int pat_no：パタ`ン番号

/// /概要：临时显示数据蓄积缓冲
///参数：UCHAR dispPrio：表示的优先级/ / 
///int x，y表示：int坐标/ / 
///int bmpNo：BMP号码/ / 
///int chr _ no：角色号码/ / int pat _ no：模式号码
//**************************************************************************/
int StockDispBuffer( int x, int y, UCHAR dispPrio, int bmpNo, BOOL hitFlag )
{	
	short dx,dy;
	int BmpNo;
	// 今回保存する鏊蓼钎丧欹工蜻Mませる  这次保存进展到地址
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	
	// カウントオ`バ`のr  倒计时结束的时候
	if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) return -2;
	
	// ＢＭＰがセットされてないとき  设置的时候
	if( -1 <= bmpNo && bmpNo <= CG_INVISIBLE ) return -2;
	
	// 通常表示のとき
	if( bmpNo > CG_INVISIBLE )
	{
		realGetNo( bmpNo , (U4 *)&BmpNo );
		realGetPos( BmpNo  , &dx, &dy);
	}
	else
	{
		// ボックス表示のときはQしない  箱表示的时候不转换
		dx = 0;
		dy = 0;
		BmpNo = bmpNo;
	}

	// 表示先ソ`ト用造体
	pDispSort->dispPrio = dispPrio;
	pDispSort->no = DispBuffer.DispCnt;
	// 表示情造体（ ソ`トしない内容 ）
	pDispInfo->x = x + dx;
	pDispInfo->y = y + dy;
	pDispInfo->bmpNo = BmpNo;
	pDispInfo->hitFlag = hitFlag;
	pDispInfo->pAct = NULL;

	// 表示カウンタ`プラス
	return DispBuffer.DispCnt++;
}

//**************************************************************************/
// 	概要：	Rr表示デ`タをバッファに溜める
// 	引数：	UCHAR dispPrio：表示のプライオリティ
//		  	int x, int y：表示座
//			int bmpNo：ＢＭＰ番号(realGetNo()をしない)
//			int chr_no：キャラクタ`番号
//			int pat_no：パタ`ン番号
//**************************************************************************/
int StockDispBuffer2( int x, int y, UCHAR dispPrio, int bmpNo, BOOL hitFlag )
{	
	short dx,dy;
	int BmpNo;
	// 今回保存する鏊蓼钎丧欹工蜻Mませる
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	
	// カウントオ`バ`のr
	if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) return -2;
	
	// ＢＭＰがセットされてないとき
	if( -1 <= bmpNo && bmpNo <= CG_INVISIBLE ) return -2;
	
	// 通常表示のとき
	if( bmpNo > CG_INVISIBLE ){
		BmpNo = bmpNo;
		realGetPos( BmpNo  , &dx, &dy);
	}else{
		// ボックス表示のときはQしない
		dx = 0;
		dy = 0;
		BmpNo = bmpNo;
	}

	// 表示先ソ`ト用造体
	pDispSort->dispPrio = dispPrio;
	pDispSort->no = DispBuffer.DispCnt;
	// 表示情造体（ ソ`トしない内容 ）
	pDispInfo->x = x + dx;
	pDispInfo->y = y + dy;
	pDispInfo->bmpNo = BmpNo;
	pDispInfo->hitFlag = hitFlag;
	pDispInfo->pAct = NULL;

	// 表示カウンタ`プラス
	return DispBuffer.DispCnt++;
}

// タスク表示デ`タをバッファに溜める ///////////////////////////////////////////////////
void StockTaskDispBuffer( void )
{
	ACTION *pActLoop; 	/* 先^のリストポインタ取得 */
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	
	/* 最後尾が来るまでル`プ  最后来之前循环*/	
	//while( pActLoop != pActBtm ){
	for(pActLoop = pActTop->pNext ; 
		pActLoop != pActBtm ;
		/* 次のポインタをセット */
		pActLoop = pActLoop->pNext )
		{
		
		// カウントオ`バ`のr
		if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) break;
		
		// ＢＭＰがセットされてないとき 设置的时候
		if( -1 <= pActLoop ->bmpNo && pActLoop ->bmpNo <= CG_INVISIBLE ) continue;
		
		// 死亡フラグが立っているとき
		if( pActLoop ->deathFlag == TRUE ) continue;
		
		/* 非表示ならiける */
		if( pActLoop->atr & ACT_ATR_HIDE ) continue;

		// 表示先ソ`ト用造体
		pDispSort->dispPrio = pActLoop->dispPrio;
		pDispSort->no = DispBuffer.DispCnt;
		// 表示情造体（ ソ`トしない内容 ）
		pDispInfo->x = pActLoop->x + pActLoop->anim_x;
		pDispInfo->y = pActLoop->y + pActLoop->anim_y;
		pDispInfo->bmpNo = pActLoop->bmpNo;
		pDispInfo->pAct = pActLoop;
		
		// 当たり判定するか？
		// }数xkのr
		if( pActLoop->atr & ACT_ATR_HIT_BOX_ALL3 ) pDispInfo->hitFlag = 5;
		else
		if( pActLoop->atr & ACT_ATR_HIT_BOX_ALL2 ) pDispInfo->hitFlag = 4;
		else 
		if( pActLoop->atr & ACT_ATR_HIT_BOX_ALL1 ) pDispInfo->hitFlag = 3;
		else // ボックス表示
		if( pActLoop->atr & ACT_ATR_HIT_BOX ) pDispInfo->hitFlag = 2;
		else // 判定のみ
		if( pActLoop->atr & ACT_ATR_HIT ) pDispInfo->hitFlag = 1;
		else pDispInfo->hitFlag = FALSE;
		// マウスカ`ソル当たり判定番号
		pActLoop->hitDispNo = DispBuffer.DispCnt;
		
		// 表示カウンタ`プラス
		DispBuffer.DispCnt++;
		// アドレスプラス
		pDispSort++;
		pDispInfo++;
	}
	
}

// タスク表示デ`タをバッファに溜める ///////////////////////////////////////////////////
// ただしprio1からprio2までのものはI理しない
void StockTaskDispBuffer2( int prio1, int prio2 )
{
	ACTION *pActLoop; 	/* 先^のリストポインタ取得 */
	DISP_SORT 	*pDispSort = DispBuffer.DispSort + DispBuffer.DispCnt;
	DISP_INFO 	*pDispInfo = DispBuffer.DispInfo + DispBuffer.DispCnt;
	int tmp;
	
	if( prio1 > prio2 )
	{
		tmp = prio1;
		prio1 = prio2;
		prio2 = tmp;
	}
	
	/* 最後尾が来るまでル`プ */	
	//while( pActLoop != pActBtm ){
	for(pActLoop = pActTop->pNext ; 
		pActLoop != pActBtm ;
		/* 次のポインタをセット */
		pActLoop = pActLoop->pNext ){
		
		// カウントオ`バ`のr
		if( DispBuffer.DispCnt >= DISP_BUFFER_SIZE ) break;
		
		// prio1からprio2の指定欷I理しない
		if( prio1 <= pActLoop->dispPrio && pActLoop->dispPrio <= prio2 )
			continue;
		
		// ＢＭＰがセットされてないとき
		if( -1 <= pActLoop ->bmpNo && pActLoop ->bmpNo <= CG_INVISIBLE ) continue;
		
		// 死亡フラグが立っているとき
		if( pActLoop ->deathFlag == TRUE ) continue;
		
		/* 非表示ならiける */
		if( pActLoop->atr & ACT_ATR_HIDE ) continue;

		// 表示先ソ`ト用造体
		pDispSort->dispPrio = pActLoop->dispPrio;
		pDispSort->no = DispBuffer.DispCnt;
		// 表示情造体（ ソ`トしない内容 ）
		pDispInfo->x = pActLoop->x + pActLoop->anim_x;
		pDispInfo->y = pActLoop->y + pActLoop->anim_y;
		pDispInfo->bmpNo = pActLoop->bmpNo;
		pDispInfo->pAct = pActLoop;
		
		// 当たり判定するか？
		if( pActLoop->atr & ACT_ATR_HIT_BOX ) pDispInfo->hitFlag = 2;
		else 
		if( pActLoop->atr & ACT_ATR_HIT ) pDispInfo->hitFlag = 1;
		else pDispInfo->hitFlag = FALSE;
		// マウスカ`ソル当たり判定番号
		pActLoop->hitDispNo = DispBuffer.DispCnt;
		
		// 表示カウンタ`プラス
		DispBuffer.DispCnt++;
		// アドレスプラス
		pDispSort++;
		pDispInfo++;
	}
	
}

// ボックス表示デ`タをバッファに溜める ***************************************/
void StockBoxDispBuffer( int x1, int y1, int x2, int y2, UCHAR dispPrio, int color, BOOL fill )
{
	int col;	// 色の指定
	// int に short を２いれる
	int x = ( x1 << 16 ) | x2; 	// 上位２バイトと下位２バイトにいれる
	int y = ( y1 << 16 ) | y2;	// 上位２バイトと下位２バイトにいれる
	
	// ボックスのとき
	if( fill == 0 ) col = color | STOCK_DISP_BUFFER_BOX | STOCK_DISP_BUFFER_NO_BMP ;
	// Tりつぶしのとき
	else if( fill == 1 ) col = color | STOCK_DISP_BUFFER_BOX_FILL | STOCK_DISP_BUFFER_NO_BMP ;
	// ラインのr
	else if( fill == 2 ) col = color | STOCK_DISP_BUFFER_LINE | STOCK_DISP_BUFFER_NO_BMP ;
	
	// ボックスをバッファに溜める
	StockDispBuffer( x, y, dispPrio, col, 0 );
}

// 表示バッファ`からボックスデ`タを取り出す *********************************/
void GetBoxDispBuffer( DISP_INFO *pDispInfo, int bmpNo )
{
	// I域のセット
	RECT rect;
	int color; // 色
	BOOL fill = FALSE; // Tりつぶしフラグ
	int DispInfoX = pDispInfo->x;
	int DispInfoY = pDispInfo->y;
	
	// 座摔蛉・瓿訾
	rect.right =	( LONG )( pDispInfo->x & 0xffff );
	rect.bottom =	( LONG )( pDispInfo->y & 0xffff );
	
	//rect.left = 	( LONG )( ( pDispInfo->x >> 16 ) & 0xffff );
	//rect.top = 		( LONG )( ( pDispInfo->y >> 16 ) & 0xffff );
	
	// インラインアセンブラ
	_asm{
		// rect.left を取り出す
		mov		cl,16				// シフト数１６をセット
		mov		eax, [ DispInfoX ]
		sar		eax, cl				// 符号付右シフト
		mov		[ rect.left ], eax
		// rect.top を取り出す
		mov		eax, [ DispInfoY ]
		sar		eax, cl
		mov		[ rect.top ], eax
	}

	
	// ボックスのとき
	if( bmpNo & STOCK_DISP_BUFFER_BOX ){ 
		color = bmpNo & 0xff; 			// 色デ`タを取り出す
		DrawBox( &rect, color, 0 );	// ボックス描画
		
	// Tりつぶしのとき
	}else if( bmpNo & STOCK_DISP_BUFFER_BOX_FILL ){ 
		color = color = bmpNo & 0xff;	// 色指定
		DrawBox( &rect, color, 1 );	// ボックス描画
		
	}else if( bmpNo & STOCK_DISP_BUFFER_LINE ){ 
		color = color = bmpNo & 0xff;	// 色指定
		DrawBox( &rect, color, 2 );	// ライン描画
	}
}			

// ソ`ト比^v数 //////////////////////////////////////////////////////////////
int SortComp( DISP_SORT *pDisp1, DISP_SORT *pDisp2 )
{
	// pDisp1 の表示先度の方が大きいr、入れ替え
	if( pDisp1->dispPrio > pDisp2->dispPrio ){
		return 1;
	}
	
	// pDisp2 の表示先度の方が大きいr、そのまま
	if( pDisp1->dispPrio < pDisp2->dispPrio ){
		return -1;
	}
	// 等しいrは登hした
	// pDisp1 の方が早かったr、そのまま（のはず？）
	if( pDisp1->no > pDisp2->no ){
		return -1;
	}
	// どれでもないr、入れ替え（のはず？）
	return 1;
}

