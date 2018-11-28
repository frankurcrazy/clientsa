/************************/
/*	produce.c			*/
/************************/
#include "system.h"
#include "battleProc.h"
#include "produce.h"
#include "anim_tbl.h"

// �ݳ�����
int ProduceNo = 0;
// �ݳ����ڻ��ե饰
int ProduceInitFlag = TRUE;

// �Хȥ륵�`�ե����λ������� **************************************************/
void CopyBackBuffer( void )
{
	// �Хå����`�ե�������Хȥ륵�`�ե����إ��ԩ`
	lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
	// �ݳ����ť����ࣨż���Τߣ�
	ProduceNo = Rnd( 0, PRODUCE_END - 1 ) & 0xfffffffe;
	//ProduceNo += 2;
	//if( ProduceNo >= PRODUCE_END ) ProduceNo = 0;
	//ProduceNo = PRODUCE_BRAN_SMALL;
}	


// �����Ƅ� *******************************************************************/
BOOL ProduceAccele( int mode )
{
	RECT rect = { 0, 0, DEF_APPSIZEX, DEF_APPSIZEY };
	int endFlag = FALSE;
	static float x = 0, y = 0; 
	static float a = 0;
	
	// ���ڻ�����r
	if( ProduceInitFlag == TRUE ){
		ProduceInitFlag = FALSE;
		x = 0; y = 0;a = 0;
	}
	
	// �Ƅӷ���Ƿ��
	switch( mode ){
	
	case 0:	// �ϼ����Ƅ�
		y -= a;	// �Ƅ�
		// ���ٶȥץ饹
		a += 0.3F; 
		// ��ߥåȥ����å�
		if( y <= -480 ) endFlag = TRUE;
		break;
		
	case 1:	// �¼����Ƅ�
		y += a;	// �Ƅ�
		// ���ٶȥץ饹
		a += 0.3F; 
		// ��ߥåȥ����å�
		if( y >= 480 ) endFlag = TRUE;
		break;
		
	case 2:	// ������Ƅ�
		x -= a;	// �Ƅ�
		// ���ٶȥץ饹
		a += 0.4F; 
		// ��ߥåȥ����å�
		if( x <= -640 ) endFlag = TRUE;
		break;
		
	case 3:	// �Ҽ����Ƅ�
		x += a;	// �Ƅ�
		// ���ٶȥץ饹
		a += 0.4F; 
		// ��ߥåȥ����å�
		if( x >= 640 ) endFlag = TRUE;
		break;
	}
	// �Хå����`�ե������\�ǥ��ꥢ�`
	ClearBackSurface();	
	// �Хå��Хåե��`�˸���ܞ�ͣ�����ָ����
	DrawSurfaceFast2( (int)x, (int)y, &rect, lpBattleSurface );
	
	
	// �Хȥ륵�`�ե����λ�������
	//CopyBackBuffer();
	if( endFlag == TRUE ){
		// ���ڻ�
		x = 0;
		y = 0; 
		a = 0;
		
		// ���`���״�B���
		if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
		else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 

		// �Хȥ륵�`�ե������\�ǥ��ꥢ
		//ClearSurface( lpBattleSurface );
		// �Хå����`�ե������\�ǥ��ꥢ�`
		//ClearBackSurface();	
		return TRUE;
	}
	return FALSE;
}	
	
