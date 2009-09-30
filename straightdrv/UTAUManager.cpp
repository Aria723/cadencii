//UTAU�����ݒ�Ǘ��p�N���X

#include "UTAUManager.h"

/*    static map<string,UTAUSetting*> SettingMap;	//!< @brief �����ݒ�\
    static list<UTAUSetting*>	SettingList;			//!< @brief �������Ǘ��p���X�g
*/
MAP_TYPE<string,UTAUSetting*> UTAUManager::SettingMap;
std::list<UTAUSetting*> UTAUManager::SettingList;

bool	UTAUManager::Initialize(string sOtoName){
    const int BUF_LEN = 256;
	char	cBuffer[BUF_LEN];
	string	sBuffer;
    FILE *ifs;
#ifdef __GNUC__
    ifs = fopen( sOtoName.c_str(), "r" );
#else
    fopen_s( &ifs, sOtoName.c_str(), "r" );
#endif

    if( NULL == ifs ){
        cout << "error; sOtoName=" << sOtoName << endl;
        cout << "error; io-error on UtauManager::Initialize; bail-out" << endl;
		return false;
    }

	SettingMap.clear();

    while( NULL != fgets( cBuffer, BUF_LEN, ifs ) ){
		sBuffer = cBuffer;
		
		//�I�u�W�F�N�g�Ƀf�[�^��ǉ�
		UTAUSetting* pTarget=new UTAUSetting;

		pTarget->sName=sBuffer.substr(0,sBuffer.find("."));

		sBuffer=sBuffer.substr(sBuffer.find(",")+1,256);
		pTarget->iLeftBlank=atoi(sBuffer.substr(0,sBuffer.find(",")).c_str());
		sBuffer=sBuffer.substr(sBuffer.find(",")+1,256);
		pTarget->iConsonant=atoi(sBuffer.substr(0,sBuffer.find(",")).c_str());
		sBuffer=sBuffer.substr(sBuffer.find(",")+1,256);
		pTarget->iRightblank=atoi(sBuffer.substr(0,sBuffer.find(",")).c_str());
		sBuffer=sBuffer.substr(sBuffer.find(",")+1,256);
		pTarget->iPrepronounce=atoi(sBuffer.substr(0,sBuffer.find(",")).c_str());
		sBuffer=sBuffer.substr(sBuffer.find(",")+1,256);
		pTarget->iOverlapped=atoi(sBuffer.substr(0,sBuffer.find(",")).c_str());
		sBuffer=sBuffer.substr(sBuffer.find(",")+1,256);
		pTarget->dBaseFrequency=atof(sBuffer.c_str());								//���ώ��g���AUTAU�����ݒ���g�����Ă���B

		//�n�b�V���}�b�v�ɓ˂�����
		SettingMap.insert(make_pair(pTarget->sName,pTarget));
		//�Ǘ��p���X�g�ɂ��o�^
        SettingList.push_back(pTarget);

	}

	return true;
}

void	UTAUManager::Uninitialize(void)
{
	for(list<UTAUSetting*>::iterator i=SettingList.begin();i!=SettingList.end();i++)
		delete (*i);
}

UTAUSetting*	UTAUManager::GetDefaultSetting(string sLyric)
{
	MAP_TYPE<string,UTAUSetting*>::iterator i;
	i=SettingMap.find(sLyric);

	//�����ƈꏏ�̏ꍇ�̓L�[�Ɉ�v���Ȃ��̂�NULL��Ԃ��B
	if(i!=SettingMap.end())
		return i->second;
	else
		return NULL;
}
