#ifndef __straightFrame_h__
#define __straightFrame_h__

#include <math.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <map>

using namespace std;

#define SAFE_DELETE_ARRAY(x)	if(x){delete[] x;x=NULL;}
#define SAFE_DELETE(x)			if(x){delete x;x=NULL;}

class straightFrame
{
public:

	/**
	* @brief �R���X�g���N�^�B
	*/
	straightFrame(){pApBuffer=NULL;pSpBuffer=NULL;}

	/**
	* @brief �f�X�g���N�^�B
	*/
	~straightFrame(){SAFE_DELETE_ARRAY(pApBuffer);SAFE_DELETE_ARRAY(pSpBuffer);}

	/**
	* @brief F0��ݒ肷��B
	*/
	void	SetF0(double dSrcF0){dF0=dSrcF0;}

	/**
	* @brief ������
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	SetFrame(straightFrame* pSrcFrame);

	/**
	* @brief �^����ꂽ�������g�ɃR�s�[����B
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	SetFrame(double dSrcF0,double* pSrcApBuffer,long nSrcApLength,double* pSrcSpBuffer,long nSrcSpLength);

	/**
	* @brief ��������ړx�o�b�t�@��^����ꂽ�|�C���^�֏������ށB
	* @param nDstApLength �������ރo�b�t�@�̔z�񒷁B
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	GetApBuffer(double* pDstApBuffer,long nDstApLength);

	/**
	* @brief �X�y�N�g���O�����o�b�t�@��^����ꂽ�|�C���^�֏������ށB
	* @param nDstSpLength �������ރo�b�t�@�̔z�񒷁B
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	GetSpBuffer(double* pDstSpBuffer,long nDstSpLength);

	/**
	* @brief ���g�̊�{���g����Ԃ��B
	* @return ���g�̊�{���g��
	*/
	double	GetF0(void){return dF0;}

	/**
	* @brief ���g�̔�������ړx�o�b�t�@�̔z�񒷂�Ԃ��B
	*/
	long	GetApLength(void){if(nApLength<=0)nApLength=0; return nApLength;}

	/**
	* @brief ���g�̃X�y�N�g���O�����o�b�t�@�̔z�񒷂�Ԃ��B
	*/
	long	GetSpLength(void){if(nSpLength<=0)nSpLength=0; return nSpLength;}

	/**
	* @brief �^����ꂽ�䗦�ɏ]���ăt�H���}���g��ω�������B
	*/
	void	ApplyFormantChange(double dChangeRate);

	/**
	* @brief �^����ꂽ�䗦�ɏ]���ă_�C�i�~�N�X��ω�������B
	*/
	void	ApplyDynamicsChange(double dChangeRate);

private:

	/**
	* @brief �^����ꂽ�z�񒷂ɏ]���o�b�t�@���쐬����B
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	CreateBuffer(long nApLength,long nSpLength);

	/**
	* @brief �^����ꂽ�z��ւ̃|�C���^�Ɣz�񒷂ɏ]���A�o�b�t�@�Ƀf�[�^���������ށB
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	SetApBuffer(double* pSrcApBuffer,long nSrcApLength);

	/**
	* @brief �^����ꂽ�z��ւ̃|�C���^�Ɣz�񒷂ɏ]���A�o�b�t�@�Ƀf�[�^���������ށB
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	SetSpBuffer(double* pSrcSpBuffer,long nSrcSpLength);

	double	dF0;				//!< @brief ��{���g��
	double*	pApBuffer;			//!< @brief ��������ړx�p�o�b�t�@
	long	nApLength;			//!< @brief ��������ړx�̎��g��������
	double*	pSpBuffer;			//!< @brief �X�y�N�g���O�����p�o�b�t�@
	long	nSpLength;			//!< @brief �X�y�N�g���O�����̎��g��������
};

#endif
