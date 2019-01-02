#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

struct state{
	bool penMode;
	int xpos;
	int ypos;
	long dx;
	int DT;//last non-zero DT value
	unsigned long operand; //operand under construction
	int length; //number of 6 bit sections in operand
	bool valInit; //is operand initialised
};

typedef struct state state;

state *newState(){
	state *s = malloc(sizeof(state));
	s -> penMode = false;
	s -> xpos = 0;
        s -> ypos = 0;
	s -> dx = 0;
	s -> DT = 0;
	s -> operand = 0;
	s -> valInit = false;
	return s;
}


int getOpcode(unsigned int b){
	unsigned int c = (b>>6)&0xFF;
	printf("%02x,%d\n",b,c);
       	return c;
}

int getOperand(unsigned int b){
       	int c = (b&0x1F);
       	if ((b&0x20) != 0) c = -32 + c;
       	printf("%02x,%d\n",b,c);
       	return c;
}

long getOperandVal(state *s){
	long n = s -> operand;
	return n;
}


void updatePos(state *s, int dy){
	s -> xpos = s -> xpos + s-> dx;
	s -> ypos = s -> ypos + dy;
	s -> dx = 0;
}

void drawLine(state *s, display *d, int dy){
	line(d, s -> xpos, s -> ypos, s -> xpos + s -> dx, s -> ypos + dy);
	updatePos(s,dy);
}

void togglePen(state *s){
	if (s -> penMode == true) s -> penMode = false;
	else s -> penMode = true;
}

void updateLongOperand(state *s, unsigned int b){
	s -> valInit = true;
	s -> operand = s -> operand << 6 | b;
	s -> length++;
}

void resetLongOperand(state *s){
	s -> operand = 0;
	s -> valInit = false;
	s -> length = 0;
}


void doDX(state *s, unsigned int dx){
	if (s->valInit == true){
	       	updateLongOperand(s,dx);
		s -> dx = s -> operand;
		resetLongOperand(s);
	}
	else s -> dx = dx;
}

//if pen down, draw a line otherwise update the current position
void doDY(state *s, display *d, unsigned int dy){
	if (s -> penMode == true) drawLine(s,d,dy);
	else updatePos(s,dy);
}

void doDT(state *s, display *d){
	if (s -> valInit == false) pause(d,s -> DT);
	else{
		pause(d,s -> operand);
		s -> DT = s -> operand;
		resetLongOperand(s);
	}
}

void clearDisplay(display *d){
	clear(d);
}

void waitForKey(display *d){
	key(d);
}

void changeCol(state *s, display *d){
	colour(d,s->operand);
	resetLongOperand(s);
}

void update(unsigned int b, state *s, display *d){
	int opc = getOpcode(b);
	int opr = getOperand(b);
	unsigned int oprBits = b & 0x3F;
	if (opc == 0) doDX(s,oprBits);
	else if (opc == 1) doDY(s,d,oprBits);
	else if (opc == 2) updateLongOperand(s,oprBits);
	else if (opr == 0) togglePen(s);//only other opc can be 3, opr must be addtional opcodes
	else if (opr == 1) doDT(s,d);
	else if (opr == 2) clearDisplay(d);
	else if (opr == 3) waitForKey(d);
	else if (opr == 4) changeCol(s,d);
	else printf("invalid instruction\n");
}	

void testS1(){
	assert(getOpcode(0x00)==0);
	assert(getOpcode(0xC0)==3);
	assert(getOperand(0x1F)==31);
	assert(getOperand(0x20)==-32);
	state *s = newState();
	s -> dx = 30;
	updatePos(s,30);
	assert(s->xpos == 30);
	assert(s->ypos == 30);
	free(s);
}

void testS2(){
	state *s = newState();
	updateLongOperand(s,0xBF&0x3F);
	assert(s->operand == 0x3F);
	doDX(s,0x3F);
	printf("%02lx, %ld\n",s->dx, s->dx);
	assert(s -> dx == 0xFFF);
	s -> dx = 0;
	doDX(s,0x3E);
	assert(s -> dx == 0x3E);
	printf("%ld\n",s->dx);
	free(s);
}

int main(int n, char *args[n]) {
	if (n==1) testS2();
	else if (n!=2) {fprintf(stdout, "Use ./sketch filename\n"); exit(1); }
	else{
		FILE *inp = fopen(args[1],"rb");
		unsigned char b = fgetc(inp);		
		display *d = newDisplay(args[1], 200, 200);
		state *s = newState();
		while(!feof(inp)){
			update(b,s,d);
			b = fgetc(inp);
		}
		end(d);
		fclose(inp);
		free(s);
	}

}

