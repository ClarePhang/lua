/*
** $Id: lopcodes.h,v 1.48 2000/03/10 18:37:44 roberto Exp roberto $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lopcodes_h
#define lopcodes_h



/*===========================================================================
  We assume that instructions are unsigned numbers with 4 bytes.
  All instructions have an opcode in the 8 bits. Moreover,
  an instruction can have 0, 1, or 2 arguments. There are 4 types of
  Instructions:
  type 0: no arguments
  type 1: 1 unsigned argument in the higher 24 bits (called `U')
  type 2: 1 signed argument in the higher 24 bits          (`S')
  type 3: 1st unsigned argument in the higher 16 bits      (`A')
          2nd unsigned argument in the middle 8 bits       (`B')

  The signed argument is represented in excess 2^23; that is, the number
  value is the usigned value minus 2^23.
===========================================================================*/

#define SIZE_INSTRUCTION	32

#define SIZE_OP		8
#define SIZE_U		(SIZE_INSTRUCTION-SIZE_OP)
#define POS_U		SIZE_OP
#define SIZE_S		(SIZE_INSTRUCTION-SIZE_OP)
#define POS_S		SIZE_OP
#define SIZE_B		8
#define POS_B		SIZE_OP
#define SIZE_A		(SIZE_INSTRUCTION-(SIZE_OP+SIZE_B))
#define POS_A		(SIZE_OP+SIZE_B)

#define EXCESS_S	(1<<(SIZE_S-1))		/* == 2^23 */


/* creates a mask with `n' 1 bits at position `p' */
#define MASK1(n,p)	((~((~(Instruction)0)<<n))<<p)

/* creates a mask with `n' 0 bits at position `p' */
#define MASK0(n,p)	(~MASK1(n,p))

/*
** the following macros help to manipulate instructions
*/

#define MAXARG_U	((1<<SIZE_U)-1)
#define MAXARG_S	((1<<(SIZE_S-1))-1)  /* `S' is signed */
#define MAXARG_A	((1<<SIZE_A)-1)
#define MAXARG_B	((1<<SIZE_B)-1)

#define GET_OPCODE(i)	((OpCode)((i)&MASK1(SIZE_OP,0)))
#define GETARG_U(i)	((int)((i)>>POS_U))
#define GETARG_S(i)	((int)((i)>>POS_S)-EXCESS_S)
#define GETARG_A(i)	((int)((i)>>POS_A))
#define GETARG_B(i)	((int)(((i)>>POS_B) & MASK1(SIZE_B,0)))

#define SET_OPCODE(i,o)	((i) = (((i)&MASK0(SIZE_OP,0)) | (Instruction)(o)))
#define SETARG_U(i,u)	((i) = (((i)&MASK0(SIZE_U,POS_U)) | \
                               ((Instruction)(u)<<POS_U)))
#define SETARG_S(i,s)	((i) = (((i)&MASK0(SIZE_S,POS_S)) | \
                               ((Instruction)((s)+EXCESS_S)<<POS_S)))
#define SETARG_A(i,a)	((i) = (((i)&MASK0(SIZE_A,POS_A)) | \
                               ((Instruction)(a)<<POS_A)))
#define SETARG_B(i,b)	((i) = (((i)&MASK0(SIZE_B,POS_B)) | \
                               ((Instruction)(b)<<POS_B)))

#define CREATE_0(o)	 ((Instruction)(o))
#define CREATE_U(o,u)	 ((Instruction)(o) | (Instruction)(u)<<POS_U)
#define CREATE_S(o,s)	 ((Instruction)(o) | ((Instruction)(s)+EXCESS_S)<<POS_S)
#define CREATE_AB(o,a,b) ((Instruction)(o) | ((Instruction)(a)<<POS_A) \
                                           |  ((Instruction)(b)<<POS_B))


/*
** K = U argument used as index to `kstr'
** J = S argument used as jump offset (relative to pc of next instruction)
** L = U argument used as index of local variable
** N = U argument used as index to `knum'
*/

