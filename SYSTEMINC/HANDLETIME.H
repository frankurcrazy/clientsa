#ifndef __HANDLETIME_H__
#define __HANDLETIME_H__

//#undef EXTERN
//#ifdef __HANDLETIME_C__
//#define EXTERN
//#else  /*__HANDLETIME_C__*/
//#define EXTERN extern
//#endif

//EXTERN struct timeval NowTime;
//EXTERN int     DEBUG_ADJUSTTIME;



//BOOL setNewTime( void );
#if 0

#define NIGHT_TO_MORNING	906
#define MORNING_TO_NOON		1006
#define NOON_TO_EVENING		356
#define EVENING_TO_NIGHT	456

#else

#define NIGHT_TO_MORNING	700
#define MORNING_TO_NOON		930
#define NOON_TO_EVENING		200
#define EVENING_TO_NIGHT	300

#endif

/*ここから下、HiO。LSrgvS。*/
/*  grg150(分)*60=9000秒=750*12 で   LS1日   */
/*  LS1200rg  LS1日   */
/*  LS100日で   LS1年   */
typedef struct tagLSTIME
{
	int year;
	int day;
	int hour;
}LSTIME;


typedef enum
{
	LS_NOON,
	LS_EVENING ,
	LS_NIGHT,
	LS_MORNING ,
}LSTIME_SECTION;

// スト`ンエイジrg
extern LSTIME SaTime;
extern long serverTime;
extern long clientTime;
extern int SaTimeZoneNo;	// F在のrg
extern BOOL TimeZonePalChangeFlag; // rgでパレットチェンジするフラグ

void RealTimeToSATime( LSTIME *lstime );
/*void LSTimeToRealTime( LSTIME *lstime, long *t);*/
LSTIME_SECTION getLSTime (LSTIME *lstime);
// rgでパレットチェンジI理 ***********************************/
void TimeZoneProc( void );

#endif  /*__HANDLETIME_H__*/

