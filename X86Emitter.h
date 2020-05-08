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
			101: ch, bp, ebp (or disp if mod is 00)
			110: dh, si, esi
			111: bh, di, edi

	r/m (detailed list on what if mod is not 00):
			mod r/m
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

	ex) movzx eax, al -> 0F B6 C0 = 00001111 101101 1 0 11 000 000
									 extend  movzx  d s mod reg r/m

	ex) mov al, memoryaddr -> 88 05 ?? ?? ?? ?? = 100010 0 0 00 000 101 ????...
													mov  d s mod reg r/m
	ex) mov memoryaddr, al -> 8A 05 ?? ?? ?? ?? = 100010 1 0 00 000 101 ????...
													mov  d s mod reg r/m
	ex) mov al, imm -> B0 ?? = 101100 0 0 ????...
								mov   d s 
	ex) mov ax, imm -> 66 B8 ?? ?? = 01100110 101110 0 0 00 ????...
									prefix    mov    d s mod
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

	enum Direction{ srcToDest, destToSrc = 1 }; //Direction -> srcToDest = 0, destToSrc = 1
	enum Bitsize{ byteOnly, wordAndDword = 1 }; //Bitsize -> byteOnly = 0, wordAndDword = 1
	enum Mod{ forDisp = 0x0, byteSignedDisp = 0x1, dwordSignedDisp = 0x2, forReg = 0x3 }; //Mod -> forDisp = 00, byteSignedDisp = 01, dwordSignedDisp = 10, forReg = 11
	enum X86Regs{ Areg = 0x0, Creg = 0x1, Dreg = 0x2, Breg = 0x3, memaddr = 0x5 }; //X86Regs -> Areg = 000, Creg = 001, Dreg = 010, Breg = 011, memaddr = 101


	enum Movsize{ movByte, movWord, movDword }; //for mov only

	//disp or sib
	typedef struct _DispOrSib{
		union{
			uint8_t byte;
			uint16_t word;
			uint32_t dword = 0;
		};

		void operator=(uint32_t dword){
			this->dword = dword;
		}
		void operator=(uint16_t word){
			this->word = word;
		}
		void operator=(uint8_t byte){
			this->byte = byte;
		}
		
	} DispOrSib;
	DispOrSib dispOrSib;

	//byte
	void addExtension(){ addByte(0x0F); }
	//byte
	void addPrefix(){ addByte(0x66); }
	//byte
	void addOpcode(uint8_t opcode, Direction direction, Bitsize bitsize){
		uint8_t d = 0x0;
		uint8_t s = 0x0;
		if (direction == 1) d = 0x2;
		if (bitsize == 1) s = 0x1;
		opcode |= d;
		opcode |= s;
		addByte(opcode);
	}
	//byte
	void addModrm(Mod mod, X86Regs src, X86Regs dest){
		uint8_t opcode = 0x0;
		opcode |= ((uint8_t)mod << 6);
		opcode |= ((uint8_t)src << 3);
		opcode |= (uint8_t)dest;
		addByte(opcode);
	}


	//no need for the opposite(use only for zeroing out high area)
	void movzx(std::vector<uint8_t>* memoryBlock, Direction direction, Bitsize bitsize, Mod mod, X86Regs src, X86Regs dest){
		init(memoryBlock, 3);
		addExtension();
		addOpcode(0xB4, direction, bitsize); //10110100
		addModrm(mod, src, dest);
	}
	//convert byte to dword
	void movzx_al_to_eax(std::vector<uint8_t>* memoryBlock){ movzx(memoryBlock, destToSrc, byteOnly, forReg, Areg, Areg); }
	void movzx_bl_to_ebx(std::vector<uint8_t>* memoryBlock){ movzx(memoryBlock, destToSrc, byteOnly, forReg, Breg, Breg); }
	void movzx_cl_to_ecx(std::vector<uint8_t>* memoryBlock){ movzx(memoryBlock, destToSrc, byteOnly, forReg, Creg, Creg); }
	void movzx_dl_to_edx(std::vector<uint8_t>* memoryBlock){ movzx(memoryBlock, destToSrc, byteOnly, forReg, Dreg, Dreg); }
	
	//convert word to dword
	void movzx_ax_to_eax(std::vector<uint8_t>* memoryBlock){ movzx(memoryBlock, destToSrc, wordAndDword, forReg, Areg, Areg); }
	void movzx_bx_to_ebx(std::vector<uint8_t>* memoryBlock){ movzx(memoryBlock, destToSrc, wordAndDword, forReg, Breg, Breg); }
	void movzx_cx_to_ecx(std::vector<uint8_t>* memoryBlock){ movzx(memoryBlock, destToSrc, wordAndDword, forReg, Creg, Creg); }
	void movzx_dx_to_edx(std::vector<uint8_t>* memoryBlock){ movzx(memoryBlock, destToSrc, wordAndDword, forReg, Dreg, Dreg); }


	//memoryaddr must always be dword!
	void mov(std::vector<uint8_t>* memoryBlock, Movsize getMovsize, Direction direction, Bitsize bitsize, Mod mod, X86Regs src, X86Regs dest){
		if ((getMovsize == movByte || getMovsize == movDword) && dest == memaddr) 
			init(memoryBlock, 6);
		else if ((getMovsize == movWord) && dest == memaddr){ 
			init(memoryBlock, 7);
			addPrefix();
		}

		else if (getMovsize == movByte || getMovsize == movDword) 
			init(memoryBlock, 2);
		else if (getMovsize == movWord){
			init(memoryBlock, 3); 
			addPrefix();
		}
		
		addOpcode(0x88, direction, bitsize); //10001000
		addModrm(mod, src, dest);

		if (dest == memaddr) addDword(dispOrSib.dword);
	}
	//load store byte
	void mov_memoryaddr_to_al(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movByte, destToSrc, byteOnly, forDisp, Areg, memaddr); }
	void mov_al_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Areg, memaddr); }
	void mov_memoryaddr_to_bl(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movByte, destToSrc, byteOnly, forDisp, Breg, memaddr); }
	void mov_bl_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Breg, memaddr); }
	void mov_memoryaddr_to_cl(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movByte, destToSrc, byteOnly, forDisp, Creg, memaddr); }
	void mov_cl_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Creg, memaddr); }
	void mov_memoryaddr_to_dl(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movByte, destToSrc, byteOnly, forDisp, Dreg, memaddr); }
	void mov_dl_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Dreg, memaddr); }

	//load store word
	void mov_memoryaddr_to_ax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movWord, destToSrc, wordAndDword, forDisp, Areg, memaddr); }
	void mov_ax_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Areg, memaddr); }
	void mov_memoryaddr_to_bx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movWord, destToSrc, wordAndDword, forDisp, Breg, memaddr); }
	void mov_bx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Breg, memaddr); }
	void mov_memoryaddr_to_cx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movWord, destToSrc, wordAndDword, forDisp, Creg, memaddr); }
	void mov_cx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Creg, memaddr); }
	void mov_memoryaddr_to_dx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movWord, destToSrc, wordAndDword, forDisp, Dreg, memaddr); }
	void mov_dx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Dreg, memaddr); }

	//load store dword
	void mov_memoryaddr_to_eax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movDword, destToSrc, wordAndDword, forDisp, Areg, memaddr); }
	void mov_eax_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Areg, memaddr); }
	void mov_memoryaddr_to_ebx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movDword, destToSrc, wordAndDword, forDisp, Breg, memaddr); }
	void mov_ebx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Breg, memaddr); }
	void mov_memoryaddr_to_ecx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movDword, destToSrc, wordAndDword, forDisp, Creg, memaddr); }
	void mov_ecx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Creg, memaddr); }
	void mov_memoryaddr_to_edx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movDword, destToSrc, wordAndDword, forDisp, Dreg, memaddr); }
	void mov_edx_to_memoryaddr(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Dreg, memaddr); }

	//store to regaddr byte (AB <-> CD)

	//100010 0 0 00 000 011
	//opcode d s mod src dst
	void mov_al_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Areg, Breg); }
	void mov_al_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Areg, Creg); }
	void mov_al_to_edxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Areg, Dreg); }

	void mov_bl_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Breg, Areg); }
	void mov_bl_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Breg, Creg); }
	void mov_bl_to_edxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Breg, Dreg); }

	void mov_cl_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Creg, Areg); }
	void mov_cl_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Creg, Breg); }
	void mov_cl_to_edxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Creg, Dreg); }

	void mov_dl_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Dreg, Areg); }
	void mov_dl_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Dreg, Breg); }
	void mov_dl_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movByte, srcToDest, byteOnly, forDisp, Dreg, Creg); }

	//store to regaddr word (AB <-> CD)
	//prefix 100010 0 1 00 000 011
	void mov_ax_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Areg, Breg); }
	void mov_ax_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Areg, Creg); }
	void mov_ax_to_edxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Areg, Dreg); }

	void mov_bx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Breg, Areg); }
	void mov_bx_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Breg, Creg); }
	void mov_bx_to_edxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Breg, Dreg); }

	void mov_cx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Creg, Areg); }
	void mov_cx_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Creg, Breg); }
	void mov_cx_to_edxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Creg, Dreg); }

	void mov_dx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Dreg, Areg); }
	void mov_dx_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Dreg, Breg); }
	void mov_dx_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movWord, srcToDest, wordAndDword, forDisp, Dreg, Creg); }

	//store to regaddr dword (AB <-> CD)
	//100010 0 1 00 000 011
	void mov_eax_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Areg, Breg); }
	void mov_eax_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Areg, Creg); }
	void mov_eax_to_edxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Areg, Dreg); }

	void mov_ebx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Breg, Areg); }
	void mov_ebx_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Breg, Creg); }
	void mov_ebx_to_edxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Breg, Dreg); }

	void mov_ecx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Creg, Areg); }
	void mov_ecx_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Creg, Breg); }
	void mov_ecx_to_edxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Creg, Dreg); }

	void mov_edx_to_eaxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Dreg, Areg); }
	void mov_edx_to_ebxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Dreg, Breg); }
	void mov_edx_to_ecxaddr(std::vector<uint8_t>* memoryBlock){ mov(memoryBlock, movDword, srcToDest, wordAndDword, forDisp, Dreg, Creg); }


	//kinda different - use Direction and Bitsize to point reg
	void mov_imm(std::vector<uint8_t>* memoryBlock, Movsize movsize, Direction direction, Bitsize bitsize){
		if (movsize == movByte){
			init(memoryBlock, 2);
			addOpcode(0xB0, direction, bitsize);
			addByte(dispOrSib.byte);
		}
		else if (movsize == movWord){
			init(memoryBlock, 4); 
			addPrefix(); 
			addOpcode(0xB8, direction, bitsize); 
			addWord(dispOrSib.word);
		}
		else if (movsize == movDword){
			init(memoryBlock, 5);
			addOpcode(0xB8, direction, bitsize);
			addDword(dispOrSib.dword);
		}
	}
	//load immediate byte
	void mov_imm_to_al(std::vector<uint8_t>* memoryBlock, uint8_t byte){ dispOrSib = byte; mov_imm(memoryBlock, movByte, srcToDest, byteOnly); }
	void mov_imm_to_bl(std::vector<uint8_t>* memoryBlock, uint8_t byte){ dispOrSib = byte; mov_imm(memoryBlock, movByte, destToSrc, wordAndDword); }
	void mov_imm_to_cl(std::vector<uint8_t>* memoryBlock, uint8_t byte){ dispOrSib = byte; mov_imm(memoryBlock, movByte, srcToDest, wordAndDword); }
	void mov_imm_to_dl(std::vector<uint8_t>* memoryBlock, uint8_t byte){ dispOrSib = byte; mov_imm(memoryBlock, movByte, destToSrc, byteOnly); }

	//load immediate word
	void mov_imm_to_ax(std::vector<uint8_t>* memoryBlock, uint16_t word){ dispOrSib = word; mov_imm(memoryBlock, movWord, srcToDest, byteOnly); }
	void mov_imm_to_bx(std::vector<uint8_t>* memoryBlock, uint16_t word){ dispOrSib = word; mov_imm(memoryBlock, movWord, destToSrc, wordAndDword); }
	void mov_imm_to_cx(std::vector<uint8_t>* memoryBlock, uint16_t word){ dispOrSib = word; mov_imm(memoryBlock, movWord, srcToDest, wordAndDword); }
	void mov_imm_to_dx(std::vector<uint8_t>* memoryBlock, uint16_t word){ dispOrSib = word; mov_imm(memoryBlock, movWord, destToSrc, byteOnly); }

	//load immediate dword
	void mov_imm_to_eax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov_imm(memoryBlock, movDword, srcToDest, byteOnly); }
	void mov_imm_to_ebx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov_imm(memoryBlock, movDword, destToSrc, wordAndDword); }
	void mov_imm_to_ecx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov_imm(memoryBlock, movDword, srcToDest, wordAndDword); }
	void mov_imm_to_edx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; mov_imm(memoryBlock, movDword, destToSrc, byteOnly); }


	//easy shortcut to load to register
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




	//dword add
	void dword_add(std::vector<uint8_t>* memoryBlock, Direction direction, Bitsize bitsize, Mod mod, X86Regs src, X86Regs dest){
		init(memoryBlock, 2);
		addOpcode(0x00, direction, bitsize); //000000
		addModrm(mod, src, dest);
	}
	//add regs dword (AB <-> CD)

	//000000 0 1 11 000 011
	void add_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Breg); }
	void add_eax_to_ecx(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Creg); }
	void add_eax_to_edx(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Dreg); }

	void add_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Breg, Areg); }
	void add_ebx_to_ecx(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Breg, Creg); }
	void add_ebx_to_edx(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Breg, Dreg); }

	void add_ecx_to_eax(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Creg, Areg); }
	void add_ecx_to_ebx(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Creg, Breg); }
	void add_ecx_to_edx(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Creg, Dreg); }

	void add_edx_to_eax(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Dreg, Areg); }
	void add_edx_to_ebx(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Dreg, Breg); }
	void add_edx_to_ecx(std::vector<uint8_t>* memoryBlock){ dword_add(memoryBlock, srcToDest, wordAndDword, forReg, Dreg, Creg); }

	//dword imm add
	void dword_add_imm(std::vector<uint8_t>* memoryBlock, Direction direction, Bitsize bitsize, Mod mod, X86Regs src, X86Regs dest){
		init(memoryBlock, 6);
		addOpcode(0x80, direction, bitsize);	//100000
		addModrm(mod, src, dest);
		addDword(dispOrSib.dword);
	}
	//add imm dword (AB <-> CD)
	//100000 0 1 11 000(?) 000 disp32
	void add_imm_to_eax(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; dword_add_imm(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Areg); }
	void add_imm_to_ebx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; dword_add_imm(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Breg); }
	void add_imm_to_ecx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; dword_add_imm(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Creg); }
	void add_imm_to_edx(std::vector<uint8_t>* memoryBlock, uint32_t dword){ dispOrSib = dword; dword_add_imm(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Dreg); }
	
	//dword inc
	void inc(std::vector<uint8_t>* memoryBlock, Direction direction, Bitsize bitsize){
		init(memoryBlock, 1);
		addOpcode(0x40, direction, bitsize);
	}
	//inc dword
	//010000 0 0
	void inc_eax(std::vector<uint8_t>* memoryBlock){ inc(memoryBlock, srcToDest, byteOnly); }
	void inc_ebx(std::vector<uint8_t>* memoryBlock){ inc(memoryBlock, destToSrc, wordAndDword); }
	void inc_ecx(std::vector<uint8_t>* memoryBlock){ inc(memoryBlock, srcToDest, wordAndDword); }
	void inc_edx(std::vector<uint8_t>* memoryBlock){ inc(memoryBlock, destToSrc, byteOnly); }

	//dword sub
	void dword_sub(std::vector<uint8_t>* memoryBlock, Direction direction, Bitsize bitsize, Mod mod, X86Regs src, X86Regs dest){
		init(memoryBlock, 2);
		addOpcode(0x28, direction, bitsize); //001010
		addModrm(mod, src, dest);
	}
	//sub dword (AB <-> CD)
	//001010 0 1 11 000 011
	void sub_eax_to_ebx(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Breg); }
	void sub_eax_to_ecx(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Creg); }
	void sub_eax_to_edx(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Areg, Dreg); }
	
	void sub_ebx_to_eax(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Breg, Areg); }
	void sub_ebx_to_ecx(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Breg, Creg); }
	void sub_ebx_to_edx(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Breg, Dreg); }
	
	void sub_ecx_to_eax(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Creg, Areg); }
	void sub_ecx_to_ebx(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Creg, Breg); }
	void sub_ecx_to_edx(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Creg, Dreg); }

	void sub_edx_to_eax(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Dreg, Areg); }
	void sub_edx_to_ebx(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Dreg, Breg); }
	void sub_edx_to_ecx(std::vector<uint8_t>* memoryBlock){ dword_sub(memoryBlock, srcToDest, wordAndDword, forReg, Dreg, Creg); }

	//dword dec
	void dec(std::vector<uint8_t>* memoryBlock, Direction direction, Bitsize bitsize){
		init(memoryBlock, 1);
		addOpcode(0x48, direction, bitsize);	//010010 0 0
	}
	//dec dword
	void dec_eax(std::vector<uint8_t>* memoryBlock){ dec(memoryBlock, srcToDest, byteOnly); }
	void dec_ebx(std::vector<uint8_t>* memoryBlock){ dec(memoryBlock, destToSrc, wordAndDword); }
	void dec_ecx(std::vector<uint8_t>* memoryBlock){ dec(memoryBlock, srcToDest, wordAndDword); }
	void dec_edx(std::vector<uint8_t>* memoryBlock){ dec(memoryBlock, destToSrc, byteOnly); }


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

