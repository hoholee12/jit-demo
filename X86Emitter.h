#pragma once

/*
	super simple and easy to understand x86 emitter

	http://www.c-jump.com/CIS77/CPU/x86/lecture.html
	^check this for detailed x86 opcode structures and stuff^

	x86 structure:
	prefix 0,4bytes | opcode 1,2bytes | modrm 0,1byte | sib 0,1byte | displacement 0,1,2,4bytes | immediate 0,1,2,4bytes	= 1 ~ 16 bytes long

	prefix -> idgaf

	opcode -> 1byte opcode(000000 + d + s) or 0x0F(extension) + 1byte opcode(000000 + d + s)
	000000 -> opcode(for example this is add)
	d ->	0: reg->r/m
			1: r/m->reg
	s ->	0: byte
			1: word or dword


	modrm -> mod 2b | reg 3b | r/m 3b	= 1byte
	
	mod ->	00: register indirect addressing, or sib with no disp(lacement)(r/m = 100), or displacement only addressing(r/m = 101)
			01: 1byte signed disp follows addressing bytes 
			10: 4byte signed disp follows addressing bytes 
			11: register addressing

	reg ->	000: Areg
			001: Creg
			010: Dreg
			011: Breg
			100: ah, sp, esp
			101: ch, bp, ebp
			110: dh, si, esi
			111: bh, di, edi

	r/m ->	MOD R/M Addressing Mode
			 00 000 [ eax ]
			 01 000 [ eax + disp8 ]              
			 10 000 [ eax + disp32 ]
			 11 000 register  ( al / ax / eax )  
			 00 001 [ ecx ]
			 01 001 [ ecx + disp8 ]
			 10 001 [ ecx + disp32 ]
			 11 001 register  ( cl / cx / ecx )
			 00 010 [ edx ]
			 01 010 [ edx + disp8 ]
			 10 010 [ edx + disp32 ]
			 11 010 register  ( dl / dx / edx )
			 00 011 [ ebx ]
			 01 011 [ ebx + disp8 ]
			 10 011 [ ebx + disp32 ]
			 11 011 register  ( bl / bx / ebx )
			 00 100 SIB  Mode                     
			 01 100 SIB  +  disp8  Mode
			 10 100 SIB  +  disp32  Mode
			 11 100 register  ( ah / sp / esp )
			 00 101 32-bit Displacement-Only Mode
			 01 101 [ ebp + disp8 ]
			 10 101 [ ebp + disp32 ]
			 11 101 register  ( ch / bp / ebp )
			 00 110 [ esi ]
			 01 110 [ esi + disp8 ]
			 10 110 [ esi + disp32 ]
			 11 110 register  ( dh / si / esi )
			 00 111 [ edi ]
			 01 111 [ edi + disp8 ]
			 10 111 [ edi + disp32 ]
			 11 111 register  ( bh / di / edi )


	sib(scaled index byte) -> scale 2b | index 3b | base 3b		= 1byte
	
	scale ->	00: index * 1
				01: index * 2
				10: index * 4
				11: index * 8

	index ->	000: eax
				001: ecx
				010: edx
				011: ebx
				100: illegal
				101: ebp
				110: esi
				111: edi

	base ->		000: eax
				001: ecx
				010: edx
				011: ebx
				100: esp
				101: disp if mod = 00, ebp if mod = 01 or 10
				110: esi
				111: edi


	ex) add cl, al ->	000000 00 11 000 001	= 00 C1

	opcode ->	000000: 1byte add
	d ->		0: reg->mem
	s ->		0: byte (l)
	mod ->		11: r/m is a register
	reg ->		000: src Areg (al)
	r/m ->		001: dest Creg (cl)

	ex) add ecx, eax ->	000000 01 11 000 001	= 01 C1
	ex) add edx, disp -> 000000 11 00 011 101 disp32	= 03 1D ?? ?? ?? ??
	ex) add edi, [ebx] -> 000000 11 00 111 011		= 03 3B

*/

#include<iostream>
#include<vector>
#include<stdint.h>

class X86Emitter{
private:

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

public:
	//memoryval must always be dword!
	//load to register
	void loadByteToDwordRegA(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_al(memoryBlock, dword); movzx_al_to_eax(memoryBlock); }
	void loadWordToDwordRegA(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_ax(memoryBlock, dword); movzx_ax_to_eax(memoryBlock); }
	void loadDwordToDwordRegA(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_eax(memoryBlock, dword); }

	void loadByteToDwordRegB(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_bl(memoryBlock, dword); movzx_bl_to_ebx(memoryBlock); }
	void loadWordToDwordRegB(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_bx(memoryBlock, dword); movzx_bx_to_ebx(memoryBlock); }
	void loadDwordToDwordRegB(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_ebx(memoryBlock, dword); }
	
