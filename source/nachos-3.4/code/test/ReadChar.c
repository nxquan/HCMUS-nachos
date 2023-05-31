#include "syscall.h"
#include "copyright.h"

int main()
{
    char a;
    PrintString("Nhap ki tu: ");
    a = ReadChar();
    PrintString("Ki tu nhap: ");
    PrintChar(a);
    Halt();
}
