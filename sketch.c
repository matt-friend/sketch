#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
	if ((b & 0x20) != 0) c = -32 + c;
	printf("%02x,%d\n",b,c);
	return c;
}

void updatePos(state *s, display *d, int dy){
	s -> xpos = s -> xpos + s-> dx;
	s -> ypos = s -> ypos + dy;
	s -> dx = 0;
}

void drawLine(state *s, display *d, int dy){
	line(d, s -> xpos, s -> ypos, s -> xpos + s -> dx, s -> ypos + dy);
	updatePos(s,d,dy);
}

void togglePen(state *s){
	if (s -> penMode == true) s -> penMode = false;
	else s -> penMode = true;
}

void updateState(unsigned int b, state *s, display *d){
	int opc = getOpcode(b);
	int opr = getOperand(b);
	if (opc == 1 && s -> penMode == true) drawLine(s,d,opr);
	else if (opc == 1) updatePos(s,d,opr);
	else if (opc == 0) s -> dx = opr;
	else if (opc == 3) togglePen(s);
	else printf("opc = 2\n");
}	

int main(int n, char *args[n]) {
	if (n!=2) {fprintf(stdout, "Use ./sketch filename\n"); exit(1); }
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

