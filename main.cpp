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
		X86Emitter::mov_imm_to_edx(&code, 100);

		//if condition
		X86Emitter::cmp_ecx_to_edx(&code);
		X86Emitter::short_jne(&code, movDwordSize + storeWordArraySize + addByteToMemaddrSize + movDwordSize + storeWordArraySize + byteRelJmpSize);
		//equal


		//stack[0] <- 100
		X86Emitter::mov_ecx_to_eax(&code);
		X86Emitter::storeWordArray_AregAsInput(&code, pstack, pstackPointer);

		//ptr <- 0xf
		X86Emitter::addByteToMemaddr(&code, pstackPointer, 0xa);

		//stack[0xf] <- 99
		X86Emitter::mov_edx_to_eax(&code);
		X86Emitter::storeWordArray_AregAsInput(&code, pstack, pstackPointer);

		X86Emitter::short_jmp(&code, loadWordArraySize);
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
