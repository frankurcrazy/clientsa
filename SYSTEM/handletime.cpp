#define __HANDLETIME_C__

#include <system.h>
#include <time.h>
#include "handletime.h"

//#ifdef _DEBUG
//(180)秒（３分）で1日
//#define LSTIME_SECONDS_PER_DAY 180
//#else
//(750X12)秒（１rg半）で1日
#define LSTIME_SECONDS_PER_DAY 5400 /* LSTIME一日ぶんのFgの秒数 */
//#endif
/*
  LSTIME_SECONDS_PER_DAY を浃à毪取rgのMみ具合を浃à毪长趣扦搿

              LSrgで一日あたりのFgのrg数
  9000 ()   2.5 [hour]
  900           0.25[hour] = 15[min]
  90            0.025[hour] = 1.5[min] = 90[sec]
  9             9[sec]

*/

#define LSTIME_HOURS_PER_DAY 1024 /* LSTIME一日ぶんのLSTIMEのrg数 */
#define LSTIME_DAYS_PER_YEAR 100 /* LSTIME一年ぶんのLSTIMEの日数 */


// スト`ンエイジrg
LSTIME SaTime;
long serverTime;
long clientTime;
int SaTimeZoneNo;	// F在のrg
BOOL TimeZonePalChangeFlag; // rgでパレットチェンジするフラグ
#if 0
/*------------------------------------------------------------
 * 一ル`プに一回呼ばれて、rgをMめる。
 * 引数
 *  なし
 * 返り
 *  成功    TRUE(1)
 *  失    FALSE(0)
 ------------------------------------------------------------*/
BOOL setNewTime( void )
{
    if( gettimeofday( &NowTime, (struct timezone*)NULL) != 0 )
        return FALSE;
    NowTime.tv_sec += DEBUG_ADJUSTTIME;
    return TRUE;
}
#endif

/*******************************************************************
今元旦にした。byHiO 1998/12/4 18:37
*******************************************************************/
static long era = (long)912766409 + 5400;
									/* SAの椁摔氦椁筏 */
									/* LS元年を定xするてきと`な数
                                    クライアントと同じじゃないとだめ。*/

/*******************************************************************
	マシンrgからLSrgにする
	long t : timeで出る
	LSTIME *lstime : LSTIME造体へのポインタ
*******************************************************************/
void RealTimeToSATime( LSTIME *lstime )
{
	long lsseconds; /* LS元年からの秒数 */
    long lsdays; /* LS元年からの日数 */

	lsseconds = time( NULL ) + serverTime - clientTime - era; /* LS元年からの秒数 */

    /* 元年からの秒数を1年当りの秒数で割ると、年になる */
	lstime->year = (int)( lsseconds/(LSTIME_SECONDS_PER_DAY*LSTIME_DAYS_PER_YEAR) );

    lsdays = lsseconds/LSTIME_SECONDS_PER_DAY;/* まず元年からの日数を算して */
	lstime->day  = lsdays % LSTIME_DAYS_PER_YEAR;/* 年当たりの日数で割ったあまりが日*/


    /*(750*12)秒で1日*/
    lstime->hour = (int)(lsseconds % LSTIME_SECONDS_PER_DAY )
/* ここまでで、一日が始まってから何秒たったか。 */
        * LSTIME_HOURS_PER_DAY / LSTIME_SECONDS_PER_DAY;
    /* 一日あたりの秒数で割ってから一日あたりのrg数をかけるとF在何r
     なのかがわかる。*/

	return;
}

/*******************************************************************
	LSrgからマシンrgにする
	LSTIME *lstime : LSTIME造体へのポインタ
	long *t : grgへのポインタ
*******************************************************************/
void LSTimeToRealTime( LSTIME *lstime, long *t)
{
	*t=(long)(
        ( lstime->hour*LSTIME_DAYS_PER_YEAR+lstime->day) /* rg */
               *LSTIME_HOURS_PER_DAY

        +     lstime->year)
        /*このv数はばぐっているようにえる。nakamura      */


        *450;
	return;
}

/*******************************************************************
	LSrgで今のrg区分を得る
	 int : 夜0、朝1、昼2、夜3
	LSTIME *lstime : LSTIME造体へのポインタ
*******************************************************************/
LSTIME_SECTION getLSTime (LSTIME *lstime)
{
	if (NIGHT_TO_MORNING < lstime->hour
        && lstime->hour <= MORNING_TO_NOON)
		return LS_MORNING;
	else if(NOON_TO_EVENING < lstime->hour
            && lstime->hour <= EVENING_TO_NIGHT)
		return LS_EVENING;
	else if(EVENING_TO_NIGHT < lstime->hour
            && lstime->hour <= NIGHT_TO_MORNING)
		return LS_NIGHT;
	else
		return LS_NOON;
}

// rgI理とパレットチェンジI理 ***********************************/
void TimeZoneProc( void )
{
	int	timeZoneNo;
	
	//ＳＡrgで今のrg区分を得る
	timeZoneNo = getLSTime ( &SaTime );
	// rgが浃铯盲r
	if( SaTimeZoneNo != timeZoneNo ){
		SaTimeZoneNo = timeZoneNo;	// rg更新
		// rgでパレットチェンジするr
		if( TimeZonePalChangeFlag == TRUE ){
			PaletteChange( SaTimeZoneNo, PAL_CHANGE_TIME );// パレットチェンジ
		}
	}
}
	