	void loadByteToDwordRegC(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_cl(memoryBlock, dword); movzx_cl_to_ecx(memoryBlock); }
	void loadWordToDwordRegC(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_cx(memoryBlock, dword); movzx_cx_to_ecx(memoryBlock); }
	void loadDwordToDwordRegC(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_ecx(memoryBlock, dword); }
	
	void loadByteToDwordRegD(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_dl(memoryBlock, dword); movzx_dl_to_edx(memoryBlock); }
	void loadWordToDwordRegD(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_dx(memoryBlock, dword); movzx_dx_to_edx(memoryBlock); }
	void loadDwordToDwordRegD(std::vector<uint8_t>* memoryBlock, uint32_t dword){ mov_memoryaddr_to_edx(memoryBlock, dword); }


	//no need for the opposite(use only for zeroing out high area)
	//convert byte to dword
	void movzx_al_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB6); addByte(0xC0); }
	void movzx_bl_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB6); addByte(0xDB); }
	void movzx_cl_to_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB6); addByte(0xC9); }
	void movzx_dl_to_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB6); addByte(0xD2); }
	
	//convert word to dword
	void movzx_ax_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB7); addByte(0xC0); }
	void movzx_bx_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB7); addByte(0xDB); }
	void movzx_cx_to_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB7); addByte(0xC9); }
	void movzx_dx_to_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 3); addByte(0x0F); addByte(0xB7); addByte(0xD2); }


	//memoryaddr must always be dword!
	//load store byte
	void mov_memoryaddr_to_al(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8A); addByte(0x05); addDword(dword); }
	void mov_al_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x88); addByte(0x05); addDword(dword); }
	void mov_memoryaddr_to_bl(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8A); addByte(0x1D); addDword(dword); }
	void mov_bl_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x88); addByte(0x1D); addDword(dword); }
	void mov_memoryaddr_to_cl(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8A); addByte(0x0D); addDword(dword); }
	void mov_cl_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x88); addByte(0x0D); addDword(dword); }
	void mov_memoryaddr_to_dl(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8A); addByte(0x15); addDword(dword); }
	void mov_dl_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x88); addByte(0x15); addDword(dword); }
	//load store word
	void mov_memoryaddr_to_ax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x8B); addByte(0x05); addDword(dword); }
	void mov_ax_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x89); addByte(0x05); addDword(dword); }
	void mov_memoryaddr_to_bx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x8B); addByte(0x1D); addDword(dword); }
	void mov_bx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x89); addByte(0x1D); addDword(dword); }
	void mov_memoryaddr_to_cx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x8B); addByte(0x0D); addDword(dword); }
	void mov_cx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x89); addByte(0x0D); addDword(dword); }
	void mov_memoryaddr_to_dx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x8B); addByte(0x15); addDword(dword); }
	void mov_dx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 7); addByte(0x66); addByte(0x89); addByte(0x15); addDword(dword); }
	//load store dword
	void mov_memoryaddr_to_eax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8B); addByte(0x05); addDword(dword); }
	void mov_eax_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x89); addByte(0x05); addDword(dword); }
	void mov_memoryaddr_to_ebx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8B); addByte(0x1D); addDword(dword); }
	void mov_ebx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x89); addByte(0x1D); addDword(dword); }
	void mov_memoryaddr_to_ecx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8B); addByte(0x0D); addDword(dword); }
	void mov_ecx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x89); addByte(0x0D); addDword(dword); }
	void mov_memoryaddr_to_edx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x8B); addByte(0x15); addDword(dword); }
	void mov_edx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x89); addByte(0x15); addDword(dword); }


	//load immediate byte
	//imm al 1011 00 00
	//imm bl 1011 00 11
	//imm cl 1011 00 01
	//imm dl 1011 00 10
	void mov_imm_to_al(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 2); addByte(0xB0); addByte(byte); }
	void mov_imm_to_bl(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 2); addByte(0xB3); addByte(byte); }
	void mov_imm_to_cl(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 2); addByte(0xB1); addByte(byte); }
	void mov_imm_to_dl(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 2); addByte(0xB2); addByte(byte); }
	//load immediate word
	void mov_imm_to_ax(std::vector<uint8_t>* memoryBlock, uint16_t word){ init(memoryBlock, 4); addByte(0x66); addByte(0xB8); addWord(word); }
	void mov_imm_to_bx(std::vector<uint8_t>* memoryBlock, uint16_t word){ init(memoryBlock, 4); addByte(0x66); addByte(0xBB); addWord(word); }
	void mov_imm_to_cx(std::vector<uint8_t>* memoryBlock, uint16_t word){ init(memoryBlock, 4); addByte(0x66); addByte(0xB9); addWord(word); }
	void mov_imm_to_dx(std::vector<uint8_t>* memoryBlock, uint16_t word){ init(memoryBlock, 4); addByte(0x66); addByte(0xBA); addWord(word); }
	//load immediate dword
	void mov_imm_to_eax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xB8); addDword(dword); }
	void mov_imm_to_ebx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xBB); addDword(dword); }
	void mov_imm_to_ecx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xB9); addDword(dword); }
	void mov_imm_to_edx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 5); addByte(0xBA); addDword(dword); }


	//store to regaddr byte (AB <-> CD)
	void mov_al_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x03); }
	void mov_al_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x01); }
	void mov_al_to_edxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x02); }

	void mov_bl_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x18); }
	void mov_bl_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x19); }
	void mov_bl_to_edxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x1A); }
	
	void mov_cl_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x08); }
	void mov_cl_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x0B); }
	void mov_cl_to_edxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x0A); }
	
	void mov_dl_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x10); }
	void mov_dl_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x13); }
	void mov_dl_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x88); addByte(0x11); }
	
	//store to regaddr word (AB <-> CD)
	void mov_ax_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x03); }
	void mov_ax_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x01); }
	void mov_ax_to_edxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x02); }

	void mov_bx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x18); }
	void mov_bx_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x19); }
	void mov_bx_to_edxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x1A); }

	void mov_cx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x08); }
	void mov_cx_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x0B); }
	void mov_cx_to_edxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x0A); }

	void mov_dx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x10); }
	void mov_dx_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x13); }
	void mov_dx_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x66); addByte(0x89); addByte(0x11); }

	//store to regaddr dword (AB <-> CD)
	void mov_eax_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x03); }
	void mov_eax_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x01); }
	void mov_eax_to_edxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x02); }

	void mov_ebx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x18); }
	void mov_ebx_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x19); }
	void mov_ebx_to_edxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x1A); }

	void mov_ecx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x08); }
	void mov_ecx_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x0B); }
	void mov_ecx_to_edxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x0A); }

	void mov_edx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x10); }
	void mov_edx_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x13); }
	void mov_edx_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x89); addByte(0x11); }


	//add regs dword (AB <-> CD)
	void add_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xC3); }
	void add_eax_to_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xC1); }
	void add_eax_to_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xC2); }

	void add_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xD8); }
	void add_ebx_to_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xD9); }
	void add_ebx_to_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xDA); }

	void add_ecx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xC8); }
	void add_ecx_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xCB); }
	void add_ecx_to_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xCA); }

	void add_edx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xD0); }
	void add_edx_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xD3); }
	void add_edx_to_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x01); addByte(0xD1); }

	//add imm dword (AB <-> CD)
	void add_imm_to_eax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x81); addByte(0xC0); addDword(dword); }
	void add_imm_to_ebx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x81); addByte(0xC3); addDword(dword); }
	void add_imm_to_ecx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x81); addByte(0xC1); addDword(dword); }
	void add_imm_to_edx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ init(memoryBlock, 6); addByte(0x81); addByte(0xC2); addDword(dword); }
	//inc dword
	void inc_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x40); }
	void inc_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x43); }
	void inc_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x41); }
	void inc_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x42); }
	//sub dword (AB <-> CD)
	void sub_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x29); addByte(0xC3); }
	void sub_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x29); addByte(0xD8); }
	void sub_ecx_to_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x29); addByte(0xCA); }
	void sub_edx_to_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x29); addByte(0xD1); }
	//dec dword
	void dec_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x48); }
	void dec_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x4B); }
	void dec_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x49); }
	void dec_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0x4A); }


	//bitwise and dword (AB <-> CD)
	void and_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x21); addByte(0xC3); }
	void and_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x21); addByte(0xD8); }
	void and_ecx_to_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x21); addByte(0xCA); }
	void and_edx_to_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x21); addByte(0xD1); }
	//bitwise or dword (AB <-> CD)
	void or_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x09); addByte(0xC3); }
	void or_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x09); addByte(0xD8); }
	void or_ecx_to_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x09); addByte(0xCA); }
	void or_edx_to_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x09); addByte(0xD1); }
	//bitwose xor dword (AB <-> CD)
	void xor_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x31); addByte(0xC3); }
	void xor_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x31); addByte(0xD8); }
	void xor_ecx_to_edx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x31); addByte(0xCA); }
	void xor_edx_to_ecx(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 2); addByte(0x31); addByte(0xD1); }
	//bitwise shl by immediate dword
	void shl_imm_eax(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xE0); addByte(byte); }
	void shl_imm_ebx(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xE3); addByte(byte); }
	void shl_imm_ecx(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xE1); addByte(byte); }
	void shl_imm_edx(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xE2); addByte(byte); }
	//bitwise shr by immediate dword
	void shr_imm_eax(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xE8); addByte(byte); }
	void shr_imm_ebx(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xEB); addByte(byte); }
	void shr_imm_ecx(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xE9); addByte(byte); }
	void shr_imm_edx(std::vector<uint8_t>* memoryBlock, uint8_t byte){ init(memoryBlock, 3); addByte(0xC1); addByte(0xEA); addByte(byte); }


	//return from eax
	void ret(std::vector<uint8_t>* memoryBlock){ init(memoryBlock, 1); addByte(0xC3); }

};

