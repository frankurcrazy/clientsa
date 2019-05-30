#include <stdio.h>
#include <stdlib.h>

/* WIN32_LEAN_AND_MEANをdefineするとWINDOWS.Hのうち
のあまり使われない部分がインクル`ドされなくなって
ちょっとだけコンパイルがはやくなる */
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <math.h>

#include "system.h"
#include "action.h"
#include "map.h"
#include "pattern.h"
#include "loadrealbin.h"
#include "loadsprbin.h"
#include "sprdisp.h"
#include "math2.h"

#include "character.h"
#include "pc.h"
#include "menu.h"
#include "main.h"
#include "anim_tbl.h"


// キャラ管理テ`ブル
enum
{
	CHAROBJ_USE_FREE,
	CHAROBJ_USE_STAY,
	CHAROBJ_USE_VIEW
};
#define MAX_CHAROBJ	1000
CHAROBJ charObj[MAX_CHAROBJ];
int maxCharObj;		// 登h数
int tailCharObj;	// テ`ブルの最後尾位置
int freeCharObj;	// 空きテ`ブル位置
int searchCharObj;	// 仕魑恢


// キャラごとにマ`クの高さを浃à
int charIconOffsetY[12] =
{
	-71, -88, -84, -88, -96, -102, -88, -88, -88, -85, -85, -88
};

// バトルアイコン
int battleIconTbl[][2] =
{
	{  CG_VS_MARK_1A,  CG_VS_MARK_1B },
	{  CG_VS_MARK_2A,  CG_VS_MARK_2B },
	{  CG_VS_MARK_3A,  CG_VS_MARK_3B },
	{  CG_VS_MARK_4A,  CG_VS_MARK_4B },
	{  CG_VS_MARK_5A,  CG_VS_MARK_5B },
	{  CG_VS_MARK_6A,  CG_VS_MARK_6B },
	{  CG_VS_MARK_7A,  CG_VS_MARK_7B },
	{  CG_VS_MARK_8A,  CG_VS_MARK_8B },
	{  CG_VS_MARK_9A,  CG_VS_MARK_9B },
	{ CG_VS_MARK_10A, CG_VS_MARK_10B }
};


void charProc( ACTION * );


// キャラクタアクションのデフォルト
void charProc( ACTION *ptAct )
{
	float mx, my;
	int animLoop;
	int no;
	CHAREXTRA *ext;


	// 移I理
	// PC以外のキャラは自前で移I理
	// PCはマップ移婴谓Y果を入れてもらって移婴工
	if( pc.ptAct != ptAct )
	{
		ext = (CHAREXTRA *)ptAct->pYobi;
		no = ext->charObjTblId;

		// パ`ティに入っていないキャラだけI理する
		if( (charObj[no].status & CHR_STATUS_PARTY) == 0 )
		{
			charMove( ptAct );
		}
#if 1
		if( ptAct->vx == 0 && ptAct->vy == 0
		 && charObj[no].stockDir != -1
		 && charObj[no].stockDirX == ptAct->gx && charObj[no].stockDirY == ptAct->gy )
		{
			ptAct->anim_ang = charObj[no].stockDir;
			charObj[no].stockDir = -1;
			charObj[no].stockDirX = 0;
			charObj[no].stockDirY = 0;
		}
#endif
	}

	// 画面表示位置
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)mx;
	ptAct->y = (int)my;


	// ステ`タスエフェクト表示
	drawCharStatus( ptAct );

	// アニメ`ションI理
	if( ptAct->anim_no == ANIM_HAND
	 || ptAct->anim_no == ANIM_HAPPY
	 || ptAct->anim_no == ANIM_ANGRY
	 || ptAct->anim_no == ANIM_SAD
	 || ptAct->anim_no == ANIM_WALK
	 || ptAct->anim_no == ANIM_STAND
	 || ptAct->anim_no == ANIM_NOD )
	{
		animLoop = ANM_LOOP;
	}
	else
	{
		animLoop = ANM_NO_LOOP;
	}
	pattern( ptAct, ANM_NOMAL_SPD, animLoop );


	if( pc.ptAct != ptAct )
	{
		if( charObj[no].newFoundFlag )
		{
			S2 xx, yy, ww, hh;

			// 画面内ならI理する
			realGetPos( ptAct->bmpNo, &xx, &yy );
			realGetWH( ptAct->bmpNo, &ww, &hh );
			xx += ptAct->x;
			yy += ptAct->y;
			if( 0 <= xx && xx+ww <= DEF_APPSIZEX
			 && 0 <= yy && yy+hh <= DEF_APPSIZEY )
			{
				CheckNewPet( ptAct->anim_chr_no );
				charObj[no].newFoundFlag = 0;
			}
		}
	}


#if 0
	// オリジナルソ`ト用にストック
	setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my );
#else
	if( 20000 <= ptAct->anim_chr_no && ptAct->anim_chr_no <= 24999 )
	{
		// アイテム

#if 1
		if( pc.ptAct != ptAct )
		{
			// 焱猡顺訾郡椁盲皮讼
			if( ptAct->gx < nowGx-16 || nowGx+16 < ptAct->gx
			 || ptAct->gy < nowGy-16 || nowGy+16 < ptAct->gy )
			{
				delCharObj( charObj[no].id );
				return;
			}
		}
#endif

		// 同じ位置にあるrは表示しない
		if( !itemOverlapCheck( ptAct->bmpNo, ptAct->gx, ptAct->gy ) )
		{
			// オリジナルソ`ト用にストック
			setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my );
			ptAct->atr &= (~ACT_ATR_HIDE);
		}
		else
		{
			ptAct->atr |= ACT_ATR_HIDE;
		}
	}
	else
	{
		// アイテムじゃない
		// オリジナルソ`ト用にストック
		setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my );
	}
#endif
}


// キャラクタのアクション作成
ACTION *createCharAction( int graNo, int gx, int gy, int dir )
{
	ACTION *ptAct;
	float mx, my;

	/* アクションリストに登h */
	ptAct = GetAction( PRIO_CHR, sizeof( CHAREXTRA ) );
	if( ptAct == NULL )
		return NULL;

	// g行v数
	ptAct->func = charProc;
	// グラフィックの番号
	ptAct->anim_chr_no = graNo;
	// 幼鞣
	ptAct->anim_no = ANIM_STAND;
	// アニメ`ション向き( ０～７ )( 下が０で右回り )
	ptAct->anim_ang = dir;
	// 表示先度
	ptAct->dispPrio = DISP_PRIO_CHAR;
	// 1行インフォ表示フラグ
	ptAct->atr = ACT_ATR_INFO |	ACT_ATR_HIT | ACT_ATR_HIDE2;
	// 初期位置
	ptAct->nextGx = gx;					// マップグリッド座耍ㄒ酉龋
	ptAct->nextGy = gy;
	ptAct->bufCount = 0;
	ptAct->gx = gx;						// マップグリッド座耍ìF在地）
	ptAct->gy = gy;
	ptAct->mx = (float)gx * GRID_SIZE;	// マップ座
	ptAct->my = (float)gy * GRID_SIZE;
	ptAct->vx = 0;						// 移分
	ptAct->vy = 0;

	// 画面表示位置
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)mx;
	ptAct->y = (int)my;

	return ptAct;
}


// キャラの移酉趣颔啸氓榨・摔郡幛
void stockCharMovePoint( ACTION *ptAct, int nextGx, int nextGy )
{
	if( ptAct == NULL )
		return;

	// バッファが空いていたらためる
	if( ptAct->bufCount < sizeof( ptAct->bufGx )/sizeof( int ) )
	{
		ptAct->bufGx[ptAct->bufCount] = nextGx;
		ptAct->bufGy[ptAct->bufCount] = nextGy;
		ptAct->bufCount++;
	}
	// バッファが一杯なのでワ`プさせる
	else
	{
		ptAct->bufCount = 0;
		setCharWarpPoint( ptAct, nextGx, nextGy );
	}
}


