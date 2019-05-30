/************************/
/*	battleMap.c			*/
/************************/
#include "system.h"
#include "ime_sa.h"
#include "battleMap.h"

// バトルマップのサイズ
#define BATTLE_MAP_SIZE 400

// F在のバトルマップ番号
int BattleMapNo = 0;

// バトルマップ ファイル名
char *BattleMapFile[] = {	
	#include "battleMapName.h"
};
						
// バトルマップiみzみとバトルサ`フェスの画像作成 ***************************/
BOOL ReadBattleMap( int no )
{
	char msg[ 256 ];
	FILE *fp;
	char header[ 5 ];
	unsigned short tile[ BATTLE_MAP_SIZE ];
	int cnt = 0;
	int i, j, x = 0, y = 0, posX, posY;
	unsigned short c1, c2;
	
	// リミットチェック
	if( no >= BATTLE_MAP_FILES ) no = 0;
	
	// バトルマップ番号
	BattleMapNo = no;
	
	// バトルマップファイルオ`プン
	if( ( fp = fopen( BattleMapFile[ no ], "rb" ) ) == NULL )
	{
		sprintf( msg, "%s のiみzみに失・筏蓼筏俊", BattleMapFile[ no ] );
		MessageBox( hWnd, msg, "バトルマップエラ`", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	// ヘッダ`のiみzみ
	fread( header, sizeof( char ), 4, fp );
	header[ 4 ] = '\0';
	
	// ヘッダ`チェック
	if( !( strstr( header, "SAB" )) )
	{
		MessageBox( hWnd, "SAB ファイルではありません。", "バトルマップエラ`", MB_OK | MB_ICONSTOP );
	}
	
	// ポインタMませる（ ヘッダwばし ）
	//fseek( fp, 4, SEEK_SET );
	
	// タイルデ`タのiみzみ
	//fread( tile, sizeof( short ), BATTLE_MAP_SIZE, fp );
	
	//fread( tile, 1, 800, fp );
	
	// iみzみル`プ
	for( i = 0 ; i < BATTLE_MAP_SIZE ; i++ )
	{
		// １バイトずつiみzむ（ デ`タ造が逆のため ）
		c1 = fgetc( fp );
		c2 = fgetc( fp );
		
		// ２バイトにする
		tile[ i ] = ( c1 << 8 ) | c2;
	}
	// ファイル]じる
	fclose( fp );
	
	// 位置O定
	posX = 32 * ( -9 );
	posY = 24 * 10;
	
	// タイル分ル`プ
	for( i = 0 ; i < 20 ; i++ ){
		x = 0;
		y = 0;
		for( j = 0 ; j < 20 ; j++ ){
			// ＢＭＰ表示バッファにためる
			StockDispBuffer( posX + x, posY + y, DISP_PRIO_TILE, tile[ cnt++ ], 0 );
			
			x += 32;
			y -= 24;
		}
		posX += 32;
		posY += 24;
	}
	return TRUE;
}	
	
// バトルマップ描画 ***********************************************************/
#define	RASTER_CLEARANCE	8
int piyo_tbl[]={
	0,1,2,3,4,5,6,7,8,9,10,10,11,11,11,12,
	12,12,11,11,11,10,9,9,8,7,6,5,4,3,2,1,
	0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-10,-11,-11,-11,-12,
	-12,-12,-11,-11,-11,-10,-9,-9,-8,-7,-6,-5,-4,-3,-2,-1,
};
int piyo_point = 0;

void DrawBattleMap( void )
{
	RECT rect = { 0, 0, DEF_APPSIZEX, DEF_APPSIZEY };
	int d0,d1,d6,d7;
	short bx , by;

	if(BattleMapNo >= 148 && BattleMapNo <= 150){		//ラスタ`スクロ`ルなら
		d1 = 1;
	} else {
		d1 = 0;
	}

//	switch( BattleMapNo & 1 ){
	switch( d1 ){
	case 0:
		//bx = Rnd( 0,10);
		//by = Rnd( 0,10);
		bx = 0;
		by = 0;
		short x0, y0;
		long w, h;

		// 送I域のセット
		x0 = bx;
		y0 = by;
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;

		// クリッピングI理
		//   （ちなみに RECT の右下座摔违丧氓趣媳硎兢丹欷胜ぃ

		// 全く表示する部分がoければ
		if( bx >= lpDraw->xSize || bx + w <= 0 || by >= lpDraw->ySize || by + h <= 0 ){
			return;
		}
		
		// 左端のチェック
		if( bx < 0 ){
			rect.left -= bx;
			x0 = 0;
		}
		// 右端のチェック
		if( bx + w > lpDraw->xSize ){
			rect.right -= bx + w - lpDraw->xSize;
		}
		// 上端のチェック
		if( by < 0 ){
			rect.top -= by;
			y0 = 0;
		}
		// 下端のチェック
		if( by + h > lpDraw->ySize ){
			rect.bottom -= by + h - lpDraw->ySize;
		}
		
#ifdef _DEBUG		
		// F在表示しているサ`フェスの数カウント
		SurfaceDispCnt++;
#endif
		
		// バックサ`フェスへ高速送
		lpDraw->lpBACKBUFFER->BltFast( x0, y0, lpBattleSurface, &rect, DDBLTFAST_WAIT );
		break;

	case 1:
		d6 = 0;
		rect.left = 0;		//左端
		rect.right = 640;		//右端
		for(d7=0; d7<480; d7+=RASTER_CLEARANCE){
			rect.top = d7 + piyo_tbl[(d6+piyo_point) & 63] + 12;		//上端
			rect.bottom = rect.top + RASTER_CLEARANCE;		//下端
			// バックサ`フェスへ高速送
			lpDraw->lpBACKBUFFER->BltFast( 0, d7, lpBattleSurface, &rect, DDBLTFAST_WAIT );
			d6++;
		}
		break;

	case 2:
		d6 = 0;
		for(d7=0; d7<480; d7+=RASTER_CLEARANCE/2){
			rect.top = d7;		//上端
			rect.bottom = rect.top + RASTER_CLEARANCE;		//下端

			d0 = piyo_tbl[(d6+piyo_point) & 63];		//左端
			if(d0 < 0){		//マイナスなら
				rect.left = 0 - d0;
				rect.right = 640 + d0;		//右端
				d0 = 0;
			} else {
				rect.left = 0;
				rect.right = 640 - d0;		//右端
			}
			// バックサ`フェスへ高速送
			lpDraw->lpBACKBUFFER->BltFast( d0, d7, lpBattleSurface, &rect, DDBLTFAST_WAIT );
			d6++;
		}
		break;
	}
}



