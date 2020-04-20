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
			if(B >> 31)
				B *= -1;
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


	All the control signals to account for
	controls->ALUOp;// L/S => 00, Beq => 01, Arithmetic => 1X
	controls->ALUSrc;// Selects second source for ALU
	controls->Branch;
	control->Jump;
	control->MemRead;
	controls->MemtoReg;
	control->MemWrite;
	controls->RegDst; // Asserted for R-type
	controls->RegWrite; // Asserted for R-type/Load
	*/

  // Set the default state of the control signals
  controls->ReDst = 0;
  controls->Jump = 0;
  controls->Branch = 0;
  controls->MemRead = 0;
  controls->MemtoReg = 0;
  controls->MemWrite = 0;
  controls->ALUOp = 0b00;
  controls->ALUSrc = 0;
  controls->RegWrite = 0;

	//R-type instruction
	if(op == 0b000000)
	{
		controls->RegDst = 1; 
		controls->RegWrite = 1;
    control->ALUOp = 0b111;
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
      controls->ALUSrc = 1;
      controls->RegWrite = 1;
      controls->ALUOp = 0b000;
    }
		// branch on equal: op == 000100
		if(op == 0b000100)
		{
      controls->ALUOp = 0b01;//sub
			controls->Branch = 1;
			return 0;
		}
		// Store word: op == 101011
		if(op == 0b101011)
		{
			controls->MemWrite = 1;
      controls->ALUSrc = 1;
			return 0;
		}
		// Load word: op == 100011 
		if(op == 100011)
		{
      controls->ALUSrc = 1;
      controls->MemtoReg = 1;
			controls->MemRead = 1;
			controls->RegWrite = 1;
			return 0;
		}
    // Load upper immediate: op == 001111
    if(op == 0b00111)
    {
      controls->RegWrite = 1;
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
      controls->ALUOp = 0b10;
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

	}
  // op code not recognized
	return 1;
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
	if(ALUOp > 8) 
	{
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
    //need to double check funct field
    funct = funct & 0b001111;// Get four bits
		if(funct == 0b0000)
			ALUcontrol = 0b000;//add

		else if(funct == 0b0010)
			ALUcontrol = 0b001;//sub
		
		else if(funct == 0b0100)
			ALUcontrol = 0b100;//and
		
		else if(funct == 0b0101)
			ALUcontrol = 0b101;
		
		else if(funct == 0b1010)
			ALUcontrol = 0b010;
		// funct code wasn't recognized if we get here
		else
			return 1;
	}	

	
	// in the end, call ALU function 
	if(ALU(A, B, ALUcontrol, ALUresult, Zero))
  {
    return 1;
  }
	return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	/*If MemWrite = 1, write into memory*/
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
	/*If MemRead = 1, read from memory*/
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

	/*If the instruction is not a branch, the new PC will be PC + 4*/
	*PC = *PC + 4;

	/*If the instruction is a branch, we have to calculate the target address*/

	/*Calculate target address: Sign-extend displacement, Shift left 2 places (word displacement), Add to PC + 4*/
	if (Branch == 1 && Zero)
	{
		*PC = (extended_value << 2) + *PC;
	}

	/*Left shift bits of jsec by 2 and use upper 4 bits of PC*/
	else if (Jump == 1)
	{
		
		*PC = (*PC | 0xf0000000) | (jsec << 2);

	}
}

