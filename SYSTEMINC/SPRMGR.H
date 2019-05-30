/************************/
/*	sprmgr.h			*/
/************************/
#ifndef _SPRMGR_H_
#define _SPRMGR_H_

//#define BMP_FILE			120000	// ＢＭＰファイルの数
#define MAX_GRAPHICS	 	130000	// 画像の最大数
#define	DEF_COLORKEY		0	// 透明色とする色番号 透明色的色号
//#define	DEF_COLORKEY		240	// 透明色とする色番号

#define SURFACE_WIDTH   64 	// 分割サ`フェイスの幅 分割接口的宽度
#define SURFACE_HEIGHT  48  // 分割サ`フェイスの高さ
//#define SURFACE_WIDTH   32 	// 分割サ`フェイスの幅
//#define SURFACE_HEIGHT  24  // 分割サ`フェイスの高さ

// サ`フェスインフォ造体 回程信息结构
struct surfaceInfo{

	LPDIRECTDRAWSURFACE lpSurface; 	// 指针

	int	bmpNo;						// 位图号码
	//这个值- 1的时候，这个表面是空。
	
	// この -1 のrはこのサ`フェスは空である
	//使用了这个接口日期记忆
	UINT date;						// このサ`フェスを使用した日付を
	//显示位置所在X坐标
	short offsetX;					// 表示位置からのオフセットＸ座
	short offsetY;					// 表示位置からのオフセットＹ座

	surfaceInfo	*pNext;				// 次のサ`フェスインフォ造体へのポインタ
	
};

typedef struct surfaceInfo SURFACE_INFO;

// スプライトインフォ造体  妖怪的信息结构？
typedef struct{

//	LPBITMAPINFO lpBmp; 			//位图信息结构的指针
	
	SURFACE_INFO *lpSurfaceInfo; 	// 使用しているサ`フェスリストの先^アドレス
	/// /使用的表面名单的前头地址/ /这个值NULL的时候这个BMP是VRAM没有
	// この NULL のrはこのＢＭＰは VRAM にはいない
	short width,height;				//BMP的尺寸
	
}SPRITE_INFO;



// 全局变量 ////////////////////////////////////////////////////////////

// スプライトインフォ  程序的接口
extern SPRITE_INFO SpriteInfo[];

// サ`フェスインフォ
extern SURFACE_INFO SurfaceInfo[];

// バトルサ`フェスのポインタ
//战斗图片指针？
extern LPDIRECTDRAWSURFACE lpBattleSurface;

// サ`フェスの数  接口数量
extern int SurfaceCnt;

// VRAM に作成したサ`フェスの数カウント
//全部接口 计数？
extern int VramSurfaceCnt;

// SYSTEMRAM に作成したサ`フェスの数カウント
extern int SysramSurfaceCnt;

// サ`フェスを使用した日付
//接口使用日期
extern UINT SurfaceDate;

// F在のサ`フェス仕魑恢
//接口位置检测
extern int SurfaceSearchPoint;

#ifdef _DEBUG		
// F在使っているサ`フェスの数カウント
extern int SurfaceUseCnt;

// F在表示しているサ`フェスの数カウント
extern int SurfaceDispCnt;
#endif
// 分割サ`フェイスの幅
extern int SurfaceSizeX;
// 分割サ`フェイスの高さ
extern int SurfaceSizeY;

// v数の型宣言 //////////////////////////////////////////////////////////////

/* ビットマップのiみzみ 位图加载  *****************************************************/
void OpenBmp( void );      

/* オフスクリ`ンサ`フェスの作成 *********************************************/
BOOL InitOffScreenSurface( void );

/* サ`フェスインフォ造体の初期化　******************************************/
void InitSurfaceInfo( void );

/* スプライトインフォ造体の初期化　**************************************/
void InitSpriteInfo( void );

/* ＢＭＰをサ`フェスへ割り当て ***********************************************/
void AllocateBmpToSurface( int bmpNo );

// ＢＭＰをロ`ドする *********************************************************/
BOOL LoadBmp( int bmpNo );
//编号转换？
// ＢＭＰをロ`ドする（グラフィック番号Qもする） ***************************/
BOOL LoadBmp2( int bmpNo );

#endif

