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

	 1) Find out if its 16 or 32 bits
 	2) create S,B,J,U instructions
 	3) complete R, I instructions
 	4) BU: salma
	SJ: Allaa
	IR: MO
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc = 0x0;

string reg[32] = { "zero", "ra", "sp", "gp", "tp","t0", "t1", "t2", "s0", "s1", //to retrive and cout the names of registers easily
"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7","s2", "s3", 
"s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6" };

string regC[8] = { "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5" }; //retriving names of registers for 16 bit 

char memory[8*1024];	// only 8KB of memory located at address 0

void emitError(char *s)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW){
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void instDecExec(unsigned int instWord, bool is32)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode; //for 32 bits
	unsigned int rd_16, rs1_16, rs2_16; //for 16 bits
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm; //for the immediate values of the 32 bits
	unsigned int address;

	if(is32){
	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;

	// â€” inst[31] â€” inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
	B_imm = ((instWord >> 7 & 0x1) << 12) | ((instWord >> 25 & 0x3F) << 5) | (instWord >> 8 & 0xF) | ((instWord >> 31) ? 0xFFFFF800 : 0x0);
	U_imm = ((instWord & 0xFFFFF00) >> 12);

	printPrefix(instPC, instWord);

	if(opcode == 0x33){		// R Instructions
		switch(funct3){
			case 0:{
				if (funct7 == 32)
					cout << "\tSUB\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				else
					cout << "\tADD\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;}
			case 1:
				cout << "\tSLL\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			case 2:
				cout << "\tSLT\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			case 3:
				cout << "\tSLTU\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			case 4:
				cout << "\tXOR\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			case 5:
				if (funct7 == 32)
					cout << "\tSRA\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				else
					cout << "\tSRL\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";

				break;
			case 6:
				cout << "\tOR\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;

			case 7:
				cout << "\tAND\t" << reg[rd] << ", " << reg[rs1] << ", " << reg[rs2] << "\n";
				break;
			default:
				cout << "\tUnknown R Instruction \n";
		}
	} else if(opcode == 0x13){	// I instructions
		switch(funct3){
			case 0:	cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			default:
					cout << "\tUnkown I Instruction \n";
		}
	} else if(opcode==0x63){ //B instructions
		switch(funct3){
			case 0:
				cout << "\tBEQ\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 1:
				cout << "\tBNE\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 4:
				cout << "\tBLT\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 5:
				cout << "\tBGE\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 6:
				cout << "\tBLTU\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			case 7:
				cout << "\tBGEU\t" << reg[rs1] << ", " << reg[rs2] << ", " << hex << "0x" << (int)B_imm << "\n";
				break;
			default:
				cout << " Unkown B Instruction " << "\n";
		}
	} 		else if (opcode == 0x37) // LUI-U instruction
		{
			cout << "\tLUI\t" << reg[rd] << ", " << hex << "0x" << (int)U_imm << endl;
		}

		else if (opcode == 0x17) //AUIPC- U instruction
		{
			cout << "\tAUIPC\t" << reg[rd] << ", " << hex << "0x" << (int)U_imm << endl;
		}
	else {
		cout << "\tUnkown Instruction \n";
	}

	} else{ //16 bit instructions
		unsigned int instPC = pc - 2;	//decrementing by 2 because its 16 bits and not 32


		opcode = instWord & 0x00000003;	//...11	//same spot same number of bits in all of them.

		//There is 2 funct2 variables because funct2 is used in different locations in different instruction words.
		funct2_1 = (instWord >> 10) & 0x00000003;
		funct2_2 = (instWord >> 5) & 0x00000003;

		funct3 = (instWord >> 13) & 0x00000007;
		funct4 = (instWord >> 12) & 0x0000000F;
		funct6 = (instWord >> 10) & 0x0000003F;

		//uncompressed registers 
		rd = (instWord >> 7) & 0x0000001F;	
		rs1 = (instWord >> 7) & 0x0000001F;
		rs2 = (instWord >> 2) & 0x0000001F;

		//compressed registers
		rd_16 = (instWord >> 2) & 0x00000007; 
		rs1_16 = (instWord >> 7) & 0x00000007; 
		rs2_16 = (instWord >> 2) & 0x00000007; 

		unsigned int I_Shift = ((instWord >> 7) & 0x0020) | ((instWord >> 2) & 0x001F);	//Used in SRAI, SRLI, SLLI

		printPrefix(instPC, instWord);

		if (opcode == 0x0) //opcode = 00
		{
			unsigned int I_LS;	//Immediate for LW and SW instructions.
			I_LS = (instWord & 0x0010) | ((instWord >> 9) & 0x000E) | ((instWord >> 6) & 0x1);

			if (funct3 == 2)
				cout << "\tC.LW\t" << reg[rd] << ", " << hex << "0x" << (int)I_LS << "(" << reg[rs1] << ")" << "\n";
			else if (funct3 == 6)
				cout << "\tC.SW\t" << reg[rs2] << ", " << hex << "0x" << (int)I_LS << "(" << reg[rs1] << ")" << "\n";
		}
		else if (opcode == 0x1) //opcode = 01
		{
			int I_ADDI = ((instWord >> 7) & 0x0020) | ((instWord >> 2) & 0x001F) | ((instWord >> 12) ? 0xFFFFFF0 : 0x0);

			int I_JAL = ((instWord << 2) & 0x200) | ((instWord >> 1) & 0x180) | ((instWord << 1) & 0x40) | ((instWord >> 1) & 0x20)
				| ((instWord << 3) & 0x10) | ((instWord >> 7) & 0x8) | ((instWord >> 2) & 0xE) | ((instWord >> 12) ? 0xFFFFFF0 : 0x0);

			int I_LUI = ((instWord << 5) & 0x10000) | ((instWord >> 2) & 0xF800) | ((instWord >> 12) ? 0xFFE0000 : 0x0);		//Double Check


			int I_ANDI = ((instWord >> 7) & 0x0020) | ((instWord << 10) & 0x001F) | ((instWord >> 12) ? 0xFFFFFF0 : 0x0);

			if (funct3 == 0)
				cout << "\tC.ADDI\t" << regC[rd_16] << ", " << hex << "0x" << (int)I_ADDI << "\n"; //Addi	rd, rd, nzimm[5:0]
			else if (funct3 == 1)
				cout << "\tC.JAL\t" << I_JAL << "\n"; //Jal	x1, imm[11:1]	
			else if (funct3 == 3)
				cout << "\tC.LUI\t" << reg[rd] << ", " << hex << "0x" << (int)I_LUI << endl;	//Lui	rd, nzimm[17:12]
			else if (funct3 == 4)
			{
				if (funct2_1 == 0)
					cout << "\tC.SRLI\t" << regC[rd_16] << ", " << hex << "0x" << (int)I_Shift << "\n"; //Srli	rd', rd', shamt[5:0]
				else if (funct2_1 == 1)
					cout << "\tC.SRAI\t" << regC[rd_16] << ", " << hex << "0x" << (int)I_Shift << "\n"; //Srai	rd', rd', shamt[5:0]
				else if (funct2_1 == 2)
					cout << "\tC.ANDI\t" << regC[rd_16] << ", " << hex << "0x" << (int)I_ANDI << "\n";  //Addi	rd', rd', imm[5:0]
				else if (funct2_1 == 3)
				{
					if (funct2_2 == 0)
						cout << "\tC.SUB\t" << regC[rs1_16] << ", " << regC[rs2_16] << "\n";		//Sub	rd', rd', rs2'
					else if (funct2_2 == 1)
						cout << "\tC.XOR\t" << regC[rs1_16] << ", " << regC[rs2_16] << "\n";		//Xor	rd', rd', rs2'
					else if (funct2_2 == 2)
						cout << "\tC.OR\t" << regC[rs1_16] << ", " << regC[rs2_16] << "\n";			//Or	rd', rd', rs2'
					else if (funct2_2 == 3)
						cout << "\tC.AND\t" << regC[rs1_16] << ", " << regC[rs2_16] << "\n";		//Xor	rd', rd', rs2'

				}
			}
		}
		else if (opcode == 0x2)//opcode = 10
		{
			if (funct3 == 0)
				cout << "\tC.SLLI\t" << reg[rd] << ", " << hex << "0x" << (int)I_Shift << "\n";			//Slli	rd, rd, shamt[5:0]
			else if (funct3 == 4)
			{
				if (rs2 == 0)
					cout << "\tC.JALR\t" << reg[rs1] << "\n";											//Jalr	x1, 0(rs1)
				else
					cout << "\tC.ADD\t" << reg[rs1] << ", " << reg[rs2] << "\n";						//Add	rd, rd, rs2
			}
		}
		else
			cout << "\tUnknown Compressed Instruction \n";

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
			int temp = memory[pc] & 0x00000003;	//Checking the 1st and 2nd bit to determine the size of the instruction.
			bool is32 = true;

			if(is32==3){
				instWord = 	(unsigned char)memory[pc] |
							(((unsigned char)memory[pc+1])<<8) |
							(((unsigned char)memory[pc+2])<<16) |
							(((unsigned char)memory[pc+3])<<24);
				pc += 4;
			}
			else {
				instWord = (unsigned char)memory[pc] | (((unsigned char)memory[pc + 1]) << 8);
				is32 = false;
				pc += 2;
			}
				// remove the following line once you have a complete simulator
				instDecExec(instWord, is32);
				if (!memory[pc]) break;	//stop when we've gone through all the instructions in the memory.
		}

	} else emitError("Cannot access input file\n");
}
