/************************/
/*	action.h			*/
/************************/
#ifndef _ACTION_H_
#define _ACTION_H_

/* �I�탞���λ  prio �΂� ****************************************************/
enum{
	PRIO_TOP,			/* ��� 	*/

	PRIO_CHR,			/* ����饯���� */

	PRIO_BG,			/* �£� 	*/
	PRIO_JIKI,			/* �ԙC 	*/
	PRIO_ENEMY,			/* �� 		*/
	PRIO_ENEMY_TAMA,	/* ���� 	*/
	PRIO_JIKI_TAMA,		/* �ԙC�� 	*/
	PRIO_ITEM,			/* �����ƥ�	*/
	PRIO_BOSS,			/* �ܥ�		*/
	PRIO_GAME_OVER,		/* GAME OVER */
	PRIO_BTM = 255		/* ����β 	*/
};

/* ����������״�B ***********************************************************/
enum{
	ACT_STATE_ACT = 0,	/* ͨ��״�B */
	ACT_STATE_DEAD		/* ����״�B */
};
/* ���� ***********************************************************************/
#define ACT_ATR_HIDE 			( 1 <<  1 )	/* �Ǳ�ʾ */
#define ACT_ATR_HIT 			( 1 <<  2 )	/* �������ж����� */
#define ACT_ATR_HIT_BOX 		( 1 <<  3 )	/* �������ж� + �ܥå�����ʾ */
#define ACT_ATR_INFO 			( 1 <<  4 )	/* һ�Х���ե���ʾ */
#define ACT_ATR_HIDE2 			( 1 <<  5 )	/* �Ǳ�ʾ+�������ж����� */
#define ACT_ATR_BTL_CMD_END 	( 1 <<  6 )	/* ���L���ޥ�������K�˥ե饰 */
#define ACT_ATR_TYPE_PC 		( 1 <<  7 )	/* �У� */
#define ACT_ATR_TYPE_OTHER_PC 	( 1 <<  8 )	/* ���ΣУ� */
#define ACT_ATR_TYPE_PET 		( 1 <<  9 )	/* �ڥå� */
#define ACT_ATR_TYPE_ITEM 		( 1 << 10 )	/* �����ƥ� */
#define ACT_ATR_TYPE_GOLD 		( 1 << 11 )	/* ���� */
#define ACT_ATR_TYPE_OTHER 		( 1 << 12 )	/* ���������ɥ�����ˡ����壩 */
#define ACT_ATR_HIT_BOX_ALL1 	( 1 << 13 )	/* �������ж� + �ܥå�����ʾ �}���x�k�� */
#define ACT_ATR_HIT_BOX_ALL2 	( 1 << 14 )	/* �������ж� + �ܥå�����ʾ �}���x�k�� */
#define ACT_ATR_HIT_BOX_ALL3 	( 1 << 15 )	/* �������ж� + �ܥå�����ʾ �}���x�k�� */
#define ACT_ATR_TRAVEL 			( 1 << 16 )	/* ������ */

typedef unsigned char UCHAR;
typedef int                 BOOL;
typedef unsigned int        UINT;
#define FALSE 0
#define TRUE 1
/* ������������� *********�����ṹ*************************************************/
struct action{
	struct 	action *pPrev, *pNext;		/* ǰ��Υ��������ݥ��� */
	void 	( *func )( struct action * );	/* �g���v���ؤΥݥ��� */
	void 	*pYobi;						/* ��䘋����ݥ��� */
	void 	*pOther;					/* ����ӛ���ݥ��� */
	UCHAR 	prio;						/* �I���΃����λ */
	UCHAR 	dispPrio;					/* ��ʾ�΃����λ */
	int 	x, y;						/* ��ʾ���� */
	int		hitDispNo;					/* �������ж��÷��� */
	BOOL	deathFlag;					/* ��������������ե饰 */
	
	int 	dx, dy;						/* ���� */
	int 	dir;						/* ���� */
	int 	delta;  					/* �ϳɥ٥��ȥ� */
	
