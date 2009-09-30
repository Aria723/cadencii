/*
 * waveplay.h
 * Copyright (c) 2009 kbinani
 *
 * This file is part of Boare.Cadencii.
 *
 * Boare.Cadencii is free software; you can redistribute it and/or
 * modify it under the terms of the BSD License.
 *
 * Boare.Cadencii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __PlaySound_h__
#define __PlaySound_h__

#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#pragma comment(lib, "winmm.lib")

using namespace std;

// �ŏ��̃o�b�t�@���������܂ꂽ�Ƃ��Ăяo�����R�[���o�b�N�֐�
typedef void (*FirstBufferWrittenCallback)();

class waveplay{
private:
    static const int _NUM_BUF = 3;              // �o�b�t�@�̐�
    static int          s_block_size;           // 1�̃o�b�t�@�̃T�C�Y(�T���v��)
    static int          s_sample_rate;          // �T���v�����O���[�g
    static WAVEFORMATEX s_wave_formatx;         // WAVE�t�@�C���w�b�_
    static HWAVEOUT     s_hwave_out;            // WAVE�Đ��f�o�C�X
    static WAVEHDR      s_wave_header[_NUM_BUF];// WAVE�w�b�_
    static unsigned long* s_wave[_NUM_BUF];     // �o�b�t�@
    static bool         s_done[_NUM_BUF];
    static int          s_current_buffer;       // ���񏑂����ނׂ��o�b�t�@�̃C���f�N�X
    static unsigned int s_processed_count;      // ����̓o�b�t�@��_NUM_BUF�S�����߂Ȃ���΂����Ȃ��̂ŁA�ŏ��� _NUM_BUF + 1 ��̓J�E���g���s���B���̂��߂̃J�E���^
    static bool         s_abort_required;       // �Đ��̒��f���v�����ꂽ�����t���O
    static int          s_buffer_loc;           // �������ݒ��̃o�b�t�@���́A���݈ʒu
    static bool         s_playing;              // �Đ������ǂ�����\���t���O
    static int          s_error_samples;        // append���ꂽ�g�`�f�[�^�̓��A�擪��s_error_samples�����Ȃ��B�ʏ�̎g�����Ȃ���0�����Avocaloid2 vsti�Ŏg���ꍇ�A�v���Z���h����������waveOutWrite���Ȃ���΂����Ȃ��̂Ŕ�0�ɂȂ�B
    static int          s_last_buffer;          // �Ō�ɍĐ������o�b�t�@�̔ԍ��B���l�̏ꍇ�Aappend_last�������Ă΂�Ă��Ȃ����Ƃ��Ӗ�����B
    static double *s_wave_buffer_l;
    static double *s_wave_buffer_r;
    static FirstBufferWrittenCallback s_first_buffer_written_callback; // �ŏ��̃o�b�t�@���������܂ꂽ�Ƃ��Ăяo�����R�[���o�b�N�֐�

    /// �R�[���o�b�N�֐�
    static void CALLBACK wave_callback( HWAVEOUT hwo, unsigned int uMsg, unsigned long dwInstance, unsigned long dwParam1, unsigned long dwParam2 );
    static void append_cor( double** a_data, unsigned int length, double amp_left, double amp_right, bool is_last_mode );
    static void mix( int processed_count, float amp_left, float amp_right );
#ifdef __cplusplus_cli
    static System::String ^waveplay::util_get_errmsg( MMRESULT msg );
#else
    static string waveplay::util_get_errmsg( MMRESULT msg );
#endif
public:
#ifdef TEST
    static ofstream logger;
#endif
    static void on_your_mark();
    /// �������֐�
    static void init( int block_size, int sample_rate );
    /// �g�`�f�[�^���o�b�t�@�ɒǉ�����B�o�b�t�@���Đ����Ȃǂ̗��R�ő����ɏ������߂Ȃ��ꍇ�A�o�b�t�@���������݉\�ƂȂ�܂őҋ@��������
	static void append( double** data, unsigned int length, double amp_left, double amp_right );
    static void flush_and_exit( double amp_left, double amp_right );
    /// �Đ����f��v������
    static void abort();
    /// ���݂̍Đ��ʒu���擾����B�Đ����łȂ��ꍇ���̒l�ƂȂ�B
    static double get_play_time();
    /// ���Z�b�g����Babort�֐��ł��Ăяo�����B
    static void reset();
    /// �R�[���o�b�N�֐���ݒ肷��
    static void set_first_buffer_written_callback( FirstBufferWrittenCallback proc );
    static void terminate();
    /// ���ݍĐ������ǂ������擾����
    static bool is_alive();
    /// �u���b�N�T�C�Y��ύX���܂�
    static bool change_block_size( int block_size );
};

extern "C" {

    void SoundInit( int block_size, int sample_rate );
    void SoundAppend( double *left, double *right, int length );
    void SoundReset();
    double SoundGetPosition();
    bool SoundIsBusy();
    void SoundWaitForExit();
    void SoundSetResolution( int resolution );

}

#endif // __PlaySound_h__
