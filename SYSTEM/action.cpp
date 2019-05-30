/************************/
/*	action.c			*/
/************************/
#include "system.h"



/* はじめと後のリストポインタ */
ACTION *pActTop;
ACTION *pActBtm;

#ifdef _DEBUG		
/* アクション数カウント */
int ActCnt;
#endif

/* アクションリスト初期化 *****动作名单初期化************************************************/
void InitAction( void )
{
	/* 最初と最後のポインタ取得 */
	pActTop = ( ACTION * )calloc( 1, sizeof( ACTION ) );
	pActBtm = ( ACTION * )calloc( 1, sizeof( ACTION ) );

	pActTop->pPrev = NULL;	 	/* 前のリストはoし */
	pActTop->pNext = pActBtm;	/* 次のリストは最後尾 */
	pActTop->func  = NULL;		/* g行v数はoし */
	pActTop->prio  = PRIO_TOP;	/* 先位最大 */
	pActTop->bmpNo = -1;		/* ＢＭＰ番号初期化 */
	
	pActBtm->pPrev = pActTop;	/* 前のリストは先^ */
	pActBtm->pNext = NULL;		/* 次のリストはoし */
	pActBtm->func  = NULL;		/* g行v数はoし */
	pActBtm->prio  = PRIO_BTM;	/* 先位最低 */
	pActBtm->bmpNo = -1;		/* ＢＭＰ番号初期化 */
	
	//MessageBox( hWnd, "アクション初期化完了。", "_J", MB_OK );
}

/* アクションリストに登h *****************************************************/
ACTION *GetAction( UCHAR prio, UINT yobiSize )
{
	ACTION *pAct;
	ACTION *pActLoop;
	
	/* アクション造体メモリ_保 */
	pAct = ( ACTION * )calloc( 1, sizeof( ACTION ) );
	if( pAct == NULL ){
//#ifdef _DEBUG		
		//MessageBox( hWnd, "GetAction Erorr!!", "_J", MB_OK );
		MessageBox( hWnd, "ハ`ドディスクの空き容量が足りません！", "GetAction Erorr", MB_OK );
//#endif
		return NULL;
	}
	/* 予造体使うr */
	if( yobiSize > 0 ){
		/* 予造体メモリ_保 */
		pAct->pYobi = calloc( 1, yobiSize );
		// 失・筏郡
		if( pAct->pYobi == NULL ){
			/* アクション造体のメモリも解放 */
			free( pAct );
//#ifdef _DEBUG		
			//MessageBox( hWnd, "GetYobi Erorr!!", "_J", MB_OK );
			MessageBox( hWnd, "ハ`ドディスクの空き容量が足りません！", "GetYobi Erorr", MB_OK );
//#endif
			return NULL;
		}
	}
	
	/* 初期化 */
	pAct->func  = NULL;			/* g行v数はoし */
	pAct->prio  = prio;			/* 先位 */
	pAct->bmpNo = -1;			/* ＢＭＰ番号初期化 */
	pAct->hitDispNo = -2;		/* マウスカ`ソル当たり判定番号初期化 */
	
	/* 先にKべる */
	for( pActLoop = pActTop->pNext ; pActLoop != pActBtm->pNext ; 
		pActLoop = pActLoop->pNext ){
		
		/* 先度が低いr */
		if( pActLoop->prio > prio ){
			/* この上に追加 */
			/* 前後から教えてもらう */
			pAct->pPrev = pActLoop->pPrev;	/* 前のリスト */
			pAct->pNext = pActLoop;			/* 次のリスト */
			/* 前後に教える */
			pActLoop->pPrev->pNext = pAct;	/* 前のリスト */
			pActLoop->pPrev = pAct;			/* 次のリスト */
#ifdef _DEBUG		
			/* アクション数カウント */
   			ActCnt++;
#endif			
			break;
		}
	}
	
	return pAct;
}

/* アクション走らせる ******动作运行***************************************************/
void RunAction( void )
{
	ACTION *pActLoop = pActTop->pNext; 	/* 先^のリストポインタ取得  最前头的名单指针取得*/
	ACTION *pActLoopBak;  /* 死亡rバックアップ用 */
	
	// I理ル`プ	
	while(1){
	
		/* 最後尾が来るまでル`プ  最后来之前循环*/	
		if( pActLoop == pActBtm ) break;

		/* 生きていたら 如果活着的话*/
		if( pActLoop->deathFlag == FALSE )
		{
		
			/* アドレスがあったらg行   地址的话，执行*/
			if( pActLoop->func != NULL )
			{
				pActLoop->func( pActLoop );
			}
			
			/* 次のポインタをセット */
			pActLoop = pActLoop->pNext;
			
		}
		else
		{	/* 死んでいたら */
		
			/* 前後に教える */
			pActLoop->pPrev->pNext = pActLoop->pNext;
			pActLoop->pNext->pPrev = pActLoop->pPrev;
			/* バックアップ */
			pActLoopBak = pActLoop->pNext;
			/* リストから抹 */
			ClearAction( pActLoop );
			/* バックアップ */
			pActLoop = pActLoopBak;
		}
	}
}

/* アクションリストから抹 ***********************************************/
void DeathAction( ACTION *pAct )
{
	if( pAct == NULL ) return;
	pAct->deathFlag = TRUE;
}

/* アクションリストから完全抹 ***********************************************/
void ClearAction( ACTION *pAct )
{
	// 予造体があるとき
	if( pAct->pYobi != NULL ){
		// 予造体を_放
		free( pAct->pYobi );
	}
	// アクション造体を_放
	free( pAct );
	
#ifdef _DEBUG		
	/* アクション数マイナス */
	ActCnt--;
#endif		

}

/* 全アクション抹 ***********************************************************/
void DeathAllAction( void )
{
	ACTION *pActLoop = pActTop->pNext;
	
	/* 最後まできたらK了 */
	while( pActLoop != pActBtm ){
		/* 死亡フラグ立てる */
		pActLoop->deathFlag = TRUE;
		/* 次のポインタへ */
		pActLoop = pActLoop->pNext;
	}
	//MessageBox( hWnd, "DeathAllAction OK!!", "_J", MB_OK );
}

/* アクションK了I理 *********************************************************/
void EndAction( void )
{
	// 全アクション抹
	DeathAllAction();
	//アクション走らせる（完全抹
	RunAction();
	
	/* 最初と最後のポインタ_放 */
	free( pActTop );
	free( pActBtm );
	
	//MessageBox( NULL, "アクション_放完了。", "_J", MB_OK );
}
