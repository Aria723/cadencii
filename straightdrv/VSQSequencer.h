#ifndef __VSQSequencer_h__
#define __VSQSequencer_h__
/////includes
//
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
using namespace std;

#include <math.h>

#include "straightFrame.h"
#include "straightNote.h"
#include "UTAUManager.h"
#include "VSQEventManager.h"
#include "VSQNoteManager.h"

//�R���g���[���g���b�N�Ƃ��̖���
#define CONTROL_TRACK_NUM 5
#define ST_PI 3.14159216

enum{
	PITCH_BEND=0,
	PITCH_SENS=1,
	DYNAMICS=2,
	BRETHINESS=3,
	GENDOR=4
};

const string sControlNames[CONTROL_TRACK_NUM]=
{"[PitchBendBPList]"
,"[PitchBendSensBPList]"
,"[DynamicsBPList]"
,"[EpRResidualBPList]"
,"[GenderFactorBPList]"
};

const int iControlDefaultValue[CONTROL_TRACK_NUM]=
{0
,2
,64
,0
,64
};

const string sNoteTrackName="[EventList]";

class VSQSequencer{
public:

	/**
	* @brief �R���X�g���N�^�B
	*/
	VSQSequencer();

	/**
	* @brief �f�X�g���N�^�B
	*/
	~VSQSequencer();

	/**
	* @brief �w�肳�ꂽ�t�@�C����ǂݍ��ށB
	* @param dFrameShift �P�t���[���̎����Ԓ�(ms)�B
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool		LoadFile(string sFileName,double dFrameShift);

	/**
	* @brief �w�肳�ꂽ�t���[�������̃m�[�g�C�x���g�ւ̃|�C���^��Ԃ��B
	* @return ���s����NULL�B
	*/
	NoteEvent*	GetNoteEvent(long nFrame);

	/**
	* @brief �w�肳�ꂽ�t���[�������̊�{���g����Ԃ��B
	* @param dFrameShift �P�t���[���̎����Ԓ�(ms)�B
	*/
	double		GetF0(long nFrame,double dFrameShift);

	/**
	* @brief �w�肳�ꂽ�t���[�������̃_�C�i�~�N�X��Ԃ��B
	* @param dFrameShift �P�t���[���̎����Ԓ�(ms)�B
	*/
	double		GetDynamics(long nFrame,double dFrameShift);

	/**
	* @brief �V�[�P���X�̊J�n�t���[��������Ԃ��B
	*/
	long		GetBeginFrame(void){return nBeginFrame;}

	/**
	* @brief �V�[�P���X�̏I���t���[��������Ԃ��B
	*/
	long		GetEndFrame(void){return nEndFrame;}

	/**
	* @brief �w�肳�ꂽ�R���g���[���g���b�N�̎w�肳�ꂽ�t���[�������̒l��Ԃ��B
	* @param dFrameShift �P�t���[���̎����Ԓ�(ms)�B
	*/
	int			GetControlValue(int iControlNum,long nFrame,double dFrameShift);

	/**
	* @brief ���ݐݒ肳��Ă���t�@�C���p�X��Ԃ��B
	*/
	string		GetFilePath(void){return sFilePath;}

protected:
private:

	/**
	* @brief �w�肳�ꂽ�t���[�������ł̃s�b�`�x���h�̊�����Ԃ��B
	*/
	double		GetPitchBendRate(long nFrame,double dFrameShift);

	/**
	* @brief �w�肳�ꂽ�m�[�g�R���̎w�肳�ꂽ�t���[�������ł́w��{���g���x��Ԃ�
	*/
	double		GetPitchFromNote(NoteEvent* pNoteEvent,long nFrame,double dFrameShift);

	/**
	* @brief �w�肳�ꂽ�����ł̃{�����[���ω�����Ԃ��B
	*/
	double		GetDynamicsRate(long nFrame,double dFrameShift);

	/**
	* @brief �w�肳�ꂽ�m�[�g�R���̎w�肳�ꂽ�t���[�������ł̃{�����[���ω�����Ԃ��B
	*/
	double		GetDynamicsFromNote(NoteEvent* pNoteEvent,long nFrame,double dFrameShift);

	/**
	* @brief �w�肳�ꂽ�m�[�g�̎w�莞���ł�Vibrato�ɂ��ω�����Ԃ�
	*/
	double		GetVibratoRate(NoteEvent* pNoteEvent,long nFrame,double dFrameShift);

	UTAUManager UtauManager;					//!< @brief �����ݒ�Ǘ��N���X

	MAP_TYPE<string,EventManager*>	EventMap;	//!< @brief �C�x���g�Ăяo���p�n�b�V��
	list<EventManager*>				ManagerList;//!< @brief �������Ǘ��p���X�g

	long				nBeginFrame;			//!< @brief �J�nFrame
	long				nEndFrame;				//!< @brief �I�[Frame
	long				nEndTick;
	long				nEndNoteID;				//!< @brief �I�[NoteID
	double				dNoteFrequency[129];	//!< @brief �e�m�[�g�̎��g���B128�͖������p�B

	string				sFilePath;
	double				dTempo;
	double				dFrameShift;
};

#endif