// キャラの移酉趣颔啸氓榨・摔郡帷⒀a正の必要があればa正する
//（たまにサ`バから１グリッドwばした搐毪韦蜓a正する）
void correctCharMovePoint( ACTION *ptAct, int nextGx, int nextGy )
{
	int dx, dy;
	int nGx[2], nGy[2], nCnt = 0;
	int i;
	int preCnt;
	int nextGx2, nextGy2;

	if( ptAct == NULL )
		return;

	// a正が必要かチェック
	// バッファが空のときはキャラのnextと比^する
	if( ptAct->bufCount <= 0 )
	{
		nextGx2 = ptAct->nextGx;
		nextGy2 = ptAct->nextGy;
	}
	else
	{
		preCnt = ptAct->bufCount - 1;
		nextGx2 = ptAct->bufGx[preCnt];
		nextGy2 = ptAct->bufGy[preCnt];
	}

	dx = nextGx - nextGx2;
	dy = nextGy - nextGy2;

	if( ABS( dx ) == 2 && ABS( dy ) == 2 )
	{
		nGx[nCnt] = nextGx2+dx/2;
		nGy[nCnt] = nextGy2+dy/2;
		nCnt++;
	}
	else
	if( ABS( dx ) == 2 )
	{
		nGx[nCnt] = nextGx2+dx/2;
		nGy[nCnt] = nextGy;
		nCnt++;
	}
	else
	if( ABS( dy ) == 2 )
	{
		nGx[nCnt] = nextGx;
		nGy[nCnt] = nextGy2+dy/2;
		nCnt++;
	}

	nGx[nCnt] = nextGx;
	nGy[nCnt] = nextGy;
	nCnt++;


	// バッファが空いていたらためる
	if( ptAct->bufCount+nCnt <= sizeof( ptAct->bufGx )/sizeof( int ) )
	{
		for( i = 0; i < nCnt; i++ )
		{
			ptAct->bufGx[ptAct->bufCount] = nGx[i];
			ptAct->bufGy[ptAct->bufCount] = nGy[i];
			ptAct->bufCount++;
		}
	}
	// バッファが一杯なのでワ`プさせる
	else
	{
		ptAct->bufCount = 0;
		nCnt--;
		setCharWarpPoint( ptAct, nGx[nCnt], nGy[nCnt] );
	}
}


// キャラの移酉仍O定
void setCharMovePoint( ACTION *ptAct, int nextGx, int nextGy )
{
	float dir1;
	int dir;
	float dx, dy;
	float len;
	float rate = 1.0F;

	if( ptAct == NULL )
		return;

	if( ptAct->bufCount > 5 )
	{
		rate = 2.0F;
	}
	else
	if( ptAct->bufCount >= 4 )
	{
		rate = 1.6F;
	}
	else
	if( ptAct->bufCount >= 2 )
	{
		rate = 1.2F;
	}

	dx = nextGx*GRID_SIZE-ptAct->mx;
	dy = nextGy*GRID_SIZE-ptAct->my;
	len = (float)sqrt( (double)(dx*dx+dy*dy) );

	if( len > 0 )
	{
		dx /= len;
		dy /= len;
	}
	else
	{
		dx = 0;
		dy = 0;
	}

	ptAct->vx = dx * MOVE_SPEED * rate;
	ptAct->vy = dy * MOVE_SPEED * rate;

	ptAct->nextGx = nextGx;
	ptAct->nextGy = nextGy;

	if( dx != 0 || dy != 0 )
	{
		dir1 = Atan( dx, dy ) + 22.5F;
		AdjustDir( &dir1 );
		dir = (int)(dir1/45);

		ptAct->anim_ang = dir;
		ptAct->walkFlag = 1;
	}
}


void _setCharMovePoint( ACTION *ptAct, int nextGx, int nextGy )
{
	float dir1;
	int dir;
	float dx, dy;
	float len;

	if( ptAct == NULL )
		return;

	dx = nextGx*GRID_SIZE-ptAct->mx;
	dy = nextGy*GRID_SIZE-ptAct->my;
	len = (float)sqrt( (double)(dx*dx+dy*dy) );

	if( len > 0 )
	{
		dx /= len;
		dy /= len;
	}
	else
	{
		dx = 0;
		dy = 0;
	}

	ptAct->vx = dx * MOVE_SPEED;
	ptAct->vy = dy * MOVE_SPEED;

	ptAct->nextGx = nextGx;
	ptAct->nextGy = nextGy;

	if( dx != 0 || dy != 0 )
	{
		dir1 = Atan( dx, dy ) + 22.5F;
		AdjustDir( &dir1 );
		dir = (int)(dir1/45);

		ptAct->anim_ang = dir;
		ptAct->walkFlag = 1;
	}
}


// 次の座衰啸氓榨・颔伐榨趣工
void shiftBufCount( ACTION *ptAct )
{
	int i;

	if( ptAct == NULL )
		return;

	if( ptAct->bufCount > 0 )
		ptAct->bufCount--;

	for( i = 0; i < ptAct->bufCount; i++ )
	{
		ptAct->bufGx[i] = ptAct->bufGx[i+1];
		ptAct->bufGy[i] = ptAct->bufGy[i+1];
	}
}


// キャラ移I理
void charMove( ACTION *ptAct )
{
	float mx, my;

	if( ptAct == NULL )
		return;

	mx = (float)ptAct->nextGx*GRID_SIZE;
	my = (float)ptAct->nextGy*GRID_SIZE;

	// グリッドの中心に来たら次の移酉趣蛟O定する
	if( mx == ptAct->mx && my == ptAct->my )
	{
		// バッファに次の座摔毪韦窃O定
		if( ptAct->bufCount > 0 )
		{
			setCharMovePoint( ptAct, ptAct->bufGx[0], ptAct->bufGy[0] );
			shiftBufCount( ptAct );
		}
	}

	mx = (float)ptAct->nextGx*GRID_SIZE;
	my = (float)ptAct->nextGy*GRID_SIZE;

	// 移又肖
	if( ptAct->vx != 0 || ptAct->vy != 0 )
	{
		// 目的地に着いたらパラメ`タ初期化
		if( pointLen2( ptAct->mx, ptAct->my, mx, my )
			<= ptAct->vx*ptAct->vx+ptAct->vy*ptAct->vy )
		{
			ptAct->mx = mx;
			ptAct->my = my;
			ptAct->vx = 0;
			ptAct->vy = 0;
		}
		// 移
		else
		{
			ptAct->mx += ptAct->vx;
			ptAct->my += ptAct->vy;
		}
		// 移鹰衰
		ptAct->anim_no = ANIM_WALK;
	}
	else
	{
		// iき幼鳏蛄磷Bへ
		if( ptAct->walkFlag != 0 )
			ptAct->anim_no = ANIM_STAND;
		ptAct->walkFlag = 0;
	}

	ptAct->gx = (int)(ptAct->mx/GRID_SIZE);
	ptAct->gy = (int)(ptAct->my/GRID_SIZE);
}


// キャラ移I理（移I理だけ）
void charMove2( ACTION *ptAct )
{
	float mx, my;

	if( ptAct == NULL )
		return;

	mx = (float)ptAct->nextGx*GRID_SIZE;
	my = (float)ptAct->nextGy*GRID_SIZE;

	// 移又肖
	if( ptAct->vx != 0 || ptAct->vy != 0 )
	{
		// 目的地に着いたらパラメ`タ初期化
		if( pointLen2( ptAct->mx, ptAct->my, mx, my )
			<= ptAct->vx*ptAct->vx+ptAct->vy*ptAct->vy )
		{
			ptAct->mx = mx;
			ptAct->my = my;
			ptAct->vx = 0;
			ptAct->vy = 0;
		}
		// 移
		else
		{
			ptAct->mx += ptAct->vx;
			ptAct->my += ptAct->vy;
		}
		// 移鹰衰
		ptAct->anim_no = ANIM_WALK;
	}
	else
	{
		// iき幼鳏蛄磷Bへ
		if( ptAct->walkFlag != 0 )
			ptAct->anim_no = ANIM_STAND;
		ptAct->walkFlag = 0;
	}

	ptAct->gx = (int)(ptAct->mx/GRID_SIZE);
	ptAct->gy = (int)(ptAct->my/GRID_SIZE);
}


// キャラ移I理（移I理だけ）
void _charMove( ACTION *ptAct )
{
	float mx, my;
	float vx, vy;

	if( ptAct == NULL )
		return;

	mx = (float)ptAct->nextGx*GRID_SIZE;
	my = (float)ptAct->nextGy*GRID_SIZE;

	// 移又肖
	if( ptAct->vx != 0 || ptAct->vy != 0 )
	{
		vx = ptAct->vx * nowSpdRate;
		vy = ptAct->vy * nowSpdRate;

		// 目的地に着いたらパラメ`タ初期化
		if( pointLen2( ptAct->mx, ptAct->my, mx, my )
			<= vx*vx+vy*vy )
		{
			ptAct->mx = mx;
			ptAct->my = my;
			ptAct->vx = 0;
			ptAct->vy = 0;
		}
		// 移
		else
		{
			ptAct->mx += vx;
			ptAct->my += vy;
		}
		// 移鹰衰
		ptAct->anim_no = ANIM_WALK;
	}
	else
	{
		// iき幼鳏蛄磷Bへ
		if( ptAct->walkFlag != 0 )
			ptAct->anim_no = ANIM_STAND;
		ptAct->walkFlag = 0;
	}

	ptAct->gx = (int)(ptAct->mx/GRID_SIZE);
	ptAct->gy = (int)(ptAct->my/GRID_SIZE);
}


