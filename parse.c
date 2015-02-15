#ifndef SELF
#include <stdio.h>
int
eputchar(int c)
{
    return fputc(c, stderr);
}
#endif

#define T_NAME 256
#define T_CONST 257
#define T_STRING 258

#define RESBASE 512
#define T_RETURN 512
#define T_IF 513
#define T_ELSE 514
#define T_WHILE 515
#define T_DO 516
#define T_INT 517
#define I_CHAR 6

#define RES 7

#include "defs.h"

/* Keep scalars early to get compact addressing modes. */
int curloc;   //当前pc位置
int lexval;   //当前符号在符号表中的索引，或者当前常量值
int token;    //当前词法分析到的单词
int pusharg;
int thechar;
int nglob;
int nlocal;
int nfun;
int strsize;
int narg;
int curgloboffs;
int nsym;
int pushop;

int argids[NARG];
int globids[NGLOB];
int globoffs[NGLOB];
int globscalar[NGLOB];
int funids[NFUN];
int funoffs[NFUN];
int localids[NLOCAL];

char symbol[MAXSYM];    //当前字符串
char code[MAXCODE];
char names[MAXNAMES];
#define NAMES "return\0if\0else\0while\0do\0int\0char\0getchar\0putchar\0eputchar\0exit"
#define NAMESSIZE 62

int
eputstr(char *s)
{
    while(*s)
	eputchar(*s++);
}

int
error(char *s)
{
    int i;

    eputstr(s);
    eputstr(" at: ");
    i = 0;
    while(i<20) {
	eputchar(getchar());
	i++;
    }
    eputstr("\n");
    exit(1);
}

int
digit(int c)
{
    return '0' <= c && c <= '9';
}

int
letter(int c)
{
    return 'a' <= c && c <= 'z' || 'A'<=c && c<='Z' || c == '_' || digit(c);
}

int
eqstr(char *p, char *q)
{
    while(*p) {
	if (*p++ != *q++)
	    return 0;
    }
    return !*q;
}

int
lookup(char *name)            //查找符合表，返回name在表中的位置，如果没找到，则加入符号表
{
    int i;
    char *ns;

    ns = names;
    i = 0;
    while(i < nsym) {
	if (eqstr(ns, name)) {
	    return i;               //返回*name是names数组中的第几个字符串
	}
	while(*ns++)
	    ;
	i++;
    }
    while(*ns++ = *name++)
	;
    return nsym++;                 //如果不在，则把*name加入符号表names
}

int
next()                            //读入下一个字符， 返回当前字符
{
    int r;

    r = thechar;
    thechar = getchar();
    return r;
}

int
gobble(int t, int rr, int r)    //如果当前字符是t，则读入下一个字符，并返回rr，否则返回r
{
    if (thechar == t) {
	next();
	return rr;
    }
    return r;
}

int
getstring(int delim)      //读入一个符号字符串到symbol
{
    int c;

    strsize = 0;
    while((c = next()) != delim) {
	if (c == '\\') {
	    if ( (c = next()) == 'n') c = '\n';
	    else if (c == 't') c = '\t';
	    else if (c == '0') c = 0;
	}
	symbol[strsize++] = c;
    }
    symbol[strsize++] = 0;
}

int
instr(char *s, int c)               //如果c在*s里面，则返回1，否则返回0
{
    while(*s) {
	if (*s++ == c)
	    return 1;
    }
    return 0;
}

//词法分析， 返回下一个词
int
getlex()
{
    int c;
    char *p;

    while( 0 <= (c = next()) && c <= ' ') //ASCII 0到32之间都是控制字符，不可显示。32是空格。	/* consider all control chars as whitespace */
	;

    if (c == -1 || instr("()[]{},;", c)) {
	return c;
    }
    if (c == '/') {               //处理注释或者 除号
	if (thechar == '*') {
	    /* next(); dropping this is wrong */
	    while(next() != '*' || thechar != '/')
		;
	    next();
	    return getlex();
	} else
	    return T_DIV;
    }
    if (c == '*') return T_MUL;
    if (c == '%') return T_MOD;
    if (c == '-')
	return gobble(c,T_POSTDEC,T_SUB);    //gobble是为了对付 两个字符的运算符。
    if (c == '>')
	return gobble('=',T_GE,T_GT);
    if (c == '<')
	return gobble('=',T_LE,T_LT);
    if (c == '=')
	return gobble(c,T_EQ,T_ASSIGN);
    if (c == '+')
	return gobble(c,T_POSTINC,T_ADD);
    if (c == '!')
	return gobble('=',T_NE,T_NOT);
    if (c == '&')
	return gobble(c,T_ANDAND,T_AND);
    if (c == '|')
	return gobble(c,T_OROR,T_OR);
    if (c == '\'') {
	getstring(c);
	lexval = symbol[0];          // 字符常量
	return T_CONST;
    }
    if (c == '"') {
	getstring(c);
	return T_STRING;
    }
    if (digit(c)) {             //常量计算
	lexval = c - '0';
	while(digit(thechar)) {
	    lexval = lexval * 10 + next() - '0';
	}
	return T_CONST;
    }
    if (letter(c)) {
	p = symbol;
	*p++ = c;
	while(letter(thechar))
	    *p++ = next();
	*p = 0;
	if ( (lexval = lookup(symbol)) < RES) {        //是符号表中的前7个符号
	    if (lexval == I_CHAR)                      //当前符号是 "char" 
		return T_INT;
	    return lexval + RESBASE;                   //512,513,...519
	}
	return T_NAME;
    }
    error("Bad input");
}

