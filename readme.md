<div align="center">
  <h1>chasm</h1>
  <p>Easy to use, extremely fast Runtime Assembler.</p>
</div>

```c
#include <stdio.h>
#include "asm_x64.h"

int main() {
	char* str = "Hello World!";
	x64 code = {
		{ MOV, rax, imptr(puts) },
		{ MOV, rcx, imptr(str) }, // RDI for System V
		{ JMP, rax },
	};
	
	uint32_t len = 0;
	uint8_t* assembled = x64as(code, sizeof(code) / sizeof(code[0]), &len);
	if(!assembled) return fprintf(stderr, "%s", x64error(NULL)), 1;
	
	x64exec(assembled, len)(); // Prints "Hello World!"
	return 0;
}
```

> Download [`asm_x64.c`](asm_x64.c) and [`asm_x64.h`](asm_x64.h) into your project and just include [`asm_x64.h`](asm_x64.h) to start assembling!

Features
--------

- Simple and easy to use, only requiring 2 function calls to run your code.
- Supports AVX-256 and many other x86 extensions.
- Fast, assembling up to 100 million instructions per second.
- Easy and flexible syntax, allowing you as much freedom as possible with coding practices.
- Simple and consistent error handling system, returning 0 on failure and fast error retrieval with `x64error(NULL)`.
- Stringification of the IR for easy debugging with `x64stringify(code, len)`.

Use Cases
---------

This library is useful for any code generated dynamically from user input. This includes:

- JIT Compilers
- Emulators
- Runtime optimizations / code generation
- Testing / benchmarking software
- Writing your own assemblers!

I would highly recommend using something like [`example/vec.h`](example/vec.h) (Arena library) to dynamically push code onto a single array throughout your application with very low latency. I show this off in [`example/bf_compiler.c`](example/bf_compiler.c)!

Performance
-----------

![Screenshot 2024-12-30 192539](https://github.com/user-attachments/assets/c28d8cc3-582c-4704-ad9e-816ece2f52e0)

Assembler is built in an optimized fashion where anything that can be precomputed, is precomputed.

In the above screenshot, it's shown that an optimized build can assemble most instructions in about 15 nanoseconds, which goes down to 30 for unoptimized builds.

Considering an average of 9 nanoseconds per function call, most of that 15 ns is actually wasted on function call overhead!

API: Code
---------

`x64` is an array of `x64Ins` structs. The first member of the struct is `op`, or the operation, an enum defined by the **[`asm_x64.h`](asm_x64.h)** header. The other 4 members are `x64Operand` structs, which are just a combination of the type of operand with the value.

An example instruction `mov rax, 0` would be written as:

```c
x64 code = { MOV, rax, imm(0) };
```

Notice the use of `rax` and `imm(0)`. All x86 registers like `rax` (including `mm`s, `ymm`s etc) are defined as macros with the type `x64Operand`. Other types of macros:

- `imm()`, `im8()`, `im16`, `im32()`, `im64()` and `imptr()` for immediate values, another name for numbers embedded in the instruction encoding.
- `mem()`, `m8()`, `m16()`, `m32()`, `m64()`, `m128()`, `m256()` and `m512()` for memory addresses.
- `rel()` for control flow. Please read ***Relative Instruction References*** for more information.
  - **Note:** `rel(0)` references the current instruction, so `JMP, rel(0)` jumps back to itself infinitely! 1 jumps to the next instruction and so on.
- If an instruction supports forcing a prefix, `{PREF66}` and `{PREFREX_W}` are available.

### Memory References.

Let's start off with an example of `lea rax, ds:[rax + 100 + rdx * 2]` in chasm:

```c
x64 code = { LEA, rax, mem($rax, 100, $rdx, 2, $ds) };
```

This is a **variable** length macro, with each argument being optional. Each of the **register** arguments of the `mem()` macro have to be preceeded with a `$` prefix. Any 32 bit signed integer can be passed for the offset parameter, and only 1, 2, 4 and 8 are allowed in the 4th parameter, also called the "scale" parameter (**ANY OTHER VALUE WILL GO TO 1**, x86 limitation). The last parameter is a segment register, also preceeded with a `$`.

Other valid `mem()` syntax examples are:
* `mem($rax)`,
* `mem($none, 0, $rdx, 8)`,
* `mem($none, 0x60, $none, 1, $gs)`,
* `mem($rip, 2)` (RIP memory references only use the offset, index and scale do not work),
* `mem($riprel, 2)` (read more in ***Relative Instruction References***),
* `mem($rdx, 0, $ymm2, 4)` (VSIB).

#### Be careful when using `mem()`:
- Just like in regular assemblers, not specifying a specific size can be problematic when there's multiple possible ones.
- `m8()`, `m16()`, `m32()`, `m64()`, `m128()`, `m256()` and `m512()` specify the exact size of data referenced, erroring when that size isn't available with that instruction.
- `mem()` will not error when there's multiple sizes, instead using the smallest one available, but can cause bugs when it's not the size you intend.
- You can use `x64mem` for more flexibility in size, like `{ a > b ? M8 : M16, x64mem(<normal mem() arguments>) }`. Uppercase `M<size>` are enums for the size.
- Exception: Use `mem()` with FPU, as specifying the size doesn't mean much in the encoding.

> [!important]
> **Make sure to pass in $none for register parameters you are not using, as it will assume eax if you pass in 0!** If you omit arguments though, `$none` is assumed :)

