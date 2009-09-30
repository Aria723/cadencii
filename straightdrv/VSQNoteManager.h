#ifndef __VSQNoteManager_h__
#define __VSQNoteManager_h__

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

#include "UTAUManager.h"
#include "VSQEventManager.h"

#define A4_PITCH	440.0
#define A4_NOTE		57
//Tick per Second 960/60�@or 480/60
#define TICK_PER_SEC 8.0


struct	NoteEvent
{
	string		sLyric;				//!< @brief ����
	unsigned	char ucNote;		//!< @brief ����
	long		nTick;				//!< @brief �ʒu(Tick�w��)
	long		nLength;			//!< @brief ����(Tick�w��)
	long		nBeginFrame;
	long		nEndFrame;
	unsigned	char ucVelocity;	//!< @brief Consonant�̈�̒����̔�
	unsigned	char ucDecay;		//!< @brief Consonant�̈�ȍ~�̌����
	unsigned	char ucAccent;		//!< @brief Consonant�̈�̉��̋���
	long		nVibratoDelay;		//!< @brief Vibrato�̊J�n�ʒu

	bool		bIsContinuousFront;	//!< @brief �O�ɉ��������邩�ǂ���
	bool		bIsContinuousBack;	//!< @brief ���ɉ��������邩�ǂ���

	UTAUSetting	Setting;			//!< @brief UTAU�̐ݒ���i�[����\����

	NoteEvent*	pPreviousNote;		//!< @brief �A������ꍇ�O�̉����f�[�^���i�[
	NoteEvent*	pNextNote;			//!< @brief �A������ꍇ���̉����f�[�^���i�[
	int			iID;

	EventManager emVibratoDepth;	//!< @brief Vibrato�̐[�����L�^����
	EventManager emVibratoRate;		//!< @brief Vibrato�̑������L�^����
};


class	NoteManager		:	public	EventManager
{
public:

	/**
	* @brief �R���X�g���N�^�B�Ǘ��p�ϐ��̏�����������B
	*/
	NoteManager(){pEditTarget=NULL;iCurrentID=-1;}

	/**
	* @brief �f�X�g���N�^�B�m�ۂ������\�[�X���J������B
	*/
	~NoteManager();

	/**
	* @brief �w�肳�ꂽID��ҏW�p�Ɏw�肷��B
	* @param iID ID�ԍ�
	*/
	void	SetEditNoteID(int iID){bHandle=false;iCurrentID=iID;pEditTarget=GetNoteEvent(iID);}

	/**
	* @brief �w�肳�ꂽHandle��ҏW�p�Ɏw�肷��B
	* @param iID Handle�ԍ�
	*/
	void	SetHandleID(int iID){bHandle=true;iCurrentID=iID;}

	/**
	* @brief �w�肳�ꂽID�ɑΉ�����m�[�g�C�x���g�ւ̃|�C���^��T���B
	* @param iID ID�ԍ��B
	* @return ����������|�C���^�A���s������NULL��Ԃ��B
	*/
	NoteEvent*	GetNoteEvent(int iID);

	/**
	* @brief �w�肳�ꂽTick�����̃m�[�g�C�x���g�ւ̃|�C���^��T���B
	* @param nTick Tick�����B
	* @return ����������|�C���^�A���s������NULL��Ԃ��B
	*/
	NoteEvent*	GetNoteEvent(long nTick);

	/**
	* @brief �w�肳�ꂽ�t���[�������̃m�[�g�C�x���g�ւ̃|�C���^��T���B
	* @param nFrame �t���[�������B
	* @return ���s���ɂ�NULL�B
	*/
	NoteEvent*	GetNoteEventByFrame(long nFrame);

	/**
	* @brief �m�[�g�Ԃ̘A�������`�F�b�N����B
	* @param dTempo �Ȃ̃e���|
	*/
	bool	CheckContinuity(double dTempo,UTAUManager* pUtauManager,double dFrameShift);

	/**
	* @brief �����̉�����EventID��Ԃ��B
	*/
	long	GetEndID(void);

	/**
	* @brief �V�[�P���X�f�[�^�̊J�n�t���[��������Ԃ��B
	*/
	long	GetBeginFrame(void);
	/**

	* @brief �V�[�P���X�f�[�^�̏I���t���[��������Ԃ��B
	*/
	long	GetEndFrame(void);


protected:

	/**
	* @brief �w�肳�ꂽ�����񂩂�l��o�^����B
	* @param sFront �O������ sBack �㕶����
	*/
	bool	SetValue(string sFront,string sBack);

private:

	/**
	* @brief �w�肳�ꂽTick�����Ƀm�[�g�C�x���g��o�^����B
	* @param iID ID�ԍ� nTick Tick�����B
	*/
	void	RegisterNote(long nTick,int iID);

	/**
	* @brief �w�肳�ꂽID�̃m�[�g�C�x���g���폜����B
	* @param iTargetID �폜����ID�ԍ��B
	*/
	void	UnregisterNoteByID(int iTargetID);

	/**
	* @brief �w�肳�ꂽID�ɑΉ�����m�[�g�C�x���g�̎��̂�o�^����B
	* @param iID ID�ԍ� pNoteEvent ���̂ւ̃|�C���^�B
	*/
	void	SetNoteEvent(int iID,NoteEvent* pNoteEvent);

	/**
	* @brief �w�肳�ꂽ�����񂩂�m�[�g�̃v���p�e�B��o�^����B
	* @param sFront �O������ sBack �㕶����
	*/
	bool	SetNoteProperty(string sFront,string sBack);

	/**
	* @brief �n���h���ɐݒ肳�ꂽ�f�[�^��ǂݍ��ށB
	* @param sBuffer �ǂݍ��܂ꂽ������
	*/
	bool	SetHandleProperty(string sFront,string sBack);

	list<NoteEvent*>		NoteList;			//!< @brief �m�[�g���B
	MAP_TYPE<int,NoteEvent*> NoteMap;			//!< @brief �m�[�g��ID�ƑΉ��������n�b�V���B
	MAP_TYPE<int,NoteEvent*> HandleToNoteMap;	//!< @brief Handle�ƃm�[�g��Ή��������n�b�V���B

	int	iCurrentID;						//!< @brief ���ݕҏW���̃m�[�gID�ێ��p�ϐ��B
	NoteEvent*	pEditTarget;			//!< @brief ���ݕҏW���̃m�[�g�C�x���g�ւ̃|�C���^�B

	bool				bHandle;		//!< @brief Handle��o�^�����ۂ�

};

#endif