// �p���Ƅ� *******************************************************************/
BOOL ProduceBrake( int mode )
{
	RECT rect = { 0, 0, DEF_APPSIZEX, DEF_APPSIZEY };
	static int endFlag = -1;
	static float x = 0, y = 0; 
	static float a;
	
	// ���ڻ�����r
	if( ProduceInitFlag == TRUE ){
		ProduceInitFlag = FALSE;
		endFlag = -1;
	}
	
	// �Ƅӷ���Ƿ��
	switch( mode ){
	
	case 0:	// �Ϝp���Ƅ�
		if( endFlag == -1 ){
			x = 0;
			//y = 480;
			y = 513.2F;
			a = 17.4F;
			endFlag = FALSE;
		}
		y -= a;	// �Ƅ�
		// �p�ٶȥץ饹
		a -= 0.3F; 
		// ��ߥåȥ����å�
		if( a <= 0 ) endFlag = TRUE;
		break;
		
	case 1:	// �p���Ƅ�
		if( endFlag == -1 ){
			x = 0;
			//y = -480;
			y = -513.2F;
			a = 17.4F;
			endFlag = FALSE;
		}
		y += a;	// �Ƅ�
		// �p�ٶȥץ饹
		a -= 0.3F; 
		// ��ߥåȥ����å�
		if( a <= 0 ) endFlag = TRUE;
		break;
		
	case 2:	// ��p���Ƅ�
		if( endFlag == -1 ){
			//x = 640;
			x = 661;
			y = 0;
			a = 22.8F;
			endFlag = FALSE;
		}
		x -= a;	// �Ƅ�
		// �p�ٶȥץ饹
		a -= 0.4F; 
		// ��ߥåȥ����å�
		if( a <= 0 ) endFlag = TRUE;
		break;
		
	case 3:	// �Ҝp���Ƅ�
		if( endFlag == -1 ){
			//x = -640;
			x = -661;
			y = 0;
			a = 22.8F;
			endFlag = FALSE;
		}
		x += a;	// �Ƅ�
		// �p�ٶȥץ饹
		a -= 0.4F; 
		// ��ߥåȥ����å�
		if( a <= 0 ) endFlag = TRUE;
		break;
	}
	// �Хå����`�ե������\�ǥ��ꥢ�`
	ClearBackSurface();	
	// �Хå��Хåե��`�˸���ܞ�ͣ�����ָ����
	DrawSurfaceFast2( (int)x, (int)y, &rect, lpBattleSurface );
	
	// �Хȥ륵�`�ե����λ�������
	//CopyBackBuffer();
	if( endFlag == TRUE ){
		// ���ڻ�
		
		// ���`���״�B���
		if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
		else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 
		// �ե饰���ڻ�
		endFlag = -1;
		// �Хȥ륵�`�ե������\�ǥ��ꥢ
		//ClearSurface( lpBattleSurface );
		// �Хå����`�ե������\�ǥ��ꥢ�`
		//ClearBackSurface();	
		return TRUE;
	}
	return FALSE;
}
	
// �������Ƅ� ******************************************************************/
BOOL Produce4Way( int mode )
{
	RECT rect[ 4 ] = {	{ 0, 0, 320, 240 }, { 320, 0, 640, 240 },
						{ 0, 240, 320, 480 }, { 320, 240, 640, 480 }};
	static int endFlag = -1;
	static int x[ 4 ], y[ 4 ]; 
	int d = 4,i;
	
	// ���ڻ�����r
	if( ProduceInitFlag == TRUE ){
		ProduceInitFlag = FALSE;
		endFlag = -1;
	}
	
	// �Ƅӷ���Ƿ��
	switch( mode ){
	
	case 0:	// �������
		if( endFlag == -1 ){
			x[ 0 ] = 0;
			y[ 0 ] = 0;
			x[ 1 ] = 320;
			y[ 1 ] = 0;
			x[ 2 ] = 0;
			y[ 2 ] = 240;
			x[ 3 ] = 320;
			y[ 3 ] = 240;
			endFlag = FALSE;
		}
		// �Ƅ�
		x[ 0 ] -= d;
		y[ 0 ] -= d;
		x[ 1 ] += d;
		y[ 1 ] -= d;
		x[ 2 ] -= d;
		y[ 2 ] += d;
		x[ 3 ] += d;
		y[ 3 ] += d;
		// ��ߥåȥ����å�
		if( x[ 0 ] <= -320 ) endFlag = TRUE;
		break;
		
	case 1:	// �����ڤ�
		if( endFlag == -1 ){
			x[ 0 ] = -280;
			y[ 0 ] = -280;
			x[ 1 ] = 320 + 280;
			y[ 1 ] = -280;
			x[ 2 ] = -280;
			y[ 2 ] = 240 + 280;
			x[ 3 ] = 320 + 280;
			y[ 3 ] = 240 + 280;
			endFlag = FALSE;
		}
		// �Ƅ�
		x[ 0 ] += d;
		y[ 0 ] += d;
		x[ 1 ] -= d;
		y[ 1 ] += d;
		x[ 2 ] += d;
		y[ 2 ] -= d;
		x[ 3 ] -= d;
		y[ 3 ] -= d;
		// ��ߥåȥ����å�
		if( x[ 0 ] >= 0 ) endFlag = TRUE;
		break;
	}
	
	// �Хå����`�ե������\�ǥ��ꥢ�`
	ClearBackSurface();	
	for( i = 0 ; i < 4 ; i++ ){
		//���������������ɥåȥ�`�ɤΕr
		if(ResoMode == 1){
			rect[i].top >>= 1;
			rect[i].left >>= 1;
			rect[i].right >>= 1;
			rect[i].bottom >>= 1;
			// �Хå��Хåե��`�˸���ܞ�ͣ�����ָ����
			DrawSurfaceFast2( x[ i ] / 2, y[ i ] / 2, &rect[ i ], lpBattleSurface );
		} else {
			// �Хå��Хåե��`�˸���ܞ�ͣ�����ָ����
			DrawSurfaceFast2( x[ i ], y[ i ], &rect[ i ], lpBattleSurface );
		}
	}
	// �Хȥ륵�`�ե����λ�������
	//CopyBackBuffer();
	if( endFlag == TRUE ){
		// ���ڻ�
		
		// ���`���״�B���
		if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
		else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 
		// �ե饰���ڻ�
		endFlag = -1;
		// �Хȥ륵�`�ե������\�ǥ��ꥢ
		//ClearSurface( lpBattleSurface );
		// �Хå����`�ե������\�ǥ��ꥢ�`
		//ClearBackSurface();	
		return TRUE;
	}
	return FALSE;
}	

