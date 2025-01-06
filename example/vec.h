/*
 * vec.h v1.0.0 - Aqil Contractor @aqilc 2025
 * Licenced under Attribution-NonCommercial-ShareAlike 3.0
 *
 * This file includes all of the source for the Vector library macros and functions.
 * Compile by adding a file called `vec.c` with the following contents in your project:
 *     #define VEC_H_IMPLEMENTATION
 *     #include <vec.h>
 *
 * Options provided by defines before including:
 *   VEC_H_STATIC_INLINE:
 *     Implements all methods as static inline, for increased performance when calling. Automatically defines VEC_H_IMPLEMENTATION.
 *   VEC_H_MORE_MACROS:
 *     Instead of defining functions for extremely simple operations, exports them as macros instead
 *   VEC_H_CALLOC:
 *     Name for user-provided calloc(size_t, size_t) function.
 *   VEC_H_REALLOC:
 *     Name for user-provided realloc(void*, size_t) function.
 *   VEC_H_FREE:
 *     Name for user-provided free(void*) function.
 *   VEC_H_OVERLOAD_ALLOCATORS:
 *     Define `void* vnew()` as you see fit, but after initial allocation, cast to struct vecdata_* and add 1 to pointer as done in current implementation.
 *     Define re-allocator function with the function signature: struct vecdata_* name(struct vecdata_* data, uint32_t size);
 *     Make sure to define VEC_H_REALLOC_FUNC with `name` after redefining.
 *     
 * WARNING: CURRENTLY NOT FULLY THREAD SAFE. Use with caution when using in thread safe code!
 */

#ifndef VEC_H
#define VEC_H

// #define VEC_H_IMPLEMENTATION
// #define VEC_H_STATIC_INLINE

#ifdef VEC_H_STATIC_INLINE
	#define VEC_H_IMPLEMENTATION
	#define VEC_H_EXTERN static inline
#else
	#define VEC_H_EXTERN 
#endif

#include <stdint.h>
#include <string.h>

struct vecdata_ {
	uint32_t used;
	uint32_t cap;
	uint8_t data[];
};
#define _DATA(x) ((struct vecdata_*)(x) - 1)
#define vlen(x) (_DATA(x)->used / sizeof(*(x)))
#define vcap(x) (_DATA(x)->cap / sizeof(*(x)))

// The normal push macro, pushes a value onto the obj
#define vpush(x, ...) (*((typeof(x)) vpush_((void**)&(x), sizeof(*(x)))) = (typeof(*x)) __VA_ARGS__)
// vpush(data, 5); expands to something like: *(int*)vpush_((void**) &data, 4) = (int) 5;

#define vpusharr(x, ...) (memcpy(vpush_((void**)&(x), sizeof((typeof(*(x))[]) __VA_ARGS__)), (typeof(*(x))[]) __VA_ARGS__, sizeof((typeof(*(x))[]) __VA_ARGS__)))
#define vcpyarr(x, n, ...) (memcpy((x) + (n), (typeof(*(x))[]) __VA_ARGS__, sizeof((typeof(*(x))[]) __VA_ARGS__)))

// Push n items onto the vector, so we can allocate more space at once
#define vpushn(x, n, y) vpushn_((void**) &(x), (n), sizeof(*(x)), &((typeof(*(x))) {y}))
// One for structs since that prev one didn't work for structs
#define vpushnst(x, n, y) vpushn_((void**) &(x), (n), sizeof(*(x)), &((typeof(*(x))) y))

// String push aliases so u don't have to &
#define vpushs(x, y) strcpy((char*) vpush_((void**) &(x), strlen(y) + 1) - 1, y);
#define vpushsf(x, ...) vpushsf_((void**) &(x), __VA_ARGS__)


// Push the entirety of a vec onto another
#define vpushv(x, y) (memcpy(vpush_((void**) &(x), _DATA(y)->used), y, _DATA(y)->used))

// Add values to the beginning of the vec
#define vunshift(x, ...) (*(typeof(x))vunshift_((void**)&(x), sizeof(*(x))) = (typeof(*x)) __VA_ARGS__)

// Simplifies vclear and vtostr calls
// #define vtostr(v) vtostr_((void**)&(v))

// Pops off the last element and returns it
#ifdef VEC_H_MORE_MACROS
	#define vpop(x) (_DATA(x)->data + (_DATA(x)->used -= sizeof(*(x))))
	#define vpopn(x, n) (_DATA(x)->data + (_DATA(x)->used -= (n) * sizeof(*(x))))
	#define vpopto(x, idx) (_DATA(x)->data + (_DATA(x)->used = (idx) * sizeof(*(x))))
	#define vempty(x) (_DATA(x)->data + (_DATA(x)->used = 0))
	#define vfree(x) VEC_H_FREE(_DATA(x))
