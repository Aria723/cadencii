/*
 * Copyright (C) 2003-2005 Kenji Aiko, 
 * based on http://ruffnex.oc.to/kenji/text/load_dll/ (browse: 13 Feb, 2010)
 */
#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <winnt.h>
#include <stdio.h>
#include <vcclr.h>
#include <stdlib.h>

// DllMain���̃A�^�b�`�A�f�^�b�`����
#define DLL_ATTACH    0
#define DLL_DETACH    1

#define SIZE_OF_NT_SIGNATURE       (sizeof(DWORD))
#define SIZE_OF_PARAMETER_BLOCK    4096
#define IMAGE_PARAMETER_MAGIC      0xCDC31337

#define RVATOVA(base, offset) ( \
    (LPVOID)((DWORD)(base) + (DWORD)(offset)))

// NT�V�O�l�`��
#define NTSIGNATURE(ptr) (  \
    (LPVOID)((PBYTE)(ptr) + \
    ((PIMAGE_DOS_HEADER)(ptr))->e_lfanew))

// PE�w�b�_�I�t�Z�b�g
#define PEFHDROFFSET(ptr) ( \
    (LPVOID)((PBYTE)(ptr) + \
    ((PIMAGE_DOS_HEADER)(ptr))->e_lfanew + \
    SIZE_OF_NT_SIGNATURE))

// �I�v�V�����w�b�_�I�t�Z�b�g
#define OPTHDROFFSET(ptr) ( \
    (LPVOID)((PBYTE)(ptr) + \
    ((PIMAGE_DOS_HEADER)(ptr))->e_lfanew + \
    SIZE_OF_NT_SIGNATURE +  \
    sizeof(IMAGE_FILE_HEADER)))

// �Z�N�V�����w�b�_�I�t�Z�b�g
#define SECHDROFFSET(ptr) ( \
    (LPVOID)((PBYTE)(ptr) + \
    ((PIMAGE_DOS_HEADER)(ptr))->e_lfanew + \
    SIZE_OF_NT_SIGNATURE +  \
    sizeof(IMAGE_FILE_HEADER) + \
    sizeof(IMAGE_OPTIONAL_HEADER)))

// �\���̂̋��E��1�o�C�g�ݒ�
#pragma pack(push, 1)

typedef struct{
    DWORD dwPageRVA;
    DWORD dwBlockSize;
} IMAGE_FIXUP_BLOCK, *PIMAGE_FIXUP_BLOCK;

typedef struct{
    WORD offset:12;
    WORD type:4;
} IMAGE_FIXUP_ENTRY, *PIMAGE_FIXUP_ENTRY;

// DLL�C���[�W�f�[�^�̍\����
typedef struct __imageparameters{
    PVOID pImageBase;
    TCHAR svName[MAX_PATH];
    DWORD dwFlags;
    int nLockCount;
    struct __imageparameters *next;
} IMAGE_PARAMETERS, *PIMAGE_PARAMETERS;

#pragma pack(pop)

// DllMain�̃|�C���^�֐�
typedef BOOL (WINAPI *DLLMAIN_T)(HMODULE, DWORD, LPVOID);

typedef struct{
    union{
        DWORD Function;
        DWORD Ordinal;
        DWORD AddressOfData;
    } u1;
} MY_IMAGE_THUNK_DATA, *PMY_IMAGE_THUNK_DATA;

// DLL�f�[�^�x�[�X�̃g�b�v
PIMAGE_PARAMETERS g_pImageParamHead;
// �N���e�B�J���Z�N�V�����ϐ�
CRITICAL_SECTION g_DLLCrit;
bool g_initialized = false;

using namespace System;
using namespace System::Runtime::InteropServices;

namespace org{ namespace kbinani{ namespace cadencii{ namespace util {

