#ifndef __VSQEventManager_h__
#define __VSQEventManager_h__

/////includes
//
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

#include <math.h>

#include "UTAUManager.h"

#define A4_PITCH	440.0
#define A4_NOTE		57
//Tick per Second 960/60�@or 480/60
#define TICK_PER_SEC 8.0


//�v���g�^�C�v�錾
struct	ControlEvent;
class	EventManager;

/**
* ControlEvent�̒l���i�[����\���́B
*/
struct	ControlEvent
{
	long		nTick;				//!< @brief �C�x���g�̈ʒu(Tick�w��)
	int			iValue;				//!< @brief �l
};

/**
* �C�x���g�Ǘ��p�N���X�B
* �f�[�^�̃��X�g��ێ����ēK�X�l��Ԃ��܂��B
*/
class	EventManager
{
public:

	/**
	* @brief �f�X�g���N�^�B�m�ۂ������\�[�X���J�����܂��B
	*/
	virtual ~EventManager();

	/**
	* @brief �w�肳�ꂽTick�����ł̒l��Ԃ��B
	* @param nTick Tick�����B
	* @return ����������l�A���s������-1��Ԃ��B
	*/
	virtual	int		GetValue(long nTick);

	/**
	* @brief �w�肳�ꂽTick�����ł̒l���L������B
	* @param iValue ���͂���l nTick Tick�����B
	*/
	virtual	bool	SetValue(long nTick,int iValue);

	/**
	* @brief �w�肳�ꂽ�����񂩂�l��o�^����B
	* @param sFront �O������ sBack �㕶����
	*/
	virtual	bool	SetValue(string sFront,string sBack);
protected:
	list<ControlEvent*>	EventList;	//!< @brief �C�x���g�̓��e���i�[���郊�X�g�B
private:
};

#endif