// �Ϥ���� ****************************************************************/
BOOL ProduceHagare( int mode )
{
	RECT rect;
	static int endFlag = -1;
	static int x[ 64 ], y[ 64 ], a[ 64 ], cnt,cnt2; 
	static int posX[ 64 ], posY[ 64 ]; 
	static BOOL flag[ 64 ]; 
	int i, j, nowX = 0, nowY = 0;
	int rnd = Rnd( 0, 63 );
	int no = 0;
	
	// ���ڻ�����r
	if( ProduceInitFlag == TRUE ){
		ProduceInitFlag = FALSE;
		endFlag = -1;
	}
	
	// �Ƅӷ���Ƿ��
	switch( mode ){
	
	case 0:	// ������
		// ���ڻ�
		if( endFlag == -1 ){
			for( i = 0 ; i < 8 ; i++ ){
				for( j = 0 ; j < 8 ; j++ ){
					x[ no ] = nowX;
					y[ no ] = nowY;
					posX[ no ] = nowX;
					posY[ no ] = nowY;
					a[ no ] = 0;
					flag[ no++ ] = TRUE;
					nowX += 80;
				}
				nowY += 60;
				nowX = 0;
			}
			endFlag = FALSE;
			cnt = 0;
		}
		while( flag[ rnd ] == FALSE ){
			rnd = Rnd( 0, 63 );
		}
		// ����������
		flag[ rnd ] = FALSE;
		cnt++;
		// ��ߥåȥ����å�
		if( cnt >= 64 ) endFlag = TRUE;
		break;
		
	case 1:	// ���F����
		// ���ڻ�
		if( endFlag == -1 ){
			for( i = 0 ; i < 8 ; i++ ){
				for( j = 0 ; j < 8 ; j++ ){
					x[ no ] = nowX;
					y[ no ] = nowY;
					posX[ no ] = nowX;
					posY[ no ] = nowY;
					a[ no ] = 0;
					flag[ no++ ] = FALSE;
					nowX += 80;
				}
				nowY += 60;
				nowX = 0;
			}
			endFlag = FALSE;
			cnt = 0;
		}
		while( flag[ rnd ] == TRUE ){
			rnd = Rnd( 0, 63 );
		}
		// ��������F
		flag[ rnd ] = TRUE;
		cnt++;
		// ��ߥåȥ����å�
		if( cnt >= 64 ) endFlag = TRUE;
		break;
		
	case 2:	// �����루����룩
		// ���ڻ�
		if( endFlag == -1 ){
			for( i = 0 ; i < 8 ; i++ ){
				for( j = 0 ; j < 8 ; j++ ){
					x[ no ] = nowX;
					y[ no ] = nowY;
					posX[ no ] = nowX;
					posY[ no ] = nowY;
					a[ no ] = 0;
					flag[ no++ ] = 2;
					nowX += 80;
				}
				nowY += 60;
				nowX = 0;
			}
			endFlag = FALSE;
			cnt = 0;
			cnt2 = 0;
		}
		// ��Ȥ���ĥ�����
		if( cnt2 < 64 ){
			while( flag[ rnd ] <= 1 ){
				rnd = Rnd( 0, 63 );
			}
			// ��������Ȥ�
			flag[ rnd ] = 1;
			cnt2++;
		}
		// �Ƅ�
		for( i = 0 ; i < 64 ; i++ ){
			if( flag[ i ] == 1 ){
				a[ i ]++;
				y[ i ] += a[ i ];
				if( y[ i ] >= 500 ){ 
					flag[ i ] = 0;
					cnt++;
				}
			}
		}
		// ��ߥåȥ����å�
		if( cnt >= 64 ) endFlag = TRUE;
		break;
		
	case 3:	// ���F������룩
		// ���ڻ�
		if( endFlag == -1 ){
			for( i = 0 ; i < 8 ; i++ ){
				for( j = 0 ; j < 8 ; j++ ){
					x[ no ] = nowX;
					y[ no ] = -60;
					posX[ no ] = nowX;
					posY[ no ] = nowY;
					a[ no ] = 0;
					flag[ no++ ] = FALSE;
					nowX += 80;
				}
				nowY += 60;
				nowX = 0;
			}
			endFlag = FALSE;
			cnt = 0;
			cnt2 = 0;
		}
		// ��Ȥ���ĥ�����
		if( cnt2 < 64 ){
			while( flag[ rnd ] >= 1 ){
				rnd = Rnd( 0, 63 );
			}
			// ��������Ȥ�
			flag[ rnd ] = 1;
			cnt2++;
		}
		// �Ƅ�
		for( i = 0 ; i < 64 ; i++ ){
			if( flag[ i ] == 1 ){
				a[ i ]++;
				y[ i ] += a[ i ];
				if( y[ i ] >= posY[ i ] ){ 
					y[ i ] = posY[ i ];
					flag[ i ] = 2;
					cnt++;
				}
			}
		}
		// ��ߥåȥ����å�
		if( cnt >= 64 ) endFlag = TRUE;
		break;
		
	}
	
	// �Хå����`�ե������\�ǥ��ꥢ�`
	ClearBackSurface();	
	for( i = 0 ; i < 64 ; i++ ){
		if( flag[ i ] == 2 ){
			rect.left = posX[ i ];
			rect.top = posY[ i ];
			rect.right = posX[ i ] + 80;
			rect.bottom = posY[ i ] + 60;
			//���������������ɥåȥ�`�ɤΕr
			if(ResoMode == 1){
				rect.top >>= 1;
				rect.left >>= 1;
				rect.right >>= 1;
				rect.bottom >>= 1;
				// �Хå��Хåե��`�˸���ܞ�ͣ�����ָ����
				DrawSurfaceFast2( x[ i ] / 2, y[ i ] / 2, &rect, lpBattleSurface );
			} else {
				// �Хå��Хåե��`�˸���ܞ�ͣ�����ָ����
				DrawSurfaceFast2( x[ i ], y[ i ], &rect, lpBattleSurface );
			}
		}
	}
	for( i = 0 ; i < 64 ; i++ ){
		if( flag[ i ] == 1 ){
			rect.left = posX[ i ];
			rect.top = posY[ i ];
			rect.right = posX[ i ] + 80;
			rect.bottom = posY[ i ] + 60;
			//���������������ɥåȥ�`�ɤΕr
			if(ResoMode == 1){
				rect.top >>= 1;
				rect.left >>= 1;
				rect.right >>= 1;
				rect.bottom >>= 1;
				// �Хå��Хåե��`�˸���ܞ�ͣ�����ָ����
				DrawSurfaceFast2( x[ i ] / 2, y[ i ] / 2, &rect, lpBattleSurface );
			} else {
				// �Хå��Хåե��`�˸���ܞ�ͣ�����ָ����
				DrawSurfaceFast2( x[ i ], y[ i ], &rect, lpBattleSurface );
			}
		}
	}
	// �Хȥ륵�`�ե����λ�������
	//CopyBackBuffer();
	if( endFlag == TRUE ){
		// ���ڻ�
		
		// ���`���״�B���
		if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
		else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 
		// �ե饰���ڻ�
		endFlag = -1;
		// �Хȥ륵�`�ե������\�ǥ��ꥢ
		//ClearSurface( lpBattleSurface );
		// �Хå����`�ե������\�ǥ��ꥢ�`
		//ClearBackSurface();	
		return TRUE;
	}
	return FALSE;
}	

