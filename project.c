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
	*op = (instruction >> 26) && 0b111111; // instruction [31-26]
	*r1 = (instruction >> 21) && 0b11111; // instruction [25-21]
	*r2 = (instruction >> 16) && 0b11111; // instruction [20-16]
	*r3 = (instruction >> 11) && 0b11111; // instruction [15-11]
	*funct = instruction      && 0b111111; // instruction [5-0]
	*r1 = instruction         && 0xFFFF; // instruction [15-0]
	*jsec = instruction       && 0b11111; // instruction [25-0]
}


/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

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

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}

