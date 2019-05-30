/************************/
/*	sprmgr.c			*/
/************************/
#include "system.h"
#include "loadrealbin.h"
#include "loadsprbin.h"

//#define SURACE_INFO_MAX 512 	// サ`フェスインフォ造体の最大数
//#define SURACE_INFO_MAX 1024 	// 接口信息结构体的最大数量
#define SURACE_INFO_MAX 2048 	// サ`フェスインフォ造体の最大数
//#define SURACE_INFO_MAX 4096 	// サ`フェスインフォ造体の最大数

//表面和SYSRAM回程的分界线
#define VRAM_SYSRAM_POINT 0 	// VRAM サ`フェスと SYSRAM サ`フェスの境目
//表面上的BMP抹杀被允许为止的天数
#define SURACE_BMP_DEATH_DATE 2 // サ`フェス上のＢＭＰの抹S可されるまでの日数

// スプライトインフォ
SPRITE_INFO SpriteInfo[ MAX_GRAPHICS ];

// サ`フェスインフォ
SURFACE_INFO SurfaceInfo[ SURACE_INFO_MAX ];

// バトルサ`フェスのポインタ
LPDIRECTDRAWSURFACE lpBattleSurface; 	

// サ`フェスを使用した日付   表面使用的日期
UINT SurfaceDate = 3;

// F在のサ`フェス仕魑恢
int SurfaceSearchPoint;

#ifdef _DEBUG		
// F在使っているサ`フェスの数カウント
int SurfaceUseCnt;

// F在表示しているサ`フェスの数カウント 现在显示的回程的数量计数
int SurfaceDispCnt;
#endif

// 作成したサ`フェスの数カウント
int SurfaceCnt;

// VRAM に作成したサ`フェスの数カウント
int VramSurfaceCnt;

// SYSTEMRAM に作成したサ`フェスの数カウント
int SysramSurfaceCnt;

// 分割サ`フェイスの幅
int SurfaceSizeX;
// 分割サ`フェイスの高さ
int SurfaceSizeY;