#define BRAN_SIZE_X 20
#define BRAN_SIZE_Y 16
static int bran_cnt;
static int pos_tbl[(480/BRAN_SIZE_Y)*(640/BRAN_SIZE_X)*2];
static int bran_flg = 0;
//static int bran_flg = 0;
// �ե��� ******************************************************************/
BOOL Produce_bran_small(int ang)
{
	RECT rect;
	int d0,d6,d7;
	int *a0;
	
	// ���ڻ�����r
	if( ProduceInitFlag == TRUE ){
		ProduceInitFlag = FALSE;
		bran_flg = 0;
	}
	
	if(bran_flg == 0){		//����ʤ�
		bran_flg = 1;		//�Ʃ`�֥�����
		d0 = 0;
		for(d7=-240 + BRAN_SIZE_Y/2; d7<240 + BRAN_SIZE_Y/2; d7+=BRAN_SIZE_Y){
			for(d6=-320 + BRAN_SIZE_X/2; d6<320 + BRAN_SIZE_X/2; d6+=BRAN_SIZE_X){
				pos_tbl[d0++] = d6;
				pos_tbl[d0++] = d7;
			}
		}
		if(ang < 0){
			bran_cnt = 64;
		} else {
			bran_cnt = 0;
		}
	}
	// �Хå����`�ե������\�ǥ��ꥢ�`
	ClearBackSurface();	

	a0 = pos_tbl + (480/BRAN_SIZE_Y)*(640/BRAN_SIZE_X)*2 - 2;
	for(d7=480-BRAN_SIZE_Y; d7>=0; d7-=BRAN_SIZE_Y){
		for(d6=640-BRAN_SIZE_X; d6>=0; d6-=BRAN_SIZE_X){
			//���������������ɥåȥ�`�ɤΕr
			if(ResoMode == 1){
				rect.left = d6/2;
				rect.right = (d6+BRAN_SIZE_X)/2;
				rect.top = d7/2;
				rect.bottom = (d7+BRAN_SIZE_Y)/2;
			} else {
				rect.left = d6;
				rect.right = (d6+BRAN_SIZE_X);
				rect.top = d7;
				rect.bottom = (d7+BRAN_SIZE_Y);
			}
#if 0
			DrawSurfaceFast2( *a0*bran_cnt/64+320, *(a0+1)*bran_cnt/64+240, &rect, lpBattleSurface );
#else
			//���������������ɥåȥ�`�ɤΕr
			if(ResoMode == 1){
				lpDraw->lpBACKBUFFER->BltFast( (*a0*bran_cnt/64+320 - BRAN_SIZE_X/2)/2, (*(a0+1)*bran_cnt/64+240 - BRAN_SIZE_Y/2)/2, lpBattleSurface, &rect, DDBLTFAST_WAIT );
			} else {
				lpDraw->lpBACKBUFFER->BltFast( *a0*bran_cnt/64+320 - BRAN_SIZE_X/2, *(a0+1)*bran_cnt/64+240 - BRAN_SIZE_Y/2, lpBattleSurface, &rect, DDBLTFAST_WAIT );
			}
//			lpDraw->lpBACKBUFFER->BltFast( *(a0++)*bran_cnt/64+320, *(a0++)*bran_cnt/64+240, lpBattleSurface, &rect, DDBLTFAST_WAIT );
#endif
			a0 -= 2;
		}
	}

	bran_cnt += ang;
	if(ang < 0){
//		if(!bran_cnt){
		if(bran_cnt < 0){
			// ���`���״�B���
			if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
			else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 
			bran_flg = 0;
			return TRUE;
		}
	} else {
//		if(bran_cnt == 64){
		if(bran_cnt > 64){
			// ���`���״�B���
			if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
			else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 
			bran_flg = 0;
			return TRUE;
		}
	}
	return FALSE;
}

