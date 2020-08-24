#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif
#include "X86Emitter.h"

class myMain : public X86Emitter{

private:
	
	uint16_t stack[0x10];
	uint8_t stackPointer = 0x0;


	uint32_t pstack = (uint32_t)stack;		//word
	uint32_t pstackPointer = (uint32_t)&stackPointer;	//byte

	std::vector<vect8> bigcode;


public:
	void test(){	//what???
		bigcode.push_back(vect8{});
	}
	myMain(){
		//std::memcpy(stack, 0, sizeof(stack)); //memcpy doesnt work with non-POD apparently
		test();
		vect8* code = &bigcode.at(0);

		//loop demo
		//X86Emitter::mov_imm(&code, dwordMovImmToDregMode, insertDisp(85)); //copy 85 to D

		X86Emitter::BlockInitializer(code);

		
		X86Emitter::parse(code, "mov ebx, extra", insertDisp(85));
		//loop starts here
		int count = 0;
		count += X86Emitter::parse(code, "mov eax, ebx");
		
		//count += X86Emitter::mov(&code, movDwordRegToRegMode, Dreg, Areg);			//copy D to A
		count += storeArray_AregAsInput(code, pstack, pstackPointer, Word); //store A to stack
		
		//count += X86Emitter::add_imm(&code, byteAddImmToMemaddrMode, insertAddr(pstackPointer), insertDisp(1));		//increase pointer 1
		count += X86Emitter::parse(code, "add byte ptr [extra], 1", insertAddr(pstackPointer));

		count += X86Emitter::parse(code, "mov edx, extra", insertDisp(pstack));
		//count += X86Emitter::parse(code, "add byte ptr [edx], extra", insertDisp(1));
		
		//demo code
		//count += addToMemaddr(code, pstack, 100, Word);	//adds 100 to stack[0]
		
		count += loadMemToDwordReg(code, pstack, Areg, Byte); //loads stack[0] value to eax

		
		count += X86Emitter::Add_imm(code, dwordAddImmToRegMode, insertDisp(-1), Breg);	//decrease B 1
		count += X86Emitter::Mov_imm(code, dwordMovImmToRegMode, Creg, insertDisp(70));		//copy 100 to C

		
		count += X86Emitter::parse(code, "cmp ecx, ebx");		//cmp C and D
		X86Emitter::parse(code, "jbe extra", insertDisp((byteRelJbeSize + count) * -1));
		//X86Emitter::Jcc(code, byteRelJbeMode, insertDisp((byteRelJbeSize + count) * -1)); //keep looping when C is below D
		
		X86Emitter::Mov_imm(code, dwordMovImmToRegMode, Breg, insertAddr(pstackPointer));
		X86Emitter::parse(code, "movzx edx, cl");
		X86Emitter::parse(code, "mov BYTE PTR [ebx], al");
		X86Emitter::parse(code, "mov eax, 1234");
		X86Emitter::setToMemaddr(code, pstackPointer, 0, Byte);
		X86Emitter::loadArray_AregAsResult(code, pstack, pstackPointer, Word);
		X86Emitter::parse(code, "mov ebx, eax");
		X86Emitter::setToMemaddr(code, pstackPointer, 1, Byte);
		X86Emitter::parse(code, "mov eax, ebx");
		
		
		X86Emitter::setToMemaddr(code, pstackPointer, 3, Byte);
		X86Emitter::loadArray_AregAsResult(code, pstack, pstackPointer, Word);
		X86Emitter::Lea(code, leaWithoutDispMode, Areg, x4, Areg, Areg);
		X86Emitter::parse(code, "add BYTE PTR [extra], eax", insertAddr(pstackPointer));
		
		//ends here
		X86Emitter::BlockFinisher(code);

		

	
	}

	void execute(){

		vect8* code = &bigcode.at(0);
		int result = 0;

		//print block
		for (int i = 0; i < code->size(); i++){
			printf("%02X ", code->at(i));
		}

#ifdef _WIN32

		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		auto const page_size = system_info.dwPageSize;

		// prepare the memory in which the machine code will be put (it's not executable yet):
		void* buffer = VirtualAlloc(nullptr, page_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		// copy the machine code into that memory:
		memcpy(buffer, code->data(), code->size());

		// mark the memory as executable:
		DWORD dummy;
		VirtualProtect(buffer, code->size(), PAGE_EXECUTE_READ, &dummy);

		// interpret the beginning of the (now) executable memory as the entry
		// point of a function taking no arguments and returning a 4-byte int:


		typedef int32_t(*dank)(void);
		using weed = int32_t(*)(void);
		dank function_ptr = (weed)buffer;

		//auto const function_ptr = reinterpret_cast<std::int32_t(*)()>(buffer);


		// call the function and store the result in a local std::int32_t object:
		result = function_ptr();

		// free the executable memory:
		VirtualFree(buffer, 0, MEM_RELEASE);

#else
		void *buffer = mmap(NULL,             // address
			4096,             // size
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1,               // fd (not used here)
			0);               // offset (not used here)

		memcpy(buffer, code->data(), code->size());
		
		typedef int32_t(*dank)(void);
		using weed = int32_t(*)(void);
		dank function_ptr = (weed)buffer;

		result = function_ptr();

#endif

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
	my->execute();
	return 0;
}