#else
	#define vpop(x) vpop_((x), sizeof(*(x)))
	#define vpopn(x, n) vpop_((x), (n) * sizeof(*(x)))
	#define vpopto(x, n) vpop_((x), (vlen(x) - n) * sizeof(*(x)))
#endif

// Removes data from the middle of the array
#define vremove(x, idx) vremove_((x), sizeof(*(x)), (idx))

// Pointer to the last element of the vector
#define vlast(x) ((typeof(x)) ((char*)(x) + _DATA(x)->used) - 1)

// Prealloc more space before setting elements.
#define vprealloc(x, n) vpush_((void**)&(x), sizeof(*(x)) * (n))

// New vector initialized with a struct or array
#define vecify(x) ((typeof(x))vnewn(sizeof(x)) = x)

// For loop that iterates over the vector
#define vfor(x, y) for(typeof(x) y = x, _end = x + vlen(x); y < _end; y ++)

// V String that has length info and is automatically push-able
typedef char* vstr;

// All you need to get started with this vector lib!
VEC_H_EXTERN void* vnew();
VEC_H_EXTERN void* vnewn(uint32_t n);
// #define vnew() ((void*) ((struct vecdata_*) calloc(1, sizeof(struct vecdata_)) + 1))

// Returns a *new* concatenated vector, use `pushv` if you don't want a new vec :D
VEC_H_EXTERN void* vcat(void* a, void* b);

// Returns a 1 if they're different, 0 if they're the same
VEC_H_EXTERN char  vcmp(void* a, void* b);

// Initialize a vector with a string straight away
// VEC_H_EXTERN char* strtov(char* s);
VEC_H_EXTERN char* vtostr(void* v);
VEC_H_EXTERN void  vremove_(void* v, uint32_t size, uint32_t pos);
VEC_H_EXTERN void* vpush_(void** v, uint32_t size);
VEC_H_EXTERN void  vpushsf_(void** v, char* fmt, ...);
VEC_H_EXTERN void  vpushn_(void** v, uint32_t n, uint32_t size, void* thing);
// VEC_H_EXTERN void* vunshift_(void** v, uint32_t size);
VEC_H_EXTERN char* vfmt(char* str, ...);

#ifndef VEC_H_MORE_MACROS
	VEC_H_EXTERN void* vpop_(void* v, uint32_t size);
	VEC_H_EXTERN void* vempty(void* v);
	VEC_H_EXTERN void  vfree(void* v);
#endif

#endif

#ifdef VEC_H_IMPLEMENTATION

#if !defined VEC_H_FREE || !defined VEC_H_REALLOC || !defined VEC_H_CALLOC
	#include <stdlib.h>
#endif

#ifndef VEC_H_CALLOC
	#define VEC_H_CALLOC calloc
#endif

#ifndef VEC_H_REALLOC
	#define VEC_H_REALLOC realloc
#endif

#ifndef VEC_H_FREE
	#define VEC_H_FREE free
#endif

#include <stdarg.h>
#include <stdio.h>

#ifndef VEC_H_OVERLOAD_ALLOCATORS
	// Callocs a vec with a cap of 16 so subsequent pushes don't immediately trigger reallocation.
	VEC_H_EXTERN void* vnew() {
		struct vecdata_* v = VEC_H_CALLOC(1, sizeof(struct vecdata_) + 16 * sizeof(char));
		v->cap = 16;
		return v + 1;
	}

	VEC_H_EXTERN void* vnewn(uint32_t n) {
		struct vecdata_* v = VEC_H_CALLOC(1, sizeof(struct vecdata_) + n * sizeof(char));
		v->cap = n;
		return v + 1;
	}
#endif

// Combines two vectors into a new vector
VEC_H_EXTERN void* vcat(void* a, void* b) {
	struct vecdata_* v = VEC_H_CALLOC(1, _DATA(b)->used + _DATA(a)->used + sizeof(struct vecdata_));
	v->cap = v->used = _DATA(b)->used + _DATA(a)->used;
	v += 1;
	memcpy(v, a, _DATA(a)->used);
	memcpy((char*) v + _DATA(a)->used, b, _DATA(b)->used);
	return v;
}

