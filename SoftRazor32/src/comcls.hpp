#include "common.h"
#include "g_type.h"

class MemoryManager
{
private:
  static PMEMORY_MODIFY         MM_HEADER;

public:
  MemoryManager() {}

  ~MemoryManager() {}

  /*��ѯ�ڴ����*/
  UINT QueryMemoryModify(PBYTE MemAddr, UINT MemSize);
  /*�����޸��ڴ�*/
  BOOL TryModifyMemory(PBYTE destAddr, PBYTE pData, UINT DataSize);
  /*��ԭ�ڴ�*/
  BOOL RevertMemory(PBYTE MemAddr, UINT DataSize);
  /*��ԭȫ���ڴ�*/
  BOOL RevertAllMemory(PBYTE MemAddr);
  /*��ȡԭʼ�ڴ�*/
  UINT GetOriginalMemory(PBYTE MemAddr, PBYTE destAddr, UINT MemAndDestSize);
};