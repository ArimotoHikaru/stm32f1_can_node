#include "can.h"

#define LIST 0
#define MASK 1
#define BIT16 16
#define BIT32 32

typedef struct _CAN_f{
	int FNE;	//FilterNumberEnable
	int MODE;	//LISTorMASK
	int SCALE;	//16bitor32bit
	int STID[4];	//
	int RTR[4];	//
	int	IDE[4];	//
	int EXID[4];	//
}CAN_f;
//RTR 0:�f�[�^�t���[���@1:�����[�g�t���[��
CAN_f CAN_fm[14]={
//FNE	MODE	SCALE	STID    					RTR 		IDE			EXID
 {1,	LIST,   BIT16,	{0x400,0x401,0x402,0x403},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//0
 {1,	LIST,   BIT16,	{0x440,0x441,0x442,0x443},	{1,1,1,1},	{0,0,0,0},	{0,0,0,0}	},//1
 {0,	LIST,   BIT16,	{0x001,0x000,0x000,0x000},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//2
 {0,	MASK,   BIT32,	{0x000,0x000},				{0,0},		{0,0},		{0,0}		},//3
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//4
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//5
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//6
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//7
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//8
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//9
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//10
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//11
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//12
 {0,	LIST,   BIT16,	{0x000,0x00A,0x00B,0x00C},	{0,0,0,0},	{0,0,0,0},	{0,0,0,0}	},//13
};
/* ���̐���
 *�@FNE:���̃t�B���^�[���g�����ۂ�
 *
 * STID�ARTR�AIDE,EXID�̔z��[0][1][2][3]��
 * LIST,BIT16�̂Ƃ�	[0]:filter 	[1]:filter		[2]:filter 	[3]:filter
 * MASK,BIT16�̂Ƃ�	[0]:filter 	[1]:[0]��mask  	[2]:filter 	[3]:[2]��mask
 * LIST,BIT32�̂Ƃ�	[0]:filter 	[1]:filter 		[2]:���g�p 	[3]:���g�p
 * MASK,BIT32�̂Ƃ�	[0]:filter 	[1]:[0]��mask 	[2]:���g�p 	[3]:���g�p
 * �Ƃ����\���ɂȂ��Ă���
 */


void CAN_Configuration(void)
{
	/* Define variable ------------------------------------------------------*/
	int filter_num = 0;

	/* Define InitTypeDef ---------------------------------------------------*/
	GPIO_InitTypeDef 		GPIO_InitStructure;
	CAN_InitTypeDef 		CAN_InitStructure;
	CAN_FilterInitTypeDef	CAN_FilterInitStructure;

	/* initialize InitTypeDef -----------------------------------------------*/
	CAN_DeInit(CAN2);

	/* Supply clock source --------------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2, ENABLE);//CAN2���g���ꍇ��CAN1���L���ɂ��Ȃ���΂Ȃ�Ȃ�filter�̐ݒ肪CAN1�̐ݒ���g������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//RCC_APB2Periph_AFIO�L���ɂ���

	/* Define gpio_config ---------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IPU;//������厖
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Set up CAN function -------------------------------------------------*/
	/* �^�C���g���K�ʐM���[�h�̗L�����E��������ݒ肷�� */
	CAN_InitStructure.CAN_TTCM 		= ENABLE;
	/* �����o�X�I�t�Ǘ�(Automatic Bus-Off Management)�̗L�����E��������ݒ肷�� */
	CAN_InitStructure.CAN_ABOM 		= DISABLE;
	/* �����ċN�����[�h�̗L�����E��������ݒ肷��  */
	CAN_InitStructure.CAN_AWUM 		= DISABLE;
	/* �����đ��M�֎~��L�����E���������� DISABLE: �����đ��M�֎~�𖳌���(�܂�đ��M�͗L��) ENABLE: �����đ��M�֎~�B����ɑ��M����Ȃ��Ă����M��1�񂾂��s���� */
	CAN_InitStructure.CAN_NART 		= DISABLE;
	/* ��MFIFO���b�N���[�h�̗L�����E��������ݒ肷�� */
	CAN_InitStructure.CAN_RFLM 		= DISABLE;
	/* ���MFIFO�̑��M�������w�肷��BDISABLE:���b�Z�[�WID�ő��M���������肳���  ENABLE:�\�t�g�E�F�A�ő��M�v�����������ꂽ���Ԃő��M����� */
	CAN_InitStructure.CAN_TXFP 		= DISABLE;
	/* CAN��Mode��ݒ肷�� */
	CAN_InitStructure.CAN_Mode 		= CAN_Mode_Normal;
	/* �ē����W�����v��(CAN�n�[�h�E�F�A���ē������s���ۂ̃r�b�g��)�����ԒP�ʂ̐��Őݒ肷�� */
	CAN_InitStructure.CAN_SJW 		= CAN_SJW_1tq;
	/* CAN�r�b�g�^�C�~���O���W�X�^(CAN_BTR)��TS1[3:0]��ݒ肷��B */
	CAN_InitStructure.CAN_BS1 		= CAN_BS1_5tq;
	/* CAN�r�b�g�^�C�~���O���W�X�^(CAN_BTR)��TS2[2:0]��ݒ肷�� */
	CAN_InitStructure.CAN_BS2 		= CAN_BS2_6tq;
	/* �{�[���[�g�v���X�P�[���ݒ肷�� 1�`1024 APB1=36MHz*/
	CAN_InitStructure.CAN_Prescaler	= 3;
	//CAN�{�[���[�g = 1Mbps
	CAN_Init(CAN2, &CAN_InitStructure);

	/* Set up CAN Filter function -------------------------------------------------*/
	while(filter_num < 14){//�t�B���^�[�̐������J��Ԃ�0~13

		CAN_FilterInitStructure.CAN_FilterNumber = filter_num+14;

		CAN_FilterInitStructure.CAN_FilterFIFOAssignment	= 0;

		if(CAN_fm[filter_num].FNE != 0){//�t�B���^�[�̐ݒ�������Ă���ꍇ

			CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;

			if(CAN_fm[filter_num].MODE == MASK){/* �t�B���^�[���[�h�ݒ� */
				CAN_FilterInitStructure.CAN_FilterMode	= CAN_FilterMode_IdMask;
			}else if(CAN_fm[filter_num].MODE == LIST){
				CAN_FilterInitStructure.CAN_FilterMode	= CAN_FilterMode_IdList;
			}

			if(CAN_fm[filter_num].SCALE == BIT16){

				CAN_FilterInitStructure.CAN_FilterScale			= CAN_FilterScale_16bit;

				CAN_FilterInitStructure.CAN_FilterIdLow			= 	(CAN_fm[filter_num].STID[0] << 5) |
																	(CAN_fm[filter_num].RTR[0] << 4) |
																	(CAN_fm[filter_num].IDE[0] << 3) |
																	(CAN_fm[filter_num].EXID[0] >> 15);

				CAN_FilterInitStructure.CAN_FilterMaskIdLow		= 	(CAN_fm[filter_num].STID[1] << 5) |
																	(CAN_fm[filter_num].RTR[1] << 4) |
																	(CAN_fm[filter_num].IDE[1] << 3) |
																	(CAN_fm[filter_num].EXID[1] >> 15);

				CAN_FilterInitStructure.CAN_FilterIdHigh		= 	(CAN_fm[filter_num].STID[2] << 5) |
																	(CAN_fm[filter_num].RTR[2] << 4) |
																	(CAN_fm[filter_num].IDE[2] << 3) |
																	(CAN_fm[filter_num].EXID[2] >> 15);

				CAN_FilterInitStructure.CAN_FilterMaskIdHigh	= 	(CAN_fm[filter_num].STID[3] << 5) |
																	(CAN_fm[filter_num].RTR[3] << 4) |
																	(CAN_fm[filter_num].IDE[3] << 3) |
																	(CAN_fm[filter_num].EXID[3] >> 15);

			}else if(CAN_fm[filter_num].SCALE == BIT32){

				CAN_FilterInitStructure.CAN_FilterScale			= CAN_FilterScale_32bit;

				CAN_FilterInitStructure.CAN_FilterIdLow			= 	((CAN_fm[filter_num].EXID[0]&0x1FFF)<<3) |
																	(CAN_fm[filter_num].IDE[0] << 2) |
																	(CAN_fm[filter_num].RTR[0] << 1);

				CAN_FilterInitStructure.CAN_FilterMaskIdLow		= 	(CAN_fm[filter_num].STID[0] << 5) |
																	(CAN_fm[filter_num].EXID[0] >> 13);

				CAN_FilterInitStructure.CAN_FilterIdHigh		= 	((CAN_fm[filter_num].EXID[1]&0x1FFF)<<3) |
																	(CAN_fm[filter_num].IDE[1] << 2) |
																	(CAN_fm[filter_num].RTR[1] << 1);

				CAN_FilterInitStructure.CAN_FilterMaskIdHigh	= 	(CAN_fm[filter_num].STID[1] << 5) |
																	(CAN_fm[filter_num].EXID[1] >> 13);
			}

		}else{//�t�B���^�[�̐ݒ�������Ă��Ȃ��ꍇ���̃t�B���^�[�i���o�[�̓m���A�N�e�B�u

			CAN_FilterInitStructure.CAN_FilterActivation 	= DISABLE;
		}

		CAN_FilterInit(&CAN_FilterInitStructure);

		filter_num++;
	}

#ifdef USE_INTERRUPT_CAN_RX
	CAN_ITConfig(CAN2, CAN_IT_FMP0,ENABLE);//message pending Interrupt
#endif

#ifdef USE_INTERRUPT_CAN_TX
	CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);//Transmit mailbox empty Interrupt
#endif

}