// キャラのワ`プ先O定
void setCharWarpPoint( ACTION *ptAct, int gx, int gy )
{
	if( ptAct == NULL )
		return;

	ptAct->gx = gx;
	ptAct->gy = gy;

	ptAct->nextGx = gx;
	ptAct->nextGy = gy;

	ptAct->mx = (float)gx*GRID_SIZE;
	ptAct->my = (float)gy*GRID_SIZE;

	ptAct->vx = 0;
	ptAct->vy = 0;
}


// ステ`タスビットのO定
//
//   status : ステ`タスビットのポインタ
//   smsg   : ステ`タス情笪淖至
//
//  smsgを解析しstatusにO定する
//
void setCharStatus( unsigned short *status, char *smsg )
{
	// 毒
	if( strstr( smsg, "P" ) )
	{
		*status |= CHR_STATUS_P;
	}
	else
	{
		*status &= (~CHR_STATUS_P);
	}
	// 麻w
	if( strstr( smsg, "N" ) )
	{
		*status |= CHR_STATUS_N;
	}
	else
	{
		*status &= (~CHR_STATUS_N);
	}
	// 沈a
	if( strstr( smsg, "Q" ) )
	{
		*status |= CHR_STATUS_Q;
	}
	else
	{
		*status &= (~CHR_STATUS_Q);
	}
	// 石化
	if( strstr( smsg, "S" ) )
	{
		*status |= CHR_STATUS_S;
	}
	else
	{
		*status &= (~CHR_STATUS_S);
	}
	// 暗
	if( strstr( smsg, "D" ) )
	{
		*status |= CHR_STATUS_D;
	}
	else
	{
		*status &= (~CHR_STATUS_D);
	}
	// 混乱
	if( strstr( smsg, "C" ) )
	{
		*status |= CHR_STATUS_C;
	}
	else
	{
		*status &= (~CHR_STATUS_C);
	}
}


// キャラをリ`ダ`にする
void setCharLeader( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_LEADER;
}


// キャラをリ`ダ`からやめさせる。
void delCharLeader( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~CHR_STATUS_LEADER);
}


// キャラをパ`ティ`の一Tにする
void setCharParty( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_PARTY;
}


// キャラをパ`ティ`からやめさせる。
void delCharParty( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~CHR_STATUS_PARTY);
}


// キャラをQ樽Bにする
void setCharWatch( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_WATCH;
}


// キャラをQ樽Bから元にもどす
void delCharWatch( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~CHR_STATUS_WATCH);
}


// キャラが殛L中
void setCharBattle( ACTION *ptAct, int battleNo, short sideNo, short helpMode )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_BATTLE;

	charObj[no].battleNo = battleNo;
	charObj[no].sideNo = sideNo;
	charObj[no].helpMode = helpMode;

	if( helpMode )
	{
		charObj[no].status |= CHR_STATUS_HELP;
	}
	else
	{
		charObj[no].status &= ~CHR_STATUS_HELP;
	}
}


// キャラが殛LK了
void delCharBattle( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~(CHR_STATUS_BATTLE | CHR_STATUS_HELP));
}


// キャラが庑gエフェクトを出す
void setCharUseMagic( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_USE_MAGIC;
}


// キャラが庑gエフェクトを出したらすぐにこれで
// フラグをおろす
void delCharUseMagic( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status &= (~CHR_STATUS_USE_MAGIC);
}


// キャラに吹き出しを出す
void setCharFukidashi( ACTION *ptAct, unsigned int offTime )
{
	CHAREXTRA *ext;
	int no;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;
	charObj[no].status |= CHR_STATUS_FUKIDASHI;
	ext->drawFukidashiTime = offTime + GetTickCount();
}


// ステ`タスの表示
void drawCharStatus( ACTION *ptAct )
{
	CHAREXTRA *ext;
	int no;
	unsigned short status;
	int battleNo;
	int sideNo;
	int helpMode;

	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;

	if( pc.ptAct != NULL && pc.ptAct == ptAct )
	{
		// ＰＣキャラのr
		status = pc.status;
		battleNo = 0;
		sideNo = 0;
		helpMode = 0;
	}
	else
	{
		status = charObj[no].status;
		battleNo = charObj[no].battleNo;
		sideNo = charObj[no].sideNo;
		helpMode = charObj[no].helpMode;
	}

	// バトルマ`ク
	if( (status & CHR_STATUS_BATTLE) != 0 )
	{
		int no;
		U4 bmpNo;

		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;	// 暴走しないよう念のため

		realGetNo( battleIconTbl[(battleNo%10)][(sideNo%1)], &bmpNo );

		// オリジナルソ`ト用にストック
		setCharPrio( bmpNo, ptAct->x, ptAct->y, 0, charIconOffsetY[no], ptAct->mx, ptAct->my );
	}

	// Q楗蕞`ク
	if( (status & CHR_STATUS_WATCH) != 0 )
	{
		int no;
		U4 bmpNo;

		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;	// 暴走しないよう念のため

		realGetNo( CG_ICON_WATCHING, &bmpNo );

		// オリジナルソ`ト用にストック
		setCharPrio( bmpNo, ptAct->x, ptAct->y, 0, charIconOffsetY[no], ptAct->mx, ptAct->my );
	}

	// ヘルプマ`ク
	if( (status & CHR_STATUS_HELP) != 0 )
	{
		int no;
		U4 bmpNo;

		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;	// 暴走しないよう念のため

		realGetNo( CG_SPEECH_HELP, &bmpNo );

		// オリジナルソ`ト用にストック
		setCharPrio( bmpNo, ptAct->x, ptAct->y, 0, charIconOffsetY[no]-20, ptAct->mx, ptAct->my );
	}


	// リ`ダ`マ`ク
	//（バトルマ`ク出るrは消す）
	//（Q楗蕞`ク出るrは消す）
	if( (status & CHR_STATUS_LEADER) != 0
	 && ext->ptActLeaderMark == NULL
	 && ((status & CHR_STATUS_BATTLE) == 0 || (status & CHR_STATUS_WATCH) == 0) )
	{
		// リ`ダ`マ`ク表示用アクション作成
		ext->ptActLeaderMark =
			createCommmonEffectAction( SPR_leader, ptAct->gx, ptAct->gy, 0, 0, DISP_PRIO_CHAR );
	}
	else
	if( ((status & CHR_STATUS_LEADER) == 0 || (status & CHR_STATUS_BATTLE) != 0)
	 && ext->ptActLeaderMark != NULL )
	{
		// リ`ダ`マ`ク表示用アクション抹消
		DeathAction( ext->ptActLeaderMark );
		ext->ptActLeaderMark = NULL;
	}
	else
	if( (status & CHR_STATUS_LEADER) != 0
	 && ext->ptActLeaderMark != NULL )
	{
		int no;

		no = (ptAct->anim_chr_no - SPR_001em) / 20;
		no %= 12;	// 暴走しないよう念のため

		// リ`ダ`マ`クのアニメ`ション
		ext->ptActLeaderMark->x  = ptAct->x;
		ext->ptActLeaderMark->y  = ptAct->y;
		ext->ptActLeaderMark->mx = ptAct->mx;
		ext->ptActLeaderMark->my = ptAct->my;

		// アニメ`ションI理
		pattern( ext->ptActLeaderMark, ANM_NOMAL_SPD, ANM_LOOP );
		// オリジナルソ`ト用にストック
		setCharPrio( ext->ptActLeaderMark->bmpNo,
			ext->ptActLeaderMark->x, ext->ptActLeaderMark->y, 0, charIconOffsetY[no],
				ext->ptActLeaderMark->mx, ext->ptActLeaderMark->my );
	}


	// 庑gエフェクト
	if( (status & CHR_STATUS_USE_MAGIC) != 0
	 && ext->ptActMagicEffect == NULL )
	{
		// 庑gエフェクト表示用アクション作成
		ext->ptActMagicEffect =
			createCommmonEffectAction( SPR_effect01, ptAct->gx, ptAct->gy+1, 0, 0, DISP_PRIO_CHAR );
		if( pc.ptAct != NULL && pc.ptAct == ptAct )
		{
			delPcUseMagic();
		}
		else
		{
			delCharUseMagic( ptAct );
		}
	}
	else
	if( ext->ptActMagicEffect != NULL )
	{
		// 庑gエフェクトのアニメ`ション
		ext->ptActMagicEffect->x  = ptAct->x;
		ext->ptActMagicEffect->y  = ptAct->y;
		ext->ptActMagicEffect->mx = ptAct->mx;
		ext->ptActMagicEffect->my = ptAct->my;

		// アニメ`ションI理
		if( pattern( ext->ptActMagicEffect, ANM_NOMAL_SPD, ANM_NO_LOOP ) == 0 )
		{
			// オリジナルソ`ト用にストック
			setCharPrio( ext->ptActMagicEffect->bmpNo,
				ext->ptActMagicEffect->x, ext->ptActMagicEffect->y+1, 0, 0,
				ext->ptActMagicEffect->mx, ext->ptActMagicEffect->my );
		}
		else
		{
			// アニメ`ションル`プKわり
			DeathAction( ext->ptActMagicEffect );
			ext->ptActMagicEffect = NULL;
		}
	}


	// 吹き出しマ`ク
	if( (status & CHR_STATUS_FUKIDASHI) != 0 )
	{
		if( ext->drawFukidashiTime > GetTickCount() )
		{
			int no;
			U4 bmpNo;



			realGetNo( CG_ICON_FUKIDASI, &bmpNo );

			if( SPR_001em <= ptAct->anim_chr_no
			 && ptAct->anim_chr_no <= SPR_114bw )
			{
				// プレイヤ`キャラならこっち
				no = (ptAct->anim_chr_no - SPR_001em) / 20;
				no %= 12;	// 暴走しないよう念のため

				// オリジナルソ`ト用にストック
				setCharPrio( bmpNo, ptAct->x, ptAct->y,
					20, charIconOffsetY[no], ptAct->mx, ptAct->my );
			}
			else
			{
				// オリジナルソ`ト用にストック
				setCharPrio( bmpNo, ptAct->x, ptAct->y,
					20, -84, ptAct->mx, ptAct->my );
			}
		}
		else
		{
			status &= (~CHR_STATUS_FUKIDASHI);
		}
	}

}




