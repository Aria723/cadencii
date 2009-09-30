#include "straightdrv.h"

#include "straightNote.h"

class straightNoteManager{
public:

	/**
	* @brief �f�X�g���N�^�B�J���������s���B
	*/
	~straightNoteManager();

	/**
	* @brief �������֐��B
	*/
	void	Initialize(string sFilePath,Straight* pStraight){this->sFilePath=sFilePath;this->pStraight=pStraight;}

	/**
	* @brief �����̓���t���[���������ǂݏo���B
	* @return �����Ftrue�@���s�Ffalse
	*/
	bool	GetStraightFrame(straightFrame* pDstFrame,string sLyric,long nAbsoluteFrame);

	/**
	* @brief �̎��f�[�^���猴���̃t���[������ǂݏo���B
	* @return �����̃t���[����
	*/
	long	GetSrcFrameLength(string sLyric);

	/**
	* @brief �^����ꂽ�̎��f�[�^�ɊY������f�[�^���J������B
	*/
	void	ReleaseSTF(string sLyric);

protected:
private:

	MAP_TYPE<string,straightNote*> NoteMap;	//!< @brief �ǂݍ���STF�t�@�C���Ǘ��p�n�b�V���}�b�v
	list<straightNote*> NoteList;			//!< @brief �������Ǘ��p

	string sFilePath;						//!< @brief �����Z�b�g�ւ̃t�@�C���p�X
	Straight* pStraight;					//!< @brief STRAIGHT�p
};
