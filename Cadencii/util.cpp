
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <winnt.h>

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

// DLL�f�[�^�x�[�X�̃g�b�v
PIMAGE_PARAMETERS g_pImageParamHead;
// �N���e�B�J���Z�N�V�����ϐ�
CRITICAL_SECTION g_DLLCrit;
bool g_initialized = false;

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------
// ����������
// -------------------------------------------------------------
void InitializeDllLoad(void)
{
	if( g_initialized ){
		return;
	}
	InitializeCriticalSection(&g_DLLCrit);
	g_pImageParamHead = NULL;
	g_initialized = true;
}

bool IsInitialized(){
	return g_initialized;
}

// -------------------------------------------------------------
// �I������
// -------------------------------------------------------------
void KillDllLoad(void)
{
	if( !g_initialized ){
		return;
	}
	PIMAGE_PARAMETERS cur = g_pImageParamHead;

	while(cur != NULL){
		PIMAGE_PARAMETERS next = cur->next;
		delete [] cur;
		cur = next;
	}

	DeleteCriticalSection(&g_DLLCrit);
}


// -------------------------------------------------------------
// �f�[�^�x�[�X�ɐV����DLL��ǉ�
// �����@�FDLL�n���h���ADLL���i���ʎq�j
// �߂�l�Ferror -1, success(find 0, make 1)
// -------------------------------------------------------------
int AddDllReference(PVOID pImageBase, 
					PTCHAR szName,
					DWORD dwFlags)
{
	// szName���Ȃ���΃G���[
	if(szName == NULL)
		return -1;

	EnterCriticalSection(&g_DLLCrit);

	PIMAGE_PARAMETERS cur = g_pImageParamHead;
	
	// DLL������
	while(cur != NULL){
		if(cur->pImageBase != pImageBase)
			cur = cur->next;
		else{
			cur->nLockCount++;
			LeaveCriticalSection(&g_DLLCrit);
			return 0;
		}
	}
	 
	// �V����DLL�̐���
	if((cur = (PIMAGE_PARAMETERS)new IMAGE_PARAMETERS[1]) == NULL){
		LeaveCriticalSection(&g_DLLCrit);
		return -1;
	}
	cur->pImageBase = pImageBase;
	cur->nLockCount = 1;
	cur->dwFlags    = dwFlags;
	cur->next       = g_pImageParamHead;
	lstrcpyn(cur->svName, szName, MAX_PATH);

	g_pImageParamHead = cur;

	LeaveCriticalSection(&g_DLLCrit);
	return 1;
}


// -------------------------------------------------------------
// �f�[�^�x�[�X����DLL���폜
// �����@�FDLL�n���h���ADLL���i���ʎq�j
// �߂�l�Ferror -1, success(keep 0, delete 1)
// -------------------------------------------------------------
int RemoveDllReference(PVOID pImageBase, 
					   PTCHAR svName,
					   PDWORD pdwFlags)
{
	EnterCriticalSection(&g_DLLCrit);

	PIMAGE_PARAMETERS prev, cur = g_pImageParamHead;

	// DLL������
	while(cur != NULL){
		if(cur->pImageBase == pImageBase)
			break;
		prev = cur;
		cur = cur->next;
	}

	// �����ł��Ȃ�������G���[
	if(cur == NULL){
		LeaveCriticalSection(&g_DLLCrit);
		return -1;
	}
	
	cur->nLockCount--;
	*pdwFlags = cur->dwFlags;
	lstrcpyn(svName, cur->svName, MAX_PATH);

	// �J�E���^���܂�0����Ȃ��Ȃ�I��
	if(cur->nLockCount != 0){
		LeaveCriticalSection(&g_DLLCrit);
		return 0;
	}

	// �A�����X�V
	if(prev == NULL)
		g_pImageParamHead = g_pImageParamHead->next;
	else
		prev->next = cur->next;

	delete [] cur;
	LeaveCriticalSection(&g_DLLCrit);
	return 1;
}


