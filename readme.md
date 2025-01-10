<div align="center">
  <h1>chasm</h1>
  <p>Easy to use, extremely fast Runtime Assembler.</p>
</div>

```c
#include <stdio.h>
#include "lib/asm/asm_x64.h"

int main() {
	char* str = "Hello World!";
	x64 code = {
		{ MOV, rax, imptr(puts) },
		{ MOV, rcx, imptr(str) }, // RDI for System V
		{ JMP, rax },
	};
	
	uint32_t len = 0;
	uint8_t* assembled = x64as(code, sizeof(code) / sizeof(code[0]), &len);
	if(!assembled) return 1;
	
	x64exec(assembled, len)(); // Prints "Hello World!"
	return 0;
}
```
#### Usage

> Download [`asm_x64.c`](asm_x64.c) and [`asm_x64.h`](asm_x64.h) into your project and just include [`asm_x64.h`](asm_x64.h) to start assembling!

Features
--------

- Simple and easy to use, only requiring 2 function calls to run your code.
- Supports AVX-256 and many other x86 extensions.
- Fast, assembling up to 100 million instructions per second.
- Easy and flexible syntax, allowing you as much freedom as possible with coding practices.
- Simple error handling system, returning 0 from a function if it failed and fast error retrieval with `x64error(NULL)`.
- Stringification of the IR for easy debugging with `x64stringify(code, len)`.

Use Cases
---------

This library is useful for any code generated dynamically from user input. This includes:

- JIT Compilers
- Emulators
- Runtime optimizations / code generation
- Testing / benchmarking software
- Writing your own assemblers!

Performance
-----------