#define BREAK_UP 8
//static int break_up_cnt = 480;
// �ۤꤿ������ ******************************************************************/
BOOL Produce_break_up(int ang)
{
	RECT rect = { 0, 0, 640, 0 };
	int d7;

	// ���ڻ�����r
	if( ProduceInitFlag == TRUE ){
		ProduceInitFlag = FALSE;
		bran_flg = 0;
	}
	
	if(bran_flg == 0){		//����ʤ�
		bran_flg = 1;		//�������ڻ�
		if(ang < 0){
			bran_cnt = 480;
		} else {
			bran_cnt = 0;
		}
	}
	// �Хå����`�ե������\�ǥ��ꥢ�`
	ClearBackSurface();	

	for(d7=0; d7<bran_cnt; d7++){
		rect.top = d7*480/bran_cnt;
		rect.bottom = rect.top+1;
		lpDraw->lpBACKBUFFER->BltFast( 0, d7, lpBattleSurface, &rect, DDBLTFAST_WAIT );
	}

	if(ang < 0){
		bran_cnt -= BREAK_UP;
//		if(bran_cnt == 0){
		if(bran_cnt < 0){
			// ���`���״�B���
			if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
			else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 
			bran_flg = 0;		//�������ڻ�
			return TRUE;
		}
	} else {
		bran_cnt += BREAK_UP;
//		if(bran_cnt == 480){
		if(bran_cnt > 480){
			// ���`���״�B���
			if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
			else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 
			bran_flg = 0;		//�������ڻ�
			return TRUE;
		}
	}
	return FALSE;
}

