#include "syscall.h"
#include "copyright.h"

int main()
{
    int a;
    PrintString("Nhap so: ");
    a = ReadInt();
    PrintString("So nhap: ");
    PrintInt(a);
    Halt();
}
