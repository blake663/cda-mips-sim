#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	*Zero = 0;//Assume the output is not zero
	switch(ALUControl){
		case 0://ADD
			*ALUresult = A + B;
			if(*ALUresult == 0)
				*Zero = 0;
			break;
		case 1://SUB
			*ALUresult = A - B;
			if(*ALUresult == 0)
				*Zero = 0;
			break;
		case 2://SET-LT
			*ALUresult = (A < B);
			break;
		case 3://SET-LT unsigned
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
				*Zero = 0;
			break;
		case 6://shift left by 16 bits
			if(B == 0){
				*ALUresult = 0;
				*Zero = 1;
			}else
				*ALUresult = (B<<16);
			break;
		case 7://Inverse A
			*ALUresult = ~A;
			break;

	}
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	if (PC % 4)
		return 1;
	 
	*instruction = Mem[PC/4];
	return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	*op = (instruction >> 26) && 0b111111;// instruction [31-26]
	*r1 = (instruction >> 21) && 0b11111; // instruction [25-21]
	*r2 = (instruction >> 16) && 0b11111; // instruction [20-16]
	*r3 = (instruction >> 11) && 0b11111; // instruction [15-11]
	*funct = instruction      && 0b111111;// instruction [5-0]
	*r1 = instruction         && 0xFFFF;  // instruction [15-0]
	*jsec = instruction       && 0b11111; // instruction [25-0]
}

	

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
	// If halt occurs return 1
	// Else return 0

	// All the control signals to account for
	/*
	ALUOp = 3;// L/S => 00, Beq => 01, Arithmetic => 1X
	controls->ALUSrc;
	Branch;
	Jump;
	controls->MemRead;
	controls->MemtoReg;
	controls->MemWrite;
	controls->RegDst;
	controls->RegWrite;
	*/

	//R-type instruction
	if(op == 0b000000)
	{
		controls->RegDst = 1; 
		controls->RegWrite = 1;
	}
	// J-type instruction
	else if((op == 0b000010 || op == 0b000011)
	{
		control->Jump = 1;
		//control->branch = 1;
	}
	// I-type instruction
	else
	{
		// if op == 0001XX branch
		if((op >> 2) == 1)
			control->Branch = 1;
		






	}
	return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
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
	return;
}


/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	// Illegal operation check
	if(ALUOp > 4) return 1;
	
	// set Parameters for A, B, and ALUControl
	unsigned A = data1;
	unsigned B;
	char ALUcontrol;
	
	// ALUsrc = 1 => read extended_value
	if(ALUSrc == 1) 
		B = extended_value;
	else 
		B = data2;
	
	// Add for Load / Store
	if(ALUOp == 0)
		ALUcontrol = 0b010;

	// Sub for Beq	
	else if(ALUOp == 1)
		ALUcontrol = 0b110;

	// look at funct for operation
	else
		if	   (funct == 0b0000)
			ALUcontrol = 0b010;

		else if(funct == 0b0010)
			ALUcontrol = 0b110;
		
		else if(funct == 0b0100)
			ALUcontrol = 0b000;
		
		else if(funct == 0b0101)
			ALUcontrol = 0b001;
		
		else if(funct == 0b1010)
			ALUcontrol = 0b111;
		// We shouldn't get here
		else
			return 1;
		
	
	// in the end, call ALU function 
	ALU(A, B, ALUcontrol, *ALUresult, *Zero);
	return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

	if (MemWrite == 1 )
	{
		/*Hault check*/
		if (ALUresult % 4 != 0)
		{
			return 1;
		}

		else
		{
			Mem[ALUresult >> 2] = data2;
		}
	}

	if (MemRead == 1)
	{
		/*Hault check*/
		if (ALUresult % 4 != 0)
		{
			return 1;
		}

		else
		{
			*memdata = Mem[ALUresult >> 2];
		}
	}

	return 0;

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

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
	*PC = *PC + 4;

	if (Branch == 1 && Zero)
	{
		*PC = (extended_value << 2) + *PC;
	}

	else if (Jump == 1)
	{
		*PC = (jsec << 2) + *PC; /* IT SHOULD BE: PC = PC[31-28] : Offset << 2 BUT I AM NOT SURE HOW TO DO "PC[31-28] (WHICH ARE THE $ MOST SIGNIFICANT BITS FROM THE CURRENT PC) AND I ALSO DON'T KNOW WHAT ":" MEANS*/

		/*THIS IS WHAT THE PROJECT DETAILS POWERPOINT SAYS ABOUT THE JUMP IN THIS FUNCTION "Jump: Left shift bits of jsec by 2 and use upper 4 bits of PC" BUT I AM NOT SURE HOW TO "USE THE UPPER 4 BITS OF PC*/

	}
}

