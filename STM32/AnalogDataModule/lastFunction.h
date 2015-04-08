#ifndef _INC_LastFunction_
  #define _INC_LastFunction_

typedef enum {
  I2cModule,
  EndOfModuleArray
} Module_t;

void initLastFunctionNames();
void SetLastFunctionName(Module_t moduleId, const char* functionName);
void DumpLastFunctionNames();

#endif