### Relative Instruction References.

In assemblers, when you see `$+n` (`. + n` in GAS), it's a special syntax that lets you have a relative instruction based offset, as calculating the actual offset is impossible with a variable length encoding. Chasm also has an answer to this with `rel()` and `mem($riprel)`. Here's an example of both:

```c
x64 code = {
  { MOV,  rax,   imm(1)          }, // 1 Iteration
  { LEA,  rcx,   mem($riprel, 2) }, // ━┓ "lea rcx, [$+2]"
  { PUSH, rcx                    }, //  ┃ Pushes this address on the stack. Equivalent to "call $+2"
  { DEC,  rax                    }, // ◄┛
  { JZ,   rel(2)                 }, // ━┓ "jz $+2" (jumps out of the loop).
  { RET                          }, //  ┃ Pops the pushed pointer off and jumps, basically "jmp $-2"
};
```

Simply, the number supplied is used to reference that many instructions ahead of the current instruction. `0` means the current instruction. `{ JMP, rel(0) }` would halt the processor, so be careful.

More examples in [`example/bf_compiler.c`](example/bf_compiler.c).

> [!Important]
> To get actual results with this syntax, you need to link your code with `x64as()`!


API: Functions
--------------

### <pre lang="c">uint8_t* x64as(x64 code, size_t len, uint32_t* outlen);</pre>

#### Assembles and soft links code, dealing with `$riprel` and `rel()` syntax and returning the assembled code.

- Returns NULL if an error occured, retrieved with `x64error()`.
- The length of the assembled code is stored in `outlen`.
- Internally allocates returned code, freed with `free()`.

### <pre lang="c">uint32_t x64emit(const x64Ins* ins, uint8_t* opcode_dest);</pre>

#### Assembles a single instruction and stores it in `opcode_dest`.

- Returns the length of the instruction in bytes. If it returns 0, an error has occurred.
- `opcode_dest` needs to be a buffer of at least 15 bytes to accomodate any/all x86 instructions.
- This function does not perform any linking, so in the case of there being no relative references in your code, it's likely much faster to loop with this function than to use `x64as()`.

Example of such loop:

```c
char buf[128];
uint32_t buf_len = 0;

for(size_t i = 0; i < sizeof(code) / sizeof(code[0]); i++) {
  const uint32_t len = x64emit(&code[i], buf + buf_len);
  
  if(!len) {
    fprintf(stderr, "%s", x64error(NULL));
    return 1;
  }
  
  buf_len += len;
}
```