/* ＢＭＰをサ`フェスへ割り当て  把对表面分配 ***********************************************/
void AllocateBmpToSurface( int bmpNo )
{
	//int bmpWidth  = lpBmpInfo->bmiHeader.biWidth;	// ＢＭＰの横サイズ
	//int bmpHeight = lpBmpInfo->bmiHeader.biHeight; 	// ＢＭＰのkサイズ

	int bmpWidth  = RealBinWidth;	// ＢＭＰの横サイズ
	int bmpHeight = RealBinHeight; 	// ＢＭＰのkサイズ
	
	int offsetX, offsetY; 			// 送るＢＭＰのオフセットポイント  送BMP的坐标点
	int sizeX, sizeY;				// 送するサイズ
	int surfaceCntX;				// 必要なサ`フェスの横枚数
	int surfaceCntY;				// 必要なサ`フェスの横枚数
	int totalSurface;				// 必要なサ`フェスのt数
	int totalSurfaceCnt = 0;		// F在の_保したサ`フェスの枚数（ ル`プカウンタ ）
	//周游检索了结束为了最初的位置记忆
	int	SurfaceSearchPointBak = SurfaceSearchPoint; // 一周仕鳏筏郡榻K了するため最初の位置を
	int amariSizeX = FALSE;		// 横に余りがあるかフラグ  旁边多有转折点
	int amariSizeY = FALSE;		// kに余りがあるかフラグ
	BOOL vramFullFlag = FALSE; 		// VRAMがいっぱいかどうか
	SURFACE_INFO *prevSurfaceInfo; 	// 前のサ`フェスインフォ造体のアドレス
	
	// 送るＢＭＰのオフセットポイント   送BMP的坐标点
	offsetX = 0; 
	offsetY = bmpHeight;
	
	// 必要なサ`フェスの横枚数算
	surfaceCntX = bmpWidth / SURFACE_WIDTH;
	
	// 横に余りがあったら半端サイズを
	if( ( amariSizeX = bmpWidth % SURFACE_WIDTH ) ){ 
		surfaceCntX++;		// もうひとつ必要
	}
	
	// 必要なサ`フェスのk枚数算
	surfaceCntY = bmpHeight / SURFACE_HEIGHT;
	
	// kに余りがあったら半端サイズを
	if( ( amariSizeY = bmpHeight % SURFACE_HEIGHT ) ){ 
		surfaceCntY++;		// もうひとつ必要
	}
	// 必要なサ`フェスのt数算
	totalSurface  = surfaceCntX * surfaceCntY;
	
#if 0
	// 横幅を４のバウンダリにする
	if( ( lpBmpInfo->bmiHeader.biWidth & 3 ) ){
		lpBmpInfo->bmiHeader.biWidth += 4 - lpBmpInfo->bmiHeader.biWidth & 3;
	}
#else	// Realbin iみzむとき
	// 横幅を４のバウンダリにする
	if( ( RealBinWidth & 3 ) ){
		RealBinWidth += 4 - RealBinWidth & 3;
	}
#endif

	// 空いているサ`フェスを探す
	// サ`フェスの数だけル`プ
	/// /接口的表面寻找/ /表面的数量循环
	while( 1 ){
		// 使用できる状Bなら  可使用状态的话
		if( SurfaceInfo[ SurfaceSearchPoint ].date < SurfaceDate - SURACE_BMP_DEATH_DATE ){
#ifdef _DEBUG		
			// F在使っているサ`フェスの数カウント
			SurfaceUseCnt++;
#endif
			// 上工r
			if( SurfaceInfo[ SurfaceSearchPoint ].bmpNo != -1 ){
				// サ`フェスインフォ造体のアドレス
				SURFACE_INFO *lpSurfaceInfo;
				
				// サ`フェスリストの先^アドレスを
				lpSurfaceInfo = SpriteInfo[ SurfaceInfo[ SurfaceSearchPoint ].bmpNo ].lpSurfaceInfo;
				
				// 前にいたＢＭＰとサ`フェスのリンクを削除
				SpriteInfo[ SurfaceInfo[ SurfaceSearchPoint ].bmpNo ].lpSurfaceInfo = NULL;
				
				// サ`フェスリストの初期化ル`プ
				for( ; lpSurfaceInfo != NULL;
					lpSurfaceInfo = lpSurfaceInfo->pNext ){
					
					// ＢＭＰ番号初期化
					lpSurfaceInfo->bmpNo = -1;
					
#ifdef _DEBUG		
					// F在使っているサ`フェスの数マイナス
					SurfaceUseCnt--;
#endif
				}
				
			}
			// サ`フェスリストの先^アドレスをするとき
			if( SpriteInfo[ bmpNo ].lpSurfaceInfo == NULL ){
				// スプライトインフォに先^アドレスをする
				SpriteInfo[ bmpNo ].lpSurfaceInfo = &SurfaceInfo[ SurfaceSearchPoint ];
				
			}else{ // つなげるとき
			
				// えておいた前のサ`フェスインフォ造体にアドレスを教える
				prevSurfaceInfo->pNext = &SurfaceInfo[ SurfaceSearchPoint ];
			}
			
			// ＢＭＰ番号をする
			SurfaceInfo[ SurfaceSearchPoint ].bmpNo = bmpNo;
			
			// オフセット座摔蛞える
			//３２０＊２４０ドットモ`ドのr
			//胶印坐标记320×240像素/ /模式的时候
			if(ResoMode == 1){
				SurfaceInfo[ SurfaceSearchPoint ].offsetX = offsetX / 2;
				SurfaceInfo[ SurfaceSearchPoint ].offsetY = ( bmpHeight - offsetY ) / 2;
			} else {
				//３２０＊２４０ドットモ`ド以外のr
				SurfaceInfo[ SurfaceSearchPoint ].offsetX = offsetX;
				SurfaceInfo[ SurfaceSearchPoint ].offsetY = bmpHeight - offsetY;
			}
			
			// 横に余りがあるときは送する前にサ`フェスを\でクリア`
			if( offsetX >= bmpWidth - SURFACE_WIDTH && amariSizeX ){
				ClearSurface( SurfaceInfo[ SurfaceSearchPoint ].lpSurface );
				// 余りサイズ
				sizeX = amariSizeX;				
			}else sizeX = SURFACE_WIDTH;
			
			// kに余りがあるときは送する前にサ`フェスを\でクリア`
			if( offsetY - SURFACE_HEIGHT <= 0 && amariSizeY ){
				// 先に\でクリア`されてないとき
				if( sizeX != amariSizeX ){
					ClearSurface( SurfaceInfo[ SurfaceSearchPoint ].lpSurface );
				}
				// 余りサイズ
				sizeY = amariSizeY;
			}else sizeY = SURFACE_HEIGHT;
			
			// ＢＭＰをサ`フェスへ送
			DrawBitmapToSurface2( 	SurfaceInfo[ SurfaceSearchPoint ].lpSurface, 
									offsetX, 
									offsetY - 1, 
									sizeX,
									sizeY,
									lpBmpInfo );

			// F在の_保したサ`フェスの枚数をカウント
			totalSurfaceCnt++;

			// すべて_保しKわったら
			if( totalSurfaceCnt >= totalSurface ){
				//　pNext にＮＵＬＬを入れてK了
				SurfaceInfo[ SurfaceSearchPoint ].pNext = NULL;
				// 仕魑恢盲蜻Mませる
				SurfaceSearchPoint++;
				
#if 0			// VRAMを先使用バ`ジョン

				// VRAMのサ`フェスを仕鳏工毪趣
				if( vramFullFlag == FALSE ){
					// リミットチェック
					if( SurfaceSearchPoint >= VramSurfaceCnt ) SurfaceSearchPoint = 0;
				}else{
					// 仕魑恢盲
					SurfaceSearchPoint = SurfaceSearchPointBak;
				}
				
#else			// VRAMとSYSRAMを同レベルで使用バ`ジョン

				// リミットチェック
				if( SurfaceSearchPoint >= SurfaceCnt ) SurfaceSearchPoint = 0;
#endif
				break;
				
			}else{
				// 今のサ`フェスインフォ造体のアドレスをえる
				prevSurfaceInfo = &SurfaceInfo[ SurfaceSearchPoint ];
				
				// 右端まで送ったら
				if( offsetX >= bmpWidth - SURFACE_WIDTH ){ 
					offsetX = 0;
					offsetY -= SURFACE_HEIGHT;
				}else{ 
					offsetX += SURFACE_WIDTH;
				}
			}
		}
		// 仕魑恢盲蜻Mませる
		SurfaceSearchPoint++;
		
#if 0	// VRAMを先使用バ`ジョン

		// VRAMのサ`フェスを仕鳏工毪趣
		if( vramFullFlag == FALSE ){
			// VRAMサ`フェスの最後まで仕鳏筏郡趣
			if( SurfaceSearchPoint >= VramSurfaceCnt ) SurfaceSearchPoint = 0;
			// 一周仕鳏筏郡VRAMに空きなし状Bであきらめる
			if( SurfaceSearchPoint == SurfaceSearchPointBak ){ 
				//MessageBox( hWnd, "ＶＲＡＭに空きがありません。", "_J", MB_OK );
				// 仕魑恢盲SYSTEMサ`フェスが存在するところへ移
				SurfaceSearchPoint = VramSurfaceCnt + 1;
				vramFullFlag = TRUE;
			}
		}
		// SYSTEMRAMのサ`フェスを仕鳏工毪趣
		if( vramFullFlag == TRUE ){
			// 最後まで仕鳏筏郡
			if( SurfaceSearchPoint >= SurfaceCnt ){ 
				// 仕魑恢盲
				SurfaceSearchPoint = SurfaceSearchPointBak;
				//MessageBox( hWnd, "サ`フェス不足です。", "_J", MB_OK );
				break;
			}
		}
		
#else	// VRAMとSYSRAMを同レベルで使用バ`ジョン
		
		// 最後まで仕鳏筏郡
		if( SurfaceSearchPoint >= SurfaceCnt ){ 
			// 最初に
			SurfaceSearchPoint = 0;
		}
		// 一周仕鳏筏郡楗旦`フェスに空きなし状Bであきらめる
		if( SurfaceSearchPoint == SurfaceSearchPointBak ){ 
			//MessageBox( hWnd, "サ`フェス不足です。", "_J", MB_OK );
			break;
		}
	}
#endif
}

