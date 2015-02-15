#define NARG 50
#define NLOCAL 50
#define NGLOB 300
#define NFUN 300

#define MAXNAMES 30000
#define MAXCODE 10000
#define MAXDATA 30000
#define MAXSYM 30000

#define OPMOD 100

/* operations corresponding to operators, no operands */
/* code the most frequent operators as whitespace */
#define C_ASSIGNPOP	  9 /* frequent */
#define C_ASSIGN 	  1
#define T_ASSIGN	101
#define C_QUEST 	---

#define C_OROR 		  2
#define T_OROR		302
#define C_ANDAND 	  3
#define T_ANDAND	403
#define C_OR 		  4
#define T_OR		504
#define C_XOR 		---

#define C_AND 		  5
#define T_AND		705

#define C_EQ 		  6
#define T_EQ		806
#define C_NE 		  7
#define T_NE		807
#define C_LT 		  8
#define T_LT		808
#define C_GT 		 26
#define T_GT		826
#define C_LE 		 24
#define T_LE		824
#define C_GE 		 11
#define T_GE		811

#define C_LSH 		---
#define C_RSH		---

#define C_ADD		  12
#define T_ADD		1012
#define C_SUB		  13
#define T_SUB		1013

#define C_MUL		  14
#define T_MUL		1114
#define C_DIV		  15
#define T_DIV		1115
#define C_MOD		  16
#define T_MOD		1116

#define C_NOT		  17
#define T_NOT		1217
#define C_NEG		  18
#define T_NEG		1218
#define C_POSTINC	  19
#define T_POSTINC	1219
#define C_INV		  20
#define T_INV		1220
#define C_POSTDEC	  25
#define T_POSTDEC	1225

#define P_NONE 0
#define P_PRE 100

/* operations with no operands */
#define C_DUP		21
#define C_POP		22
#define C_DEREF		23
#define C_RETURN	10 /* frequent */
#define C_EXIT		27

#define LITMOD 16
#define LITMAX 7
#define LITMUL 8
#define LITLIM 32

/* operations with one operand */
#define C_PUSHAA	 32       //C_xxx和下面的 X_xxx 是一条指令的两种表示方法。字节码中用C_xxx, 但由于指令都是16的整数倍，所以用倍数也可以表示一条指令。 X 是乘号，表示倍数。
#define X_PUSHAA	  2
#define C_PUSHA		 48
#define X_PUSHA		  3
#define C_PUSHAL	 64
#define X_PUSHAL	  4
#define C_PUSHL		 80
#define X_PUSHL		  5
#define C_PUSHAG	 96
#define X_PUSHAG	  6
#define C_PUSHG		112
#define X_PUSHG		  7
#define C_PUSHC		128
#define X_PUSHC		  8
#define C_PUSHS		144
#define X_PUSHS		  9
#define C_ALLOC		160
#define X_ALLOC		 10
#define C_CALL		176
#define X_CALL		 11
#define C_PUSHAC	192
#define X_PUSHAC	 12

/* jumps */
#define C_JUMP		208
#define X_JUMP		 13
#define C_JFALSE	224
#define X_JFALSE	 14
#define C_TRUE		240
#define X_TRUE		 15

#define FRAMESIZE 3
#define F_PC 1
#define F_FP 2
#define F_NA 3

#define DEFSTK 16

#define NPRECALL 4       //number of prefedined call