// �ۤꤿ������ ******************************************************************/
BOOL Produce_break_up2(int ang)
{
	RECT rect = { 0, 0, 640, 0 };
	int d7;

	// ���ڻ�����r
	if( ProduceInitFlag == TRUE ){
		ProduceInitFlag = FALSE;
		bran_flg = 0;
	}
	
	if(bran_flg == 0){		//����ʤ�
		bran_flg = 1;		//�������ڻ�
		if(ang < 0){
			bran_cnt = 480;
		} else {
			bran_cnt = 0;
		}
	}
	// �Хå����`�ե������\�ǥ��ꥢ�`
	ClearBackSurface();	

	for(d7=0; d7<bran_cnt; d7++){
		rect.top = 480-d7*480/bran_cnt;
		rect.bottom = rect.top+1;
		lpDraw->lpBACKBUFFER->BltFast( 0, 480-d7, lpBattleSurface, &rect, DDBLTFAST_WAIT );
	}

	if(ang < 0){
		bran_cnt -= BREAK_UP;
//		if(bran_cnt == 0){
		if(bran_cnt < 0){
			// ���`���״�B���
			if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
			else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 
			bran_flg = 0;		//�������ڻ�
			return TRUE;
		}
	} else {
		bran_cnt += BREAK_UP;
//		if(bran_cnt == 480){
		if(bran_cnt > 480){
			// ���`���״�B���
			if( GameState == GAME_FIELD_TO_ENCOUNT ) GameState = GAME_ENCOUNT_TO_BATTLE; 
			else if( GameState == GAME_ENCOUNT_TO_BATTLE ) GameState = GAME_BATTLE; 
			bran_flg = 0;		//�������ڻ�
			return TRUE;
		}
	}
	return FALSE;
}



// ����R�s ******************************************************************/
BOOL ProduceCenterPress( int mode )
{
	static int line;
	RECT rect1, rect2;
	int i;
	int h = 240;
	int spd = 8;

	// ���ڻ�
	if( ProduceInitFlag == TRUE )
	{
		ProduceInitFlag = FALSE;
		if( mode == 0 )
		{
			line = 240;
			if( ResoMode == 1 )
			{
				line /= 2;
			}
		}
		else
		{
			line = 0;
		}
	}


	// �Хå����`�ե������\�ǥ��ꥢ�`
	ClearBackSurface();	

	rect1.left  = 0;
	rect1.right = DEF_APPSIZEX;
	rect2.left  = 0;
	rect2.right = DEF_APPSIZEX;
	if( ResoMode == 1 )
	{
		h /= 2;
		rect1.right /= 2;
		rect2.right /= 2;
		spd /= 2;
	}
	for( i = 0; i < line; i++ )
	{
		rect1.top = h-i*h/line;
		rect1.bottom = rect1.top + 1;
		lpDraw->lpBACKBUFFER->BltFast( 0, h-i, lpBattleSurface, &rect1, DDBLTFAST_WAIT );

		rect2.top = i*h/line+h;
		rect2.bottom = rect2.top + 1;
		lpDraw->lpBACKBUFFER->BltFast( 0, i+h, lpBattleSurface, &rect2, DDBLTFAST_WAIT );
	}

	if( mode == 0 )
	{
		line -= spd;
		if( line < 0 )
		{
			return TRUE;
		}
	}
	else
	{
		line += spd;
		if( line > h )
		{
			return TRUE;
		}
	}

	return FALSE;
}



