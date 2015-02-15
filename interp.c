#include <stdio.h>
#include <string.h>

int
eputchar(int c)
{
    return fputc(c, stderr);
}

#include "defs.h"
#define MAXSTACK 10000
#define MAXMEM (MAXCODE+MAXDATA+MAXSTACK)

int mem[MAXMEM];       //都是int类型

int verbosef;

#define TOS mem[sp]
#define POP mem[sp++]
#define PUSH(x) sp--;TOS = x
#define CASE(l, s) case l: s; break;
#define DEFAULT default:
#define OP2(l, o) CASE(l, r = POP; TOS = TOS o r)
#define OP1(l, o) CASE(l, TOS = o TOS)
#define OPPUSH(l, s) CASE(l, PUSH(s))
#define LIT mem[pc++]
#define DORET sp = sp + lit/2;TOS = r;if (lit%2) sp++


#define code mem
#include "dumpcode.c"

int
main(int argc, char **argv)
{
    int *q, sp, fp, r, opc, lit, pc, codesize;
    int oldpc;

    verbosef = argc > 1;

    codesize = getchar();
    codesize = codesize + getchar()*256;          //前两个字节是bytecode长度
    pc = codesize;
    q = mem;
    /* read the code */
    while(pc--)
        *q++ = getchar();          //所有bytecode读入mem数组中

    if (verbosef)
        fprintf(stderr, "code size %d\n", codesize);
    sp = MAXMEM;                   //堆栈从顶端开始
    pc = 0;                        //代码从低端开始
    /* Start execution */
    while(1) {
        if (verbosef) {
            fprintf(stderr, "%5d %5d(%5d,%5d,%5d) ", fp, sp, mem[sp], mem[sp+1], mem[sp+2]);
            dumpcode(pc, pc+1);          //反汇编当前的机器码，仅反汇编一条指令
        }
        oldpc = pc;
        switch (opc = LIT) {            //LIT 是当前的字节码 mem[pc++] , pc同时++了
        OP1(C_NOT, !)                   //mem[sp] = !mem[sp]
        OP1(C_NEG, -)                   //mem[sp] = -mem[sp]
        OP2(C_ADD, +)//弹出栈顶两个数，相加后入栈 r=mem[sp++]; mem[sp] = r + mem[sp]
        OP2(C_SUB, -)//两个数相减， 被减数在栈顶
        OP2(C_MUL, *)
        OP2(C_DIV, /)             
        OP2(C_MOD, %)             
        OP2(C_AND, &)             
        OP2(C_OR, |)              
        OP2(C_GT, >)              
        OP2(C_GE, >=)             
        OP2(C_LE, <=)
        OP2(C_EQ, ==)
        OP2(C_NE, !=)
        OP2(C_LT, <)
        CASE(C_ASSIGN, r = POP; mem[TOS] = r; TOS = r)//栈顶为立即数，次顶为待赋值变量地址；弹出此二数，执行赋值，并把结果压入栈 
        CASE(C_ASSIGNPOP, r = POP; mem[TOS] = r; sp++) //同上，但结果不入栈
        CASE(C_RETURN,
             r = TOS;
             sp = fp;
             pc = mem[sp-F_PC];
             fp = mem[sp-F_FP];
             lit = mem[sp-F_NA];
             DORET)
        CASE(C_POP, sp++)                  //丢弃栈顶内容
        CASE(C_EXIT, return TOS)           //返回当前栈顶内容
        CASE(C_DUP, r = TOS; PUSH(r))      //栈顶内容再次入栈一份
        CASE(C_DEREF, TOS = mem[TOS])      //*sp = **sp
        CASE(C_POSTINC, TOS = mem[TOS]++)  //*sp = (**sp)++
        CASE(C_POSTDEC, TOS = mem[TOS]--)
        DEFAULT
        {
            lit = opc % LITMOD;
            opc = opc / LITMOD;           //是16的几倍 转换成 X_XXX
            if (lit == LITMAX) {
                lit = mem[pc++];
                lit = lit+mem[pc++]*256;
            } else if (LITMAX < lit)
                lit = lit + mem[pc++]*LITMUL - 1;
            switch(opc) {
            OPPUSH(X_PUSHAA, fp+lit)
            OPPUSH(X_PUSHAL, fp-lit-FRAMESIZE-1)
            OPPUSH(X_PUSHA, mem[fp+lit])
            OPPUSH(X_PUSHL, mem[fp-lit-FRAMESIZE-1])
            OPPUSH(X_PUSHAG, codesize+lit)
            OPPUSH(X_PUSHG, mem[codesize+lit])
            OPPUSH(X_PUSHAC, lit)
            OPPUSH(X_PUSHC, lit)
            CASE(X_PUSHS, PUSH(pc); pc = pc + lit)
            CASE(X_ALLOC, sp = sp - lit)
            CASE(X_CALL,
            r = mem[sp - -lit/2];
            if (r < NPRECALL+1) {                  //r<5 ?
                switch(r) {
                case 0: r = getchar(); break;
                case 1: r = putchar(TOS); break;
                case 2: r = eputchar(TOS); break;
                case 3: exit(TOS); break;
                }
                DORET;
            } else {                      //保存现场，进行函数调用跳转
                mem[sp - F_PC] = pc;
                mem[sp - F_FP] = fp;
                mem[sp - F_NA] = lit;
                fp = sp;
                sp = sp - FRAMESIZE - DEFSTK;
                pc = r;
            }
            )
            CASE(X_JUMP, pc = lit)
            CASE(X_TRUE, if (POP) pc = lit)
            CASE(X_JFALSE, if (!POP) pc = lit)
            DEFAULT ;
            fprintf(stderr, "unimpl op %d at %d\n", mem[oldpc], oldpc);
            exit(1);
            }
        }
        }
    }
}
