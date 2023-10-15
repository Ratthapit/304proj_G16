#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {// ************ ห้ามแก้ *********************
    int pc;                 //program counter
    int mem[NUMMEMORY];
    int reg[NUMREGS];       //register --> เดี๋ยวต้องทำให้ทุกตัว = 0
    int numMemory;          //ต้องเท่ากับจำนวนบรรทัด ใน machine-code file
} stateType;

void printState(stateType *); // ************ ห้ามแก้้้้้้้้ *********************print @@@ at this func only
int  convertNum(int num);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
	    printf("error: usage: %s <machine-code file>\n", argv[0]);
	    exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	    printf("error: can't open file %s", argv[1]);
	    perror("fopen");
	    exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {
	    if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
	        printf("error in reading address %d\n", state.numMemory);
	        exit(1);
	    }
	    printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    ///////////////////////////////////////////////////////////////////////////////
  
    int i;
    for(i=0;i<NUMREGS;i++)  state.reg[i]=0;                 // Initialize ทุก registers ให้เป็น 0
    state.pc = 0;                                               // set program counter เป็น 0 ไว้ก่อน-->เอาไว้แสดงใน printState

    for(i=0; 1 ;i++){                           // สร้างลูปมาวนให้ทำงานไปเรื่อยๆจนครบทุกบรรทัด
        int opcodeInBinary = (state.mem[i] >> 22) & 0x7;        // เก็บค่า opcode In Binary
        
        printState(&state);

        int rs = (state.mem[i] >> 19) & 0x7;    //regA
        int rt = (state.mem[i] >> 16) & 0x7;    //regB

        if(opcodeInBinary == 000 || opcodeInBinary == 001){
            int rd = (state.mem[i] >> 0) & 0x7; //destReg   

            if(opcodeInBinary == 000)       state.reg[rd] = state.reg[rs]+state.reg[rt];            // 000 regA + regB เก็บใน destReg
            else if(opcodeInBinary == 001)  state.reg[rd] = ~(state.reg[rs] & state.reg[rt]);       // 001 Nand regA with regB เก็บใน destReg
        }
        else if(opcodeInBinary == 010 || opcodeInBinary == 011 ||opcodeInBinary == 100){
            int offset_16Bits = (state.mem[i] >> 0) & 0x10;
            int offsetField = convertNum(offset_16Bits);

            if(opcodeInBinary == 010)       state.reg[rt] = state.mem[offsetField+state.reg[rs]];
            else if(opcodeInBinary == 011)  state.mem[offsetField+state.reg[rs]] = state.reg[rt];
            else if(opcodeInBinary == 100){
                if(state.reg[rs]==state.reg[rt])    state.pc = 1+offsetField;
            }
        }
        else if(opcodeInBinary == 101){
           state.reg[rt] = state.pc+1;              //เก็บ pc+1 ใน regB

           if(rs==rt)   state.pc++;
           else         state.pc = state.reg[rs];
        }
        else if(opcodeInBinary == 111){ /*ปล่อยมันไป อย่างที่เป็น~~*/}
        else if(opcodeInBinary == 110){
            printf("machine halted\ntotal of %d instructions executed\nfinal state of machine:",i);
            state.pc++;
            printState(&state);
            return;
        }

        state.pc++; //จบรอบ นับค่าเพิ่ม
    }

    ///////////////////////////////////////////////////////////////////////////////

    return(0);

}

int convertNum(int num)
{          /* convert a 16-bit number into a 32-bit integer */
    if (num & (1<<15) ) num -= (1<<16);
    return(num);
}

void printState(stateType *statePtr)  //  ************ ห้ามแก้้้้้้้้ ********************* print @@@ at this func only
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}