// 基本エフェクト表示I理 /////////////////////////////////////////////////
ACTION *createCommmonEffectAction( int graNo, int gx, int gy, int anim, int dir, int prio )
{
	ACTION *ptAct;
	float mx, my;

	/* アクションリストに登h */
	ptAct = GetAction( PRIO_CHR, 0 );
	if( ptAct == NULL )
		return NULL;

	// g行v数
//	ptAct->func = charProc;
	// グラフィックの番号
	ptAct->anim_chr_no = graNo;
	// 幼鞣
	ptAct->anim_no = anim;
	// アニメ`ション向き( ０～７ )( 下が０で右回り )
	ptAct->anim_ang = dir;
	// 表示先度
	ptAct->dispPrio = prio;
	// 1行インフォ表示フラグ
	ptAct->atr = ACT_ATR_HIDE2;
	// 初期位置
	ptAct->nextGx = gx;					// マップグリッド座耍ㄒ酉龋
	ptAct->nextGy = gy;
	ptAct->gx = gx;						// マップグリッド座耍ìF在地）
	ptAct->gy = gy;
	ptAct->mx = (float)gx * GRID_SIZE;	// マップ座
	ptAct->my = (float)gy * GRID_SIZE;
	ptAct->vx = 0;						// 移分
	ptAct->vy = 0;

	// 画面表示位置
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)mx;
	ptAct->y = (int)my;

	return ptAct;
}


// 基本エフェクト表示I理（ル`プしない） /////////////////////////////////
// アニメ`ションを１ル`プしたらアクションをK了する
void commmonEffectNoLoop( ACTION *ptAct )
{
	float mx, my;

	// 画面表示位置
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)mx;
	ptAct->y = (int)my;

	// アニメ`ションI理
	if( pattern( ptAct, ANM_NOMAL_SPD, ANM_NO_LOOP ) == 0 )
	{
		// オリジナルソ`ト用にストック
		setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y+1, 0, 0, ptAct->mx, ptAct->my );
	}
	else
	{
		// アニメ`ションKわったのでアクションKわる
		DeathAction( ptAct );
		ptAct = NULL;
	}

}


ACTION *createCommmonEffectNoLoop( int graNo, int gx, int gy, int anim, int dir, int prio )
{
	ACTION *ptAct;
	float mx, my;

	/* アクションリストに登h */
	ptAct = GetAction( PRIO_CHR, 0 );
	if( ptAct == NULL )
		return NULL;

	// g行v数
	ptAct->func = commmonEffectNoLoop;
	// グラフィックの番号
	ptAct->anim_chr_no = graNo;
	// 幼鞣
	ptAct->anim_no = anim;
	// アニメ`ション向き( ０～７ )( 下が０で右回り )
	ptAct->anim_ang = dir;
	// 表示先度
	ptAct->dispPrio = prio;
	// 1行インフォ表示フラグ
	ptAct->atr = ACT_ATR_HIDE2;
	// 初期位置
	ptAct->nextGx = gx;					// マップグリッド座耍ㄒ酉龋
	ptAct->nextGy = gy;
	ptAct->gx = gx;						// マップグリッド座耍ìF在地）
	ptAct->gy = gy;
	ptAct->mx = (float)gx * GRID_SIZE;	// マップ座
	ptAct->my = (float)gy * GRID_SIZE;
	ptAct->vx = 0;						// 移分
	ptAct->vy = 0;

	// 画面表示位置
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)mx;
	ptAct->y = (int)my;

	return ptAct;
}




// キャラ幼涓 /////////////////////////////////////////////////////////
void changeCharAct( ACTION *ptAct, int x, int y, int dir, int action,
								int effectno, int effectparam1, int effectparam2 )
{
	CHAREXTRA *ext;
	int no;
	int i;


	if( ptAct == NULL )
		return;

	ext = (CHAREXTRA *)ptAct->pYobi;
	no = ext->charObjTblId;

	switch( action )
	{
		// 立ち状B
		case 0:
#if 0
#if 1
			// 仲g状Bの人には通常（特殊なrはe）はこないので
			// こちらのI理で良い
			setCharWarpPoint( ptAct, x, y );
			ptAct->bufCount = 0;
#else
			// NPCがパ`ティに入ってないrだけI理
			if( (charObj[no].status & CHR_STATUS_PARTY) == 0 )
			{
				setCharWarpPoint( ptAct, x, y );
				ptAct->bufCount = 0;
			}
#endif
#endif
			ptAct->anim_no = ANIM_STAND;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// ワ`プ
		case 31:
#if 1
			// 仲g状Bの人には通常（特殊なrはe）はこないので
			// こちらのI理で良い
			setCharWarpPoint( ptAct, x, y );
			ptAct->bufCount = 0;
#else
			// NPCがパ`ティに入ってないrだけI理
			if( (charObj[no].status & CHR_STATUS_PARTY) == 0 )
			{
				setCharWarpPoint( ptAct, x, y );
				ptAct->bufCount = 0;
			}
#endif
			ptAct->anim_no = ANIM_STAND;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			charObj[no].stockDir = -1;
			break;

		// 方向Q
		case 30:
#if 1		// F在地と座摔`うならiかせる
			charObj[no].stockDir = dir;
			charObj[no].stockDirX = x;
			charObj[no].stockDirY = y;
			if( (x == ptAct->nextGx && y == ptAct->nextGy)
			 || ABS( ABS( x - ptAct->nextGx ) - ABS( y - ptAct->nextGy ) ) > 1 )
			{
				break;
			}
#else
			ptAct->anim_ang = dir;
			break;
#endif

		// iく
		case 1:
			// NPCがリ`ダのrは移鏊颔攻骏氓巳毪欷I理する
			if( (charObj[no].status & CHR_STATUS_PARTY) != 0
			 && (charObj[no].status & CHR_STATUS_LEADER) != 0 )
			{
				// バッファが空いていたらためる
				if( ptAct->bufCount < sizeof( ptAct->bufGx )/sizeof( int ) )
				{
//					stockCharMovePoint( ptAct, x, y );
					correctCharMovePoint( ptAct, x, y );
				}
				else
				{
					// バッファが一杯のrはxれすぎているのでワ`プさせる
					stockCharMovePoint( ptAct, x, y );
					charObj[no].stockDir = -1;

					// 仲gも一wにワ`プ
					for( i = 1; i < MAX_PARTY; i++ )
					{
						if( party[i].useFlag != 0 )
						{
							if( party[i].id != pc.id )
							{
								// ＮＰＣのr
								party[i].ptAct->bufCount = 0;
								setCharWarpPoint( party[i].ptAct, x, y );
							}
							else
							{
								// ＰＣのr
								if( pc.ptAct != NULL )
								{
									pc.ptAct->bufCount = 0;
								}
								setPcWarpPoint( x, y );
							}
						}
					}
				}
			}
			else

#if 0
// 今は仲gのrにこのI理にこない
			// パ`ティiきのrはoする（moveProcで入れてもらう）
			if( (charObj[no].status & CHR_STATUS_PARTY) == 0 )
#endif
			{
				stockCharMovePoint( ptAct, x, y );
			}
			ptAct->anim_no = ANIM_WALK;
			//ptAct->anim_ang = dir;
			break;

		// 攻
		case 2:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_ATTACK;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// 投げる
		case 3:
			ptAct->anim_no = ANIM_THROW;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// ダメ`ジ
		case 4:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_DAMAGE;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// 死亡
		case 5:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_DEAD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// 魔法使う
		case 6:
			break;

		// アイテム使う
		case 7:
			break;

		// エフェクト
		case 8:		/* UsedMagic */
			break;

		// 倒れる
		case 10:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_DEAD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// 座る
		case 11:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_SIT;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// 手を振る
		case 12:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_HAND;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// 喜ぶ
		case 13:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_HAPPY;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// 怒る
		case 14:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_ANGRY;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// 悲しむ
		case 15:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_SAD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// ガ`ド
		case 16:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_GUARD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// iく（幼鳏坤保
		case 17:
			ptAct->anim_no = ANIM_WALK;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;

		// うなずく
		case 18:
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_no = ANIM_NOD;
			ptAct->anim_no_bak = -1;
			ptAct->anim_ang = dir;
			break;


		// 殛L情
		case 20:
			if( effectno >= 0 )
			{
				setCharBattle( ptAct, effectno, effectparam1, effectparam2 );
				// 立ちアニメにかえる
				ptAct->anim_no = ANIM_STAND;
				ptAct->anim_no_bak = -1;
			}
			else
			{
				delCharBattle( ptAct );
			}
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_ang = dir;
			break;

		// リ`ダ`情
		case 21:
			if( effectno == 1 )
			{
				setCharLeader( ptAct );
			}
			else
			{
				delCharLeader( ptAct );
			}
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_ang = dir;
			break;

		// Q榍
		case 22:
			if( effectno == 1 )
			{
				setCharWatch( ptAct );
			}
			else
			{
				delCharWatch( ptAct );
			}
			setCharWarpPoint( ptAct, x, y );
			ptAct->anim_ang = dir;
			break;
	}
}




