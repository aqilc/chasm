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
			vpusharr(ret, {
				{ LEA, rsi, m64($riprel, 0) }, // 0 here means $+0 or the current instruction
				{ PUSH, rsi }
			});
			rax_garbled = true; // Because ] overwrites rax, so it's probably overwritten and if it's not, nothing bad happens.
			break;
		case '.':
			rax_garbled = true;
			vpusharr(ret, {
				{ MOV, rax, mem($rbp, -8) },
				{ MOV, rcx, mem($rax) },
				{ SUB, rsp, imm(64) }, // Should investigate aligining the stack to 16 bytes but this works for now(what msvc does).
				{ MOV, rax, imptr(putchar) },
				{ CALL, rax },
				{ ADD, rsp, imm(64) },
			});
			break;
		case ']':
			rax_garbled = true;
			vpusharr(ret, {
				{ MOV, rax, mem($rbp, -8) },
				{ CMP, m8($rax), imm(0) },
				{ JZ, rel(3) },
				{ POP, rax },
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
	return ret;
}




int main() {
	x64Ins* ins = bf_compile(prog1);
	uint32_t len = 0;
	void* compiled = x64as(ins, vlen(ins), &len);

	uint8_t buf[256];
	((void (*)(uint8_t*)) x64exec(compiled, len))(buf); // prints "Hello World!"
}