	public ref class DllLoad{
	public:
        // -------------------------------------------------------------
        // ����������
        // -------------------------------------------------------------
        static void initialize();

		static bool isInitialized();

		// -------------------------------------------------------------
        // �I������
        // -------------------------------------------------------------
        static void terminate();

        // -------------------------------------------------------------
        // DLL���ɂ���G�N�X�|�[�g�֐�����������
        // �����@�FDLL�n���h���A�֐���
        // �߂�l�F�����Ȃ�֐��A�h���X�A���s�Ȃ�NULL
        // -------------------------------------------------------------
        static IntPtr getProcAddress( IntPtr hModule, String ^lpProcName );

        // -------------------------------------------------------------
        // DLL�����[�h����֐�
        // �����@�FDLL�t�@�C�����A�\���iNULL�Œ�j�A�t���O
        // �߂�l�F����DLL�n���h���A���sNULL
        // -------------------------------------------------------------
        static IntPtr loadDllEx( String^ lpLibFileName,
                                 IntPtr hReserved,
                                 DWORD dwFlags );

        // -------------------------------------------------------------
        // DLL�����[�h����֐��iLoadDLLEx�ւ̋��n���j
        // �����@�FDLL�t�@�C����
        // �߂�l�F����DLL�n���h���A���sNULL
        // -------------------------------------------------------------
        static IntPtr loadDll( String^ lpLibFileName );

        // -------------------------------------------------------------
        // DLL���J������֐�
        // �����@�FDLL�n���h��
        // �߂�l�F����TRUE�A���sFALSE
        // -------------------------------------------------------------
        static BOOL freeDll( IntPtr hLibModule );

	private:
		static void lptstrFromString( String ^from, LPTSTR to );

		// -------------------------------------------------------------
        // �f�[�^�x�[�X�ɐV����DLL��ǉ�
        // �����@�FDLL�n���h���ADLL���i���ʎq�j
        // �߂�l�Ferror -1, success(find 0, make 1)
        // -------------------------------------------------------------
        static int AddDllReference( PVOID pImageBase, 
                             PTCHAR szName,
                             DWORD dwFlags );

        // -------------------------------------------------------------
        // �f�[�^�x�[�X����DLL���폜
        // �����@�FDLL�n���h���ADLL���i���ʎq�j
        // �߂�l�Ferror -1, success(keep 0, delete 1)
        // -------------------------------------------------------------
        static int RemoveDllReference( PVOID pImageBase, 
                                PTCHAR svName,
                                PDWORD pdwFlags );

        // -------------------------------------------------------------
        // �p�����[�^�e�[�u������DLL���������Ă��̃n���h����Ԃ�
        // �����@�FDLL�t�@�C����
        // �߂�l�F������΂���DLL�̃n���h���A������Ȃ����NULL
        // -------------------------------------------------------------
        static IntPtr^ GetDllHandle( PTCHAR svName );

        // -------------------------------------------------------------
        // �p�����[�^�e�[�u������DLL���������Ă��̃t�@�C������Ԃ�
        // �����@�FDLL�n���h���A�i�[��|�C���^�A�i�[�̈�̃T�C�Y
        // �߂�l�F������΃t�@�C�����̃T�C�Y�A������Ȃ����0
        // -------------------------------------------------------------
        static DWORD GetDllFileName( HMODULE hModule, 
                              LPTSTR lpFileName, 
                              DWORD dwSize );

		// -------------------------------------------------------------
        // DLL��DLLMain�֐��𑖂点��֐�
        // �����@�FDLL�n���h���ADLL�T�C�Y�AAttach or Detach�̃t���O
        // �߂�l�Ferror -1, success(keep 0, delete 1)
        // -------------------------------------------------------------
        static BOOL RunDllMain( PVOID pImageBase, 
                                DWORD dwImageSize, 
                                BOOL bDetach );

        // -------------------------------------------------------------
        // �C���|�[�g�֐��̃A�h���X�����֐�
        // �����@�FDLL�t�@�C���C���[�W�ADLL�t�@�C���C���[�W�̃T�C�Y
        // �߂�l�F����TRUE�A���sFALSE
        // -------------------------------------------------------------
        static BOOL PrepareDllImage( PVOID pMemoryImage, 
                              DWORD dwImageSize );

		// -------------------------------------------------------------
        // DLL�C���[�W���v���e�N�g����
        // �����@�FDLL�t�@�C���C���[�W
        // �߂�l�F����TRUE�A���sFALSE
        // -------------------------------------------------------------
        static BOOL ProtectDllImage( PVOID pMemoryImage );

        // -------------------------------------------------------------
        // DLL�C���[�W���R�s�[����֐�
        // �����@�FDLL�t�@�C���C���[�W�A�R�s�[��|�C���^
        // �߂�l�F����TRUE�A���sFALSE
        // -------------------------------------------------------------
        static BOOL MapDllFromImage( PVOID pDLLFileImage, 
                              PVOID pMemoryImage );

		// -------------------------------------------------------------
        // DLL�C���[�W����DLL�����[�h����֐�
        // �����@�FDLL�t�@�C���C���[�W�A�}�b�s���O���i���ʎq�j�A�t���O
        // �߂�l�F����DLL�n���h���A���sNULL
        // -------------------------------------------------------------
        static HMODULE LoadDllFromImage( LPVOID pDLLFileImage, 
                                  PTCHAR szMappingName,
                                  DWORD dwFlags );
	};

} } } }
