#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

struct state{
	bool penMode;
	int xpos;
	int ypos;
	int dx;
};

typedef struct state state;

state *newState(){
	state *s = malloc(sizeof(state));
	s -> penMode = false;
	s -> xpos = 0;
        s -> ypos = 0;
	s -> dx = 0;
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

void updateState(unsigned int b, state *s, display *d){
	int opc = getOpcode(b);
	int opr = getOperand(b);
	if (opc == 1 && s -> penMode == true) drawLine(s,d,opr);
	else if (opc == 1) updatePos(s,opr);
	else if (opc == 0) s -> dx = opr;
	else if (opc == 3) togglePen(s);
	else printf("opc = 2\n");
}	

void test(){
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

int main(int n, char *args[n]) {
	if (n==1) test();
	else if (n!=2) {fprintf(stdout, "Use ./sketch filename\n"); exit(1); }
	else{
		FILE *inp = fopen(args[1],"rb");
		unsigned char b = fgetc(inp);		
		display *d = newDisplay(args[1], 200, 200);
		state *s = newState();
		while(!feof(inp)){
			updateState(b,s,d);
			b = fgetc(inp);
		}
		end(d);
		fclose(inp);
		free(s);
	}

}

