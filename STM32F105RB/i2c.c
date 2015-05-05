#include "i2c.h"

void I2C_Configuration()
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin          =GPIO_Pin_8|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed        =GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode         =GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_Mode                =I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle           =I2C_DutyCycle_2;
  I2C_InitStructure.I2C_Ack                 =I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress =I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed          =400000;
  I2C_Init(I2C2,&I2C_InitStructure);
}

void AWrite_ATP(char slave_add, char cmd)
{
 
  I2C_GenerateSTART(I2C1,ENABLE);
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C1,slave_add<<1,I2C_Direction_Transmitter);
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  I2C_SendData(I2C1,cmd);
  while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)!=SUCCESS);
  I2C_GenerateSTOP(I2C1, ENABLE);
 
}


char ARead_ATP(char slave_add)
{
  char rdata;
  //I2C1���Ӊ�H��L���ɂ���
  I2C_Cmd(I2C1,ENABLE);
  //I2C1���Ӊ�H��I2C�o�X�ŃX�^�[�g�E�R���f�V�����𐶐�
  I2C_GenerateSTART(I2C1,ENABLE);
  //�}�X�^���[�h��I��
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
  //��M������I��
  I2C_Send7bitAddress(I2C1,slave_add<<1,I2C_Direction_Receiver);
  //�}�X�^�͎�M���[�h��I��
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  //I2C1���Ӊ�H��I2C�o�X��ACK����������L���ɂ���
  I2C_AcknowledgeConfig(I2C1,DISABLE);
  //I2C1���Ӊ�H��I2C�o�X�ŃX�g�b�v�E�R���f�V�����𐶐�
  I2C_GenerateSTOP(I2C1, ENABLE);
  //�}�X�^�̓o�C�g�R�[�h����M
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));
  //I2C1���Ӊ�H��I2C�o�X�Ńf�[�^����M
  rdata = I2C_ReceiveData(I2C1);
  //��M�����f�[�^��Ԃ�
  return rdata;
}