int 
istoken(int t)
{
    if (token == t) {
	token = getlex();
	return 1;
    }
    return 0;
}

int
expect(int t)
{
    if (!istoken(t)) {
	error("syntax error");
    }
}

// int,char,char*,int*
int
type()
{
    expect(T_INT);
    while(istoken(T_MUL))
	;
}

//取名字的index （变量，函数...)
int
name()
{
    int r;

    if (token != T_NAME)
	error("name expected");
    r = lexval;
    token = getlex();
    return r;
}

int
emit(int opc)
{
    code[curloc++] = opc;
}

int
emitat(int a, int c)      //在位置a，写入字节码c
{
    code[a++] = c;
    code[a] = c/256;
}

//产生一个三个字节的指令？
int
emitop(int rator, int rand)
{
    int r;

    emit(rator+LITMAX);     
    r = curloc;
    emit(rand);
    emit(rand/256);
    return r;
}

#define emitj emitop

//查看*arr中有没有当前lexval对应的值，有责返回该index
int
pushloop(int puop, int max, int *arr)
{
    int i;

    i = 0;
    pushop = puop;
    while (i < max) {
	if (arr[i] == lexval) {
	    pusharg = i;
	    return 1;
	}
	i++;
    }
    return 0;
}

/* return 1 if lvalue */
int
pushval()
{
    int lval;

    lval = 1;
    if (pushloop(C_PUSHAL, nlocal, localids)) {     //push argument local
    } else if (pushloop(C_PUSHAA, narg, argids)) {  //push argument arguments
	pusharg = narg-pusharg-1;
    } else if (pushloop(C_PUSHAG, nglob, globids)) {  //push argument global 
	lval = globscalar[pusharg];
	pusharg = globoffs[pusharg];
    } else {
	lval = 0;
	if (pushloop(C_PUSHAC, nfun, funids)) {       //PUSH AC?  C表示什么？
	    pusharg = funoffs[pusharg];
	} else if (lexval < RES+NPRECALL) {
	    /* predefined function is the last possibility, reuse C_PUSHAC */
	    pusharg = lexval-RES;  //第一个预置的函数getchar在符号表里的序号正好是7 （RES的取值） 
	} else {
	    error("undefined variable");
	}
    }
    emitop(pushop, pusharg);
    return lval;
}

int
pderef(int l)
{
    if (l)
	emit(C_DEREF);
}

/* returns true if lvalue, else false */
int
expr(int needval, int prec)
{
    int na;
    int islval;
    int jdst;
    int op;
    int any;
    int opprec;

    islval = 0;

    /* parse one expr */
    if (istoken(T_CONST)) {
	emitop(C_PUSHC, lexval);
    } else if (istoken(T_STRING)) {
	/* If the syntax is ok the string must still be in the symbol buffer.
	   Stuff it into global space. */
	emitop(C_PUSHS, strsize);
	any = 0;
	while(any < strsize)
	    emit(symbol[any++]);
    } else if (istoken(T_NAME)) {
	islval = pushval();
    } else if (istoken('(')) {
	islval = expr(0, P_NONE);
	expect(')');
    } else if (istoken(T_NOT)) {
	expr(1, P_PRE);
	emit(C_NOT);
    } else if (istoken(T_SUB)) {
	expr(1, P_PRE);
	emit(C_NEG);
    } else if (istoken(T_MUL)) {
	expr(1, P_PRE);
	islval = 1;
    } else if (istoken(T_AND)) {
	if (expr(0, P_PRE) == 0)
	    error("lvalue required");
    } else
	error("syntax error in expr");

    /* one expression parsed, try for hi prec ops */
    any = 1;
    while(any) {
	op = token % OPMOD;
	if (istoken('(')) {
	    /* function call */
	    pderef(islval);
	    na = 0;
	    if (!istoken(')')) {
		do {
		    expr(1, P_NONE);
		    na++;
		} while(istoken(','));
		expect(')');
	    }
	    emitop(C_CALL, na*2);     //na是参数个数？
	    islval = 0;
	} else if (istoken('[')) {
	    /* array ref */
	    pderef(islval);
	    expr(1, P_NONE);
	    emit(C_ADD);
	    expect(']');
	    islval = 1;
	} else if (istoken(T_POSTINC) || istoken(T_POSTDEC)) {
	    if (!islval)
		error("no lval for ++");
	    emit(op);
	    islval = 0;
	} else
	    any = 0;
    }

    opprec = token / OPMOD;
    while (prec < opprec) {
	if ( (op = token % OPMOD) != C_ASSIGN) {
	    pderef(islval);
	} else {
	    if (!islval)
		error("no lval for =");
	}
	if (istoken(T_ANDAND) || istoken(T_OROR)) {
	    emit(C_DUP);
	    if (op == C_OROR)
		emit(C_NOT);
	    jdst = emitj(C_JFALSE, 0);
            emit(C_POP);
	    expr(1, opprec);
	    emitat(jdst, curloc);
	} else {
	    /* emit binop code */
	    token = getlex();
	    expr(1, opprec);
	    emit(op);
	}
	islval = 0;
	opprec = token / OPMOD;
    }
    if (needval) {
	pderef(islval);
	islval = 0;
    }
    return islval;
}