A loop similar to this is used internally in `x64as()`!

### <pre lang="c">void (*x64exec(void* mem, uint32_t size))();</pre>

#### Uses a Syscall to allocate memory with the EXecute bit set, so you can execute your code.

- Returns a function pointer to the code, which you can call to run your code.
  - Free this memory with `x64exec_free()`.

### <pre lang="c">void x64exec_free(void* mem, uint32_t size);</pre>

#### Frees memory allocated by `x64exec()`.

> [!note]
> Store the size of the memory you requested with `x64exec()` as you will need to pass it in here, at least for Unix.

### <pre lang="c">char* x64stringify(const x64 p, uint32_t num);</pre>

#### Stringifies the IR. Useful for debugging and inspecting it.

- Returns a string, NULL if an error occurred which will be accessible with `x64error()`.
- Returned string uses Intel ASM Syntax, like `mov [rax + rdx * 2], 20`. Multiple instructions are preceeded with a tab.

### <pre lang="c">char* x64error(int* errcode);</pre>

#### Gets the error message of the last error that occured.

- Returns a string with a description of the error.
- If `errcode` is not NULL, it will be set to the error code.


Limitations
-----------

- No support for 32 bit legacy / protected mode instructions.
- No support for AVX-512.
  - Trying to change this, maybe with syntax like `ymm(10, k1, z)`.
- No support for architectures other than x86-64 (like ARM).
- Labels, as they are extremely difficult to implement in a sane manner and take up too much memory.

If people seem to need support for any of these limitations, I will try my best to add them! In my personal use, I haven't needed them so I haven't gone through the effort.

I have tried very hard to add labels, and nothing seems to be elegant. I'm open to it if someone can draft a good plan for it! My goal is to support it fully without limiting strings to string literals only, if I were to support it at all. You can still see remnants of previous attempts in [`asm_x64.c`](asm_x64.c).

Also, support for other instruction sets will come when I get to them, and I when get some good tables that give me the exact information I need! I currently use a modified table from [StanfordPL/x64asm](https://github.com/StanfordPL/x64asm). Their table has some incorrect instructions, so I wouldn't suggest using that one for your own projects.

License
-------

Chasm is dual licensed under the MIT Licence and Public Domain. You can choose the licence that suits your project the best. The MIT Licence is a permissive licence that is short and to the point. The Public Domain licence is a licence that makes the software available to the public for free and with no copyright.

FAQ
---

### Why the name **chasm**?

> A chasm is a deep ravine or hole formed through millenia of erosion and natural processes. This name struck a chord in my heart, as I have been working on this library for over a year, and it's the best way I know of getting low and deep into the heart of computing. I also loved that it had "asm" and "c" in it, which are big parts of what this library is about.

### This library doesn't fit me. What are some others?

> - [asmjit/asmjit](https://github.com/asmjit/asmjit) - A popular choice for C++ developers and many times more complex and feature rich than chasm.
> - [aengelke/fadec](https://github.com/aengelke/fadec) - Similar library to chasm but a completely different API that might be more flexible but harder for some.
> - [bitdefender/bddisasm](https://github.com/bitdefender/bddisasm) - Fast, easy to use Disassembler library.
> - [garc0/CTAsm](https://github.com/garc0/CTAsm) - Compile time assembler for C++ using only templates.

### Where did you get the idea to make something like this?

> The first time I saw a library like this was when I found https://github.com/StanfordPL/x64asm. I loved the idea, but I couldn't use their library from C or Windows, so I took the liberty to redesign some of their library. I use their table to generate my own table in [`asm_x64.c`](asm_x64.c) and while I haven't used any of their code, I did take inspiration from how they did instruction operands.

### Where is the full source for all scripts and tests shown?

> All source is in [aqilc/rustscript](https://github.com/aqilc/rustscript/tree/main/lib/asm). Tests testing all the features and operands are [here](https://github.com/aqilc/rustscript/blob/main/tests/asm.c).
