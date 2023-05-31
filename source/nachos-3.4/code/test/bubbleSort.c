#include "syscall.h"
int A[101];
int main()
{
    int n, k, luachon, i, j, temp, haveSwap, in;

    PrintString("\nNhap vao so nguyen n: ");
    n = ReadInt();
    for (k = 0; k < n; k++)
    {
	PrintString("\nNhap so nguyen thu ");
	PrintInt(k + 1);
	PrintString(": ");
    	A[k] = ReadInt();
    }
    

while (1)
{
    PrintString("\n\n: ");
    PrintString("\n1.Chuoi tang dan: ");
    PrintString("\n2.Chuoi giam dan: ");
    PrintString("\n\nLuaChon: ");
    luachon = ReadInt();
    if (luachon != 1 && luachon != 2)
    {
        PrintString("\nVui long nhap lai!  ");
    }
    else if (luachon == 1)
    {

        haveSwap = 0;
        for (i = 0; i < n - 1; i++)
        {
PrintString("  ");
            // i phần tử cuối cùng đã được sắp xếp
            haveSwap = 0;
            for (j = 0; j < n - i - 1; j++)
            {
                if (A[j] > A[j + 1])
                {
                    temp = A[j];
                    A[j] = A[j + 1];
                    A[j + 1] = temp;
                    haveSwap = 1; // Kiểm tra lần lặp này có swap không
		PrintString("  ");
                }
            }
            // Nếu không có swap nào được thực hiện => mảng đã sắp xếp. Không cần lặp thêm
            if (haveSwap == 0)
            {
                break;
            }
        }
	PrintString("  ");
	for(in = 0;in<n;in++) {
		PrintInt(A[in]);
		PrintString(" ");
	}
	PrintString("   ");
	break;
    }
    else
    {
	haveSwap = 0;
        for (i = 0; i < n - 1; i++)
        {
PrintString("  ");
            // i phần tử cuối cùng đã được sắp xếp
            haveSwap = 0;
            for (j = 0; j < n - i - 1; j++)
            {
                if (A[j] < A[j + 1])
                {
                    temp = A[j];
                    A[j] = A[j + 1];
                    A[j + 1] = temp;
                    haveSwap = 1; // Kiểm tra lần lặp này có swap không
		PrintString("  ");
                }
            }
            // Nếu không có swap nào được thực hiện => mảng đã sắp xếp. Không cần lặp thêm
            if (haveSwap == 0)
            {
                break;
            }
        }
	PrintString("  ");
	for(in = 0;in<n;in++) {
		PrintInt(A[in]);
		PrintString(" ");
	}
	PrintString("   ");
	break;
    }
}
}