//表达式求值
int
pexpr()
{
    expect('(');
    expr(1, P_NONE);
    expect(')');
}

//编译一条语句
int
stmt()
{
    int jdest;
    int tst;

    if (istoken('{')) {
	while(!istoken('}'))
	    stmt();
    } else if (istoken(T_IF)) {
	pexpr();
	jdest = emitj(C_JFALSE, 0);
	stmt();
	if (istoken(T_ELSE)) {
	    tst = emitj(C_JUMP, 0);
	    emitat(jdest, curloc);
	    stmt();
	    emitat(tst, curloc);
	} else {
	    emitat(jdest, curloc);
	}
    } else if (istoken(T_WHILE)) {
	tst = curloc;
	pexpr();
	jdest = emitj(C_JFALSE, 0);
	stmt();
	emitj(C_JUMP, tst);
	emitat(jdest, curloc);
    } else if (istoken(T_DO)) {
	jdest = curloc;
	stmt();
	expect(T_WHILE);
	pexpr();
	emit(C_NOT);
	emitj(C_JFALSE, jdest);
    } else if (istoken(T_RETURN)) {
	expr(1, P_NONE);
	expect(';');
	emit(C_RETURN);
    } else if (istoken(';')) {
	/* empty */
    } else {
	/* just an expression */
	expr(1, P_NONE);
	emit(C_POP);
	expect(';');
    }
}

int
parse()
{
    int objid;

    token = getlex();
    while(1) {
	if (token < 0)
	    return 1;
	type();			/* T_INT or CHAR */
	objid = name();		/* T_NAME */
	if (istoken('(')) {
	    /* function */
	    funids[nfun] = objid;
	    funoffs[nfun++] = curloc;
	    narg = 0;
	    if (!istoken(')')) {
		do {
		    type();
		    argids[narg++] = name();
		} while(istoken(','));
		expect(')');
	    }
	    expect('{');
	    /* body */
	    /* decls */
	    nlocal = 0;
	    while(token == T_INT) {
		type();
		do {
		    localids[nlocal++] = name();
		} while (istoken(','));
		expect(';');
	    }
	    if (DEFSTK < nlocal)
		emitop(C_ALLOC, nlocal);
	    /* stmts */
	    while(!istoken('}'))
		stmt();
	    emit(C_RETURN);
	} else {
	    globoffs[nglob] = curgloboffs;
	    if (istoken('[')) {
		/* array */
		expect(T_CONST);
		curgloboffs = curgloboffs + lexval;
		expect(']');
		globscalar[nglob] = 0;
	    } else {
		curgloboffs++;
		globscalar[nglob] = 1;
	    }
	    globids[nglob++] = objid;
	    expect(';');
	}
    }
}

int
main()
{
    int n;
    char *p;
    char *q;

    /* Initialize symbol table */
    nsym = RES+NPRECALL;            //7+4 = 11
    p = names;                      //char names[MAXNAMES];
    q = NAMES;                      //"return\0if\0else\0while\0do\0int\0char\0getchar\0putchar\0eputchar\0exit"

    n = NAMESSIZE;                 //62
    do
	*p++ = *q++;               //初始化names数组
    while(n--);

    curloc = 10;	/* some space to avoid low addrs */
    thechar = getchar();
    parse();
    n = curloc;

    /* Generate code to jump to main */
    curloc = 0;
    lexval = lookup("main");
    pushval();
    emitop(C_CALL, 0);
    emit(C_EXIT);
    
    putchar(n);                   //输出字节码长度
    putchar(n/256);
    p = code;
    while(n--) {                  //输出生成的字节码
	putchar(*p++);
    }
    return 0;
}