// �ݳ��軭 ********************************************************************/
BOOL DrawProduce( int no )
{
#ifdef _DEBUG		
	// �ݩ`��
	if( joy_con[ 0 ] & JOY_UP ){ 
		// �r�g���W�����줵����
		NowTime = GetTickCount();
		return FALSE;
	}
#endif
	
	// �ݳ����ŤǷ��
	switch( no ){
	
	// ̫��ץ��ǥ�`��
	// �����Ƅ�
	case PRODUCE_UP_ACCELE:		// �ϼ����Ƅ�
		return ProduceAccele( 0 );
		
	case PRODUCE_DOWN_ACCELE:	// �¼����Ƅ�
		return ProduceAccele( 1 );
		
	case PRODUCE_LEFT_ACCELE:	// ������Ƅ�
		return ProduceAccele( 2 );
		
	case PRODUCE_RIGHT_ACCELE:	// �Ҽ����Ƅ�
		return ProduceAccele( 3 );
		
	// �p���Ƅ�
	case PRODUCE_UP_BRAKE:		// �Ϝp���Ƅ�
		return ProduceBrake( 0 );
		
	case PRODUCE_DOWN_BRAKE:	// �p���Ƅ�
		return ProduceBrake( 1 );
		
	case PRODUCE_LEFT_BRAKE:	// ��p���Ƅ�
		return ProduceBrake( 2 );
		
	case PRODUCE_RIGHT_BRAKE:	// �Ҝp���Ƅ�
		return ProduceBrake( 3 );
		
	case PRODUCE_4WAY_OUT:		// �ķ����Ƅӻ������
		return Produce4Way( 0 );
		
	case PRODUCE_4WAY_IN:		// �ķ����Ƅӻ����ڤ�
		return Produce4Way( 1 );
		
	case PRODUCE_HAGARE_OUT:	// �Ϥ���I���������룩
		return ProduceHagare( 0 );
		
	case PRODUCE_HAGARE_IN:		// �Ϥ���I�������F���룩
		return ProduceHagare( 1 );
		
	case PRODUCE_HAGARE_OCHI_OUT:	// �Ϥ�������������룩
		return ProduceHagare( 2 );
		
	case PRODUCE_HAGARE_OCHI_IN:	// �Ϥ�����������F���룩
		return ProduceHagare( 3 );
		
	//�ϣƣԥץ��ǥ�`���`
	case PRODUCE_BRAN_SMALL:	// �ե��޿sС
		return Produce_bran_small(-1);

	case PRODUCE_BRAN_BIG:		// �ե��ޒ���
		return Produce_bran_small(1);

	case PRODUCE_BREAK_UP1:		//�ۤꤿ�����ϥ�����
		return Produce_break_up(-1);

	case PRODUCE_BREAK_UP2:		//�ۤꤿ�����ϥ���
		return Produce_break_up(1);

	case PRODUCE_BREAK_UP3:		//�ۤꤿ�����ϥ�����
		return Produce_break_up(-1);

	case PRODUCE_BREAK_UP4:		//�ۤꤿ�����¥���
		return Produce_break_up2(1);

	case PRODUCE_BREAK_UP5:		//�ۤꤿ�����¥�����
		return Produce_break_up2(-1);

	case PRODUCE_BREAK_UP6:		//�ۤꤿ�����¥���
		return Produce_break_up2(1);

	case PRODUCE_BREAK_UP7:		//�ۤꤿ�����¥�����
		return Produce_break_up2(-1);

	case PRODUCE_BREAK_UP8:		//�ۤꤿ�����ϥ���
		return Produce_break_up(1);

	// dwaf�ץ��ǥ�`��
	case PRODUCE_CENTER_PRESSIN:
		return ProduceCenterPress( 0 );
	case PRODUCE_CENTER_PRESSOUT:
		return ProduceCenterPress( 1 );

	}
	return TRUE;
}