VEC_H_EXTERN char vcmp(void* a, void* b) {
	uint32_t len = _DATA(a)->used;
	if(len != _DATA(b)->used) return 1;
	for(uint32_t idx = 0; idx < len; idx ++)
		if(((char*)a)[idx] != ((char*)b)[idx]) return 1;
	return 0;
}

VEC_H_EXTERN char* strtov(char* s) {
	uint32_t len = strlen(s);
	struct vecdata_* v = VEC_H_CALLOC(1, len + sizeof(struct vecdata_));
	v->used = v->cap = len;
	v += 1;
	memcpy(v, s, len);
	return (char*) v;
}

VEC_H_EXTERN char* vtostr(void* v) {
	uint32_t len = _DATA(v)->used;
	char* str = malloc(len + 1);
	memcpy(str, v, len);
	str[len] = 0;
	return str;
}

#ifndef VEC_H_MORE_MACROS
	VEC_H_EXTERN void* vempty(void* v) { _DATA(v)->used = 0; return v; }
	VEC_H_EXTERN void* vpop_(void* v, uint32_t size) { _DATA(v)->used -= size; return _DATA(v)->data + _DATA(v)->used; }
	VEC_H_EXTERN void vfree(void* v) { VEC_H_FREE(_DATA(v)); }
#endif



// Reallocs more size for the array, hopefully without moves
#ifndef VEC_H_OVERLOAD_ALLOCATORS
	#define VEC_H_REALLOC_FUNC alloc_
	static inline void* alloc_(struct vecdata_* data, uint32_t size) {
		data->used += size;
		if(data->cap < data->used) {
			data->cap = data->used + (data->used >> 2) + 16;
			return (struct vecdata_*)VEC_H_REALLOC(data, sizeof(struct vecdata_) + data->cap) + 1;
		}
		return data + 1;
	}
#elif !defined VEC_H_REALLOC_FUNC
	#error "Need to define re-allocator for the Vector library before implementation."
#endif


// Pushes more data onto the array, CAN CHANGE THE PTR U PASS INTO IT
#ifdef VEC_H_STATIC_INLINE
	#define VEC_INTERNAL_PUSH_NAME vpush_
#else
	#define VEC_INTERNAL_PUSH_NAME vpush__
#endif

static inline void* VEC_INTERNAL_PUSH_NAME(void** v, uint32_t size) {
	struct vecdata_* data = _DATA(*v = VEC_H_REALLOC_FUNC(_DATA(*v), size));
	return data->data + data->used - size;
}

#ifndef VEC_H_STATIC_INLINE
	VEC_H_EXTERN void* vpush_(void** v, uint32_t size) { return VEC_INTERNAL_PUSH_NAME(v, size); }
#endif

// Gets length of formatted string to allocate from vector first, and then basically writes to the ptr returned by push
VEC_H_EXTERN void vpushsf_(void** v, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	va_list args2;
	va_start(args2, fmt);
	uint32_t len = vsnprintf(NULL, 0, fmt, args);
	vsnprintf(VEC_INTERNAL_PUSH_NAME(v, len), len, fmt, args2);
	va_end(args);
	va_end(args2);
}

VEC_H_EXTERN void vpushn_(void** v, uint32_t n, uint32_t size, void* thing) {
	char* place = VEC_INTERNAL_PUSH_NAME(v, n * size);
	if(size == 1) memset(place, *((char*) thing), size);
	else for(uint32_t i = 0; i < n; i ++) memcpy(place + size * i, thing, size);
}

// Adds an element at the start of the vector, ALSO CHANGES PTR
VEC_H_EXTERN void* vunshift_(void** v, uint32_t size) {
	memmove((char*) (*v = alloc_(_DATA(*v), size)) + size, *v, _DATA(*v)->used);
	return *v;
}

// Deletes data from the middle of the array
VEC_H_EXTERN void vremove_(void* v, uint32_t size, uint32_t pos) {
	memmove(_DATA(v) + pos, _DATA(v) + pos + size, _DATA(v)->used - pos - size);
	_DATA(v)->used -= size;
}

VEC_H_EXTERN char* vfmt(char* str, ...) {
	static char* fmtstr;
	if(!fmtstr) fmtstr = vnew();
	
	va_list args, args2;
	va_start(args, str);
	va_copy(args2, args);
	uint32_t len = vsnprintf(NULL, 0, str, args) + 1;
	if(len - _DATA(fmtstr)->used > 0)
		VEC_INTERNAL_PUSH_NAME((void**) &fmtstr, len - _DATA(fmtstr)->used);
	vsnprintf(fmtstr, len, str, args2);
	va_end(args);
	va_end(args2);
	return fmtstr;
}
#endif
