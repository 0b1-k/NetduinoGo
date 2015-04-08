#ifndef _INC_VT100_
#define _INC_VT100_

#define VT100_Black          0
#define VT100_Red            1
#define VT100_Green          2
#define VT100_Yellow         3
#define VT100_Blue           4
#define VT100_Magenta        5
#define VT100_Cyan           6
#define VT100_White          7

void vt100TextColor(int foregroundColor, int backgroundColor);
void vt100TextDefault();

void PrintNeutralMessage(const char* msg);
void PrintErrorMessage(const char* msg);
void PrintSuccessMessage(const char* msg);
void PrintAnomalyMessage(const char* msg);

#endif