// �����ȥ��ݳ�
void TitleProduce( void )
{
	static float x[ 10 ], y[ 10 ], a[ 10 ];
	static int flag = 0, cnt,cnt2, cnt3;
	static int fall[ 8 ];
	static int time;
	int i,bmpNo;
	
#ifdef _DEBUG		
	// �ݩ`��
	if( joy_con[ 0 ] & JOY_UP ){ 
		// �r�g���W�����줵����
		NowTime = GetTickCount();
		return;
	}
#endif
	//if( time++ < 30 ) return;
	
	// ���ڻ�����r
	if( ProduceInitFlag == TRUE ){
		ProduceInitFlag = FALSE;
		flag = 0;
	}
	
	// ���ڻ�
	if( flag == 0 ){
		for( i = 0 ; i < 8 ; i++ ){
			x[ i ] = 320;
			y[ i ] = -100;
			a[ i ] = 1;
			fall[ i ] = 0;
			cnt = 0;
			cnt2 = 0;
			cnt3 = 0;
			time = 0;
			
		}
		// ������ȥɥ�`��
		x[ 9 ] = -319;
		y[ 9 ] = 410;
		a[ 9 ] = 25.5;
		x[ 8 ] = 959;
		y[ 8 ] = 444;
		a[ 8 ] = 25.5;
		
		// �£ͣФ���`�ɤ���
		for( bmpNo = CG_TITLE_NAME_S ; bmpNo <= CG_TITLE_DREAM_LOGO ; bmpNo++ ){
			LoadBmp2( bmpNo );
		}
		// �r�g���W�����줵����
		NowTime = GetTickCount();
		
		flag = 1;
	}
	// �r�g����
	if( flag == 1 ){
		//time++;
		//if( time >= 120 ) 
		flag = 2;
	}
	
	// ������Ƅ�
	if( flag == 2 ){
		a[ 8 ] -= 0.5;
		x[ 8 ] -= a[ 8 ];
		//if( x[ 8 ] <= 320 ){
		if( a[ 8 ] == 0 ){
			x[ 8 ] = 320;
			flag = 3;
		}
		// �ɥ�`���Ƅ�
		a[ 9 ] -= 0.5;
		x[ 9 ] += a[ 9 ];
		//if( x[ 9 ] >= 320 ){
		if( a[ 9 ] == 0 ){
			x[ 9 ] = 320;
			flag = 3;
		}
		
	}
	
	// �󤫤���Ȥ�
	if( cnt < 8 && flag == 3 ){
		if( cnt2 % 4 == 0 ){
			fall[ cnt ] = 1;
			cnt++;
		}
		cnt2++;
	}
	
	// �Ƅ�
	if( flag == 3 ){
		for( i = 0 ; i < 8 ; i++ ){
			if( fall[ i ] >= 1 && fall[ i ] <= 3 ){
				a[ i ] += 1.0;
				y[ i ] += a[ i ];
				if( y[ i ] >= 160 ){
					y[ i ] = 160;
					fall[ i ]++;
					if( fall[ i ] == 4 ){ 
						cnt3++;
						continue;
					}
					a[ i ] *= -0.5F;
					y[ i ] += a[ i ];
				}
			}
		}
	}
	
	// �����?�ɥ�`���ʾ
	StockDispBuffer( ( int )x[ 8 ], ( int )y[ 8 ], DISP_PRIO_CHAR, CG_TITLE_JSS_LOGO, 0 );
	StockDispBuffer( ( int )x[ 9 ], ( int )y[ 9 ], DISP_PRIO_CHAR, CG_TITLE_DREAM_LOGO, 0 );
	
	if( cnt3 < 8 ){
		// ��ʾ
		for( i = 0 ; i < 8 ; i++ ){
			StockDispBuffer( ( int )x[ i ], ( int )y[ i ], DISP_PRIO_CHAR, CG_TITLE_NAME_S + i, 0 );
		}
	}else
	//if( cnt3 >= 8 && cnt3 < 28 ){
	if( cnt3 >= 8 && cnt3 < 31 ){
		if( cnt3 < 26 ){
			StockDispBuffer( ( int )x[ 0 ], ( int )y[ 0 ], DISP_PRIO_CHAR, CG_TITLE_NAME_FLASH, 0 );
		}else{
			StockDispBuffer( ( int )x[ 0 ], ( int )y[ 0 ], DISP_PRIO_CHAR, CG_TITLE_NAME_FLASH1 + cnt3 - 26 , 0 );
		}	
		cnt3++;
	}
	else StockDispBuffer( ( int )x[ 0 ], ( int )y[ 0 ], DISP_PRIO_CHAR, CG_TITLE_NAME, 0 );
	
	// ���ڻ����`
	//if( joy_trg[ 0 ] & JOY_HOME ) flag = 0;
}

