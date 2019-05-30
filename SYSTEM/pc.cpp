#include"system.h"
#include"action.h"
#include"pc.h"
#include"character.h"
#include"map.h"
#include"tool.h"
#include"anim_tbl.h"
#include"loadsprbin.h"
#include"netmain.h"
#include"lssproto_cli.h"
#include"savedata.h"
#include"menu.h"
#include"field.h"
#include"login.h"

// プレイヤ`キャラデ`タ
PC pc;
short maxPcNo;			// キャラリストに登hされてるキャラの数
short selectPcNo;		// キャラxkrにxkされたキャラリストの番号
short prSendMode = 0;	// ＰＲ送信モ`ド 0 ... 除要求 / 1 ... 仲g要求
short prSendFlag = 0;	// ＰＲプロトコル 1 ... 送信

short jbSendFlag;		// 参楗抓恁去偿胨托
short duelSendFlag;		// デュエルI理用
//short duelFlag;

int loginDp;			// pc初期化rに参照するデュエルポイント

short helpFlag = 0;		// 殛L中のヘルプボタンの状B

// ペットデ`タ
PET pet[MAX_PET];


// 庑gデ`タ
MAGIC magic[MAX_MAGIC];

// 仲g情
PARTY party[MAX_PARTY];
short partyModeFlag = 0;	// F在パ`ティモ`ドか


// キャラリスト用
CHARLISTTABLE chartable[MAXCHARACTER];


// アドレスブック
ADDRESS_BOOK addressBook[MAX_ADR_BOOK];


// 殛LY果メッセ`ジ
BATTLE_RESULT_MSG battleResultMsg;


// ペットのスキル
PET_SKILL petSkill[MAX_PET][MAX_SKILL];


char newCharacterName[CHAR_NAME_LEN+1];	// 新キャラの名前
int newCharacterGraNo;				// 新キャラの画像番号
int newCharacterFaceGraNo;			// 新キャラの画像番号
int newCharacterVit;				// 新キャラの体力
int newCharacterStr;				// 新キャラの腕力
int newCharacterTgh;				// 新キャラのB丈さ
int newCharacterDex;				// 新キャラの素早さ
int newCharacterEarth;				// 新キャラの地属性
int newCharacterWater;				// 新キャラの水属性
int newCharacterFire;				// 新キャラの火属性
int newCharacterWind;				// 新キャラのL属性
int newCharacterHomeTown;			// 新キャラの出身地




void swapInteger( int *, int * );
void swapShort( short *, short * );
void swapString( char *, char * );





// PCの情箢I域のクリア
void initPcAll( void )
{
	int i;

	memset( &pc, 0, sizeof( pc ) );
	memset( &pet, 0, sizeof( pet ) );
	memset( &magic, 0, sizeof( magic ) );
	memset( &party, 0, sizeof( party ) );
	memset( &petSkill, 0, sizeof( petSkill ) );

	pc.mailPetNo = -1;

	getUserSetting( selectPcNo );
	
	partyModeFlag = 0;

	for( i = 0; i < MAX_PARTY; i++ )
	{
		party[i].useFlag = 0;
		party[i].id = 0;
		party[i].ptAct = NULL;
	}


#if 1
	pc.faceGraNo = newCharacterFaceGraNo;
	pc.dp = loginDp;
#endif
}


// PC情蟪跗诨
void initPc( void )
{
	int walk = 0, height = 0;
	int i;

#ifndef _DEBUG
	createPc( pc.graNo, nowGx, nowGy, pc.dir );
#else
	if( offlineFlag )
	{
		createPc( SPR_pet004, nowGx, nowGy, pc.dir );
	}
	else
	{
		createPc( pc.graNo, nowGx, nowGy, pc.dir );
	}
#endif

	if( (pc.status & CHR_STATUS_W) )
	{
		walk = 1;
	}
	if( (pc.status & CHR_STATUS_H) )
	{
		height = 1;
	}
	setPcParam( pc.name, pc.freeName, pc.level, pc.nameColor, walk, height );

	if( pc.ptAct != NULL )
	{
		pc.ptAct->hp = pc.hp;
		pc.ptAct->maxHp = pc.maxHp;

		// 仲g情螭盲郡楗伐绁螗荪ぅ螗郡蚋
		for( i = 0; i < MAX_PARTY; i++ )
		{
			if( party[i].useFlag != 0 && party[i].id == pc.id )
			{
				party[i].ptAct = pc.ptAct;
				break;
			}
		}
	}


	prSendMode = 0;
	prSendFlag = 0;
	jbSendFlag = 0;
	duelSendFlag = 0;
	helpFlag = 0;
}