/* オフスクリ`ンサ`フェスの作成 *********************************************/
BOOL InitOffScreenSurface( void )
{
	int i;
	BOOL vramFullFlag = FALSE; // VRAMがいっぱいかどうか 是否有很多
	
	// サ`フェスの数 表面的难到是显示器？
	SurfaceCnt = 0;
	// VRAM に作成したサ`フェスの数
	VramSurfaceCnt = 0;
	// SYSTEMRAM に作成したサ`フェスの数
	SysramSurfaceCnt = 0;
	
	// バトルサ`フェスの作成
	if( ( lpBattleSurface = CreateSurface( DEF_APPSIZEX, DEF_APPSIZEY, DEF_COLORKEY, DDSCAPS_VIDEOMEMORY )) == NULL ){
		// バトルサ`フェスの作成
		if( ( lpBattleSurface = CreateSurface( DEF_APPSIZEX, DEF_APPSIZEY, DEF_COLORKEY, DDSCAPS_SYSTEMMEMORY )) == NULL ){
			MessageBox( hWnd ,"バトルサ`フェスの作成に失・筏蓼筏俊" ,"_J",MB_OK | MB_ICONSTOP );
			return FALSE;
		}
	}
	// SURACE_INFO_MAX までサ`フェスを作る   表面到制作*/
	for( i = 0 ; i < SURACE_INFO_MAX ; i++ ){
	
#ifdef _DEBUG		
#if VRAM_SYSRAM_POINT
		// 今だけ（ チェック用 )  只是现在（用）检查
		if( i == VRAM_SYSRAM_POINT ){
			 vramFullFlag = TRUE;
		}
#endif
#endif

		// VRAMにサ`フェスを作成できるとき 表面上能作成的时候
		if( vramFullFlag == FALSE ){
			// DirectDraw用にBITMAPを格{するためのサ`フェスを作成 用BITMAP来存储的表面作成
			if( ( SurfaceInfo[ i ].lpSurface = CreateSurface( SurfaceSizeX, SurfaceSizeY, DEF_COLORKEY, DDSCAPS_VIDEOMEMORY )) == NULL ){
				//MessageBox( hWnd ,"ＶＲＡＭサ`フェスの作成に失・筏蓼筏俊" ,"_J",MB_OK | MB_ICONSTOP );
				vramFullFlag = TRUE;
			}else{
				// VRAM に作成したサ`フェスの数カウント
				VramSurfaceCnt++;
			}
		}
			
		// SYSTEMRAMにサ`フェスを作成するとき  表面上制作的时候
		if( vramFullFlag == TRUE ){
			if( ( SurfaceInfo[ i ].lpSurface = CreateSurface( SurfaceSizeX, SurfaceSizeY, DEF_COLORKEY, DDSCAPS_SYSTEMMEMORY )) == NULL ){
				MessageBox( hWnd ,"ＳＹＳＲＡＭサ`フェスの作成に失・筏蓼筏俊" ,"_J",MB_OK | MB_ICONSTOP );
				return FALSE;
			}else{
				// SYSTEMRAM に作成したサ`フェスの数カウント
				SysramSurfaceCnt++;
			}
		}
	}
	
	// 作成したサ`フェスの数を
	SurfaceCnt = i;
	
	/* サ`フェスインフォ造体の初期化  回程信息结构的初期化*/
	InitSurfaceInfo();
	/* スプライトインフォ造体の初期化 */
	InitSpriteInfo();
		
	
	return TRUE;
}

