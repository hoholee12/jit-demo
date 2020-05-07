#pragma once

/*
	super simple and easy to understand x86 emitter

	https://wiki.osdev.org/X86-64_Instruction_Encoding#Legacy_Prefixes
	check this for detailed x86 memory structures and stuff^
*/

#include<iostream>
#include<vector>
#include<stdint.h>

class X86Emitter{
public:

	std::vector<uint8_t>* memoryBlock;
	uint8_t byte0, byte1, byte2, byte3;
	void convertEndianness(uint16_t word){
		byte0 = (word & 0x00ff);
		byte1 = (word & 0xff00) >> 8;
	}
	void convertEndianness(uint32_t dword){
		byte0 = (dword & 0x000000ff);
		byte1 = (dword & 0x0000ff00) >> 8;
		byte2 = (dword & 0x00ff0000) >> 16;
		byte3 = (dword & 0xff000000) >> 24;
	}

	void addByte(uint8_t byte){
		memoryBlock->push_back(byte);
	}
	void addWord(uint16_t word){
		convertEndianness(word);
		memoryBlock->push_back(byte0);
		memoryBlock->push_back(byte1);
	}
	void addDword(uint32_t dword){
		convertEndianness(dword);
		memoryBlock->push_back(byte0);
		memoryBlock->push_back(byte1);
		memoryBlock->push_back(byte2);
		memoryBlock->push_back(byte3);
	}

	//get memoryBlock from outside
	//memoryBlock optimization
	void init(std::vector<uint8_t>* inputMemoryBlock, int index = 0){
		memoryBlock = inputMemoryBlock;
		memoryBlock->reserve(memoryBlock->capacity() + index);
		byte0 = byte1 = byte2 = byte3 = 0;
	}

	//memoryval must always be dword!
	//load to register
	void loadByteToDwordRegA(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_al(memoryBlock, dword); movzx_al_to_eax(memoryBlock); }
	void loadWordToDwordRegA(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_ax(memoryBlock, dword); movzx_ax_to_eax(memoryBlock); }
	void loadDwordToDwordRegA(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_eax(memoryBlock, dword); }
	void loadByteToDwordRegB(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_bl(memoryBlock, dword); movzx_bl_to_ebx(memoryBlock); }
	void loadWordToDwordRegB(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_bx(memoryBlock, dword); movzx_bx_to_ebx(memoryBlock); }
	void loadDwordToDwordRegB(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_ebx(memoryBlock, dword); }


	//convert byte to dword
	void movzx_al_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB6); addByte(0xC0); }
	void movzx_bl_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB6); addByte(0xDB); }
	//convert word to dword
	void movzx_ax_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB7); addByte(0xC0); }
	void movzx_bx_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB7); addByte(0xDB); }
	//no need for the opposite


	//memoryaddr must always be dword!
	//load store byte
	void mov_memoryaddr_to_al(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xA0); addDword(dword); }
	void mov_al_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xA2); addDword(dword); }
	void mov_memoryaddr_to_bl(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8A); addByte(0x1D); addDword(dword); }
	void mov_bl_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x88); addByte(0x1D); addDword(dword); }
	//load store word
	void mov_memoryaddr_to_ax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x66); addByte(0xA1); addDword(dword); }
	void mov_ax_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x66); addByte(0xA3); addDword(dword); }
	void mov_memoryaddr_to_bx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x8B); addByte(0x1D); addDword(dword); }
	void mov_bx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x89); addByte(0x1D); addDword(dword); }
	//load store dword
	void mov_memoryaddr_to_eax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xA1); addDword(dword); }
	void mov_eax_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xA3); addDword(dword); }
	void mov_memoryaddr_to_ebx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8B); addByte(0x1D); addDword(dword); }
	void mov_ebx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x89); addByte(0x1D); addDword(dword); }


	//load immediate byte
	void mov_imm_to_al(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 2); addByte(0xB0); addByte(byte); }
	void mov_imm_to_bl(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 2); addByte(0xB3); addByte(byte); }
	//load immediate word
	void mov_imm_to_ax(std::vector<uint8_t>* memoryBlock, uint16_t word){ init(memoryBlock, 4); addByte(0x66); addByte(0xB8); addWord(word); }
	void mov_imm_to_bx(std::vector<uint8_t>* memoryBlock, uint16_t word){ init(memoryBlock, 4); addByte(0x66); addByte(0xBB); addWord(word); }
	//load immediate dword
	void mov_imm_to_eax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xB8); addDword(dword); }
	void mov_imm_to_ebx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xBB); addDword(dword); }


	//store to regaddr byte
	void mov_al_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x03); }
	void mov_bl_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x18); }
	//store to regaddr word
	void mov_ax_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x66);  addByte(0x89); addByte(0x03); }
	void mov_bx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x66);  addByte(0x89); addByte(0x18); }
	//store to regaddr dword
	void mov_eax_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x03); }
	void mov_ebx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x18); }


	//add dword
	void add_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xC3); }
	void add_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xD8); }
	void add_imm_to_eax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x03); addByte(0x05); addDword(dword); }
	void add_imm_to_ebx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x03); addByte(0x1D); addDword(dword); }
	void inc_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x40); }
	void inc_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x43); }
	//sub dword
	void sub_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x29); addByte(0xC3); }
	void sub_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x29); addByte(0xD8); }
	void dec_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x48); }
	void dec_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x4B); }


	//bitwise and dword
	void and_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x21); addByte(0xC3); }
	void and_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x21); addByte(0xD8); }
	//bitwise or dword
	void or_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x09); addByte(0xC3); }
	void or_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x09); addByte(0xD8); }
	//bitwose xor dword
	void xor_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x31); addByte(0xC3); }
	void xor_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x31); addByte(0xD8); }
	//bitwise shl by immediate dword
	void shl_imm_eax(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xE0); addByte(byte); }
	void shl_imm_ebx(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xE3); addByte(byte); }
	//bitwise shr by immediate dword
	void shr_imm_eax(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xE8); addByte(byte); }
	void shr_imm_ebx(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xEB); addByte(byte); }


	//return from eax
	void ret(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0xC3); }

};