// PCキャラ作成
void createPc( int graNo, int gx, int gy, int dir )
{
	pc.graNo = graNo;
	pc.dir = dir;
	if( pc.ptAct == NULL )
	{
		pc.ptAct = createCharAction( graNo, gx, gy, dir );
		if( pc.ptAct != NULL )
		{
			pc.ptAct->atr |= ACT_ATR_TYPE_PC;
		}
	}
	else
	{
		setPcGraNo( graNo, dir );
		setPcWarpPoint( gx, gy );
		setPcPoint();
	}
}


// PCキャラリセット
void resetPc( void )
{
	if( pc.ptAct != NULL )
	{
		DeathAction( pc.ptAct );
		pc.ptAct = NULL;
	}

	// リ`ダ`マ`クは消しておく
	delPcLeader();
}


// PCキャラグラフィック番号O定
void setPcGraNo( int graNo, int dir )
{
	pc.graNo = graNo;
	pc.dir = dir;

	if( pc.ptAct == NULL )
		return;

	pc.ptAct->anim_chr_no = graNo;
	pc.ptAct->anim_ang = dir;
}


// PCキャラIDをO定
void setPcId( int id )
{
	pc.id = id;
}


// PCキャラ移酉仍O定
void setPcMovePoint( int nextGx, int nextGy )
{
//	if( pc.ptAct == NULL )
//		return;

	setMoveMap( nextGx, nextGy );
}


// PCキャラワ`プ先O定
void setPcWarpPoint( int gx, int gy )
{
//	if( pc.ptAct == NULL )
//		return;

#if 0
	forceWarpMap( gx, gy );
#endif
	setWarpMap( gx, gy );
}


// PCキャラマップ位置に同期する
void setPcPoint( void )
{
	if( pc.ptAct == NULL )
		return;

	pc.ptAct->mx = nowX;
	pc.ptAct->my = nowY;

	pc.ptAct->gx = nowGx;
	pc.ptAct->gy = nowGy;

	pc.ptAct->vx = nowVx;
	pc.ptAct->vy = nowVy;

	pc.ptAct->nextGx = nextGx;
	pc.ptAct->nextGy = nextGy;
}


// PCキャラの向きをO定
void setPcDir( int dir )
{
	pc.dir = dir;

	if( pc.ptAct == NULL )
		return;

	pc.ptAct->anim_ang = dir;
}


// アニメ`ションO定
void setPcAction( int act )
{
	if( pc.ptAct == NULL )
		return;

	pc.ptAct->anim_no = act;
	if( pc.ptAct->anim_no != ANIM_WALK )
	{
		pc.ptAct->anim_no_bak = -1;
	}
}


// アニメ`ションの状B取得
int getPcAction( void )
{
	if( pc.ptAct == NULL )
		return -1;

	return pc.ptAct->anim_no;
}


// PCキャラのパラメ`タO定
void setPcParam( char *name, char *freeName, int level, int nameColor,
	int walk, int height )
{
	int nameLen;
	int freeNameLen;

	nameLen = strlen( name );
	if( nameLen <= CHAR_NAME_LEN )
	{
		strcpy( pc.name, name );
	}

	freeNameLen = strlen( freeName );
	if( freeNameLen <= CHAR_FREENAME_LEN )
	{
		strcpy( pc.freeName, freeName );
	}

	pc.level = level;
	pc.nameColor = nameColor;
	if( walk != 0 )		// この上をiける
	{
		pc.status |= CHR_STATUS_W;
	}
	if( height != 0 )	// 高さがある
	{
		pc.status |= CHR_STATUS_H;
	}

	if( pc.ptAct == NULL )
		return;

	if( nameLen <= CHAR_NAME_LEN )
	{
		strcpy( pc.ptAct->name, name );
	}
	if( freeNameLen <= CHAR_FREENAME_LEN )
	{
		strcpy( pc.ptAct->freeName, freeName );
	}
	pc.ptAct->level = level;
	
}


