#include "spimcore.h"



/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	printf("ALU called\n");
	int Atemp = (int) A;
	int Btemp = (int) B;
	*Zero = 0;//Assume the output is not zero
	switch(ALUControl){
		case 0://ADD
			*ALUresult = (unsigned)(Atemp + Btemp);
			if(*ALUresult == 0)
				*Zero = 1;
			break;

		case 1://SUB
			*ALUresult = A - B;
			if(*ALUresult == 0)
				*Zero = 1;
			break;

		case 2://SET-LT SIGNED
			*ALUresult = (Atemp < Btemp);
			break;

		case 3://SET-LT UNSIGNED
			printf("calling SLTU\n");
			*ALUresult = (A < B);
			break;

		case 4://AND
			*ALUresult = A & B;
			if(ALUresult == 0)
				*Zero = 1;
			break;

		case 5://OR
			*ALUresult = A | B;
			if(*ALUresult == 0)
				*Zero = 1;
			break;

		case 6://shift left by 16 bits
			if(B == 0)
			{
				*ALUresult = 0;
				*Zero = 1;
			}else
				*ALUresult = (B<<16);
			break;

		case 7://Inverse A
			*ALUresult = ~A;
			break;
		default:
			printf("ALUcontrol not recognized");
			break;

	}
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	printf("instruction_fetch Called\n");
	if (PC % 4 || PC > 0xFFFF)
	{
		printf("instruction_fetch FAILED\n");
		return 1;
	}
		*instruction = Mem[PC/4];
	return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	printf("Instruction_partition Called\n");
	*op = (instruction >> 26) & 0b111111;// instruction [31-26]
	printf("op = %x\n", *op);
	*r1 = (instruction >> 21) & 0b11111; // instruction [25-21]
	*r2 = (instruction >> 16) & 0b11111; // instruction [20-16]
	*r3 = (instruction >> 11) & 0b11111; // instruction [15-11]
	*funct = instruction      & 0b111111;// instruction [5-0]
	*offset = instruction     & 0xFFFF;  // instruction [15-0]
	*jsec = instruction       & 0x3FFFFFF;//instruction [25-0]
}


/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
	printf("Instruction_decode Called\n");
	// If halt occurs return 1
	// Else return 0
	/*
	Non R-type functions to support
	jump
	load word
	store word
	branch on equal
	add imm
	load upper immediate
	set on less than immediate
	set less than unsigned immediate
	*/

	// Set the default state of the control signals
	controls->RegDst = 0;
	controls->Jump = 0;
	controls->Branch = 0;
	controls->MemRead = 0;
	controls->MemtoReg = 0;
	controls->MemWrite = 0;
	controls->ALUOp = 0b000;
	controls->ALUSrc = 0;
	controls->RegWrite = 0;

	//R-type instruction
	if(op == 0b000000)
	{
		controls->RegDst = 1; 
		controls->RegWrite = 1;
		controls->ALUOp = 0b111;
		return 0;
	}
	// J-type instruction
	else if(op == 0b000010)
	{
		controls->Jump = 1;
		return 0;
	}
	// I-type instruction
	else
	{
		// Add immediate: op == 001000

		if(op == 0b001000)
		{
			printf("Add immediate Called\n");
			controls->ALUSrc = 1;
			controls->RegWrite = 1;
			controls->ALUOp = 0b000;
			return 0;
		}
		// branch on equal: op == 000100
		if(op == 0b000100)
		{
			controls->ALUOp = 0b001;//sub
			controls->Branch = 1;
			return 0;
		}
		// Store word: op == 101011
		if(op == 0b101011)
		{
			printf("store word Called\n");
			controls->MemWrite = 1;
			controls->ALUSrc = 1;
			return 0;
		}
		// Load word: op == 100011 
		if(op == 0b100011)
		{
			controls->ALUSrc = 1;
			controls->MemtoReg = 1;
			controls->MemRead = 1;
			controls->RegWrite = 1;
			return 0;
		}
		// Load upper immediate: op == 001111
		if(op == 0b001111)
		{
			controls->ALUOp = 6;
			controls->ALUSrc = 1;
			controls->RegWrite = 1;
			return 0;
		}
		// Set on less than immediate: op == 001010
		if(op == 0b001010)
		{
			controls->RegWrite = 1;
			controls->ALUSrc = 1;
			controls->ALUOp = 0b010;
			return 0;
		}
		// Set less than unsigned immediate: op == 001011
		if(op == 0b001011)
		{
			controls->RegWrite = 1;
			controls->ALUSrc = 1;
			controls->ALUOp = 0b011;
			return 0;
		}
		// Set on less than: op == 001010
		if (op == 0b001010)
		{
			controls->RegWrite = 1;
			controls->ALUOp = 0b010;
			return 0;
		}
		// Set on less than unsigned: op == 001010
		if (op == 0b001010)
		{
			controls->RegWrite = 1;
			controls->ALUOp = 0b011;
			return 0;
		}

	}
	// op code not recognized
	printf("Opcode not recognized\n");
	return 1;
}