	char 	name[ 29 ];					/* ��ǰ */
	char 	freeName[ 13 ];				/* �ե�`��ǰ */
	int 	hp;							/* �ȣ� */
	int 	maxHp;						/* ���ȣ� */
	int 	mp;							/* �ͣ� */
	int 	maxMp;						/* ���ͣ� */
	int 	level;						/* �̣ţ֣ţ� */
	int 	status;						/* ���Ʃ`���� */
	int 	itemNameColor;				/* �����ƥ����ǰ��ɫ */
	
	int		bmpNo;						/* ���ץ饤�ȷ��� */
	int		atr;						/* ���� */
	int		state;						/* ״�B */
	int		actNo;						/* �Єӷ��ţ����ɤ�ʹ�ÿɣ� */
	int		damage;						/* ����`�� */

	int		gx, gy;						// �ޥåץ���å����ˣ��F�ڵأ�
	int		nextGx, nextGy;				// �ޥåץ���å����ˣ�Ŀ�ĵأ�
	int		bufGx[10], bufGy[10];		// �Ƅ������ˤΥХåե�
	short	bufCount;					// �Ƅ����ˤ��΂��O������Ƥ��뤫
	short	walkFlag;					// �i�����˥ᤫ���������˥���Ф��椨�����ߥ󥰥ե饰
	float	mx, my;						// �ޥå�����
	float	vx, vy;						// �ƄӉ���

// ����
	short 	earth;						// �� ����
	short 	water;						// ˮ ����
	short 	fire;						// �� ����
	short 	wind;						// �L ����

// ��������
	int		dirCnt;						// �����Q�����󥿩`

//����������  �ٶ� ����
	int		spd;						// �Ƅӥ��ԩ`��( �������� )( ���ǣ��ɥå� )
	int		crs;						// ����( �������� )( �Ϥ������һؤ� )
	int		h_mini;						// С�����
	int		v_mini;						// С����k

//������������ ͼ��
	int		anim_chr_no;				// ���˥�`����󷬺ţ� anim_tbl.h �η��� )
	int		anim_chr_no_bak;			// ǰ�إ��˥�`����󷬺�
	int		anim_no;					// ��������
	int		anim_no_bak;				// ǰ�ؤ΄�������
	int		anim_ang;					// ���˥�`�������( ������ )( �¤������һؤ� )
	int		anim_ang_bak;				// ǰ�ؤΥ��˥�`�������
	int		anim_cnt;					// ���˥�`����󥫥���
	int		anim_frame_cnt;				// ���˥�`�����ե�`�५����
	int		anim_x;						// ���ե��åȣ����� ( Sprbin + Adrnbin )
	int		anim_y;						// ���ե��åȣ����� ( Sprbin + Adrnbin )
	int		anim_hit;					// �������ж�
};

typedef struct action ACTION;


/* ���������Υ��������ꥹ�ȥݥ��� */
extern ACTION *pActTop;
extern ACTION *pActBtm;

/* �ԙC�Υݥ��� */
extern ACTION *pJiki;

#ifdef _DEBUG		
/* ������������������ */
extern int ActCnt;
#endif

/* ���������ꥹ�Ȥ˵��h *****************************************************/
ACTION *GetAction( UCHAR prio, UINT yobiSize );

/* ���������ꥹ�ȳ��ڻ� *****************************************************/
void InitAction( void );

/* ����������ߤ餻�� *********************************************************/
void RunAction( void );

/* ���������ꥹ�Ȥ���Ĩ���ʂ�************************************************/
void DeathAction( ACTION *pAct );

/* ȫ���������Ĩ���ʂ� *******************************************************/
void DeathAllAction( void );

/* ���������ꥹ�Ȥ�����ȫĨ�� ***********************************************/
void ClearAction( ACTION *pAct );

/* ���������K�˄I�� *********************************************************/
void EndAction( void );

#endif