// PCキャラの情螭颔伐绁螭嗽O定しなおす
void updataPcAct( void )
{
	if( pc.ptAct == NULL )
		return;

	if( strlen( pc.name ) <= CHAR_NAME_LEN )
	{
		strcpy( pc.ptAct->name, pc.name );
	}
	if( strlen( pc.freeName ) <= CHAR_FREENAME_LEN )
	{
		strcpy( pc.ptAct->freeName, pc.freeName );
	}
	pc.ptAct->level = pc.level;
	pc.ptAct->hp    = pc.hp;
	pc.ptAct->maxHp = pc.maxHp;
}


// PCキャラをリ`ダ`にする
void setPcLeader( void )
{
	pc.status |= CHR_STATUS_LEADER;
}


// PCキャラをリ`ダ`からやめさせる。
void delPcLeader( void )
{
	pc.status &= (~CHR_STATUS_LEADER);
}


// PCキャラをパ`ティ`の一Tにする
void setPcParty( void )
{
	pc.status |= CHR_STATUS_PARTY;
}


// PCキャラをパ`ティ`からやめさせる。
void delPcParty( void )
{
	pc.status &= (~CHR_STATUS_PARTY);
}


// PCキャラをQ楗猢`ドにする
void setPcWatch( void )
{
	pc.status |= CHR_STATUS_WATCH;
}


// PCキャラをQ楗猢`ドから元にもどす
void delPcWatch( void )
{
	pc.status &= (~CHR_STATUS_WATCH);
}


// F在iいてるフラグを立てる
//（iきと立ちのアニメ`ションの切り替えrに参照）
void setPcWalkFlag( void )
{
	if( pc.ptAct == NULL )
		return;

	pc.ptAct->walkFlag = 1;
}

// 立ちアニメに浃à郡楗榨楗挨颏恧
//（iきと立ちのアニメ`ションの切り替えrに参照）
void delPcWalkFlag( void )
{
	if( pc.ptAct == NULL )
		return;

	pc.ptAct->walkFlag = 0;
}


// iきフラグの参照
int checkPcWalkFlag( void )
{
	if( pc.ptAct == NULL )
		return 0;

	return (int)pc.ptAct->walkFlag;
}


// 庑g使用フラグを立てる
void setPcUseMagic( void )
{
	pc.status |= CHR_STATUS_USE_MAGIC;
}


// 庑g使用フラグをおろす
void delPcUseMagic( void )
{
	pc.status &= (~CHR_STATUS_USE_MAGIC);
}


// PCに吹き出しを出す
void setPcFukidashi( unsigned int offTime )
{
	CHAREXTRA *ext;

	if( pc.ptAct == NULL )
		return;

	pc.status |= CHR_STATUS_FUKIDASHI;
	ext = (CHAREXTRA *)pc.ptAct->pYobi;
	ext->drawFukidashiTime = offTime + GetTickCount();
}


