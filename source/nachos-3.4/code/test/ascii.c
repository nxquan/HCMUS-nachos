#include "syscall.h"
#include "copyright.h"

int main()
{
    int i;
    char c;
    PrintString("Bang ASCII in duoc: ");
    for (i = 32; i <= 126; i++)
    {
       c = (char)i;
       PrintChar(c);
       PrintString("\n");
    }
    Halt();
}
