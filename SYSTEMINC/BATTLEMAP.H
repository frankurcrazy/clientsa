/************************/
/*	battleMap.h			*/
/************************/
#ifndef _BATTLE_MAP_H_
#define _BATTLE_MAP_H_

// バトルマップファイルの数
#define BATTLE_MAP_FILES 218

// F在のバトルマップ番号
extern int BattleMapNo;

// バトルマップiみzみとバトルサ`フェスの画像作成 ***************************/
BOOL ReadBattleMap( int no );

// バトルマップ描画 ***********************************************************/
void DrawBattleMap( void );

#endif

