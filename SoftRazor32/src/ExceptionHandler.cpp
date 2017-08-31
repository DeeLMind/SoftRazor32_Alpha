#include "../sr_pch.h"

int NTAPI SR_SetVectoredExceptionHandler(PVECTORED_EXCEPTION_HANDLER Handler)
{
  
  return 0;
}

LONG NTAPI FirstExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
  return 0;
}

LONG NTAPI LastExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
  return 0;
}