// PCキャラ幼涓 /////////////////////////////////////////////////////////
void changePcAct( int x, int y, int dir, int action,
					int effectno, int effectparam1, int effectparam2 )
{
	switch( action )
	{
		// 立ち状B
		case 0:
#if 0
#if 1
			setPcWarpPoint( x, y );
			setPcDir( dir );
#else
			// パ`ティ以外か、リ`ダのrはI理する
			if( partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0 )
			{
				setPcWarpPoint( x, y );
				setPcDir( dir );
			}
#endif
#endif
			setPcAction( ANIM_STAND );
			break;

		// ワ`プ
		case 31:
#if 1
			setPcWarpPoint( x, y );
			setPcDir( dir );
#else
			// パ`ティ以外か、リ`ダのrはI理する
			if( partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0 )
			{
				setPcWarpPoint( x, y );
				setPcDir( dir );
			}
#endif
			setPcAction( ANIM_STAND );
			break;

		// 方向Q
		case 30:
			setPcDir( dir );
			break;

		// iく
		case 1:
#if 0
			// パ`ティ以外か、リ`ダのrはI理する
			if( partyModeFlag == 0 || (pc.status & CHR_STATUS_LEADER) != 0 )
			{
				setPcMovePoint( x, y );
				setPcDir( dir );
			}
#endif
			setPcAction( ANIM_WALK );
			break;

		// 攻
		case 2:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_ATTACK );
			break;

		// 投げる（ないかも）
		case 3:
			break;

		// ダメ`ジ
		case 4:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_DAMAGE );
			break;

		// 死亡
		case 5:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_DEAD );
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
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_DEAD );
			break;

		// 座る
		case 11:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_SIT );
			break;

		// 手を振る
		case 12:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_HAND );
			break;

		// 喜ぶ
		case 13:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_HAPPY );
			break;

		// 怒る
		case 14:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_ANGRY );
			break;

		// 悲しむ
		case 15:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_SAD );
			break;

		// ガ`ド
		case 16:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_GUARD );
			break;

		// iく（幼鳏坤保
		case 17:
			setPcDir( dir );
			setPcAction( ANIM_WALK );
			break;

		// うなずく
		case 18:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			setPcAction( ANIM_NOD );
			break;


		// 殛L情
		case 20:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			break;

		// リ`ダ`情
		case 21:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			if( effectno == 1 )
			{
				// リ`ダ`マ`ク表示
				setPcLeader();
#if 0
				if( pc.ptAct != NULL )
				{
					pc.ptAct->partyGx = -1;
					pc.ptAct->partyGy = -1;
				}
#endif
			}
			else
			{
				// リ`ダ`マ`ク消去
				delPcLeader();
			}
			break;

		// Q榍
		case 22:
			setPcWarpPoint( x, y );
			setPcDir( dir );
			if( effectno == 1 )
			{
				setPcWatch();
			}
			else
			{
				delPcWatch();
			}
			break;
	}
}


// 仲g情螭违辚 /////////////////////////////////////////////////////////
void clearPartyParam( void )
{
	int i;

	for( i = 0; i < MAX_PARTY; i++ )
	{
		// さっきまでパ`ティだったキャラのフラグをおろす
		if( party[i].useFlag != 0 )
		{
			if(  party[i].id == pc.id )
			{
				delPcParty();
				delPcLeader();
			}
			else
			{
				delCharParty( party[i].ptAct );
			}
		}
		party[i].useFlag = 0;
		party[i].id = 0;
		party[i].ptAct = NULL;
	}
}


// 仲g情螭ptActにNULLを入れてやる
void clearPtActPartyParam( void )
{
	int i;

	for( i = 0; i < MAX_PARTY; i++ )
	{
		party[i].ptAct = NULL;
	}
}





