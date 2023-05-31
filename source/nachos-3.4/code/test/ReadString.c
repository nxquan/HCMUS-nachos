#include "syscall.h"
#include "copyright.h"
#define MaxStringLength 255
int main()
{
    char a[254];
    int length = 254;
    PrintString("Nhap chuoi ki tu: ");
    ReadString(a, length);
    PrintString("Chuoi da nhap: ");
    PrintString(a);
    Halt();
}