// キャラ管理テ`ブルI理 /////////////////////////////////////////////////
void charObjProc( void )
{
#if 0
	int i;

	for( i = 0; i < tailCharObj; i++ )
	{
		// 表示されていないなら表示する
		if( charObj[i].use == CHAROBJ_USE_STAY )
		{
			charObj[i].ptAct = createCharAction( charObj[i].graNo, charObj[i].gx, charObj[i].gy, 0 );
			if( charObj[i].ptAct != NULL )
			{
				charObj[i].use = CHAROBJ_USE_VIEW;
			}
		}
	}
#endif
}


// ID仕
//  0以上 ... IDのある位置 / -1 ... oかった
int searchCharObjId( int id )
{
	int i;
	int no = -1;

	for( i = searchCharObj; i < tailCharObj; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE
		 && charObj[i].id == id )
		{
			no = i;
			searchCharObj = i;
			break;
		}
	}
	if( no < 0 && searchCharObj > 0 )
	{
		for( i = 0; i < searchCharObj; i++ )
		{
			if( charObj[i].use != CHAROBJ_USE_FREE
			 && charObj[i].id == id )
			{
				no = i;
				searchCharObj = i;
				break;
			}
		}
	}
	return no;
}


// 座仕
// TRUE ... そこに何かある
BOOL checkCharObjPoint( int gx, int gy, short type )
{
	int i;
	int no = -1;

	for( i = 0; i < tailCharObj; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE && charObj[i].ptAct != NULL )
		{
			if( charObj[i].ptAct->gx == gx && charObj[i].ptAct->gy == gy
			 && (charObj[i].type & type) != 0 )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


// 座耍Ε攻譬`タス仕
// 2 ... そこに何かありそのステ`タス
//         1 ... そこに何かある
//         0 ... 何もない
int checkCharObjPointStatus( int gx, int gy, short type, unsigned short status )
{
	int i;
	int no = -1;

	for( i = 0; i < tailCharObj; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE && charObj[i].ptAct != NULL )
		{
			if( charObj[i].ptAct->gx == gx && charObj[i].ptAct->gy == gy
			 && (charObj[i].type & type) != 0 )
			{
				if( (charObj[i].status & status /*CHR_STATUS_BATTLE*/) != 0 )
				{
					return 2;
				}
				else
				{
					return 1;
				}
			}
		}
	}

	return 0;
}


// 座耍Ε攻譬`タス仕
// 1 ... そのステ`タスじゃない何かがある
//         0 ... 何もない
int checkCharObjPointNotStatus( int gx, int gy, short type, unsigned short status )
{
	int i;
	int no = -1;

	for( i = 0; i < tailCharObj; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE && charObj[i].ptAct != NULL )
		{
			if( charObj[i].ptAct->gx == gx && charObj[i].ptAct->gy == gy
			 && (charObj[i].type & type) != 0 )
			{
				if( (charObj[i].status & status ) == 0 )
				{
					return 1;
				}
			}
		}
	}

	return 0;
}


// キャラ管理テ`ブルに登h
//
//   NPC用
//
void setNpcCharObj( int id, int graNo, int gx, int gy, int dir, 
char *name, char *freeName, int level, int nameColor, int walk, int height, int charType )
{
	int no;
	BOOL existFlag = FALSE;
	CHAREXTRA *ext;

	// 登hされているかチェック
	if( (no = searchCharObjId( id )) >= 0 )
	{
		existFlag = TRUE;
	}

	// まだ登hされていないなら登h
	if( !existFlag )
	{
		no = getCharObjBuf();
		if( no < 0 )
			return;	// テ`ブルが亥骏螭胜榻Kわる

		charObj[no].ptAct = createCharAction( graNo, gx, gy, dir );
		if( charObj[no].ptAct == NULL )
		{
			// 登hできないならKわる（本当はあっては行けない）
			return;
		}

		charObj[no].use = CHAROBJ_USE_VIEW;
		ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
		ext->charObjTblId = no;

		charObj[no].type      = getAtrCharObjType( charType );
		charObj[no].id        = id;
		charObj[no].stockDir  = -1;
	}
	else
	// 登hされていたら涓
	{
		// 移婴丹护
#if 0
		stockCharMovePoint( charObj[no].ptAct, gx, gy );
		charObj[no].ptAct->anim_chr_no = graNo;
		charObj[no].ptAct->anim_ang = dir;
#endif
	}

	charObj[no].graNo     = graNo;
	charObj[no].nameColor = nameColor;
	charObj[no].level     = level;
	charObj[no].gx = gx;
	charObj[no].gy = gy;
	charObj[no].dir = dir;
	if( walk != 0 )
	{
		charObj[no].status |= CHR_STATUS_W;
	}
	if( height != 0 )
	{
		charObj[no].status |= CHR_STATUS_H;
	}

	if( strlen( name ) <= CHAR_NAME_LEN )
	{
		strcpy( charObj[no].name, name );
	}
	if( strlen( freeName ) <= CHAR_FREENAME_LEN )
	{
		strcpy( charObj[no].freeName, freeName );
	}

	charObj[no].charType = getAtrCharType( charType );

	charObj[no].newFoundFlag = 1;	// 新kチェックをする

	// アクションがあるrだけ蛉毪欷
	if( charObj[no].ptAct == NULL )
		return;

	// アクションがあるrにＣプロトコルが呼ばれたら
	// X,Y,DIRはoする
	charObj[no].ptAct->anim_chr_no = graNo;
	charObj[no].ptAct->level = level;
	charObj[no].ptAct->atr |= charObj[no].charType;
	if( strlen( name ) <= CHAR_NAME_LEN )
	{
		strcpy( charObj[no].ptAct->name, name );
	}
	if( strlen( freeName ) <= CHAR_FREENAME_LEN )
	{
		strcpy( charObj[no].ptAct->freeName, freeName );
	}
}




//
// ペットメ`ルでペットが盲皮I理用
//
BOOL setReturnPetObj( int id, int graNo, int gx, int gy, int dir, 
char *name, char *freeName, int level, int nameColor, int walk, int height, int charType )
{
	int no;

	// 登hされているかチェック
	if( (no = searchCharObjId( id )) >= 0 )
	{
		// いたらKわる
		return FALSE;
	}

	// まだ登hされていないなら登h
	no = getCharObjBuf();
	if( no < 0 )
		return FALSE;	// テ`ブルが亥骏螭胜榻Kわる

	charObj[no].ptAct = NULL;
	charObj[no].use = CHAROBJ_USE_VIEW;
	charObj[no].type      = getAtrCharObjType( charType );
	charObj[no].id        = id;

	charObj[no].graNo     = graNo;
	charObj[no].nameColor = nameColor;
	charObj[no].level     = level;
	charObj[no].gx = gx;
	charObj[no].gy = gy;
	charObj[no].dir = dir;
	if( walk != 0 )
	{
		charObj[no].status |= CHR_STATUS_W;
	}
	if( height != 0 )
	{
		charObj[no].status |= CHR_STATUS_H;
	}

	if( strlen( name ) <= CHAR_NAME_LEN )
	{
		strcpy( charObj[no].name, name );
	}
	if( strlen( freeName ) <= CHAR_FREENAME_LEN )
	{
		strcpy( charObj[no].freeName, freeName );
	}

	charObj[no].charType = getAtrCharType( charType );

	return TRUE;
}



