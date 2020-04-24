#include <windows.h>
#include <vector>
#include <iostream>
#include <cstring>
#include<cstdint>

int main()
{
	std::vector<uint8_t> const code =
	{
		0xb8,                   // move the following value to EAX:
		0x05, 0x00, 0x00, 0x00, // 5
		0xc3                    // return what's currently in EAX
	};

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

	// use your std::int32_t:
	std::cout << result << "\n";
}
