
#ifndef __winmmhlp_h__
#define __winmmhlp_h__

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <iostream>
#include <iomanip>
#include <string>

namespace Boare{ namespace Cadencii{

class winmmhelp{
private:
    // �Q�[���p�b�h�̌�
    static unsigned int s_num_joydev;
    // �e�Q�[���p�b�h���ڑ�����Ă��邩�ǂ���
    static bool *s_joy_attatched;
    // �e�Q�[���p�b�h���F���ł���{�^���̌�
    static int *s_button_num;
    // ����������Ă��邩�ǂ���
    static bool s_initialized;
    // �ڑ�����Ă���Q�[���p�b�h�̃C���f�N�X�̃��X�g
    static int *s_joy_available;
public:
    // �������B�߂�l�́A�ڑ�����Ă���Q�[���p�b�h�̌�
    static int Init();
    // ��index�Ԗڂ̃Q�[���p�b�h���ڑ�����Ă��邩�ǂ����𒲂ׂ�
    static bool IsJoyAttatched( int index );
    // ��index�Ԗڂ̃Q�[���p�b�h�̏�Ԃ��擾����B
    static bool GetStatus( int index, unsigned char *buttons, int len, int *pov );
    // ��index�Ԗڂ̃Q�[���p�b�h���F���ł���{�^���̌����擾����B
    static int GetNumButtons( int index );
    // ���Z�b�g�B
    static void Reset();
    static int GetNumJoyDev();
};

} }

#endif