typedef enum {
/*----------------------------------------------------------------------
name		args	stack before	stack after	side effects
------------------------------------------------------------------------*/
OP_END,/*	-	-		(return)			*/
OP_RETURN,/*	U	-		(return)			*/

OP_CALL,/*	A B	v_n-v_1 f(at a)	r_b-r_1		f(v1,...,v_n)	*/
OP_TAILCALL,/*	A B	v_n-v_1 f(at a)	(return)	f(v1,...,v_n)	*/

OP_PUSHNIL,/*	U	-		nil_1-nil_u			*/
OP_POP,/*	U	a_u-a_1		-				*/

OP_PUSHINT,/*	S	-		(Number)s				*/
OP_PUSHSTRING,/* K	-		KSTR[k]				*/
OP_PUSHNUM,/*	N	-		KNUM[u]				*/
OP_PUSHNEGNUM,/* N	-		-KNUM[u]			*/

OP_PUSHUPVALUE,/* U	-		Closure[u]			*/

OP_PUSHLOCAL,/*	L	-		LOC[u]				*/
OP_GETGLOBAL,/*	K	-		VAR[KSTR[k]]			*/

OP_GETTABLE,/*	-	i t		t[i]				*/
OP_GETDOTTED,/*	K	t		t[KSTR[k]]			*/
OP_PUSHSELF,/*	K	t		t t[KSTR[k]]			*/

OP_CREATETABLE,/* U	-		newarray(size = u)		*/

OP_SETLOCAL,/*	L	x		-		LOC[u]=x	*/
OP_SETGLOBAL,/*	K	x		-		VAR[KSTR[k]]=x	*/
OP_SETTABLEPOP,/* -	v i t		-		t[i]=v		*/
OP_SETTABLE,/*	U	v a_u-a_1 i t	 a_u-a_1 i t	t[i]=v		*/

OP_SETLIST,/*	A B	v_b-v_0 t	t		t[i+a*FPF]=v_i	*/
OP_SETMAP,/*	U	v_u k_u - v_0 k_0 t	t	t[k_i]=v_i	*/

OP_ADD,/*	-	y x		x+y				*/
OP_ADDI,/*	S	x		x+s				*/
OP_SUB,/*	-	y x		x-y				*/
OP_MULT,/*	-	y x		x*y				*/
OP_DIV,/*	-	y x		x/y				*/
OP_POW,/*	-	y x		x^y				*/
OP_CONC,/*	U	v_u-v_1		v1..-..v_u			*/
OP_MINUS,/*	-	x		-x				*/
OP_NOT,/*	-	x		(x==nil)? 1 : nil		*/

OP_IFNEQJMP,/*	J	y x		-		(x~=y)? PC+=s	*/
OP_IFEQJMP,/*	J	y x		-		(x==y)? PC+=s	*/
OP_IFLTJMP,/*	J	y x		-		(x<y)? PC+=s	*/
OP_IFLEJMP,/*	J	y x		-		(x<y)? PC+=s	*/
OP_IFGTJMP,/*	J	y x		-		(x>y)? PC+=s	*/
OP_IFGEJMP,/*	J	y x		-		(x>=y)? PC+=s	*/

OP_IFTJMP,/*	J	x		-		(x!=nil)? PC+=s	*/
OP_IFFJMP,/*	J	x		-		(x==nil)? PC+=s	*/
OP_ONTJMP,/*	J	x		(x!=nil)? x : -	(x!=nil)? PC+=s	*/
OP_ONFJMP,/*	J	x		(x==nil)? x : -	(x==nil)? PC+=s	*/
OP_JMP,/*	J	-		-		PC+=s		*/

OP_PUSHNILJMP,/* -	-		nil		PC++;		*/

OP_CLOSURE,/*	A B	v_b-v_1		closure(CNST[a], v_1-v_b)	*/

OP_SETLINE/*	U	-		-		LINE=u		*/

} OpCode;



#define ISJUMP(o)	(OP_IFNEQJMP <= (o) && (o) <= OP_JMP)


#define RFIELDS_PER_FLUSH 32	/* records (SETMAP) */
#define LFIELDS_PER_FLUSH 64	/* FPF - lists (SETLIST) (<=MAXARG_B) */


#endif
