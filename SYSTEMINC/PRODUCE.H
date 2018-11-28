/************************/
/*	produce.h			*/
/************************/
#ifndef _PRODUCE_H_
#define _PRODUCE_H_

// �Хȥ륵�`�ե������ݳ��ηN�
enum{
	PRODUCE_UP_ACCELE,			// �ϼ����Ƅ�
	PRODUCE_UP_BRAKE,			// �Ϝp���Ƅ�
	
	PRODUCE_DOWN_ACCELE,		// �¼����Ƅ�
	PRODUCE_DOWN_BRAKE,			// �p���Ƅ�
	
	PRODUCE_LEFT_ACCELE,		// ������Ƅ�
	PRODUCE_LEFT_BRAKE,			// ��p���Ƅ�
	
	PRODUCE_RIGHT_ACCELE,		// �Ҽ����Ƅ�
	PRODUCE_RIGHT_BRAKE,		// �Ҝp���Ƅ�
	
	PRODUCE_4WAY_OUT,			// �ķ����Ƅӻ������
	PRODUCE_4WAY_IN,			// �ķ����Ƅӻ����ڤ�
	
	PRODUCE_HAGARE_OUT,			// �Ϥ���I���������룩
	PRODUCE_HAGARE_IN,			// �Ϥ���I�������F���룩
	
	PRODUCE_HAGARE_OCHI_OUT,	// �Ϥ�������������룩
	PRODUCE_HAGARE_OCHI_IN,		// �Ϥ�����������F���룩
	
	PRODUCE_BRAN_SMALL,			// �ե��޿sС
	PRODUCE_BRAN_BIG,			// �ե��ޒ���

	PRODUCE_BREAK_UP1,			//�ۤꤿ�����ϥ�����
	PRODUCE_BREAK_UP2,			//�ۤꤿ�����ϥ���

	PRODUCE_BREAK_UP3,			//�ۤꤿ�����ϥ�����
	PRODUCE_BREAK_UP4,			//�ۤꤿ�����¥���

	PRODUCE_BREAK_UP5,			//�ۤꤿ�����¥�����
	PRODUCE_BREAK_UP6,			//�ۤꤿ�����¥���

	PRODUCE_BREAK_UP7,			//�ۤꤿ�����¥�����
	PRODUCE_BREAK_UP8,			//�ۤꤿ�����ϥ���

	PRODUCE_CENTER_PRESSIN,		// �����ۤꤿ���ߥ���
	PRODUCE_CENTER_PRESSOUT,	// �����ۤꤿ���ߥ�����

	PRODUCE_END,				// �K��

};

// �ݳ�����
extern int ProduceNo;
// �ݳ����ڻ��ե饰
extern int ProduceInitFlag;

// �Хȥ륵�`�ե����λ������� **************************************************/
void CopyBackBuffer( void );

// �ݳ��軭 ********************************************************************/
BOOL DrawProduce( int no );

// �����ȥ��ݳ� ********************************************************************/
void TitleProduce( void );

#endif