/* サ`フェスインフォ造体の初期化　**************************************/
void InitSurfaceInfo( void )
{
	int i;
	
#ifdef _DEBUG		
	// F在使っているサ`フェスの数初期化
	SurfaceUseCnt = 0;
#endif
	
	// サ`フェス仕魑恢贸跗诨
	SurfaceSearchPoint = 0;
	
	// サ`フェス数だけル`プ */
	for( i = 0 ; i < SurfaceCnt ; i++ ){
	
		// サ`フェスインフォ造体の初期化
		SurfaceInfo[ i ].bmpNo = -1;
		SurfaceInfo[ i ].date = 0;
		SurfaceInfo[ i ].pNext = NULL;
	}
}	

/* 接口 构造体初始化 造体の初期化　**************************************/
void InitSpriteInfo( void )
{
	int i;
	
	// ＢＭＰの数だけル`プ */
	for( i = 0 ; i < MAX_GRAPHICS ; i++ ){
		// VRAM にいないことにする
		SpriteInfo[ i ].lpSurfaceInfo = NULL;
	}
}	

// ＢＭＰをロ`ドする ***从硬盘加载******************************************************/
BOOL LoadBmp( int bmpNo )
{
	// ＶＲＡＭにいないときはハ`ドディスクからロ`ドする  没有硬盘从视频卡？
	if( SpriteInfo[ bmpNo ].lpSurfaceInfo == NULL )
	{
		// ＢＭＰ番号からイメ`ジデ`タを返す( Realbin からiみzむ )
		if( realGetImage( 	bmpNo, 
							( unsigned char **)&pRealBinBits, 
							&RealBinWidth, 
							&RealBinHeight ) == FALSE ){
							
			//MessageBox( hWnd, "イメ`ジデ`タのiみzみに失・筏蓼筏俊", "_J", MB_OK );
			return FALSE;
		}
		// ＢＭＰのサイズを
		SpriteInfo[ bmpNo ].width = RealBinWidth;
		SpriteInfo[ bmpNo ].height = RealBinHeight;
		// ＢＭＰをサ`フェスへ割り当て 
		AllocateBmpToSurface( bmpNo );
	}
	return TRUE;
}

