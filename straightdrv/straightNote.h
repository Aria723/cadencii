#ifndef __straightNote_h__
#define __straightNote_h__

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <map>

using namespace std;

#include <straight/straight.h>

#include "straightFrame.h"
#include "VSQNoteManager.h"

#define SAFE_DELETE_ARRAY(x)	if(x){delete[] x;x=NULL;}
#define SAFE_DELETE(x)			if(x){delete x;x=NULL;}

class	straightNote{
public:

	/**
	* @brief �R���X�g���N�^�B�e��ϐ����������B
	*/
	straightNote(){source=NULL;specgram=NULL;pStraight=NULL;nFrameLength=-1;}

	/**
	* @brief �f�X�g���N�^�B�J�������B
	*/
	~straightNote(){if(source)straightSourceDestroy(source);if(specgram)straightSpecgramDestroy(specgram);}

	/**
	* @brief �t���[���ԍ����玩�g�̊Y���������ǂݏo���B
	* @param pDstFrame �ǂݏo����������������straightFrame�ւ̃|�C���^�B
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	GetFrame(straightFrame* pDstFrame,long nFrame);

	/**
	* @brief STF�t�@�C���ǂݍ��݊֐�
	* @param sFileName �ǂݍ��ރt�@�C����
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	LoadSTFFile(Straight* pStraight,string sFileName);

	/**
	* @brief ���g�̃t���[������Ԃ��B
	* @return ���g�̃t���[����
	*/
	long	GetFrameLength(void){return nFrameLength;}

private:
	long	nFrameLength;					//!< @brief �m�ۂ����f�[�^�̃t���[����

	Straight*			pStraight;			//!< @brief STRAIGHT�p
	StraightSource		source;				//!< @brief �m�ۂ����������
	StraightSpecgram	specgram;			//!< @brief �m�ۂ����X�y�N�g���O�������
};

#endif