/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	printf("read_register called\n");
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
	//stores the most left bit of offset
	int leftBit = offset & 0x00008000;
	
	//if left bit is a 1, fill in with 1's
	if(leftBit == 1)
		offset += 0xFFFF0000;
	*extended_value = offset;
}


/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	// Illegal operation check
	if(ALUOp >= 8) 
	{
		printf("ALUop >= 8 HALT\n");
		return 1;
	}
	// set Parameters for A, B, and ALUControl
	unsigned A = data1;
	unsigned B;
	char ALUcontrol = ALUOp;
	
	// ALUsrc = 1 => read extended_value
	if(ALUSrc == 1) 
	{
		B = extended_value;
	}
	else 
	{
		B = data2;
	}

	// look at funct for operation
	if(ALUOp == 7)
	{
		if(funct == 0b100000)
			ALUcontrol = 0b000;// Add

		else if(funct == 0b100010)
			ALUcontrol = 0b001;// Sub
				
		else if(funct == 0b101010)
			ALUcontrol = 0b010;// SLT signed

		else if(funct == 0b101011)
			ALUcontrol = 0b011;// SLTU
		
		else if(funct == 0b100100)
			ALUcontrol = 0b100;// And
		
		else if(funct == 0b100101)
			ALUcontrol = 0b101;// Or

		// funct code wasn't recognized if we get here
		else
		{
			printf("funct not recognized");
			return 1;
		}
	}	

	// in the end, call ALU function 
	ALU(A, B, ALUcontrol, ALUresult, Zero);
	return 0;
}


/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	printf("rw_memory Called\n");
	
	if(!MemWrite && !MemRead)
	{	
		return 0;
	}
	if (ALUresult % 4 != 0 || ALUresult > 0xFFFF)
	{
		printf("not aligned, ALUresult wrong in rw_memory\n");
		return 1;
	}


	/*If MemWrite = 1, write into memory*/
	if (MemWrite)
	{
		printf("MemWrite called with ALUresult = %d and ", ALUresult);
   		printf("data2 = %x\n", data2);
		Mem[ALUresult >> 2] = data2;
	}
	/*If MemRead = 1, read from memory*/
	if (MemRead)
  	{	
    	*memdata = Mem[ALUresult >> 2];
	}
	return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	/*If RegWrite == 1, and MemtoReg ==1, then data coming from memory*/
	if (RegWrite == 1 && MemtoReg == 1)
	{
		if(RegDst == 1)
		{
			Reg[r3] = memdata;
		}

		else
		{
			Reg[r2] = memdata;
		}
	}

	/*If RegWrite == 1, and MemtoReg ==0, then data coming from ALU_result*/
	else if (RegWrite == 1 && MemtoReg == 0)
	{

		if (RegDst == 1)
		{
			Reg[r3] = ALUresult;
		}

		else
		{
			Reg[r2] = ALUresult;
		}
		

	}

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	printf("PC_update was called\n");

	/*If the instruction is not a branch, the new PC will be PC + 4*/
	*PC = *PC + 4;

	//If the instruction is a branch, we have to calculate the target address

	//Calculate target address: Sign-extend displacement, Shift left 2 places (word displacement), Add to PC + 4
	if (Branch && Zero)
	{
		*PC = (extended_value << 2) + *PC;
	}

	//Left shift bits of jsec by 2 and use upper 4 bits of PC
	else if (Jump)
	{
		
		*PC = (*PC & 0xf0000000) | (jsec << 2);

	}
}
