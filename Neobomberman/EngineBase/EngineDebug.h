#pragma once
#include <Windows.h>
#include <assert.h>


#define MSGASSERT(VALUE) MessageBoxA(nullptr, VALUE, "치명적 에러", MB_OK); assert(false);