// -------------------------------------------------------------
// �p�����[�^�e�[�u������DLL���������Ă��̃n���h����Ԃ�
// �����@�FDLL�t�@�C����
// �߂�l�F������΂���DLL�̃n���h���A������Ȃ����NULL
// -------------------------------------------------------------
HMODULE GetDllHandle(PTCHAR svName)
{
	if(svName == NULL)
		return NULL;

	EnterCriticalSection(&g_DLLCrit);

	// �p���[���[�^�e�[�u���̃g�b�v���擾
	PIMAGE_PARAMETERS cur = g_pImageParamHead;
	
	// DLL������
	while(cur != NULL){
		if(lstrcmpi(cur->svName, svName) != 0){
			cur = cur->next;
		}else{
			// ����������n���h����Ԃ�
			LeaveCriticalSection(&g_DLLCrit);
			return (HMODULE)cur->pImageBase;
		}
	}

	// ������Ȃ���ΏI��
	LeaveCriticalSection(&g_DLLCrit);
	return NULL;	
}


// -------------------------------------------------------------
// �p�����[�^�e�[�u������DLL���������Ă��̃t�@�C������Ԃ�
// �����@�FDLL�n���h���A�i�[��|�C���^�A�i�[�̈�̃T�C�Y
// �߂�l�F������΃t�@�C�����̃T�C�Y�A������Ȃ����0
// -------------------------------------------------------------
DWORD GetDllFileName(HMODULE hModule, 
					 LPTSTR lpFileName, 
					 DWORD dwSize)
{
	if(hModule == NULL || lpFileName == NULL || dwSize == 0){
		return 0;
    }
	
	// �܂��͒ʏ��GetModuleFileName�Œ��ׂ�
	DWORD dwRet = GetModuleFileName(hModule, lpFileName, dwSize);
	if(dwRet != 0){
		return dwRet;
    }

	EnterCriticalSection(&g_DLLCrit);

	PIMAGE_PARAMETERS cur = g_pImageParamHead;
	
	// DLL������
	while(cur != NULL){
		if(cur->pImageBase != hModule){
			cur=cur->next;
		}else{
			// ���������當����ƃT�C�Y��Ԃ�
			LeaveCriticalSection(&g_DLLCrit);
			lstrcpyn(lpFileName, cur->svName, dwSize);
			return lstrlen(lpFileName);
		}
	} 

	LeaveCriticalSection(&g_DLLCrit);
	return 0;	
}
					