// キャラリストにデ`タがO定されているか /////////////////////////////
int existCharacterListEntry( int index )
{
	if( index < 0 || index >= MAXCHARACTER )
		return -1;

	if( chartable[index].name[0] != '\0' )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


// キャラリストに同一名があるかチェック
int cmpNameCharacterList( char *name )
{
	int i;

	for( i = 0; i < MAXCHARACTER; i++ )
	{
		if( strcmp( name, chartable[i].name ) == 0 )
			return 1;
	}

	return 0;
}


// キャラリストへデ`タO定（zむバッファは位置はデ`タの中にい皮耄
int setCharacterList( char *name, char *opt )
{

	int index;

	makeRecvString( name );
	makeRecvString( opt );

	index = getIntegerToken( opt, '|', 1 );

	if( index < 0 || index >= MAXCHARACTER )
		return -1;

	memset( &chartable[index], 0, sizeof( CHARLISTTABLE ) );

	if( strlen( name ) <= CHAR_NAME_LEN )
	{

		strcpy( chartable[index].name, name );
	}
	else
	{
		strcpy( chartable[index].name, "???" );
	}
	chartable[index].faceGraNo	= getIntegerToken( opt, '|',  2 );
	chartable[index].level		= getIntegerToken( opt, '|',  3 );
	chartable[index].hp			= getIntegerToken( opt, '|',  4 );
	chartable[index].str		= getIntegerToken( opt, '|',  5 );
	chartable[index].def		= getIntegerToken( opt, '|',  6 );
	chartable[index].agi		= getIntegerToken( opt, '|',  7 );
	chartable[index].app		= getIntegerToken( opt, '|',  8 );
	chartable[index].dp			= getIntegerToken( opt, '|',  9 );
	chartable[index].attr[0]	= getIntegerToken( opt, '|', 10 )/10;
	chartable[index].attr[1]	= getIntegerToken( opt, '|', 11 )/10;
	chartable[index].attr[2]	= getIntegerToken( opt, '|', 12 )/10;
	chartable[index].attr[3]	= getIntegerToken( opt, '|', 13 )/10;
	chartable[index].login		= getIntegerToken( opt, '|', 14 );

	return 0;
}


// キャラリストの内容をけす。
//   int index : どのキャラか。0  ~ 7
int resetCharacterList( int index )
{
	if( index < 0 || index >= MAXCHARACTER )
		return -1;

	memset( &chartable[index], 0, sizeof( CHARLISTTABLE ) );

	return 0;
}


// アイテムを拾う /////////////////////////////////////////////////////////
void getItem( void )
{
	float tmpDir;
	float tmpX, tmpY;
	int dir;
	static unsigned int piSendTime = 0;

	// お店vSのウィンドウが_いてるrは拾えないようにする
	if( windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMENU
	 || windowTypeWN == WINDOW_MWSSAGETYPE_ITEMSHOPMAIN
	 || windowTypeWN == WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN )
		return;

	// xk先が足元及びO接するグリッド以外はI理しない
	if( ABS( nowGx - mouseMapGx ) > 1
	 || ABS( nowGy - mouseMapGy ) > 1 )
		return;

	// xk先に何かあるか{べてなければI理しない
	if( !checkCharObjPoint( mouseMapGx, mouseMapGy,
		CHAROBJ_TYPE_NPC|CHAROBJ_TYPE_ITEM|CHAROBJ_TYPE_MONEY ) )
		return;

	// F在のマウスの方向を{べる
	tmpX = (float)(mouseMapGx - nowGx);
	tmpY = (float)(mouseMapGy - nowGy);
	tmpDir = Atan( tmpX, tmpY ) + 22.5F - 45.0F*3;
	AdjustDir( &tmpDir );
	dir = (int)(tmpDir/45);

	// B射抑制
	if( piSendTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )
	{
		// サ`バに送る
		lssproto_PI_send( sockfd, nowGx, nowGy, dir );
		piSendTime = GetTickCount();
	}
}


// アイテムの入れ替え /////////////////////////////////////////////////////
void swapItem( int from, int to )
{
	if( from < 0 || to < 0 )
		return;

	swapShort( &pc.item[from].useFlag, &pc.item[to].useFlag );
	swapString( pc.item[from].name, pc.item[to].name );
	swapString( pc.item[from].name2, pc.item[to].name2 );
	swapString( pc.item[from].memo, pc.item[to].memo );
	swapInteger( &pc.item[from].color, &pc.item[to].color );
	swapInteger( &pc.item[from].graNo, &pc.item[to].graNo );
	swapInteger( &pc.item[from].level, &pc.item[to].level );
	swapShort( &pc.item[from].field, &pc.item[to].field );
	swapShort( &pc.item[from].target, &pc.item[to].target );
	swapShort( &pc.item[from].deadTargetFlag, &pc.item[to].deadTargetFlag );
	swapShort( &pc.item[from].sendFlag, &pc.item[to].sendFlag );
}


// 整数稳毪欷
void swapInteger( int *a, int *b )
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}


void swapShort( short *a, short *b )
{
	short tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}


// 文字列の入れ替え（ただし、255文字までの文字列）
void swapString( char *a, char *b )
{
	char tmp[256];

	if( strlen( a ) > 255 || strlen( b ) > 255 )
		return;

	strcpy( tmp, a );
	strcpy( a, b );
	strcpy( b, tmp );
}


