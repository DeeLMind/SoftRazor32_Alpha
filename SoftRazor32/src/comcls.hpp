#include "common.h"
#include "g_type.h"

class MemoryManager
{
private:
  static PMEMORY_MODIFY         MM_HEADER;

public:
  MemoryManager() {}

  ~MemoryManager() {}

  /*查询内存更改*/
  UINT QueryMemoryModify(PBYTE MemAddr, UINT MemSize);
  /*尝试修改内存*/
  BOOL TryModifyMemory(PBYTE destAddr, PBYTE pData, UINT DataSize);
  /*还原内存*/
  BOOL RevertMemory(PBYTE MemAddr, UINT DataSize);
  /*还原全部内存*/
  BOOL RevertAllMemory(PBYTE MemAddr);
  /*获取原始内存*/
  UINT GetOriginalMemory(PBYTE MemAddr, PBYTE destAddr, UINT MemAndDestSize);
};