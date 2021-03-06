/************************/
/*	font.h				*/
/************************/
#ifndef _FONT_H_
#define _FONT_H_

// フォントの��尖互 忖悶議貸辞互
#define FONT_SIZE 16

// 畠叔猟忖の罪嫌�┘疋奪畔���泣方
#define FONT_ZENKAU_WIDTH FONT_SIZE + 1
// 磯叔猟忖の罪嫌�┘疋奪畔���
#define	FONT_HANKAKU_WIDTH	FONT_SIZE / 2 + 1

// 畠叔猟忖の罪嫌�┘疋奪畔���
extern int FontZenkauWidth;
// 磯叔猟忖の罪嫌�┘疋奪畔���
extern int FontHankakuWidth;

/* フォント燕幣��枠��了  fontPrio の�� ****************************************/
//忖悶�塋肖渡繁覚魑�* * * * fontPrio峙
enum{
	FONT_PRIO_BACK, 		/* 瘁ろに燕幣  朔中��燕幣	*/
	FONT_PRIO_FRONT			/* 念に燕幣 	*/
};

// フォントバッファ�`��夛悶 忖悶産喝潤更
typedef struct{
	short x, y;			// 燕幣恙��
	char color;			// 燕幣弼
	char str[ 256 ]; 	// 猟忖双
	UCHAR	fontPrio; 	// 燕幣の��枠��了
	BOOL hitFlag;		// 輝たり登協するかフラグ　ＯＮ�今院。錬藤藤今�  耽登協�迅�孵泣ON��1孵諏��0
}FONT_BUFFER;

// フォントバッファ�`  忖悶産喝匂
extern FONT_BUFFER FontBuffer[];

// フォントカウンタ�`  忖悶国岬
extern int FontCnt;

// フォント燕幣�v方 //  忖悶�塋昇�方/////////////////////////////////////////////////////////
void FontPrint( char *moji, int x, int y, int dispPrio );

// 方忖フォント燕幣�v方 //  方忖忖悶�塋昇�方/////////////////////////////////////////////////////
void FontPrintDec( char *moji, int x, int y, int dispPrio );

/* フォント秤�鵑鬟丱奪侫，冒錣瓩� *********************************************/
//忖悶佚連産喝* * * * * *們持壓* * 
int StockFontBuffer( int x, int y, char fontPrio, int color, char *str, BOOL hitFlag );

/* フォント秤�鵑鬟丱奪侫，冒錣瓩襭���夛悶局し��********************************/
//忖悶佚連産喝刈�┨行賁�* * * * *住公��
void StockFontBuffer2( STR_BUFFER *strBuffer );

#endif











