#pragma once
#include <Windows.h>
#include <iostream>
#include <io.h>

// 파일과 관련된 모든 기능은 여기에서 사용합니다.
// FIle 랩핑
// 코딩 애초에 잘못사용하기도 힘들게 짜는것이 베스트 입니다.
// 삭제
// 이동


const int MAXPATH = 256;

class UEngineFile
{
// 하드코딩 
public:
	UEngineFile();
	~UEngineFile();

	inline void SetPath(const char* _Path)
	{
		strcpy_s(Path, _Path);
	}

	// 그런거야 Is 
	// Path 경로에 파일이 존재해? 이파일이 존재하는 거야?
	
	// 복잡한 함수같은 경우에는 분리구현
	// 함수 하나만 썼는데?
	// 선언과 구현을 분리하면 inline은 붙이지 못한다.

	// 1. if for등으로 완전히 다른 기능을 안사용하면 저는 inline으로 만들려고 
	// 2. 그래도 너무 길고 복잡하면 안만듭니다 inline으로 
	// 3. 여러개의 외부함수를 끌어다 섰다면 그중 하나라도 inline이 없다면 inline으로 안만든다.

	// Write
	void Write(const void* _Ptr, size_t _Size);
	void Read(void* _Ptr, size_t _Size);

	void FileOpen(const char* _Mode);
	bool IsExits();
	void Close();

// 기능 클래스 랩핑
private:
	// 상수
	// _MAX_DIR 윈도우에 의존적인 프로그램
	

	char Path[MAXPATH] = "";
	FILE* File = nullptr;
};