//
//   アイテム用
//
void setItemCharObj( int id, int graNo, int gx, int gy, int dir, int classNo, char *info )
{
	int no;
	BOOL existFlag = FALSE;
	CHAREXTRA *ext;

	// 登hされているかチェック
	if( (no = searchCharObjId( id )) >= 0 )
	{
		existFlag = TRUE;
	}

	// まだ登hされていないなら登h
	if( !existFlag )
	{
		no = getCharObjBuf();
		if( no < 0 )
			return;	// テ`ブルが亥骏螭胜榻Kわる

		charObj[no].ptAct = createCharAction( graNo, gx, gy, dir );
		if( charObj[no].ptAct == NULL )
		{
			// 登hできないならKわる（本当はあっては行けない）
			return;
		}

		charObj[no].use = CHAROBJ_USE_VIEW;
		ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
		ext->charObjTblId = no;

		charObj[no].type      = CHAROBJ_TYPE_ITEM;
		charObj[no].id        = id;
		charObj[no].stockDir  = -1;
	}
	else
	// 登hされていたら涓
	{
#if 0
		// 移婴丹护
		stockCharMovePoint( charObj[no].ptAct, gx, gy );
		charObj[no].ptAct->anim_ang = dir;
#endif
	}

	charObj[no].graNo   = graNo;
	charObj[no].classNo = classNo;
	charObj[no].gx = gx;
	charObj[no].gy = gy;
	charObj[no].dir = dir;
	if( strlen( info ) <= 60 )
	{
		strcpy( charObj[no].info, info );
	}
	charObj[no].charType = ACT_ATR_TYPE_ITEM;
	charObj[no].newFoundFlag = 0;	// 新kチェックをしない

	// アクションがあるrだけ蛉毪欷
	if( charObj[no].ptAct == NULL )
		return;

	charObj[no].ptAct->anim_chr_no = graNo;
	// 1行インフォに出すためにnameに入れる
	if( strlen( info ) <= ITEM_NAME_LEN )
	{
		strcpy( charObj[no].ptAct->name, info );
	}
	else
	{
		strcpy( charObj[no].ptAct->name, "???" );
	}
	charObj[no].ptAct->atr |= ACT_ATR_TYPE_ITEM;
	charObj[no].ptAct->itemNameColor = classNo;
}


//
//   お金用
//
void setMoneyCharObj( int id, int graNo, int gx, int gy, int dir, int money, char *info )
{
	int no;
	BOOL existFlag = FALSE;
	CHAREXTRA *ext;

	// 登hされているかチェック
	if( (no = searchCharObjId( id )) >= 0 )
	{
		existFlag = TRUE;
	}

	// まだ登hされていないなら登h
	if( !existFlag )
	{
		no = getCharObjBuf();
		if( no < 0 )
			return;	// テ`ブルが亥骏螭胜榻Kわる


		charObj[no].ptAct = createCharAction( graNo, gx, gy, dir );
		if( charObj[no].ptAct == NULL )
		{
			// 登hできないならKわる（本当はあっては行けない）
			return;
		}

		charObj[no].use = CHAROBJ_USE_VIEW;
		ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
		ext->charObjTblId = no;

		charObj[no].type      = CHAROBJ_TYPE_MONEY;
		charObj[no].id        = id;
		charObj[no].stockDir  = -1;
	}
	else
	// 登hされていたら涓
	{
#if 0
		// 移婴丹护
		stockCharMovePoint( charObj[no].ptAct, gx, gy );
		charObj[no].ptAct->anim_ang = dir;
#endif
	}

	charObj[no].graNo = graNo;
	charObj[no].money = money;
	charObj[no].gx = gx;
	charObj[no].gy = gy;
	charObj[no].dir = dir;
	if( strlen( info ) <= 60 )
	{
		strcpy( charObj[no].info, info );
	}
	charObj[no].charType = ACT_ATR_TYPE_GOLD;
	charObj[no].newFoundFlag = 0;	// 新kチェックをしない

	// アクションがあるrだけ蛉毪欷
	if( charObj[no].ptAct == NULL )
		return;

	charObj[no].ptAct->anim_chr_no = graNo;
	// 1行インフォに出すためにnameに入れる
	if( strlen( info ) <= CHAR_NAME_LEN )
	{
		strcpy( charObj[no].ptAct->name, info );
	}
	else
	{
		strcpy( charObj[no].ptAct->name, "???" );
	}
	charObj[no].ptAct->atr |= ACT_ATR_TYPE_GOLD;
}



// キャラ管理バッファの取得
//  charObj配列の添えr
//          -1 ... バッファがない
int getCharObjBuf( void )
{
	int i;
	int ret;

	// テ`ブルが亥骏螭胜榻Kわる
	if( maxCharObj >= MAX_CHAROBJ )
		return -1;

	ret = freeCharObj;

	// 追加したら最大
	maxCharObj++;

	// 最後尾の登h位置
	if( freeCharObj+1 > tailCharObj )
		tailCharObj = freeCharObj+1;

	// 次の空きをつけておく
	for( i = freeCharObj+1; i < MAX_CHAROBJ; i++ )
	{
		if( charObj[i].use == CHAROBJ_USE_FREE )
		{
			freeCharObj = i;
			break;
		}
	}
	if( freeCharObj > 0 && i >= MAX_CHAROBJ )
	{
		for( i = 0; i < freeCharObj; i++ )
		{
			if( charObj[i].use == CHAROBJ_USE_FREE )
			{
				freeCharObj = i;
				break;
			}
		}
	}

	return ret;
}



// キャラのNから属性用のパラメ`タを作る
//  引  数：type ... サ`バで管理されているN番号
//  亥伐绁螭atrにO定する属性
int getAtrCharType( int type )
{
	switch( type )
	{
		// ＮＰＣ
		case CHAR_TYPEPLAYER:
			return ACT_ATR_TYPE_OTHER_PC;

		// ペット
		case CHAR_TYPEPET:
			return ACT_ATR_TYPE_PET;

		// その他
		default:
			return ACT_ATR_TYPE_OTHER;
	}

	return 0;
}


// キャラのNからキャラ管理用のNを取り出す
//  引  数：type ... サ`バで管理されているN番号
//  charObjのtypeにO定する属性
int getAtrCharObjType( int type )
{
	int ret;


	switch( type )
	{
		case CHAR_TYPEPLAYER:
			ret = CHAROBJ_TYPE_USER_NPC;
			break;

		case CHAR_TYPEDENGON:
		case CHAR_TYPEDUELRANKING:
		case CHAR_TYPEMSG:
			ret = CHAROBJ_TYPE_NPC | CHAROBJ_TYPE_LOOKAT;
			break;

		default:
			ret = CHAROBJ_TYPE_NPC;
			break;
	}

	return ret;
}


// IDをみてアクションポインタを返す
// それ以外はNULL
ACTION *getCharObjAct( int id )
{
	int no;

	if( (no = searchCharObjId( id )) < 0 )
		return NULL;

	if( charObj[no].ptAct == NULL )
		return NULL;

	return charObj[no].ptAct;
}


// キャラ管理テ`ブルから抹消
void delCharObj( int id )
{
	int i;
	int no;
	CHAREXTRA *ext;

	if( pc.id == id )
	{
		resetPc();
		return;
	}

	no = searchCharObjId( id );

	// IDつけたので抹消
	if( no >= 0 )
	{
		if( charObj[no].ptAct != NULL )
		{
			ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
			if( ext != NULL )
			{
				if( ext->ptActLeaderMark != NULL )
				{
					DeathAction( ext->ptActLeaderMark );
					ext->ptActLeaderMark = NULL;
				}
				if( ext->ptActMagicEffect != NULL )
				{
					DeathAction( ext->ptActMagicEffect );
					ext->ptActMagicEffect = NULL;
				}
			}
			DeathAction( charObj[no].ptAct );
			charObj[no].ptAct = NULL;
		}
		charObj[no].use = CHAROBJ_USE_FREE;
		charObj[no].status = 0;
		charObj[no].stockDir = -1;

		// 登h数更新
		if( maxCharObj > 0 )
			maxCharObj--;
		// 今の空き位置より前のrはこちらをあき位置にする
		if( freeCharObj > no )
			freeCharObj = no;
#if 1
		// 登hされている最後を探す
		for( i = tailCharObj; i > 0; i-- )
		{
			if( charObj[i-1].use != CHAROBJ_USE_FREE )
			{
				tailCharObj = i;
				break;
			}
		}
#endif
	}
}


// キャラ管理テ`ブル初期化
void initCharObj( void )
{
	int i;

	maxCharObj = 0;
	tailCharObj = 0;
	freeCharObj = 0;
	for( i = 0; i < MAX_CHAROBJ; i++ )
	{
		charObj[i].use = CHAROBJ_USE_FREE;
		charObj[i].ptAct = NULL;
		charObj[i].status = 0;
		charObj[i].id = 0;
		charObj[i].stockDir = -1;
	}

	searchCharObj = 0;
}


