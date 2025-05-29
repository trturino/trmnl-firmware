#include "DEV_Config.h"

// Display resolution
#define WIDTH       800
#define HEIGHT      480

#define BLACK   0x0   /// 000
#define WHITE   0x1   /// 001
#define YELLOW  0x2   /// 010
#define RED     0x3   /// 011
#define BLUE    0x5   /// 101
#define GREEN   0x6   /// 110

void Init(void);
void Clear(UBYTE color);
void Display(UBYTE *Image);
void Sleep(void);