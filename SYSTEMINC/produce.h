/************************/
/*	produce.h			*/
/************************/
#ifndef _PRODUCE_H_
#define _PRODUCE_H_

// バトルサ`フェスの演出のN
enum{
	PRODUCE_UP_ACCELE,			// 上加速移
	PRODUCE_UP_BRAKE,			// 上p速移
	
	PRODUCE_DOWN_ACCELE,		// 下加速移
	PRODUCE_DOWN_BRAKE,			// 下p速移
	
	PRODUCE_LEFT_ACCELE,		// 左加速移
	PRODUCE_LEFT_BRAKE,			// 左p速移
	
	PRODUCE_RIGHT_ACCELE,		// 右加速移
	PRODUCE_RIGHT_BRAKE,		// 右p速移
	
	PRODUCE_4WAY_OUT,			// 四方向移踊嫱猡
	PRODUCE_4WAY_IN,			// 四方向移踊婺冥
	
	PRODUCE_HAGARE_OUT,			// はがれI理（消える）
	PRODUCE_HAGARE_IN,			// はがれI理（出Fする）
	
	PRODUCE_HAGARE_OCHI_OUT,	// はがれ落ち（消える）
	PRODUCE_HAGARE_OCHI_IN,		// はがれ落ち（出Fする）
	
	PRODUCE_BRAN_SMALL,			// フスマs小
	PRODUCE_BRAN_BIG,			// フスマ大

	PRODUCE_BREAK_UP1,			//折りたたみ上アウト
	PRODUCE_BREAK_UP2,			//折りたたみ上イン

	PRODUCE_BREAK_UP3,			//折りたたみ上アウト
	PRODUCE_BREAK_UP4,			//折りたたみ下イン

	PRODUCE_BREAK_UP5,			//折りたたみ下アウト
	PRODUCE_BREAK_UP6,			//折りたたみ下イン

	PRODUCE_BREAK_UP7,			//折りたたみ下アウト
	PRODUCE_BREAK_UP8,			//折りたたみ上イン

	PRODUCE_CENTER_PRESSIN,		// 中央折りたたみイン
	PRODUCE_CENTER_PRESSOUT,	// 中央折りたたみアウト

	PRODUCE_END,				// K了

};

// 演出番号
extern int ProduceNo;
// 演出初期化フラグ
extern int ProduceInitFlag;

// バトルサ`フェスの画像作成 **************************************************/
void CopyBackBuffer( void );

// 演出描画 ********************************************************************/
BOOL DrawProduce( int no );

// タイトル演出 ********************************************************************/
void TitleProduce( void );

#endif