// ＢＭＰをロ`ドする（グラフィック番号Qもする） **人物平面号码变换的 从硬盘*************************/
BOOL LoadBmp2( int bmpNo )
{
	// RIAL.BIN　番号にする
	realGetNo( bmpNo , (U4 *)&bmpNo );
	// ＶＲＡＭにいないときはハ`ドディスクからロ`ドする
	if( SpriteInfo[ bmpNo ].lpSurfaceInfo == NULL ){
		// ＢＭＰ番号からイメ`ジデ`タを返す( Realbin からiみzむ )
		//号的图像数据返回（Realbin读取）从
		if( realGetImage( 	bmpNo, 
							( unsigned char **)&pRealBinBits, 
							&RealBinWidth, 
							&RealBinHeight ) == FALSE ){
							
			//MessageBox( hWnd, "イメ`ジデ`タのiみzみに失・筏蓼筏俊", "_J", MB_OK );
			return FALSE;
		}
		// ＢＭＰのサイズを
		SpriteInfo[ bmpNo ].width = RealBinWidth;
		SpriteInfo[ bmpNo ].height = RealBinHeight;
		// ＢＭＰをサ`フェスへ割り当て 
		AllocateBmpToSurface( bmpNo );
	}
	return TRUE;
}


/* ビットマップのiみzみ *****************************************************/
void OpenBmp( void )
{
	int i;
   	char *fileName[]=
	/* 0 */	  { "data\\spr\\自.bmp","data\\spr\\常.bmp","data\\spr\\常保.bmp","data\\spr\\常.bmp","data\\spr\\常玻.bmp",
	/* 5 */		"data\\spr\\常常.bmp","data\\spr\\常.bmp","data\\spr\\常.bmp","data\\spr\\常矗.bmp","data\\spr\\常.bmp",
	/*10 */		"data\\spr\\常担.bmp","data\\spr\\消す.bmp","data\\spr\\Ｍ.bmp","data\\spr\\Ｗ.bmp","data\\spr\\Ｓ.bmp",
	/*15 */		"data\\spr\\Ｍ２.bmp","data\\spr\\Ｗ２.bmp","data\\spr\\Ｓ２.bmp","data\\spr\\Ｍ３.bmp","data\\spr\\Ｗ３.bmp",
	/*20 */		"data\\spr\\Ｓ３.bmp","data\\spr\\爆k１.bmp","data\\spr\\爆k２.bmp","data\\spr\\爆k３.bmp", "data\\spr\\自.bmp",
	/*25 */		"data\\spr\\自.bmp","data\\spr\\.bmp","data\\spr\\.bmp","data\\spr\\黄色.bmp","data\\spr\\青色.bmp",
	/*30 */		"data\\spr\\L.bmp","data\\spr\\pl01.bmp","data\\spr\\フォント.bmp","data\\spr\\st\\ken.bmp" };

	//BITMAPファイルの展_及びメモリ`_保 文件的展开，确保及存储器
	//lpBmp[ 0 ] = LoadDirectDrawBitmap( fileName[ 0 ] ); 
	//if( lpBmp[ 0 ] == NULL ) MessageBox( hWnd, "ＢＭＰのオ`プンに失・筏蓼筏俊", "_J", MB_OK );
	
	//if( !WindowMode ){
		// システムパレット初期化
	//	InitPalette( lpBmp[ 0 ] );
	//}
	
	// 作れるだけサ`フェスを作る */
	for( i = 0 ; i < SURACE_INFO_MAX ; i++ ){
	
		// サ`フェスインフォ造体の初期化
		SurfaceInfo[ i ].bmpNo = -1;
		SurfaceInfo[ i ].pNext = NULL;
		
		// DirectDraw用にBITMAPを格{するためのサ`フェスを作成
		if( ( SurfaceInfo[ i ].lpSurface = CreateSurface( SURFACE_WIDTH, SURFACE_HEIGHT, DEF_COLORKEY, 0 )) == NULL ){
			MessageBox( hWnd ,"サ`フェスの初期化に失・筏蓼筏俊" ,"_J",MB_OK | MB_ICONSTOP );
			break;
			//DestroyWindow( hWnd );
		}
		
		// サ`フェスの数
		SurfaceCnt++;
		
		// BITMAPをサ`フェス(座[0,0])へ}写
		//DrawBitmapToSurface( lpSurface[ i ], 0, 0 ,lpBmp[ 0 ] );
		//SetRect( &Rect, 0, 0, lpBmp[ 0 ]->bmiHeader.biWidth, lpBmp[ 0 ]->bmiHeader.biHeight ); 
	}
}


