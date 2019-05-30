#ifndef _LOADREALBIN_H_
#define _LOADREALBIN_H_

#include <stdio.h>
//#include "common.h"

typedef unsigned char MOJI;
typedef unsigned char U1;
typedef          char S1;
typedef unsigned short U2;
typedef          short S2;
typedef unsigned long U4;
typedef          long S4;
typedef float  F4;
typedef double F8;

int realGetImage(int graphicNo, unsigned char **bmpdata, int *width, int *height);
int Linear(void);


int initRealbinFileOpen(char *realbinfilename, char *addrbinfilename);
void cleanupRealbin(void);

void initAutoMapColor( char * );
void makeAutoMapColor( void );
int writeAutoMapColor( char *, char * );
int readAutoMapColor( char *, char * );

extern unsigned char autoMapColorTbl[];

//extern	PBITMAPINFO         PBiInfo;
//extern	PBYTE               PBits;
//extern	BITMAPFILEHEADER    header;

//グラフィックナンバ`より立ち位置をもらう
//	 -1:失 or 以外成功
//图形号码比得到站立的位置/ /回值- 1:失败或以外成功
int realGetPos(U4 GraphicNo , S2 *x , S2 *y);

//グラフィックナンバ`からグラフィック幅、高さを返す
//	 -1:失 or 以外成功
//图形号平面宽、高回报
int realGetWH(U4 GraphicNo , S2 *w , S2 *h);

//グラフィックナンバ`より当たりサイズを求める。
//	 -1:失 or 以外成功 图形比要求每号尺寸。
int realGetHitPoints(U4 GraphicNo , S2 *HitX , S2 *HitY);

//グラフィックナンバ`より当たり判定を求める。
//	 -1:失 or 以外成功 图形号码比要求每判定 类型
int realGetHitFlag(U4 GraphicNo , S2 *Hit);

//グラフィックナンバ`より描画先位Q定法を取り出す。
//	 -1:失 or 以外成功 图形号码比绘图优先顺序决定法取出。
BOOL realGetPrioType(U4 GraphicNo , S2 *prioType);

//グラフィックナンバ`より高さの有oを求める。
//	 -1:失 or 以外成功 
int realGetHeightFlag(U4 GraphicNo , S2 *Height);

//引数にビットマップファイル番号(ファイル名の数字のみ)を与えてグラフィックナンバ`（通し番号）を返す
//	 -1:失 or 以外成功
int realGetNo( U4 CharAction , U4 *GraphicNo );

//グラフィックナンバ`よりh境音を
//	:　0 O定ない
int realGetSoundEffect(U4 GraphicNo);

//グラフィックナンバ`よりi行音を
//	:　0 O定ない
int realGetWalkSoundEffect(U4 GraphicNo);

//引数にグラフィックナンバ`（通し番号）を与えてビットマップファイブ番号を返す
int realGetBitmapNo( int num );


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//地图路径结构 魔力的 MAP_ATTR只有5位 不知道干什么的，，，
typedef struct {
	unsigned char atari_x,atari_y;	//大きさ  尺寸
	unsigned short hit;				//iけるか 0 可以走 1障碍物???
	short height;				//高さあるか 高度
	short broken;				//菠欷毪  损坏
	short indamage;				//入HPダメ`ジ  入HP损伤
	short outdamage;			//出ダメ`ジ 伤害
	short inpoison;				//入毒 ??
	short innumb;				//入しびれ 麻木
	short inquiet;				//入沈a  进入沉默  
	short instone;				//入石化  
	short indark;				//入暗 黑暗
	short inconfuse;			//入混乱  
	short outpoison;			//出毒
	short outnumb;				//出wれ
	short outquiet;				//出沈a
	short outstone;				//出石化
	short outdark;				//出暗
	short outconfuse;			//出混乱
	//效果1（画面看起来r音，画面效果等）?
	short effect1;				//エフェクト1（画面にえるr音、画面抗胜桑  ????1?????????????g?????????
	//效果2（进入的时候声音，画面效果等）
	short effect2;				//エフェクト2（入ったとき音、画面抗胜桑  ????2?????a?g?????????
	unsigned short damy_a;
	unsigned short damy_b;
	unsigned short damy_c;
	unsigned int bmpnumber;		//画像番号 图片号
} MAP_ATTR;

struct ADRNBIN{
	unsigned long	bitmapno;
	unsigned long	adder;
	unsigned long	size;
	int	xoffset;
	int	yoffset;
	unsigned int width;
	unsigned int height;

	MAP_ATTR attr;

};

#endif /*_LOADREALBIN_H_*/

/*
typedef struct {
unsigned char atari_x,atari_y; //大きさ  尺寸  占地面积是物件所占的大小，1就表示占1格
unsigned short hit;    //iけるか 0 可以走 1障碍物??? 0表示障碍物，1表示可以走上去
short height;    //高さあるか 高度
short broken;    //菠欷毪  损坏
short indamage;    //入HPダメ`ジ  入HP损伤
short outdamage;   //出ダメ`ジ 伤害
short inpoison;    //入毒 ??
short innumb;    //入しびれ 麻木
short inquiet;    //入沈a  进入沉默  
short instone;    //入石化  
short indark;    //入暗 黑暗
short inconfuse;   //入混乱  
short outpoison;   //出毒
short outnumb;    //出wれ
short outquiet;    //出沈a
short outstone;    //出石化
short outdark;    //出暗
short outconfuse;   //出混乱
//效果1（画面看起来r音，画面效果等）?
short effect1;    //エフェクト1（画面にえるr音、画面抗胜桑  ????1?????????????g?????????
//效果2（进入的时候声音，画面效果等）
short effect2;    //エフェクト2（入ったとき音、画面抗胜桑  ????2?????a?g?????????
unsigned short damy_a;
unsigned short damy_b;
unsigned short damy_c;
unsigned int bmpnumber;  //画像番号 图片号ID   在地图文件里的编号
} MAP_ATTR;
struct ADRNBIN{
unsigned long bitmapno;  // 图片序列号  图片的编号
unsigned long adder;   // 指明图片在数据文件中的起始位置
unsigned long size; // 图片数据块的大小
int xoffset;   //显示图片时，横坐标偏移X
int yoffset;
unsigned int width;    //图片宽度;
unsigned int height;
MAP_ATTR attr;
};
*/