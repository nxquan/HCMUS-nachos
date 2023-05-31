// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"



#define MaxStringLength 255
#define MaxFileLength 32

// Increase programming counter 
void IncreaseProgramCounter()
{
    int counter = machine->ReadRegister(PCReg);     //lay gia tri thanh ghi hien tai
    machine->WriteRegister(PrevPCReg, counter);     //gan gia tri thanh ghi truoc = hien tai
    counter = machine->ReadRegister(NextPCReg);     //lay gia tri thanh ghi ke tiep
    machine->WriteRegister(PCReg, counter);         //gan gia tri thanh ghi hien tai = thanh ghi ke tiep
    machine->WriteRegister(NextPCReg, counter + 4); //gan thanh ghi ke tiep ve sau 4 byte de tiep tuc nap lenh
}



// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char *User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; //need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    //printf("\n Filename u2s:");
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        //printf("%c",kernelBuf[i]);
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------



//ExceptionHandler
void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    switch (which)
    {
    case NoException:
        return;
    case PageFaultException:
        DEBUG('a', "\n\t No valid translation found");
        printf("\n\t No valid translation found");
        interrupt->Halt();
        break;
    case ReadOnlyException:
        DEBUG('a', "\n\t Write attempted to page marked read-only");
        printf("\n\t Write attempted to page marked read-only");
        interrupt->Halt();
        break;
    case BusErrorException:
        DEBUG('a', "\n\t Write attempted to page marked read-only");
        printf("\n\t Write attempted to page marked read-only");
        interrupt->Halt();
        break;
    case AddressErrorException:
        DEBUG('a', "\n\t Unaligned reference or one that was beyond the end of the address space");
        printf("\n\t Unaligned reference or one that was beyond the end of the address space");
        interrupt->Halt();
        break;
    case OverflowException:
        DEBUG('a', "\nInteger overflow in add or sub.");
        printf("\n\t Integer overflow in add or sub.");
        interrupt->Halt();
        break;
    case IllegalInstrException:
        DEBUG('a', "\n\t Unimplemented or reserved instr.");
        printf("\n\t Unimplemented or reserved instr.");
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        DEBUG('a', "\n\t Number exception types");
        printf("\n\t Number exception types");
        interrupt->Halt();
        break;
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            DEBUG('a', "\n\t Shutdown, initiated by user program.");
            printf("\n\t Shutdown, initiated by user program.");
            interrupt->Halt();
            break;
        case SC_Create:
        {
            int virtAddr;
            char *filename;
            DEBUG('a', "\n\t SC_Create call ...");
            DEBUG('a', "\n\t Reading virtual address of filename");
            // Lấy tham số tên tập tin từ thanh ghi r4
            virtAddr = machine->ReadRegister(4);
            DEBUG('a', "\n\t Reading filename.");
            // MaxFileLength là = 32
            filename = User2System(virtAddr, MaxFileLength + 1);
            if (filename == NULL)
            {
                printf("\n\t Not enough memory in system");
                DEBUG('a', "\n\t Not enough memory in system");
                machine->WriteRegister(2, -1); // trả về lỗi cho chương
                // trình người dùng
                delete filename;
                return;
            }
            DEBUG('a', "\n\t Finish reading filename.");
            //DEBUG(‘a’,"\n\t File name : '"<<filename<<"'");
            // Create file with size = 0
            // Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
            // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
            // hành Linux, chúng ta không quản ly trực tiếp các block trên
            // đĩa cứng cấp phát cho file, việc quản ly các block của file
            // trên ổ đĩa là một đồ án khác
            if (!fileSystem->Create(filename, 0))
            {
                printf("\n\t Error create file '%s'", filename);
                machine->WriteRegister(2, -1);
                delete filename;
                return;
            }
            machine->WriteRegister(2, 0); // trả về cho chương trình
                                          // người dùng thành công
            delete filename;
            break;
        }
        case SC_ReadChar:
        {
          int maxBytes = 255;
          char* buffer = new char[255];
          int numBytes = gSynchConsole->Read(buffer, maxBytes); //numBytes = so byte cua buffer = so ki tu
          if(numBytes > 1) //Neu nhap nhieu hon 1 ky tu thi khong hop le
          {
            printf("Chi duoc nhap duy nhat 1 ky tu!");
            DEBUG('a', "\nERROR: Chi duoc nhap duy nhat 1 ky tu!");
            machine->WriteRegister(2, 0);
          }
          else if(numBytes == 0) //Ky tu rong
          {
            printf("Ky tu rong!");
            DEBUG('a', "\nERROR: Ky tu rong!");
            machine->WriteRegister(2, 0);
          }
          else
          {
            //Chuoi vua lay co dung 1 ky tu, lay ky tu o index = 0, return vao thanh ghi R2
            char c = buffer[0];
            machine->WriteRegister(2, c);
          }
          delete buffer;
          break;
        }
        case SC_PrintChar:
        {
          char c = (char)machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
          gSynchConsole->Write(&c, 1); // In ky tu tu bien c, 1 byte
          break;
        }
        case SC_ReadString:
        {
          int virtAddr, length;
          char* buffer;
          virtAddr = machine->ReadRegister(4); // Lay dia chi tham so buffer truyen vao tu thanh ghi so 4
          length = machine->ReadRegister(5); // Lay do dai toi da cua chuoi nhap vao tu thanh ghi so 5
          buffer = User2System(virtAddr, length); // Copy chuoi tu vung nho User Space sang System Space
          gSynchConsole->Read(buffer, length); // Goi ham Read cua SynchConsole de doc chuoi
          System2User(virtAddr, length, buffer); // Copy chuoi tu vung nho System Space sang vung nho User Space
          delete buffer;
          IncreaseProgramCounter(); // Tang Program Counter
          break;
          }
        case SC_PrintString:
        {
          int virtAddr;
          char* buffer;
          virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
          buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai 255 ki tu
          int length = 0;
          while (buffer[length] != 0) length++; // Dem do dai that cua chuoi
          gSynchConsole->Write(buffer, length + 1); // Goi ham Write cua SynchConsole de in chuoi
          delete buffer;
          break;
        }
        case SC_ReadInt:
	{
	  char* buff;	//luu ky tu doc duoc
	  int MAX_BUFF=255;
	  buff=new char[MAX_BUFF+1];
	  int numbytes=gSynchConsole->Read(buff,MAX_BUFF); //doc toi da MAX_BUFF
	  int num=0;	//bien luu so doc duoc

	  bool Negative=false;	//so am = false -> duong
	  int firstNum=0;
	  int lastNum=0;
	  if(buff[0]=='-')	//ky tu dau tien la '-' -> am
	  {
		Negative=true;
		firstNum=1;	//so dau tien bat dau o vi tri 1
		lastNum=1;
	  }
	  for(int i=firstNum;i<numbytes;i++)
	  {
		if(buff[i]=='.')	//truong hop so co dau cham
		{
			int j=i+1;
			for(;j<numbytes;j++)
			{
			  if(buff[j]!='0')
			  {
				printf("\n\n Khong hop le");
				machine->WriteRegister(2,0);	//tra ve 0
				IncreaseProgramCounter();
				delete[] buff;
				return;
			  }
			}
			lastNum=i-1;
			break;
		}
		else if(buff[i]<'0'&&buff[i]>'9')	//bat buoc la ky tu so
		{
			printf("\n\nKhong hop le");
			machine->WriteRegister(2,0);	//tra ve 0
			IncreaseProgramCounter();
			delete[] buff;
			return;
		}
		lastNum=i;
	  }
	  for(int i=firstNum;i<=lastNum;i++)
	  {
		num=num*10 + (int)(buff[i]-48);	//chuyen tu char sang int
	  }
	  if(Negative)
	  {
		num=num*-1;	//neu la so am thi nhan -1
	  }
	  machine->WriteRegister(2,num);	//tra ve so doc duoc
	  IncreaseProgramCounter();
	  delete[] buff;
	  return;
	}
	case SC_PrintInt:
	{
	  int num=machine->ReadRegister(4);	//doc thanh ghi 4 (doi so 1)
	  if(num==0)	//truong hop so 0
	  {
		gSynchConsole->Write("0",1);
		IncreaseProgramCounter();
		return;
	  }
	  bool Negative=false;	//bien dau
	  int SNum=0;	//so chu so
	  int firstNum=0;
	  if(num<0)	//so am
	  {
		Negative=true;	
		num=num* -1;
		firstNum=1;
	  }
	  int temp=num;
	  while(temp)
	  {
		SNum++;
		temp /=10;
	  }
	  char* buff;	//bien luu ky tu char cua so de in ra mh
	  int MAX_BUFF=255;
	  buff=new char[MAX_BUFF+1];
	  for(int i=firstNum+SNum-1;i>=firstNum;i--)
	  {
		buff[i]=(char)((num%10)+48);	//chuyen int sang char
		num/=10;
	  }	
	  if(Negative)	//truong hop so am
	  {
		buff[0]='-';
		buff[SNum+1]=0;
		gSynchConsole->Write(buff,SNum+1);
		delete buff;
		IncreaseProgramCounter();
		return;
	  }
	  buff[SNum]=0;
	  gSynchConsole->Write(buff,SNum);
	  delete buff;
	  IncreaseProgramCounter();
	  return;
	}
        default:
            printf("\n\t Unexpected user mode exception (%d)", which);
            interrupt->Halt();
        }
	IncreaseProgramCounter();
    }
}