// キャラ管理テ`ブルリセット
void resetCharObj( void )
{
	int i;
	CHAREXTRA *ext;

	maxCharObj = 0;
	tailCharObj = 0;
	freeCharObj = 0;
	for( i = 0; i < MAX_CHAROBJ; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE )
		{
			if( charObj[i].ptAct != NULL )
			{
				ext = (CHAREXTRA *)charObj[i].ptAct->pYobi;
				if( ext != NULL )
				{
					if( ext->ptActLeaderMark != NULL )
					{
						DeathAction( ext->ptActLeaderMark );
						ext->ptActLeaderMark = NULL;
					}
					if( ext->ptActMagicEffect != NULL )
					{
						DeathAction( ext->ptActMagicEffect );
						ext->ptActMagicEffect = NULL;
					}
				}
				DeathAction( charObj[i].ptAct );
				charObj[i].ptAct = NULL;
			}
			charObj[i].use = CHAROBJ_USE_FREE;
			charObj[i].status = 0;
			charObj[i].id = 0;
			charObj[i].stockDir = -1;
		}
	}

	searchCharObj = 0;
}


// キャラ管理テ`ブルのアクションポインタを抹消する
void clearPtActCharObj( void )
{
	int i;

	for( i = 0; i < MAX_CHAROBJ; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE )
		{
			charObj[i].ptAct = NULL;
			charObj[i].stockDir = -1;
		}
	}
}


// キャラ管理テ`ブルのアクションポインタを全て活させる
void restorePtActCharObjAll( void )
{
	int i;
	CHAREXTRA *ext;

	for( i = 0; i < MAX_CHAROBJ; i++ )
	{
		if( charObj[i].use != CHAROBJ_USE_FREE
		 && charObj[i].ptAct == NULL )
		{
			charObj[i].ptAct =
				createCharAction( charObj[i].graNo, charObj[i].gx, charObj[i].gy, charObj[i].dir );
			if( charObj[i].ptAct == NULL )
			{
				continue;
			}

			charObj[i].use = CHAROBJ_USE_VIEW;

			ext = (CHAREXTRA *)charObj[i].ptAct->pYobi;
			ext->charObjTblId = i;

			// NPCのアクション活
			// 他のPCのアクション活
			// 谎园
			if( charObj[i].type == CHAROBJ_TYPE_NPC
			 || charObj[i].type == CHAROBJ_TYPE_USER_NPC )
			{
				charObj[i].ptAct->level = charObj[i].level;
				strcpy( charObj[i].ptAct->name, charObj[i].name );
				charObj[i].ptAct->atr |= charObj[i].charType;
			}
			else
			// Itemのアクション活
			if( charObj[i].type == CHAROBJ_TYPE_ITEM )
			{
				// 1行インフォに出すためにnameに入れる
				if( strlen( charObj[i].info ) <= ITEM_NAME_LEN )
				{
					strcpy( charObj[i].ptAct->name, charObj[i].info );
				}
				else
				{
					strcpy( charObj[i].ptAct->name, "???" );
				}
				charObj[i].ptAct->atr |= charObj[i].charType;
			}
			else
			// お金のアクション活
			if( charObj[i].type == CHAROBJ_TYPE_MONEY )
			{
				// 1行インフォに出すためにnameに入れる
				if( strlen( charObj[i].info ) <= CHAR_NAME_LEN )
				{
					strcpy( charObj[i].ptAct->name, charObj[i].info );
				}
				else
				{
					strcpy( charObj[i].ptAct->name, "???" );
				}
				charObj[i].ptAct->atr |= charObj[i].charType;
			}
		}
	}
}


// キャラ管理テ`ブルのアクションポインタを活させる
void restorePtActCharObj( int id )
{
	int no;
	CHAREXTRA *ext;

	if( id < 0 )
		return;

	// 登hされているかチェック
	if( (no = searchCharObjId( id )) < 0 )
	{
		// oかったらKわり
		return;
	}

	if( charObj[no].use != CHAROBJ_USE_FREE
	 && charObj[no].ptAct == NULL )
	{
		charObj[no].ptAct =
			createCharAction( charObj[no].graNo, charObj[no].gx, charObj[no].gy, charObj[no].dir );
		if( charObj[no].ptAct == NULL )
		{
			// アクション作れなかったのでKわり
			return;
		}

		charObj[no].use = CHAROBJ_USE_VIEW;

		ext = (CHAREXTRA *)charObj[no].ptAct->pYobi;
		ext->charObjTblId = no;

		charObj[no].ptAct->level = charObj[no].level;
		strcpy( charObj[no].ptAct->name, charObj[no].name );
		charObj[no].ptAct->atr |= charObj[no].charType;
	}
}


// キャラ位置更新
void setMovePointCharObj( int id, int nextGx, int nextGy )
{
	int no;

	no = searchCharObjId( id );
	if( no >= 0 )
	{
		stockCharMovePoint( charObj[no].ptAct, nextGx, nextGy );
	}
}




///////////////////////////////////////////////////////////////////////////
// 同一アイテムが同じ座摔摔郡丹蟪訾r１回しか表示しないためのI理

typedef struct
{
	unsigned int bmpNo;
	int gx;
	int gy;
} ITEM_OVERLAP_CHECK;

#define MAX_ITEM_OVERLAP	100
ITEM_OVERLAP_CHECK itemOverlapTbl[MAX_ITEM_OVERLAP];
int itemOverlapCheckCnt;

void initItemOverlapCheck( void )
{
	itemOverlapCheckCnt = 0;
}


//  TRUE  ... 同じ物がある
//          FALSE ... ない
BOOL itemOverlapCheck( unsigned int bmpNo, int gx, int gy )
{
	int i;
	BOOL flag = FALSE;

	for( i = 0; i < itemOverlapCheckCnt; i++ )
	{
		if( itemOverlapTbl[i].bmpNo == bmpNo
		 && itemOverlapTbl[i].gx == gx
		 && itemOverlapTbl[i].gy == gy )
		{
			flag = TRUE;
			break;
		}
	}

	if( !flag )
	{
		if( itemOverlapCheckCnt < MAX_ITEM_OVERLAP )
		{
			itemOverlapTbl[itemOverlapCheckCnt].bmpNo = bmpNo;
			itemOverlapTbl[itemOverlapCheckCnt].gx    = gx;
			itemOverlapTbl[itemOverlapCheckCnt].gy    = gy;
			itemOverlapCheckCnt++;
		}
	}

	return flag;
}






///////////////////////////////////////////////////////////////////////////
// ペットメ`ルの演出


// 向きがグリッドの斜め方向なら修正する
void limitCantClientDir( short *dir )
{
	// dirが偶数なら斜めなので奇数にする
	if( ((*dir) % 2) == 0 )
	{
		(*dir)++;
	}
}

// dir 0 から 7 になるよう修正する
void ajustClientDir( short *dir )
{
	if( *dir < 0 )
	{
		do
		{
			(*dir) += 8;
		} while( *dir < 0 );
	}
	else
	if( *dir > 7 )
	{
		do
		{
			(*dir) -= 8;
		} while( *dir > 7 );
	}
}


void getPetRoute( ACTION *ptAct )
{
	short dir;
	int i;
	int dx, dy;
	int gx, gy;
	PETEXTRA *pe;

	if( ptAct->bufCount > 0 )
		return;

	// 当たり判定がずれるのでやめる
	if( nowGx != oldGx || nowGy != oldGy )
		return;

	pe = (PETEXTRA *)ptAct->pYobi;

	dir = pe->moveDir;

	for( i = 0; i < 4; i++ )
	{
		getRouteData( dir, &dx, &dy );
		gx = ptAct->gx+dx;
		gy = ptAct->gy+dy;
		if( (gx != pe->preGx || gy != pe->preGy)
		 && checkHitMap( gx, gy ) == FALSE )
		{
			break;
		}
		dir += (2*(i+1));
		ajustClientDir( &dir );
	}

	if( i >= 4 )
	{
		pe->preGx = -1;
		pe->preGy = -1;
		pe->moveDir -= 2;
		ajustClientDir( &pe->moveDir );
		return;
	}

	if( pe->dirCnt >= 2 )
	{
		pe->dirCnt = 0;
		pe->moveDir -= 6;
		ajustClientDir( &pe->moveDir );
	}

	if( dir != pe->preDir )
	{
		pe->dirCnt++;
	}
	else
	{
		pe->dirCnt = 0;
	}

	pe->preDir = dir;

	ptAct->bufGx[ptAct->bufCount] = gx;
	ptAct->bufGy[ptAct->bufCount] = gy;
	ptAct->bufCount++;

	pe->preGx = ptAct->gx;
	pe->preGy = ptAct->gy;

	return;
}


