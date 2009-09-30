#ifndef __straightSynthesizer_h__
#define __straightSynthesizer_h__

/////includes
//
#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __GNUC__
#include <crtdbg.h>
#endif

#include <straight/straight.h>

//
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

//
#include <math.h>

#include "straightFrame.h"
#include "straightNote.h"
#include "straightNoteMAnager.h"
#include "UTAUManager.h"
#include "VSQEventManager.h"
#include "VSQNoteManager.h"
#include "VSQSequencer.h"

class straightSynthesizer{
public:
	/**
	* @brief �R���X�g���N�^�BSTRAIGHT�p�̕ϐ����������B
	*/
	straightSynthesizer(){straight=NULL;source=NULL;specgram=NULL;synth=NULL;}

	/**
	* @brief �f�X�g���N�^�B�J���������s���B
	*/
	~straightSynthesizer();

	/**
	* @brief �������֐��B
	* @param sFileName �ǂݍ��ރt�@�C����
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool Initialize(string sFileName);

	/**
	* @brief �����p�֐�
	* @param sFileName �����o���t�@�C����
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool Synthesize(string sFileName);

	/**
	* @brief �J���p�֐��B
	*/
	void Uninitialize();
protected:
private:

	/**
	* @brief �m�[�g���ƌ��݃t���[��������A��������ł̈ʒu���v�Z����B
	* @param pNoteEvent �m�[�g���ւ̃|�C���^ nFrame ���݂̈ʒu nSrcFrameLength �������̃t���[����
	* @return ��������ł̃t���[���ʒu�B
	*/
	long CalculateAbsoluteFrame(NoteEvent* pNoteEvent,long nFrame,long nSrcFrameLength);

	/**
	* @brief �N���b�s���O���Ȃ��悤�Ƀm�[�}���C�Y�������s���B
	*/
	void Normalize(void);

	VSQSequencer		Sequencer;					//!< @brief VSQ�R���̏��Ǘ�
	string				sFilePath;					//!< @brief �����Z�b�g�ւ̃t�@�C���p�X

	Straight			straight;					//!< @brief STRAIGHT�p
	StraightSource		source;						//!< @brief STRAIGHT�p
	StraightSpecgram	specgram;					//!< @brief STRAIGHT�p
	StraightSynth		synth;						//!< @brief STRAIGHT�p
	StraightConfig		config;						//!< @brief STRAIGHT�p
};

static stBool callbackFunc(Straight straight, stCallbackType callbackType,
                           void *callbackData, void *userData)
{
    int percent;
    
    if (callbackType == STRAIGHT_F0_PERCENTAGE_CALLBACK) {
        percent = (int)callbackData;
        fprintf(stderr, "STRAIGHT F0: %d %%\r", percent);
        if (percent >= 100) {
            fprintf(stderr, "\n");
        }
    } else if (callbackType == STRAIGHT_AP_PERCENTAGE_CALLBACK) {
        percent = (int)callbackData;
        fprintf(stderr, "STRAIGHT AP: %d %%\r", percent);
        if (percent >= 100) {
            fprintf(stderr, "\n");
        }
    } else if (callbackType == STRAIGHT_SPECGRAM_PERCENTAGE_CALLBACK) {
        percent = (int)callbackData;
        fprintf(stderr, "STRAIGHT spectrogram: %d %%\r", percent);
        if (percent >= 100) {
            fprintf(stderr, "\n");
        }
    } else if (callbackType == STRAIGHT_SYNTH_PERCENTAGE_CALLBACK) {
        percent = (int)callbackData;
        fprintf(stderr, "STRAIGHT synthesis: %d %%\r", percent);
        if (percent >= 100) {
            fprintf(stderr, "\n");
        }
    }

    return ST_TRUE;
}

#endif
