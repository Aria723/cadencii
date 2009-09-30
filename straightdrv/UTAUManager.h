//UTAU�����ݒ�Ǘ��p�N���X

#ifndef __UTAUManager_h__
#define __UTAUManager_h__

#include "straightdrv.h"
//
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

/**
* UTAU�����ݒ�i�[�p�\���́B
*/
struct UTAUSetting
{
	string	sName;			//!< @brief ������
	int		iLeftBlank;		//!< @brief ���u�����N(ms)
	int		iConsonant;		//!< @brief �q���Œ蒷(ms)
	int		iRightblank;	//!< @brief �E�u�����N(ms)
	int		iPrepronounce;	//!< @brief ��s������(ms)
	int		iOverlapped;	//!< @brief �I�[�o�[���b�v(ms)
	double	dBaseFrequency;	//!< @brief ��{���g��(Hz)
};



/**
* UTAU�̌����ݒ���Ǘ�����N���X�B
*/
class UTAUManager
{
public:


	/**
	* @brief UTAUManager������������B
	* @param sOtoName �����ݒ�t�@�C�������i�[����string�^�B
	* @return ����������true�A���s������false�B
	*/
	bool	Initialize(string sOtoName);

	/**
	* @brief UTAUManager���J������B
	*/
	void	Uninitialize(void);

	/**
	* @brief �w�肳�ꂽ�̎��̌����ݒ���擾����B
	* @param sLyric �̎����i�[����string�^�B
	* @return ����������ݒ�ւ̃|�C���^�A���s������NULL�B
	*/
	UTAUSetting*	GetDefaultSetting(string sLyric);

protected:
private:
    static list<UTAUSetting*> SettingList;		//!< @brief �������Ǘ��p���X�g
    static MAP_TYPE<string,UTAUSetting*> SettingMap;	//!< @brief �����ݒ�hyou(`hyou' wo kanji de kakuto g++ de compile dekinai yo!! :( )
};

#endif
