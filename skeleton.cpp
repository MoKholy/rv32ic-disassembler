/*
	This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the RV32I "ADD", "SUB" and "ADDI" instructions only. 
	It prints "Unkown Instruction" for all other instructions!
	
	Usage example:
		$ rvcdiss t1.bin
	should print out:
		0x00000000	0x00100013	ADDI	x0, x0, 0x1
		0x00000004	0x00100093	ADDI	x1, x0, 0x1
		0x00000008	0x00100113	ADDI	x2, x0, 0x1
		0x0000000c	0x001001b3	ADD		x3, x0, x1
		0x00000010	0x00208233	ADD		x4, x1, x2
		0x00000014	0x004182b3	ADD		x5, x3, x4
		0x00000018	0x00100893	ADDI	x11, x0, 0x1
		0x0000001c	0x00028513	ADDI	xa, x5, 0x0
		0x00000020	0x00000073	Unkown Instruction 
	middle column is the 32 bit number representing the I/J/U/R binary format of the assembly line
	References:
	(1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
	(2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <unordered_map>
#define MAX_LABEL_SIZE 1000
using namespace std;

// registers for 32-bit instructions
string reg32[32] = {"zero", "ra", "sp", "gp",
					"tp","t0", "t1", "t2", 
					"s0", "s1","a0", "a1", 
					"a2", "a3", "a4", "a5",
					"a6", "a7","s2", "s3",
					"s4", "s5", "s6", "s7",
					"s8", "s9", "s10", "s11",
					"t3", "t4", "t5", "t6"};
					
// registers for compressed 16-bit instructions
string reg16[8] = {"s0", "s1", "a0", "a1",
 				   "a2", "a3", "a4", "a5"};

// label table
unordered_map <int, string> labels;

unsigned int pc = 0x0;

char memory[8*1024];	// only 8KB of memory located at address 0

// functions to print instruction
// 
void printInstruction(unsigned int instWord, unsigned int instPC, string instruction, string rd, unsigned int immediate)
{
	cout << "0x" << hex << setfill('0') << setw(8) << instPC << "\t0x" << setw(8) <<instWord << "\t" << dec;
	cout << instruction << " " << rd << ", " << immediate << "\n";

}
// void printInstruction(unsigned int instWord, unsigned int instPC, string instruction, string rd, string rs1, unsigned int immediate)
// {
// 	cout << "0x" << hex << setfill('0') << setw(8) << instPC << "\t0x" << setw(8) <<instWord << "\t" << dec;
// 	cout << instruction << " " << rd << ", " << immediate << "\n";
// }
void printInstruction(unsigned int instWord, unsigned int instPC, string instruction, string rd, string rs1, string rs2, unsigned int immediate = 0, bool address = false, bool memoryAccess = false, bool unknown = false){
	
	cout << "0x" << hex << setfill('0') << setw(8) << instPC << "\t0x" << setw(8) <<instWord << "\t" << dec;
	// if unknown print unknown and exit
	if(unknown){
		cout << "Unknown Instruction Word\n";
		return; 
	}
	// if not imm and address and memory access -> function has 3 registers
	if(!immediate && !address && !memoryAccess)
	{
		 cout << instruction << " " << rd << ", " << rs1 << ", " << rs2 << "\n";
	}else{
		// default part of output for all other instructions
		cout << instruction << " " << rd << ", ";
		// check if not memory access and rs1 != ""
		if(!memoryAccess && rs1 != "")
		{
			cout << rs1 << ", ";
		}

		// check if label
		if(address)
		{
			cout << hex << "0x";
		}
		
		cout << (int)immediate;
		
		// check for address
		if(address)
		{
			// to be implemented
		}

		// check if we are accessing memory in instruction
		if(memoryAccess)
		{
			cout << "(" << rs1 << ")";
		}

		cout << "\n";
	}
}




void emitError(char *s)
{
	cout << s;
	exit(0);
}

// void printPrefix(unsigned int instA, unsigned int instW){
// 	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
// }

void instDecExec(unsigned int instWord, bool isCompressed = false)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	unsigned int instPC = pc - 4;
	// treat 16 and 32 bit instruction differently
	if(isCompressed){
		// pc -2 as it is 16 bits
		unsigned int instructionPC = pc-2; 
		unsigned int opcode = instWord & 0x0003; // and with 00..11 to get opcode
		unsigned int funct3 = (instWord >> 13) & 0x7; // funct3 in same location for all
		// check opcode
		if(opcode == 0x0) // load/store 
		{	
			// get individual imm at each index
			unsigned int imm2 = (instWord >> 6) & 0x0001;
			unsigned int imm6 = (instWord >> 5) & 0x0001;
			unsigned int imm3to5 = ( instWord >> 10) & 0x0007;
			// combine and scale by 4
			unsigned int immLS = ((imm2 & 0x1) << 2)| ((imm6 & 0x1) << 6) | ((imm3to5 & 0x7) << 3);
			
			

			unsigned int rd16 = (instWord >> 2) & 0x0007; // only supports compressed registers for L/S
			unsigned int rs1_16 = (instWord >> 7) & 0x0007; 
			switch(funct3){
				case 2: // C.LW
						printInstruction(instWord, instructionPC, "C.LW", reg16[rd16], reg16[rs1_16], "", immLS, false, true);
						break;
				case 6: // C.SW
						printInstruction(instWord, instructionPC, "C.SW", reg16[rd16], reg16[rs1], "", immLS, false, true);
						break;
				default:
						printInstruction(instWord, instructionPC, "", "", "", "", 0, false, false, true);
						break;

			}
		}
		else if (opcode == 0x1)
		{
			switch (funct3){
				case 0: // either nop or addi
						int imm_5 = (instWord >> 12) & 0x1; // check if bit at index 12 is 0 or not
						if(imm_5) // if 1 -> addi -> supports 32 bit registers -> signed imm
						{
							unsigned int rd32, rs32;
							rd32 = rs32 = (instWord >> 7) & 0b11111; // rd and rs1 are the same
							
							int imm0to4 = (instWord >> 2) & 0x1F;
							int imm = ((imm_5) ? 0xFFFFFFE0: 0x0) | imm0to4;

							printInstruction(instWord, instructionPC, "C.ADDI", reg32[rd32], reg32[rs32], "", imm);
						}
						else //NOP -> pseudo so we convert to true
						{
							printInstruction(instWord, instructionPC, "C.ADDI", reg32[0], reg32[0], "", 0);
						}
						break;
				case 1: // JAL
							unsigned int imm5 = (instWord >> 2) & 0x1;
							unsigned int imm1to3 = (instWord >> 3) & 0x7;
							unsigned int imm7 = (instWord >> 6) & 0x1;
							unsigned int imm6 = (instWord >> 7) & 0x1;
							unsigned int imm10 = (instWord >> 8) & 0x1;
							unsigned int imm8to9 = (instWord >> 9) & 0x3;
							unsigned int imm4 = (instWord >> 11) & 0x1;
							unsigned int imm11 = (instWord >> 12) & 0x1;
							unsigned int imm = ((imm1to3 & 0x7) << 1) | ((imm4 & 0x1) << 4) | ((imm5 & 0x1) << 5) | ((imm6 & 0x1) << 6) | ((imm7 & 0x1) << 7) | ((imm8to9 & 0x3) << 8) | ((imm10 & 0x1) << 10) | ((imm11 & 0x1) << 11) ;
							printInstruction(instWord, instructionPC, "C.JAL", reg32[1], imm);
							break;
				case 2: // LI -> pseudo so we convert to true addi rd, rd, imm -> signed imm
							unsigned int rd32, rs32;
							rd32 = rs32 = (instWord >> 7) & 0x1F;

							int imm5 = (instWord >> 12) & 0x1;
							int imm0to4 = (instWord >> 2) & 0x1F;
							int imm = ((imm5) ? 0xFFFFFFE0: 0x0) | imm0to4;

							printInstruction(instWord, instructionPC, "C.ADDI", reg32[rd32], reg32[rs32], "", imm);
							break;
				case 3:	// LUI -> signed imm
							unsigned int rd32 = (instWord >> 7) & 0x1F;
							int imm17 = (instWord >> 12) & 0x1;
							int imm12to16 = (instWord >> 2) & 0x1F;
							int imm = ((((imm17) ? 0xFFFFFFE0: 0x0) | imm12to16) << 12);
							printInstruction(instWord, instructionPC, "C.LUI", reg32[rd32], imm);
							break;
				case 4: // SRLI -> unsigned imm


			}

		}
	}
	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;

	// â€” inst[31] â€” inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	// printPrefix(instPC, instWord);

	if(opcode == 0x33){		// R Instructions
		switch(funct3){
			case 0: if(funct7 == 32) {
								cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
							}
							else {
								cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
							}
							break;
			default:
							cout << "\tUnkown R Instruction \n";
		}
	} else if(opcode == 0x13){	// I instructions
		switch(funct3){
			case 0:	cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			default:
					cout << "\tUnkown I Instruction \n";
		}
	} else {
		cout << "\tUnkown Instruction \n";
	}

}

int main(int argc, char *argv[]){

	unsigned int instWord=0;
	ifstream inFile;
	ofstream outFile;

	if(argc<2) emitError("use: rvcdiss <machine_code_file_name>\n");
	
	inFile.open(argv[2], ios::in | ios::binary | ios::ate);
	
	if(inFile.is_open())
	{
		int fsize = inFile.tellg();

		inFile.seekg (0, inFile.beg);
		if(!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");

		while(true){
				instWord = 	(unsigned char)memory[pc] |
							(((unsigned char)memory[pc+1])<<8) |
							(((unsigned char)memory[pc+2])<<16) |
							(((unsigned char)memory[pc+3])<<24);
				pc += 4;
				// remove the following line once you have a complete simulator
				if(pc==40) break;			// stop when PC reached address 32
				instDecExec(instWord);
		}
	} else emitError("Cannot access input file\n");
}
