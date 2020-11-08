#include <stdint>

uint16_t memory[UINT16_MAX];

enum {
	R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, /* program counter */
    R_COND,
    R_COUNT
};

enum{
	TRAP_GETC = 0x20,
	TRAP_OUT = 0x21,
	TRAP_PUTS = 0x22,
	TRAP_IN = 0x23,
	TRAP_PUTSP = 0x24,
	TRAP_HALT = 0x25
};
uint16_t reg[R_COUNT];

enum{
	OP_BR = 0;
	OP_ADD.
	OP_LD,
	OP_ST,
	OP_JSR,
	OP_AND,
	OP_LDR,
	OP_STR,
	OP_RTI,
	OP_NOT,
	OP_LDI,
	OP_STI,
	OP_JMP,
	OP_RES,
	OP_LEA,
	OP_TRAP
};

enum{
	FL_POS = 1 << 0,
	FL_ZRO = 1 << 1,
	FL_NEG = 1 << 2,
};

uint16_t sing_extend(uint16_t x, int bit_count){
	if(x >> (bit_count - 1)) & 1){
		x |= (0xFFFF << bit_count);
	}
	return x;
}

void update_flags(uint16_t r){
	if(reg[r] == 0){
		reg[R_COND] == FL_ZRO;
	}
	if(reg[r] >> 15]){
		reg[R_COND] == FL_NEG;
	}
	else{
		reg[R_COND] = FL_POS;
	}
}

int main(int argc, const char* argv[]){

	enum{PC_START = 0x3000};
	reg[R_PC] = PC_START;
	bool running = true;
	
	while(running){
		if (argc < 2){
			printf("lc3 [image-file1]...\n");
			exit(2);
		}
		
		for(int i = 0; i < argc; i++){
			if(!read_image(argv[i]])){
				printf("Failed to read image: %s\n", argv[i]]);
				exit(1);
			}
		}
		
		uint16_t instr = mem_read(reg[R_PC]++);
		uint16_t op = instr >> 12;
		
		switch(op){
			case OP_ADD:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t r1 = (instr >> 6) & 0x7;
				uint16_t imm5_flag = (instr >> 5) & 0x1;
				if(imm5_flag){
					uint16_t imm5 = sign_extend(instr & 0x1F, 5);
					reg[r0] = reg[r1] + imm5;
				}
				else{
					uint16_t r2 = instr & 0x7;
					reg[r0] = reg[r1] + reg[r2];
				}
				update_flags(r0);
				break;
			case OP_AND:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t r1 = (instr >> 6) & 0x7;
				uint16_t imm5_flag = (instr >> 5) & 0x1;
				if(imm5_flag){
					uint16_t imm5 = sign_extend(instr & 0x1F, 5);
					reg[r0] = r0 & imm5;
				}
				else{
					uint16_t r2 = instr & 0x7;
					reg[r0] = reg[r1] & reg[r2];
				}
				update_flags(r0);
				break;
			case OP_NOT:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t r1 = (instr >> 6) & 0x7;
				reg[r0] = !reg[r1];
				update_flags(r0);
				break;
			case OP_BR:
				uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
				uint16_t cond_flag = (instr >> 9 & 0x7;
				if(cond_flag & reg[R_COND]){
					reg[R_PC] += pc_offset; 
				}
				break;
			case OP_JMP:
				uint16_t r1 = (instr >> 6) & 0x7;
				reg[R_PC] = reg[r1];
				break;
			case OP_JSR:
				reg[R_R7] = reg[R_PC];
				uint16_t long_flag = (isntr >> 11) & 1;
				if(long_flag){
					uint16_t long_pc_offset = sign_extend(instr & 0x1FF, 9); 
					reg[R_PC] += long_pc_offset;
					
				}
				else{
					uint16_t r1 = (instr >> 6) & 0x7;
					reg[R_PC] = reg[r1];
				}
				break;
			case OP_LD:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
				reg[r0] = mem_read(reg[R_PC] + pc_offset);
				update_flags(r0);
				break;
			case OP_LDI:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
				reg[r0]m = mem_read(mem_read([reg[R_PC] + pc_offset]);
				update_flags(r0);
				break;
			case OP_LDR:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t r1 = (instr >> 6) & 0x7;
				uint16_t pc_offset = sign_extend(instr & 0x3F);
				reg[r0] = mem_read(reg[r1] + pc_offset);
				update_flags(r0);
				break;
			case OP_LEA:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
				reg[r0] = reg[R_PC] + pc_offset;
				update_flags(r0);
				break;
			case OP_ST:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
				mem_write(reg[R_PC] + pc_offset, reg[r0]);
				break;
			case OP_STI:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
				mem_write(mem_read(reg[R_PC] + pc_offset), reg[r0]);
				break;
			case OP_STR:
				uint16_t r0 = (instr >> 9) & 0x7;
				uint16_t r1 = (instr >> 6) & 0x7;
				uint16_t pc_offset = sign_extend(instr & 0x3F, 6);
				mem_write(reg[r1] + pc_offset, reg[r0]) 
				break;
			case OP_TRAP:
				reg[R_7] = reg[R_PC];
				switch(instr & 0xFF){
					case TRAP_GETC:
						reg[R_R0] = (uint16_t)getchar();
						break;
					case TRAP_OUT:
						putc((char)reg[R_R0], stdout);
						fflush(stoud);
						break;
					case TRAP_PUTS:
						uint16_t* c = memory + reg[R_R0];
						while(*c){
							putc((char)*c, stdout);
							++c;
						}
						fflush(stdout);
					}	
						break;
					case TRAP_IN:
						printf("Enter a character: ");
						char c = getchar();
						putc(c, stdout);
						reg[R_0] = (uint16_t)c;
						break;
					case TRAP_PUTSP:
						break;
					case TRAP_HALT:
						break;
				}
				break;
			case OP_RES:
				abort();
			case OP_RTI:
				abort();
			default:
				abort();
				break;
		}
	}
		
}

