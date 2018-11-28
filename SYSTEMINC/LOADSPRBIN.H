#ifndef _SPRITE_H_
#define _SPRITE_H_

//���ץ饤�ȤηN������x���Ƥ��롣
//?��????ϒ????�E?????
#define mxSPRITE	32768

typedef unsigned char MOJI;
typedef unsigned char U1;
typedef          char S1;
typedef unsigned short U2;
typedef          short S2;
typedef unsigned long U4;
typedef          long S4;
typedef float  F4;
typedef double F8;

typedef enum {
	ANIM_ATTACK,
	ANIM_DAMAGE,
	ANIM_DEAD,
	ANIM_STAND,
	ANIM_WALK,
	ANIM_SIT,
	ANIM_HAND,
	ANIM_HAPPY,
	ANIM_ANGRY,
	ANIM_SAD,
	ANIM_GUARD,
	ANIM_NOD,
	ANIM_THROW,
	ANIM_LIST_
} ANIM_LIST;//??????��?????????????????


// 1�ե�`������
typedef struct
{
	U4 BmpNo;				// bmp����(real.bin�Ǥ�λ��)
	S2 PosX,PosY;			// ��ʾλ�äΥ��ե��å�
	U2 SoundNo;				// ������ɷ���
} FRAMELIST;


// �����˥�`���������
typedef struct
{
	U2 dir;					// ���˥�`��������
	U2 no;					// �ΤΥ��˥�`�����(0�������ݩ`���Ȥ�)
	U4 dtAnim;				// ���˥�`�����Όg�Еr�g
	U4 frameCnt;
	FRAMELIST *ptFramelist;	// �����˥�`���������
} ANIMLIST;
//???????

// SPR��λ����� ????????
typedef struct
{
	U4 sprNo;				// �I���ϣ�����ʼ�ޤ�(�g�H��SPR���Ť�65536���餤����)
	U4 offset;				// SPR.BIN�Υե�������i���z��λ��
	U2 animSize;			// ���˥�`��������
} SPRADRN;


typedef struct
{
	U2 animSize;				// ���˥�`��������
	ANIMLIST	*ptAnimlist;	//���˥�`�����ꥹ�Ȥ�����NULL�ʤ�ֱ��BMP���ʾ
} SPRITEDATA;


typedef struct {
	U2			dtz;			//�軭�����Ȥ���혷���Q���
	U2			noSpritedata;	//���ץ饤�ȷ���
	U2			mdAnim;			//���˥�`������`��
	U2			mdAnim2;		//�ΤΥ��˥�`������`��
	U2			dir;			//����ϣ�
	U2			ctLoop;			//�λإ�`�פ�������
	float		ctAnim;			//���˥�`����󥫥��󥿩` ������δ���ϲ���С����I���������ϥե�`�෬�ńI��
	float		dtAnim;			//���ԩ`��	���줬���Ǥ���С�spr�ե����뤫�����Ï���
} SPRITE;

extern SPRITEDATA SpriteData[mxSPRITE];


BOOL InitSprBinFileOpen( char *, char * );


#endif
