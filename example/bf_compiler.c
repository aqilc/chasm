#include <stdio.h>
#include "../asm_x64.h"

#define VEC_H_IMPLEMENTATION
#include "vec.h"

#define PROG(...) #__VA_ARGS__

char* prog1 = PROG(
+++++ +++++             initialize counter (cell #0) to 10
[                       use loop to set the next four cells to 70/100/30/10
	> +++++ ++              add  7 to cell #1
	> +++++ +++++           add 10 to cell #2 
	> +++                   add  3 to cell #3
	> +                     add  1 to cell #4
	<<<< -                  decrement counter (cell #0)
]                   
> ++ .                  print 'H'
> + .                   print 'e'
+++++ ++ .              print 'l'
.                       print 'l'
+++ .                   print 'o'
> ++ .                  print ' '
<< +++++ +++++ +++++ .  print 'W'
> .                     print 'o'
+++ .                   print 'r'
----- - .               print 'l'
----- --- .             print 'd'
> + .                   print '!'
> .                     print '\n'
);

x64Ins* bf_compile(char* in) {
	x64Ins* ret = vnew();

	// Keeps track of loops in a tree structure where we can rewind and pop to the parent node when needed.
	struct { int start, end, parent_idx; }* loops = vnew();
	int parentloop = -1;
	
#ifdef _WIN32
	x64Operand arg1 = rcx, arg2 = rdx;
#else
	x64Operand arg1 = rdi, arg2 = rsi;
#endif
	
	vpusharr(ret, {
		{ PUSH, rbp },
		{ MOV, rbp, rsp },
		{ MOV, rax, arg1 },
		{ PUSH, arg1 }
	});

	bool rax_garbled = false;

	while(*in) {
		switch(*in) {
		case '>':
		case '<':
			vpush(ret, { *in == '>' ? INC : DEC, m64($rbp, -8) });
			rax_garbled = true;
			break;
		case '+':
		case '-': {
			if(rax_garbled) {
				vpush(ret, { MOV, rax, m64($rbp, -8) });
				rax_garbled = false;
			}

			vpush(ret, { *in == '+' ? INC : DEC, m8($rax) });
			break;
		}
		case '[':
			vpush(loops, { vlen(ret) + 2 /* 3 instructions down is JZ */, -1, parentloop });
			parentloop = vlen(loops) - 1; // Tree of loops, as each loop instruction length needs to be resolved to be able to be skipped.
			
			vpusharr(ret, {
				{ MOVZX, eax, m8($rax) },
				{ TEST, al, al },
				{ JZ }, // First argument (where to jump) to be filled in later!
				{ LEA, rsi, m64($riprel, 0) }, // 0 here means $+0 or the current instruction
				{ PUSH, rsi }
			});
			rax_garbled = true; // Because ] overwrites rax, so it's probably overwritten and if it's not, nothing bad happens.
			break;
		case '.':
			rax_garbled = true;
			vpusharr(ret, {
				{ MOV, rax, mem($rbp, -8) },
				{ MOV, arg1, mem($rax) },
				{ SUB, rsp, imm(64) }, // Should investigate aligining the stack to 16 bytes but this works for now(what msvc does).
				{ MOV, rax, imptr(putchar) },
				{ CALL, rax },
				{ ADD, rsp, imm(64) },
			});
			break;
		case ']':
			loops[parentloop].end = vlen(ret) + 5;
			if((parentloop = loops[parentloop].parent_idx) == -1) parentloop = 0;

			rax_garbled = true;
			vpusharr(ret, {
				{ MOV, rax, mem($rbp, -8) },
				{ CMP, m8($rax), imm(0) },
				{ POP, rax },
				{ JZ, rel(2) },
				{ JMP, rax },
			});
		default: break;
		}
		in++;
	}

	vpusharr(ret, {
		{ MOV, rsp, rbp },
		{ POP, rbp },
		{ RET },
	});

	for(int i = 0; i < vlen(loops); i ++) {
		int end = loops[i].end, start = loops[i].start;
		if (end == -1) end = vlen(ret) - 4; // compensating for the last 3 instructions.

		ret[start].params[0] = rel(end - start);
	}
	return ret;
}




int main() {
	x64Ins* ins = bf_compile(prog1);
	uint32_t len = 0;
	void* compiled = x64as(ins, vlen(ins), &len);

	uint8_t buf[256];
	void (*hello_world)(uint8_t*) = x64exec(compiled, len);
	hello_world(buf); // prints "Hello World!"

	x64exec_free(hello_world, len);
}