// O接するグリッドをる /////////////////////////////////////////////////
BOOL lookAtAround( void )
{
	float tmpDir;
	float tmpX, tmpY;
	int dir;
	static unsigned int lSendTime = 0;

	// お店vSのウィンドウが_いてるrはないようにする
	if( windowTypeWN == WINDOW_MESSAGETYPE_ITEMSHOPMENU
	 || windowTypeWN == WINDOW_MWSSAGETYPE_ITEMSHOPMAIN
	 || windowTypeWN == WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN )
		return FALSE;

	// xk先が足元及びO接するグリッド以外はI理しない
	if( ABS( nowGx - mouseMapGx ) > 1
	 || ABS( nowGy - mouseMapGy ) > 1 )
		return FALSE;

	// xk先に何かあるか{べてなければI理しない
	if( !checkCharObjPoint( mouseMapGx, mouseMapGy, CHAROBJ_TYPE_LOOKAT ) )
		return FALSE;

	// F在のマウスの方向を{べる
	tmpX = (float)(mouseMapGx - nowGx);
	tmpY = (float)(mouseMapGy - nowGy);
	if( tmpX == 0 && tmpY == 0 )	// 魇景澶紊悉亲苑证蜃螗辚氓筏郡橹工幛
		return FALSE;

	tmpDir = Atan( tmpX, tmpY ) + 22.5F - 45.0F*3;
	AdjustDir( &tmpDir );
	dir = (int)(tmpDir/45);

	// B射抑制
	if( lSendTime+FIELD_BTN_PUSH_WAIT < GetTickCount() )
	{
		// サ`バに送る
		lssproto_L_send( sockfd, dir );
		lSendTime = GetTickCount();
	}

	return TRUE;
}





// アドレスブックI理 /////////////////////////////////////////////////////
BOOL addressBookFlag = FALSE;
ACTION *ptActAddressBookWin = NULL;
int addressBookX = 16;
int addressBookY = 16;
int addressBookPage = 0;
ACTION *ptActAddressBookChar[MAX_ADR_BOOK_COUNT];

// xkできる文字列のＩＤ
int addressBookSelectStrItem[1+3*MAX_ADR_BOOK_COUNT];
// 0 ... ]じる
// n*3+1 ... 送信
// n*3+2 ... 受信
// n*3+3 ... 削除


BOOL addressBookSendMsgFlag = FALSE;
ACTION *ptActAddressBookSendMsgWin = NULL;


void addressBookWindow1( void );


// アドレスブックの初期化
void initAddressBook( void )
{
	int i;

	addressBookFlag = FALSE;
	ptActAddressBookWin = NULL;
	memset( &ptActAddressBookChar, 0, sizeof( ptActAddressBookChar ) );
	addressBookPage = 0;
	for( i = 0; i < sizeof( addressBookSelectStrItem )/sizeof( int ); i++ )
	{
		addressBookSelectStrItem[i] = -2;
	}

	addressBookSendMsgFlag = FALSE;
	ptActAddressBookSendMsgWin = NULL;
}


// アドレスブックI理
void addressBookProc( void )
{
	int i;
	int no;

	// アドレスブックを_く
	if( addressBookFlag
	 && ptActAddressBookWin == NULL )
	{
		// ベ`スウィンドウのアクション
		ptActAddressBookWin = MakeWindowDisp( addressBookX, addressBookY, 4, 8, NULL, 1 );
		// 相手キャラのアクション
		for( i = 0; i < MAX_ADR_BOOK_COUNT; i++ )
		{
			no = addressBookPage*MAX_ADR_BOOK_COUNT+i;

			ptActAddressBookChar[i] = GetAction( PRIO_CHR, 0 );
			if( ptActAddressBookChar[i] != NULL )
			{
				ptActAddressBookChar[i]->anim_chr_no = addressBook[no].graNo;
				ptActAddressBookChar[i]->atr = ACT_ATR_HIDE;
				ptActAddressBookChar[i]->anim_no = ANIM_STAND;
				ptActAddressBookChar[i]->dispPrio = DISP_PRIO_ITEM;
				ptActAddressBookChar[i]->x = addressBookX + 40;
				ptActAddressBookChar[i]->y = addressBookY + i * 110 + 80;
				pattern( ptActAddressBookChar[i], ANM_NOMAL_SPD, ANM_LOOP );
			}
		}
	}
	// アドレスブックを]じる
	else
	if( !addressBookFlag
	 && ptActAddressBookWin != NULL )
	{
		DeathAction( ptActAddressBookWin );
		ptActAddressBookWin = NULL;
		for( i = 0; i < MAX_ADR_BOOK_COUNT; i++ )
		{
			if( ptActAddressBookChar[i] != NULL )
			{
				DeathAction( ptActAddressBookChar[i] );
				ptActAddressBookChar[i] = NULL;
			}
		}
	}
	// アドレスブック表示
	else
	if( addressBookFlag
	 && ptActAddressBookWin != NULL )
	{
		addressBookWindow1();
	}
}