// -------------------------------------------------------------
// DLL���ɂ���G�N�X�|�[�g�֐�����������
// �����@�FDLL�n���h���A�֐���
// �߂�l�F�����Ȃ�֐��A�h���X�A���s�Ȃ�NULL
// -------------------------------------------------------------
FARPROC GetDllProcAddress(HMODULE hModule, 
						  LPCSTR lpProcName)
{
	// hModule��NULL�Ȃ�΃G���[
	if(hModule == NULL){
		return NULL;
    }
	
	// �f�B���N�g���J�E���g�擾
	PIMAGE_OPTIONAL_HEADER poh = 
		(PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(hModule);
	int nDirCount = poh->NumberOfRvaAndSizes;
	if(nDirCount < 16){
		return FALSE;
    }

	// �G�N�X�|�[�g�f�B���N�g���e�[�u���擾
	DWORD dwIDEE = IMAGE_DIRECTORY_ENTRY_EXPORT;
	if(poh->DataDirectory[dwIDEE].Size == 0)
		return NULL;
	DWORD dwAddr = poh->DataDirectory[dwIDEE].VirtualAddress;
	PIMAGE_EXPORT_DIRECTORY ped = 
		(PIMAGE_EXPORT_DIRECTORY)RVATOVA(hModule, dwAddr);	
	
	// �����擾
	int nOrdinal = (LOWORD(lpProcName)) - ped->Base;
	
	if(HIWORD(lpProcName) != 0){
		int count = ped->NumberOfNames;
		// ���O�Ə������擾
		DWORD *pdwNamePtr = (PDWORD)
			RVATOVA(hModule, ped->AddressOfNames);
		WORD *pwOrdinalPtr = (PWORD)
			RVATOVA(hModule, ped->AddressOfNameOrdinals);
		// �֐�����
		int i;
		for(i=0; i < count; i++, pdwNamePtr++, pwOrdinalPtr++){
			PTCHAR svName = (PTCHAR)RVATOVA(hModule, *pdwNamePtr);
			if(lstrcmp(svName, lpProcName) == 0){
				nOrdinal = *pwOrdinalPtr;
				break;
			}
		}
		// ������Ȃ����NULL��ԋp
		if(i == count)
			return NULL;
	}
	
	// ���������֐���Ԃ�
	PDWORD pAddrTable = (PDWORD)
		RVATOVA(hModule, ped->AddressOfFunctions);
	return (FARPROC)RVATOVA(hModule, pAddrTable[nOrdinal]);
}


// -------------------------------------------------------------
// DLL��DLLMain�֐��𑖂点��֐�
// �����@�FDLL�n���h���ADLL�T�C�Y�AAttach or Detach�̃t���O
// �߂�l�Ferror -1, success(keep 0, delete 1)
// -------------------------------------------------------------
static BOOL RunDllMain(PVOID pImageBase, 
					   DWORD dwImageSize, 
					   BOOL bDetach)
{
	// �t���O�̌���
	PIMAGE_FILE_HEADER pfh = (PIMAGE_FILE_HEADER)
		PEFHDROFFSET(pImageBase);
	if((pfh->Characteristics & IMAGE_FILE_DLL) == 0)
		return TRUE;

	// DLLMain�֐��̃A�h���X�擾
	PIMAGE_OPTIONAL_HEADER poh = (PIMAGE_OPTIONAL_HEADER)
		OPTHDROFFSET(pImageBase);
	DLLMAIN_T pMain = (DLLMAIN_T)
		RVATOVA(pImageBase, poh->AddressOfEntryPoint);

	// �f�^�b�`��or�A�^�b�`��
	if(bDetach)
		return pMain((HMODULE)pImageBase, DLL_PROCESS_DETACH, NULL);
	else
		return pMain((HMODULE)pImageBase, DLL_PROCESS_ATTACH, NULL);
}


// -------------------------------------------------------------
// �C���|�[�g�֐��̃A�h���X�����֐�
// �����@�FDLL�t�@�C���C���[�W�ADLL�t�@�C���C���[�W�̃T�C�Y
// �߂�l�F����TRUE�A���sFALSE
// -------------------------------------------------------------
BOOL PrepareDllImage(PVOID pMemoryImage, 
					 DWORD dwImageSize)
{
	PIMAGE_OPTIONAL_HEADER poh = 
		(PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(pMemoryImage);
	int nDirCount = poh->NumberOfRvaAndSizes;
	if(nDirCount < 16)
		return FALSE;

	PIMAGE_SECTION_HEADER psh = 
		(PIMAGE_SECTION_HEADER)SECHDROFFSET(pMemoryImage);

	DWORD dwIDEI = IMAGE_DIRECTORY_ENTRY_IMPORT;

	if(poh->DataDirectory[dwIDEI].Size != 0){
		PIMAGE_IMPORT_DESCRIPTOR pid = 
			(PIMAGE_IMPORT_DESCRIPTOR)RVATOVA(pMemoryImage, 
			poh->DataDirectory[dwIDEI].VirtualAddress);

		for(; pid->OriginalFirstThunk != 0; pid++){
			PTCHAR svDllName = (PTCHAR)
				RVATOVA(pMemoryImage, pid->Name);
			HMODULE hDll = GetModuleHandle(svDllName);
			if(hDll == NULL){
				if((hDll = LoadLibrary(svDllName)) == NULL)
					return FALSE;
			}

			if(pid->TimeDateStamp != 0)
				continue;
			
			pid->ForwarderChain = (DWORD)hDll;
			pid->TimeDateStamp  = IMAGE_PARAMETER_MAGIC;

			typedef struct{
				union{
					DWORD Function;
					DWORD Ordinal;
					DWORD AddressOfData;
				} u1;
			} MY_IMAGE_THUNK_DATA, *PMY_IMAGE_THUNK_DATA;

			PMY_IMAGE_THUNK_DATA ptd_in = (PMY_IMAGE_THUNK_DATA)
				RVATOVA(pMemoryImage, pid->OriginalFirstThunk);
			PMY_IMAGE_THUNK_DATA ptd_out = (PMY_IMAGE_THUNK_DATA)
				RVATOVA(pMemoryImage, pid->FirstThunk);
				
			for(; ptd_in->u1.Function != NULL; ptd_in++, ptd_out++){

				FARPROC func;
				if(ptd_in->u1.Ordinal & 0x80000000){
					func = GetProcAddress(hDll, 
						MAKEINTRESOURCE(ptd_in->u1.Ordinal));
				}else{
					PIMAGE_IMPORT_BY_NAME pibn = 
						(PIMAGE_IMPORT_BY_NAME)RVATOVA(
						pMemoryImage, ptd_in->u1.AddressOfData);
					func = GetProcAddress(hDll, (PTCHAR)pibn->Name);
				}
				
				if(func == NULL)
					return FALSE;
					
				ptd_out->u1.Function = (DWORD)func;
			}
		}
	}

	DWORD dwIDEB = IMAGE_DIRECTORY_ENTRY_BASERELOC;
	DWORD delta = (DWORD)pMemoryImage - (DWORD)poh->ImageBase;

	if((delta == 0) || (poh->DataDirectory[dwIDEB].Size == 0))
		return TRUE;
	
	PIMAGE_FIXUP_BLOCK pfb = (PIMAGE_FIXUP_BLOCK)RVATOVA(
		pMemoryImage, poh->DataDirectory[dwIDEB].VirtualAddress);

	while(pfb->dwPageRVA != 0){
		
		int count = (pfb->dwBlockSize - sizeof(
			IMAGE_FIXUP_BLOCK)) / sizeof(IMAGE_FIXUP_ENTRY);
		PIMAGE_FIXUP_ENTRY pfe = (PIMAGE_FIXUP_ENTRY)
			((PTCHAR)pfb + sizeof(IMAGE_FIXUP_BLOCK));

		for(int i=0; i < count; i++, pfe++){

			PVOID fixaddr = RVATOVA(
				pMemoryImage, pfb->dwPageRVA + pfe->offset);
			
			switch(pfe->type)
			{
			case IMAGE_REL_BASED_ABSOLUTE:
				break;
			case IMAGE_REL_BASED_HIGH:
				*((WORD *)fixaddr) += HIWORD(delta);
				break;
			case IMAGE_REL_BASED_LOW:
				*((WORD *)fixaddr) += LOWORD(delta);
				break;
			case IMAGE_REL_BASED_HIGHLOW:
				*((DWORD *)fixaddr) += delta;
				break;
			case IMAGE_REL_BASED_HIGHADJ:
				*((WORD *)fixaddr) = HIWORD(
					((*((WORD *)fixaddr)) << 16) | 
					(*(WORD *)(pfe+1))+ delta + 0x00008000);
				pfe++;
				break;
			default:
				return FALSE;
			}
		}

		pfb = (PIMAGE_FIXUP_BLOCK)((PTCHAR)pfb + pfb->dwBlockSize);
	}
	return TRUE;
}


// -------------------------------------------------------------
// DLL�C���[�W���v���e�N�g����
// �����@�FDLL�t�@�C���C���[�W
// �߂�l�F����TRUE�A���sFALSE
// -------------------------------------------------------------
BOOL ProtectDllImage(PVOID pMemoryImage)
{
	// �Z�N�V�������擾
	PIMAGE_FILE_HEADER pfh = 
		(PIMAGE_FILE_HEADER)PEFHDROFFSET(pMemoryImage);
	int nSectionCount = pfh->NumberOfSections;

	// �Z�N�V�����w�b�_�擾
	PIMAGE_SECTION_HEADER psh = 
		(PIMAGE_SECTION_HEADER)SECHDROFFSET(pMemoryImage);

	for(int i=0; i < nSectionCount; i++, psh++){

		// �Z�N�V�����A�h���X�ƃT�C�Y�̎擾
		PVOID secMemAddr = (PTCHAR)
			RVATOVA(pMemoryImage, psh->VirtualAddress);
		
		DWORD chr = psh->Characteristics;
		// �v���e�N�g�t���O�̐ݒ�
		BOOL bWrite  = (chr & IMAGE_SCN_MEM_WRITE)   ? TRUE : FALSE;
		BOOL bRead   = (chr & IMAGE_SCN_MEM_READ)    ? TRUE : FALSE;
		BOOL bExec   = (chr & IMAGE_SCN_MEM_EXECUTE) ? TRUE : FALSE;
		BOOL bShared = (chr & IMAGE_SCN_MEM_SHARED)  ? TRUE : FALSE;
		
		DWORD newProtect = 0;
		// �t���O����
		if(bWrite && bRead && bExec && bShared)
			newProtect = PAGE_EXECUTE_READWRITE;
		else if(bWrite && bRead && bExec)
			newProtect = PAGE_EXECUTE_WRITECOPY;
		else if(bRead && bExec)
			newProtect = PAGE_EXECUTE_READ;
		else if(bExec)
			newProtect = PAGE_EXECUTE;
		else if(bWrite && bRead && bShared)
			newProtect = PAGE_READWRITE; 
		else if(bWrite && bRead)
			newProtect = PAGE_WRITECOPY;
		else if(bRead)
			newProtect = PAGE_READONLY;

		if(chr & IMAGE_SCN_MEM_NOT_CACHED)
			newProtect |= PAGE_NOCACHE;

		if(newProtect == 0)
			return FALSE;

		DWORD oldProtect;
		// �v���e�N�g���s
		VirtualProtect(secMemAddr, 
			psh->SizeOfRawData, newProtect, &oldProtect);
	}
	return TRUE;
}


// -------------------------------------------------------------
// DLL�C���[�W���R�s�[����֐�
// �����@�FDLL�t�@�C���C���[�W�A�R�s�[��|�C���^
// �߂�l�F����TRUE�A���sFALSE
// -------------------------------------------------------------
BOOL MapDllFromImage(PVOID pDLLFileImage, 
					 PVOID pMemoryImage)
{
	// PE�w�b�_�ƃZ�N�V�����w�b�_���R�s�[
	PIMAGE_OPTIONAL_HEADER poh = 
		(PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(pDLLFileImage);
	memcpy(pMemoryImage, pDLLFileImage, poh->SizeOfHeaders);

	// �Z�N�V���������擾
	PIMAGE_FILE_HEADER pfh = (PIMAGE_FILE_HEADER)
		PEFHDROFFSET(pDLLFileImage);
	int nSectionCount = pfh->NumberOfSections;

	// �Z�N�V�����w�b�_�|�C���^�擾
	PIMAGE_SECTION_HEADER psh = 
		(PIMAGE_SECTION_HEADER)SECHDROFFSET(pDLLFileImage);

	// ���ׂẴZ�N�V�����̃R�s�[
	for(int i=0; i < nSectionCount; i++, psh++){
		PTCHAR secMemAddr  = (PTCHAR)
			((PTCHAR)pMemoryImage + psh->VirtualAddress);
		PTCHAR secFileAddr = (PTCHAR)
			((PTCHAR)pDLLFileImage + psh->PointerToRawData);
		int secLen = psh->SizeOfRawData;
		memcpy(secMemAddr, secFileAddr, secLen);
	}
	return TRUE;
}


// -------------------------------------------------------------
// DLL�C���[�W����DLL�����[�h����֐�
// �����@�FDLL�t�@�C���C���[�W�A�}�b�s���O���i���ʎq�j�A�t���O
// �߂�l�F����DLL�n���h���A���sNULL
// -------------------------------------------------------------
HMODULE LoadDllFromImage(LPVOID pDLLFileImage, 
						 PTCHAR szMappingName,
						 DWORD dwFlags)
{
	// �}�b�s���O�����Ȃ���΃G���[
	if(szMappingName == NULL)
		return NULL;

	// �}�b�s���O���̃T�C�Y�𔻒�
	if(lstrlen(szMappingName) >= MAX_PATH)
		return NULL;
	
	// PE�f�[�^�̔���
	PIMAGE_DOS_HEADER doshead = (PIMAGE_DOS_HEADER)pDLLFileImage;
	if(doshead->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;
	if(*(DWORD *)NTSIGNATURE(pDLLFileImage) != IMAGE_NT_SIGNATURE)
		return NULL;
	PIMAGE_OPTIONAL_HEADER poh = 
		(PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(pDLLFileImage);
	if(poh->Magic != 0x010B)
		return NULL;

	// �Z�N�V�������擾
	PIMAGE_FILE_HEADER pfh = 
		(PIMAGE_FILE_HEADER)PEFHDROFFSET(pDLLFileImage);
	int nSectionCount = pfh->NumberOfSections;

	DWORD pPreferredImageBase = poh->ImageBase;
	DWORD dwImageSize = poh->SizeOfImage;

	PVOID pImageBase;
	HANDLE hmapping = NULL;
	// DLL�n���h����������Ȃ���ΐV��������
	if((pImageBase = GetDllHandle(szMappingName)) == NULL){
		BOOL bCreated = FALSE;
		// ���łɃ}�b�s���O����Ă��邩�ǂ���
		hmapping = OpenFileMapping(
			FILE_MAP_WRITE, TRUE, szMappingName);
		// ����Ă��Ȃ��Ȃ琶��
		if(hmapping == NULL){
			hmapping = CreateFileMapping(
				INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 
				dwImageSize + SIZE_OF_PARAMETER_BLOCK, szMappingName);
			if(hmapping == NULL)
				return NULL;
			bCreated = TRUE;
		}

		// �}�b�s���O����Ă���f�[�^�̐擪��pImageBase��
		pImageBase = MapViewOfFileEx(
			hmapping, FILE_MAP_WRITE, 0, 0, 0, (LPVOID)pPreferredImageBase);
		if(pImageBase == NULL){
			pImageBase = MapViewOfFileEx(
				hmapping, FILE_MAP_WRITE, 0, 0, 0, NULL);
		}
		CloseHandle(hmapping);
		if(pImageBase == NULL)
			return NULL;

		// �V�����������ꂽ���A�x�[�X�A�h���X���ς���Ă�����
		if(bCreated || (pImageBase != (LPVOID)pPreferredImageBase)){
			// DLL�C���[�W���}�b�s���O
			if( ! MapDllFromImage(pDLLFileImage, pImageBase)){
				UnmapViewOfFile(pImageBase);
				return NULL;
			}
		}
		
		// LOAD_LIBRARY_AS_DATAFILE�������ĂȂ��Ȃ��
		if( ! (dwFlags & LOAD_LIBRARY_AS_DATAFILE)){
			// 
			if( ! PrepareDllImage(pImageBase, dwImageSize)){
				UnmapViewOfFile(pImageBase);
				return NULL;
			}
			
			// �t���O��DONT_RESOLVE_DLL_REFERENCES�������ĂȂ����
			if( ! (dwFlags & DONT_RESOLVE_DLL_REFERENCES)){
				// DLLMain�����s�i�A�^�b�`�j
				if( ! RunDllMain(pImageBase, dwImageSize, DLL_ATTACH)){
					UnmapViewOfFile(pImageBase);
					return NULL;
				}
			}

			// �v���e�N�g�����s
			if( ! ProtectDllImage(pImageBase)){
				UnmapViewOfFile(pImageBase);
				return NULL;
			}
		}
	}
	
	// DLL�f�[�^�x�[�X�֒ǉ�
	if(AddDllReference(pImageBase, szMappingName, dwFlags) == -1){
		if(hmapping != NULL)
			UnmapViewOfFile(pImageBase);
		return NULL;
	}	

	return (HMODULE)pImageBase;	
}


// -------------------------------------------------------------
// DLL�����[�h����֐�
// �����@�FDLL�t�@�C�����A�\���iNULL�Œ�j�A�t���O
// �߂�l�F����DLL�n���h���A���sNULL
// -------------------------------------------------------------
HMODULE LoadDllEx(LPCTSTR lpLibFileName,
				  HANDLE hReserved,
				  DWORD dwFlags)
{
	// ��փt�@�C���������@�w��
	// �iLOAD_WITH_ALTERED_SEARCH_PATH�j�̓T�|�[�g���Ȃ�
	if(dwFlags & LOAD_WITH_ALTERED_SEARCH_PATH)
		return NULL;

	// DLL�p�X�擾
	TCHAR szPath[MAX_PATH + 1], *szFilePart;
	int nLen = SearchPath(NULL, lpLibFileName, 
		".dll", MAX_PATH, szPath, &szFilePart);
	if(nLen == 0)
		return NULL;

	// �t�@�C���}�b�s���O
	HANDLE hFile = CreateFile(
		szPath, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return NULL;
	HANDLE hMapping = CreateFileMapping(
		hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFile);
	LPVOID pBaseAddr = MapViewOfFile(
		hMapping, FILE_MAP_READ, 0, 0, 0);
	if(pBaseAddr == NULL){
		CloseHandle(hMapping);
		return NULL;
	}

	// DLL�C���[�W�̓ǂݍ���
	HMODULE hRet = LoadDllFromImage(pBaseAddr, 
		szFilePart, dwFlags & ~LOAD_WITH_ALTERED_SEARCH_PATH);

	// �t�@�C���}�b�s���O����
	UnmapViewOfFile(pBaseAddr);
	CloseHandle(hMapping);
	return hRet;
}


// -------------------------------------------------------------
// DLL�����[�h����֐��iLoadDLLEx�ւ̋��n���j
// �����@�FDLL�t�@�C����
// �߂�l�F����DLL�n���h���A���sNULL
// -------------------------------------------------------------
#ifdef UNICODE
HMODULE LoadDllW(LPCWSTR lpLibFileName)
#else
HMODULE LoadDllA(LPCSTR lpLibFileName)
#endif
{
	return LoadDllEx(lpLibFileName, NULL, 0);
}

// -------------------------------------------------------------
// DLL���J������֐�
// �����@�FDLL�n���h��
// �߂�l�F����TRUE�A���sFALSE
// -------------------------------------------------------------
BOOL FreeDll(HMODULE hLibModule)
{
	// hLibModule��NULL�Ȃ���O
	if(hLibModule == NULL)
		return FALSE;
	
	// PE�f�[�^�̎���
	PIMAGE_DOS_HEADER doshead = (PIMAGE_DOS_HEADER)hLibModule;
	if(doshead->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE;
	if(*(PDWORD)NTSIGNATURE(hLibModule) != IMAGE_NT_SIGNATURE)
		return FALSE;
	PIMAGE_OPTIONAL_HEADER poh = 
		(PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(hLibModule);
	if(poh->Magic != 0x010B)
		return FALSE;

	DWORD dwFlags;
	TCHAR szName[MAX_PATH];
	// DLL�f�[�^�x�[�X����͂���
	int dllaction = RemoveDllReference(hLibModule, szName, &dwFlags);
	if(dllaction == -1)
		return FALSE;

	// DLL�̃f�^�b�`
	if( ! (dwFlags & (LOAD_LIBRARY_AS_DATAFILE | 
		DONT_RESOLVE_DLL_REFERENCES)))
	{	// �J�E���^��0�idllaction=1�j�Ȃ��DLL���f�^�b�`���ďI��
		if(dllaction){
			RunDllMain(hLibModule, poh->SizeOfImage, DLL_DETACH);
			return UnmapViewOfFile(hLibModule);
		}
	}
	return TRUE;
}


/*typedef int (*PADDFUNC)(int, int);
typedef int (*PSUBFUNC)(int, int);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	InitializeDLLLoad();
	HMODULE hHandle = LoadDLL(_T("test_dll"));
	PADDFUNC pAdd = (PADDFUNC)GetDLLProcAddress(hHandle, _T("add_num"));
	int a = (*pAdd)(3, 5);  // 3 + 5 = 8
	PSUBFUNC pSub = (PSUBFUNC)GetDLLProcAddress(
		GetDLLHandle(_T("test_dll.dll")), _T("sub_num"));
	int b = (*pSub)(8, 5);  // 8 - 5 = 3
	TCHAR szFileName[MAX_PATH];
	GetDLLFileName(hHandle, szFileName, sizeof(szFileName));
	TCHAR szBuffer[1024];
	wsprintf(szBuffer, _T(
		"FileName = %s\r\na = %d, b = %d\r\n"), szFileName, a, b);
	MessageBox(GetActiveWindow(), szBuffer, _T("Message"), MB_OK);
	FreeDLL(hHandle);
	KillDLLLoad();
	return 0;
}*/

#ifdef __cplusplus
}
#endif

