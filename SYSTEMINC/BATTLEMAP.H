/************************/
/*	battleMap.h			*/
/************************/
#ifndef _BATTLE_MAP_H_
#define _BATTLE_MAP_H_

// バトルマップファイルの方
#define BATTLE_MAP_FILES 218

// �F壓のバトルマップ桑催
extern int BattleMapNo;

// バトルマップ�iみ�zみとバトルサ�`フェスの鮫�騅�撹 ***************************/
BOOL ReadBattleMap( int no );

// バトルマップ宙鮫 ***********************************************************/
void DrawBattleMap( void );

#endif