// アドレスブック表示
void addressBookWindow1( void )
{
	int mode = 0;
	int i;
	int no;
	char msg[256];

	// 文字列がxkされてないかチェック
	// まず左クリックされてなければI理しない
	if( mouse.onceState & MOUSE_LEFT_CRICK )
	{
		for( i = 0; i < sizeof( addressBookSelectStrItem )/sizeof( int ); i++ )
		{
			if( addressBookSelectStrItem[i] == HitFontNo )
			{
				mode = i + 1;
				break;
			}
		}
	}

	// ]じるボタンを押されたのでKわる
	if( mode == 1 )
	{
		addressBookFlag = FALSE;
		return;
	}
	// 削除ボタンを押されたのでその目を削除する
	else
	if( (mode-2) < MAX_ADR_BOOK_COUNT*3
	 && ((mode-2) % 3) == 2 )
	{
		lssproto_DAB_send( sockfd, addressBookPage*MAX_ADR_BOOK_COUNT+((mode-2)/3) );
	}

	// ウィンドウ_きアニメがKわったら中身を表示
	if( ptActAddressBookWin->hp >= 1 )
	{
		for( i = 0; i < MAX_ADR_BOOK_COUNT; i++ )
		{
			no = addressBookPage*MAX_ADR_BOOK_COUNT+i;

			if( ptActAddressBookChar[i] != NULL )
			{
				// 名前がある所にデ`タが入っているとする
				if( strlen( addressBook[no].name ) > 0 )
				{
					// グラフィック番号あるrは情蟊硎
					// キャラ表示
					ptActAddressBookChar[i]->atr &= (~ACT_ATR_HIDE);
					ptActAddressBookChar[i]->anim_chr_no = addressBook[no].graNo;
					// 名前表示
					sprintf( msg, "NAME. %s", addressBook[no].name );
					StockFontBuffer( addressBookX + 68, addressBookY + i * 110 + 28,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
				
					// レベル表示
					sprintf( msg, "LV. %d", addressBook[no].level );
					StockFontBuffer( addressBookX + 68, addressBookY + i * 110 + 46,
						FONT_PRIO_FRONT, FONT_PAL_WHITE, msg, 0 );
					// オンライン表示
					if( addressBook[i].onlineFlag != 0 )
					{
						StockFontBuffer( addressBookX + 130, addressBookY + i * 110 + 46,
							FONT_PRIO_FRONT, FONT_PAL_WHITE, "オンライン", 0 );
					}
					else
					{
						StockFontBuffer( addressBookX + 130, addressBookY + i * 110 + 46,
							FONT_PRIO_FRONT, FONT_PAL_GRAY, "オフライン", 0 );
					}
					// 送信ボタン表示
					addressBookSelectStrItem[i*3+1] =
						StockFontBuffer( addressBookX + 68, addressBookY + i * 110 + 64,
							FONT_PRIO_FRONT, FONT_PAL_YELLOW, "送信", 2 );
					// 受信ボタン表示
					addressBookSelectStrItem[i*3+2] =
						StockFontBuffer( addressBookX + 112, addressBookY + i * 110 + 64,
							FONT_PRIO_FRONT, FONT_PAL_YELLOW, "受信", 2 );
					// 削除ボタン表示
					addressBookSelectStrItem[i*3+3] =
						StockFontBuffer( addressBookX + 156, addressBookY + i * 110 + 64,
							FONT_PRIO_FRONT, FONT_PAL_YELLOW, "削除", 2 );
				}
				else
				{
					ptActAddressBookChar[i]->atr |= ACT_ATR_HIDE;
					ptActAddressBookChar[i]->anim_chr_no = 0;
				}
			}
		}

		addressBookSelectStrItem[0] =
			StockFontBuffer( addressBookX + 56, addressBookY + 352,
				FONT_PRIO_FRONT, FONT_PAL_YELLOW, "]じる", 2 );
	}
}

