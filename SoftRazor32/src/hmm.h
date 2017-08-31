#pragma once
#include "windows.h"

class HeapMemoryManager
{
private:
  HANDLE          h_heap;
  BOOL            b_lfh;
  DWORD           ExceptionCode;

public:
  HeapMemoryManager(DWORD dwFlag, SIZE_T InitSize);
  ~HeapMemoryManager();

  void  * WINAPI  hmalloc(SIZE_T bsize);
  void  * WINAPI  hcalloc(SIZE_T bsize);
  void  * WINAPI  hcalloc(SIZE_T bnum, SIZE_T busize);
  void  * WINAPI  hrealloc(void * pmemblk, SIZE_T newsize);
  void  * WINAPI  hrecalloc(void * pmemblk, SIZE_T newsize);
  void  * WINAPI  hrecalloc(void * pmemblk, SIZE_T newnum, SIZE_T busize);
  BOOL    WINAPI  hfree(void * pmemblk);
  DWORD   WINAPI  hgeterrorcode();
  SIZE_T  WINAPI  hcompact();
  BOOL    WINAPI  hreset(DWORD dwFlag, SIZE_T InitSize);
  BOOL    WINAPI  hdestroy();
  BOOL    WINAPI  IsLowFragmentationHeap();
};