![Screenshot 2024-12-30 192539](https://github.com/user-attachments/assets/c28d8cc3-582c-4704-ad9e-816ece2f52e0)

Assembler is built in an optimized fashion where anything that can be precomputed, is precomputed.
In the above screenshot, it's shown that an optimized build can assemble most instructions in about 15 nanoseconds, which goes down to 30 for unoptimized builds.

API
---

### Code

`x64` is an array of `x64Ins` structs. The first member of the struct is `op`, or the operation, an enum defined by the **[`asm_x64.h`](asm_x64.h)** header. The other 4 members are `x64Operand` structs, which are just a combination of the type of operand with the value.

An example instruction `mov rax, 0` would be written as:

```c
x64 code = { MOV, rax, imm(0) };
```

Notice the use of `rax` and `imm(0)`. All x86 registers like `rax` (including `mm`s, `ymm`s etc) are defined as macros with the type `x64Operand`. Other types of macros:

- `imm()`, `im8()`, `im16`, `im32()`, `im64()` and `imptr()` for immediate values.
- `mem()`, `m8()`, `m16()`, `m32()`, `m64()`, `m128()`, `m256()` and `m512()` for memory addresses.
- `rel()` for relative offsets referencing other instructions. The use of this macro requires soft linking if used that way.
  - **Note:** `rel(0)` references the current instruction, so `JMP, rel(0)` jumps back to itself infinitely! Pass in 1 to jump to the next instruction.

#### `mem()` and `m<size>()` syntax.

> `m8()`, `m16()`, `m32()`, `m64()`, `m128()`, `m256()` and `m512()` are more specific versions of the `mem()` macro, which references any and every size of memory for ease of use. Generally, if you know the size of memory accessed, use the size specific version of the macro that matches with the bit width of the other operands. All of these macros have the same syntax.

Let's start off with an example:

```c
x64 code = { LEA, rax, mem($rax, 0x0ffe, $rdx, 2, $ds) };
```

This is a **variable** length macro, with each argument being optional. Each of the **register** arguments of the `mem()` macro have to be preceeded with a `$` prefix. Any 32 bit signed integer can be passed for the offset parameter, and only 1, 2, 4 and 8 are allowed in the 4th parameter, also called the "scale" parameter (**ANY OTHER VALUE WILL GO TO 1**, x86 limitation). The last parameter is a segment register, also preceeded with a `$`. **Make sure to pass in $none for register parameters you are not using, as it will assume eax otherwise**

Other valid `mem()` syntax examples are: `mem($rax)`, `mem($none, 0, $rdx, 8)` and with VSIB `mem($rdx, 0, $ymm2, 4)`.

#### `mem($riprel)`

`$rip` is a valid register to use with `mem()`, but it's not very useful when you might not know the byte-length of the instructions in between the ones you're trying to reference. This is where `$riprel` can be used as the base register for `mem()` allowing you to reference other instructions without knowing the byte-length in between! In `$riprel`, just like `rel()`, 0 means the current instruction. This is the answer to `lea rax, [$+1]` syntax provided by many assemblers. Here's an example:

```c
x64 code = {
  { MOV,  rax,   imm(1)          }, // 1 Iteration
  { LEA,  rcx,   mem($riprel, 3) }, // ━┓
  { PUSH, rcx                    }, //  ┃ Pushes this address on the stack.
  { XOR,  rcx,   rcx             }, //  ┃
  { DEC,  rax                    }, // ◄┛
  { JZ,   rel(2)                 }, // Jumps out of the loop.
  { RET                          }, // Pops the previously pushed pointer off and goes to it, basically JMP, rel(-2)
};
```

There's also an example in [`examples/bf_compiler.c`](examples/bf_compiler.c).

> [!Important]
> To get actual results with this syntax, you need to link your code with `x64as()`! Index and scale also do not work with `$rip` or `$riprel` as base registers.

### Functions

#### <pre lang="c">uint8_t* x64as(x64 code, size_t len, uint32_t* outlen);</pre>

#### Assembles and soft links code, dealing with `$riprel` and `rel()` syntax and returning the assembled code.

- Returns NULL if an error occured, and sets the error code to the `x64error` variable.
- The length of the assembled code is stored in `outlen`.

#### <pre lang="c">uint32_t x64emit(const x64Ins* ins, uint8_t* opcode_dest);</pre>

#### Assembles a single instruction and stores it in `opcode_dest`.

- Returns the length of the instruction in bytes. If it returns 0, an error has occurred.
- This function does not perform any linking, so it's likely much faster to loop with this function than to use x64as() if you do not have any `rel()` or `mem($riprel)`s in your code.

Example of such loop:

```c
char buf[128];
uint32_t buf_len = 0;

for(size_t i = 0; i < sizeof(code) / sizeof(code[0]); i++) {
  const uint32_t len = x64emit(&code[i], buf + buf_len);
  
  if(!len) { // Or any other kind of error handling code. This is what x64as() does internally.
    fprintf(stderr, "%s", x64error(NULL));
    return 1;
  }
  
  buf_len += len;
}
```

#### <pre lang="c">void (*x64exec(void* mem, uint32_t size))();</pre>

#### Uses a Syscall to allocate memory with the EXecute bit set, so you can execute your code.

- Returns a function pointer to the code, which you can call to run your code.
  - Free this memory with `x64exec_free()`.

#### <pre lang="c">void x64exec_free(void* mem, uint32_t size);</pre>

#### Frees memory allocated by `x64exec()`.

> [!note]
> Store the size of the memory you requested with `x64exec()` as you will need to pass it in here, at least for Unix.

#### <pre lang="c">char* x64stringify(const x64 p, uint32_t num);</pre>

#### Stringifies the IR. Useful for debugging and inspecting it.

- Returns a string, NULL if an error occurred which will be accessible with `x64error()`.

#### <pre lang="c">char* x64error(int* errcode);</pre>

#### Gets the error message of the last error that occured.

- Returns a string with a description of the error.
- If `errcode` is not NULL, it will be set to the error code.


Limitations
-----------

Currently does not support protected mode instructions (32 bit instructions) and ARM instructions at all. I do plan to eventually add support for them in the future as soon as I can find a good table of all available instructions.

**I currently do not support AVX-512!!**

This is because AVX-512 has a lot of limitations ([SIMD instructions lowering CPU frequency](https://stackoverflow.com/questions/56852812/simd-instructions-lowering-cpu-frequency)) and little performance benefits outside of very niche use cases. Even big compilers like GCC and Clang refuse to emit AVX-512 unless forced to do so.

AVX-512 addition is definitely a possibility though and I do have some ideas of how the syntax would work. `ymm(10, k1, z)` for example.

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

Thanks to
---------

Very grateful to https://github.com/StanfordPL/x64asm for giving me the idea and inspiration to create this! I couldn't use their library from Windows or through C, so I took inspiration from what they did and wrote my library in C. I use their table to generate some of the code in [`asm_x64.c`](asm_x64.c) and while I didn't take any code from them, I did take inspiration for how to do operands.