// 移I理
BOOL petMoveProc( ACTION *ptAct )
{
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;

	// 焱猡顺訾郡椁盲皮讼
	// 作成から8秒以上たったら消す
	if( ABS( ptAct->gx - nowGx ) >= 13
	 || ABS( ptAct->gy - nowGy ) >= 13
	 || (pe->createTime+8000 < GetTickCount() && ptAct->vx == 0 && ptAct->vy == 0) )
	{
		restorePtActCharObj( pe->id );
		DeathAction( ptAct );
		ptAct = NULL;
		return FALSE;
	}

	if( pe->ptAct == NULL && pe->createTime+7200 < GetTickCount() )
	{
		pe->ptAct = createCommmonEffectNoLoop( SPR_difence, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}
	if( pe->ptAct2 == NULL && pe->createTime+7500 < GetTickCount() )
	{
		pe->ptAct2 = createCommmonEffectNoLoop( SPR_difence, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}


	if( ptAct->vx == 0 && ptAct->vy == 0 )
	{
		// 移婴Kわったら次の位置を仕
		getPetRoute( ptAct );
	}
	charMove( ptAct );

	if( pe->ptAct != NULL )
	{
		pe->ptAct->mx = ptAct->mx;
		pe->ptAct->my = ptAct->my;
	}
	if( pe->ptAct2 != NULL )
	{
		pe->ptAct2->mx = ptAct->mx;
		pe->ptAct2->my = ptAct->my;
	}

	return TRUE;
}


// 到着I理
BOOL uprisePetProc( ACTION *ptAct )
{
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;

	if( pe->ptAct == NULL && pe->createTime < GetTickCount() )
	{
		pe->ptAct = createCommmonEffectNoLoop( SPR_difence, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}
	if( pe->ptAct2 == NULL && pe->createTime+500 < GetTickCount() )
	{
		pe->ptAct2 = createCommmonEffectNoLoop( SPR_difence, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}

	// 数秒gはペットを消しておく
	if( pe->createTime+1200 < GetTickCount() )
	{
		restorePtActCharObj( pe->id );
		DeathAction( ptAct );
		ptAct = NULL;
	}

	return FALSE;
}


// 回I理（外へがる）
BOOL petCircleOutProc( ACTION *ptAct )
{
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;
	float mx, my;
	float dx, dy;
	float angle;
	int i;

	if( pe->ptAct == NULL )
	{
		pe->ptAct = (ACTION *)1;
		pe->angle = 0.0F;
		pe->r = 0;
		createCommmonEffectNoLoop( SPR_hoshi, ptAct->gx, ptAct->gy,
			0, 0, ptAct->dispPrio );
	}

	if( pe->r > 800 )
	{
		restorePtActCharObj( pe->id );
		DeathAction( ptAct );
		ptAct = NULL;
		return FALSE;
	}

	if( pe->ptAct != NULL && pe->createTime+1000 < GetTickCount() )
	{
		pe->r += 4;
		pe->angle += 6.0F;
	}

	angle = pe->angle;
	for( i = 0; i < 3; i++ )
	{
		dx = (float)pe->r * CosT( angle );
		dy = (float)pe->r * SinT( angle );

		// 画面表示位置
		camMapToGamen( ptAct->mx+dx, ptAct->my+dy, &mx, &my );
		ptAct->x = (int)mx;
		ptAct->y = (int)my;

		// アニメ`ションI理
		pattern( ptAct, ANM_NOMAL_SPD, ANM_LOOP );

		// オリジナルソ`ト用にストック
		setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx+dx, ptAct->my+dy );
		angle += 120;
		AdjustDir( &angle );
	}

	return FALSE;
}


// 回I理（内に狭まる）
BOOL petCircleInProc( ACTION *ptAct )
{
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;
	float mx, my;
	float dx, dy;
	float angle;
	int i;

	if( pe->ptAct == NULL )
	{
		pe->ptAct  = (ACTION *)1;
		pe->ptAct2 = (ACTION *)NULL;
		pe->angle = 0.0F;
		pe->r = 800;
	}
	else
	{
		if( pe->r <= 0 )
		{
			restorePtActCharObj( pe->id );
			DeathAction( ptAct );
			ptAct = NULL;
			return TRUE;
		}
	}

	if( pe->ptAct != NULL )
	{
		if( pe->ptAct2 == NULL && pe->createTime+2300 < GetTickCount() )
		{
			pe->ptAct2 = createCommmonEffectNoLoop( SPR_hoshi, ptAct->gx, ptAct->gy,
				0, 0, ptAct->dispPrio );
		}

		pe->r -= 4;
		pe->angle += 6.0F;

		angle = pe->angle;
		for( i = 0; i < 3; i++ )
		{
			dx = (float)pe->r * CosT( angle );
			dy = (float)pe->r * SinT( angle );

			// 画面表示位置
			camMapToGamen( ptAct->mx+dx, ptAct->my+dy, &mx, &my );
			ptAct->x = (int)mx;
			ptAct->y = (int)my;

			// アニメ`ションI理
			pattern( ptAct, ANM_NOMAL_SPD, ANM_LOOP );

			// オリジナルソ`ト用にストック
			setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx+dx, ptAct->my+dy );
			angle += 120;
			AdjustDir( &angle );
		}
	}

	return FALSE;
}


// ペットアクションのデフォルト
void petProc( ACTION *ptAct )
{
	float mx, my;
	int animLoop;
	PETEXTRA *pe = (PETEXTRA *)ptAct->pYobi;

	switch( pe->mode )
	{
		// メ`ル送信r：iく`＞消える
		case 0:
			if( !petMoveProc( ptAct ) )
			{
				return;
			}
			break;

		// メ`ル着信r：
		// ペットが盲评搐郡趣撼霈F
		case 1:
			if( !uprisePetProc( ptAct ) )
			{
				return;
			}
			break;

		// メ`ル送信r：分身回`＞消える
		case 2:
			if( !petCircleOutProc( ptAct ) )
			{
				return;
			}
			break;

		// メ`ル受信r：分身回`＞Fれる
		case 3:
			if( !petCircleInProc( ptAct ) )
			{
				return;
			}
			break;
	}

	// 画面表示位置
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)mx;
	ptAct->y = (int)my;

	// アニメ`ションI理
	if( ptAct->anim_no == ANIM_HAND
	 || ptAct->anim_no == ANIM_HAPPY
	 || ptAct->anim_no == ANIM_ANGRY
	 || ptAct->anim_no == ANIM_SAD
	 || ptAct->anim_no == ANIM_WALK
	 || ptAct->anim_no == ANIM_STAND
	 || ptAct->anim_no == ANIM_NOD )
	{
		animLoop = ANM_LOOP;
	}
	else
	{
		animLoop = ANM_NO_LOOP;
	}
	pattern( ptAct, ANM_NOMAL_SPD, animLoop );

	// オリジナルソ`ト用にストック
	setCharPrio( ptAct->bmpNo, ptAct->x, ptAct->y, 0, 0, ptAct->mx, ptAct->my );
}



// ペットのアクション作成
ACTION *createPetAction( int graNo, int gx, int gy, int dir, int mode, int moveDir, int id )
{
	ACTION *ptAct;
	float mx, my;
	PETEXTRA *pe;

	/* アクションリストに登h */
	ptAct = GetAction( PRIO_CHR, sizeof( PETEXTRA ) );
	if( ptAct == NULL )
		return NULL;

	// g行v数
	ptAct->func = petProc;
	// グラフィックの番号
	ptAct->anim_chr_no = graNo;
	// 幼鞣
	ptAct->anim_no = ANIM_STAND;
	// アニメ`ション向き( ０～７ )( 下が０で右回り )
	ptAct->anim_ang = dir;
	// 表示先度
	ptAct->dispPrio = DISP_PRIO_CHAR;
	// 1行インフォ表示フラグ
	ptAct->atr = ACT_ATR_INFO |	ACT_ATR_HIT | ACT_ATR_HIDE2;
	// 初期位置
	ptAct->nextGx = gx;					// マップグリッド座耍ㄒ酉龋
	ptAct->nextGy = gy;
	ptAct->bufCount = 0;
	ptAct->gx = gx;						// マップグリッド座耍ìF在地）
	ptAct->gy = gy;
	ptAct->mx = (float)gx * GRID_SIZE;	// マップ座
	ptAct->my = (float)gy * GRID_SIZE;
	ptAct->vx = 0;						// 移分
	ptAct->vy = 0;

	// 画面表示位置
	camMapToGamen( ptAct->mx, ptAct->my, &mx, &my );
	ptAct->x = (int)mx;
	ptAct->y = (int)my;

	pe = (PETEXTRA *)ptAct->pYobi;
	pe->mode = mode;
	pe->moveDir = moveDir;
	limitCantClientDir( &pe->moveDir );
	pe->preDir = pe->moveDir;
	pe->dirCnt = 0;
	pe->preGx = gx;
	pe->preGy = gy;
	pe->walkCnt = 0;
	pe->createTime = GetTickCount();
	pe->ptAct  = NULL;
	pe->ptAct2 = NULL;
	pe->angle = 0.0F;
	pe->r = 0;

	pe->id = id;

	return ptAct;
}

