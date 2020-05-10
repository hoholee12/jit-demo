#include <windows.h>
#include "X86Emitter.h"

class myMain : public X86Emitter{
public:
	myMain(){
		uint16_t stack[0x10] = {0};
		uint8_t stackPointer = 0x0;
		uint32_t pstack = (uint32_t)&stack;
		uint32_t pstackPointer = (uint32_t)&stackPointer;


		std::vector<uint8_t> code;

		X86Emitter::mov_imm_to_ecx(&code, 100);
		X86Emitter::mov_imm_to_edx(&code, 90);

		//if condition
		X86Emitter::cmp_ecx_to_edx(&code);
		X86Emitter::short_jne(&code, movDwordSize + storeWordArraySize + addByteToMemaddrSize + subByteToMemaddrSize + movDwordSize + storeWordArraySize + byteRelJmpSize);
		//equal

			
		X86Emitter::mov_ecx_to_eax(&code);
		/*	2bytes
		e:  89 c8                   mov    eax,ecx
		*/
			
		X86Emitter::storeWordArray_AregAsInput(&code, pstack, pstackPointer);
		/*	20bytes
		10: bb e4 fc 4f 00          mov    ebx,0x4ffce4
		15: 8a 0d db fc 4f 00       mov    cl,BYTE PTR ds:0x4ffcdb
		1b: 0f b6 c9                movzx  ecx,cl
		1e: 8d 0c 4b                lea    ecx,[ebx+ecx*2]
		21: 66 89 01                mov    WORD PTR [ecx],ax
		*/
		
		X86Emitter::addByteToMemaddr(&code, pstackPointer, 0xa);
		/*	21bytes
		24: 8a 05 db fc 4f 00       mov    al,BYTE PTR ds:0x4ffcdb
		2a: 0f b6 c0                movzx  eax,al
		2d: 81 c0 0a 00 00 00       add    eax,0xa
		33: 88 05 db fc 4f 00       mov    BYTE PTR ds:0x4ffcdb,al
		*/
		X86Emitter::subByteToMemaddr(&code, pstackPointer, 0x1);
		/*	21bytes
		39: 8a 05 db fc 4f 00       mov    al,BYTE PTR ds:0x4ffcdb
		3f: 0f b6 c0                movzx  eax,al
		42: 81 e8 01 00 00 00       sub    eax,0x1
		48: 88 05 db fc 4f 00       mov    BYTE PTR ds:0x4ffcdb,al
		*/
		X86Emitter::mov_edx_to_eax(&code);
		/*	2bytes
		4e: 89 d0                   mov    eax,edx
		*/
		X86Emitter::storeWordArray_AregAsInput(&code, pstack, pstackPointer);
		/*	20bytes
		50: bb e4 fc 4f 00          mov    ebx,0x4ffce4
		55: 8a 0d db fc 4f 00       mov    cl,BYTE PTR ds:0x4ffcdb
		5b: 0f b6 c9                movzx  ecx,cl
		5e: 8d 0c 4b                lea    ecx,[ebx+ecx*2]
		61: 66 89 01                mov    WORD PTR [ecx],ax
		*/
		X86Emitter::short_jmp(&code, loadWordArraySize);
		/*	2bytes
		64: eb 13                   jmp    0x79
		*/

		//not equal

		//return as 99
		X86Emitter::loadWordArray_AregAsResult(&code, pstack, pstackPointer);

		X86Emitter::ret(&code);

		//print block
		for (int i = 0; i < code.size(); i++){
			printf("%02X ", code.at(i));
		}

	

		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		auto const page_size = system_info.dwPageSize;

		// prepare the memory in which the machine code will be put (it's not executable yet):
		void* buffer = VirtualAlloc(nullptr, page_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		// copy the machine code into that memory:
		std::memcpy(buffer, code.data(), code.size());

		// mark the memory as executable:
		DWORD dummy;
		VirtualProtect(buffer, code.size(), PAGE_EXECUTE_READ, &dummy);

		// interpret the beginning of the (now) executable memory as the entry
		// point of a function taking no arguments and returning a 4-byte int:


		typedef int32_t(*dank)(void);
		using weed = int32_t(*)(void);
		dank function_ptr = (weed)buffer;
		//auto const function_ptr = reinterpret_cast<std::int32_t(*)()>(buffer);


		// call the function and store the result in a local std::int32_t object:
		auto const result = function_ptr();

		// free the executable memory:
		VirtualFree(buffer, 0, MEM_RELEASE);

		for (int i = 0; i < 0x10; i++){
			printf("\nstack%d = %d", i, stack[i]);
		}

		// use your std::int32_t:
		printf("\nret = %d stack0 = %d stackf = %d\n", result, stack[0], stack[0xF]);

	}

};

int main()
{
	/*
	uint8_t var = 0;
	uint8_t var2 = 0;
	uint8_t var3 = 0;
	uint8_t var4 = 0;
	uint32_t pointer = (uint32_t)&var4;
	uint8_t b0, b1, b2, b3;
	b0 = (pointer & 0x000000ff);
	b1 = (pointer & 0x0000ff00) >> 8;
	b2 = (pointer & 0x00ff0000) >> 16;
	b3 = (pointer & 0xff000000) >> 24;


	std::vector<uint8_t> const code =
	{
	0xa1,                   // move the following value to EAX:
	b0, b1, b2, b3,
	0xc3                    // return what's currently in EAX
	};
	*/

	myMain* my = new myMain();
	return 0;
}
