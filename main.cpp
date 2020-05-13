#include <windows.h>
#include "X86Emitter.h"

class myMain : public X86Emitter{
public:
	myMain(){
		uint16_t stack[0x10] = {0};
		uint8_t stackPointer = 0x0;
		uint32_t pstack = (uint32_t)&stack;		//word
		uint32_t pstackPointer = (uint32_t)&stackPointer;	//byte


		std::vector<uint8_t> code;

		//loop demo
		X86Emitter::mov_imm(&code, dwordMovImmToDregMode, insertDisp(85)); //copy 85 to D

		//loop starts here
		int count = 0;
		count += X86Emitter::mov(&code, movDwordRegToRegMode, Dreg, Areg);			//copy D to A
		count += X86Emitter::storeWordArray_AregAsInput(&code, pstack, pstackPointer);		//store A to stack

		count += X86Emitter::add_imm(&code, dwordAddImmToMemaddrMode, insertAddr(pstackPointer), insertDisp(1));		//increase pointer 1

		count += X86Emitter::add_imm(&code, dwordAddImmToRegMode, insertDisp(-1), Dreg);	//increase D 1
		count += X86Emitter::mov_imm(&code, dwordMovImmToCregMode, insertDisp(70));		//copy 100 to C

		count += X86Emitter::cmp(&code, cmpMode, Creg, Dreg);		//cmp C and D
		X86Emitter::jcc(&code, byteRelJbeMode, insertDisp((byteRelJbeSize + count) * -1)); //keep looping when C is below D

		X86Emitter::mov_imm(&code, dwordMovImmToBregMode, insertAddr(pstackPointer));
		X86Emitter::parse(&code, "movzx edx, cl");
		X86Emitter::parse(&code, "mov BYTE PTR [ebx], al");

		//ends here
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
		printf("\nret = %d stack0 = %d stackf = %d ptr = %d\n", result, stack[0], stack[0xF], stackPointer);

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
