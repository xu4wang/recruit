简介
-----------

我们在寻找具有强大逻辑思维能力的软件和驱动开发同学。

以下五个实验题目都是基于本项目既有源代码。 源码的初始作者是：
http://en.wikipedia.org/wiki/Lennart_Augustsson

五道题中， 实验一出自：
https://www.cs.tcd.ie/David.Gregg/

其余的四道题是我们自己准备的。

您在实验过程中，有任何问题可以通过邮件（wangxu AT mail DOT dspread DOT com ）联系我们。 
也可以通过QQ或者微信和我交流（请邮件我获取联系方式）。 

实验一
-----------

CS2014 Lab 9 : Understanding Existing Code
           (Or why you should write simple, clean, self-documenting code)

Thursday 17th December 2009

Most software construction does not involve designing and building
software from scratch. Building new systems is only a small proportion
of the work done by people who write programs. A much more common task
is to take an existing program and modify it to change its behaviour.

I have downloaded a program from the web, and modified it to make it a
little easier to compile and understand. The program is representative
of a lot of software that people use and modify. By the standards of
software that you run into, it is not badly written, and it has some
very clever ideas in it. However, it is not at all representative of
the kind of simple, clear, maintainable and (mostly) correct software
that we write in our course. In other words, other people may write
software like this, but you shouldn't.

To compile the code, and test that it's working, use the command:
```
	make test
```

To run a test program you can type:
```
	./interp < fac.oo.1
```

The file fac.oo.1 is a bytecode version of the C program fac.c. The
program fac.c has been written in a small subset of C. This is compiled
to a bytecode, which is then interpreted by the program interp. If you
want to look more closely at how this all works, and you are feeling
a bit adventurous, take a look at the file Makefile. (We'll be looking
at makefiles next term).

Your job is to examine and modify the file interp.c (the bytecode
interpreter).  You should do three things with this file: 

* You should add appropriate commments to the file to make it easier for the reader to understand (this will involve you figuring it out sufficiently to explain it). 
*  You should rewrite the code in areas of the file where you think it could be simplified and clarified. (I suggest that you avoid really large changes that would completely restructure the program). 
* There is a facility in the program to dump out information on each bytecode instruction as it is being executed. You should figure out how to use this, and dump out the trace of executed bytecode instructions for the sample program (fac.c).

Finally, just to familiarise yourself with the wider system, modify
fac.c so that it computes the factorial of all positive integers
up to 11 (instead of 10). Compile the modified program to bytecode
with the command:
```
	./parse < fac.c > fac.oo.1
```

Now run the modified version with your modified bytecode interpreter:
```
	./interp < fac.oo.1
```

This lab will not be marked. The main goal is to get used to reading
other people's code, and to understand better why it is important that
code is readable. We will, however, be doing exciting things with this
code over the next couple of labs, and the resulting modified code
will be submitted and marked.


实验二
---------

请结合上面的实验， 描述下该虚拟机的实现机制，包括：
* 有几个寄存器？
* 如何实现if语句的翻译？
* 对于C语言的局部变量和全局变量如何分配内存？
* 如何实现函数调用？


实验三
--------

在原有实现中， 虚拟机（interp）支持4个原生的C语言调用， 分别是getchar,putchar,eputchar,exit. 请尝试增加一个原生调用：

```c
void *oc_memcpy(char *dest, char *src, int n);
```

其作用是把n个字节的字符串从 src 拷贝到 dest。 请在parse, interp中增加该功能后， 写一个简单的例子，用parse编译，用interp执行。证明该函数可以被interp支持。

提示：

您需要

1. 改 interp.c 代码，在里面加入一个oc_memcpy的实现， 例如：
```c
void *oc_memcpy(char *dest, char *src, int count)
{
 char *tmp = dest;
 char *s = src;
 while (count--)
  *tmp = *s ;
 return dest;
}
```
2. 改写interp.c 中的虚拟机bytecode翻译执行部分， 使得其能在碰到 ```X_CALL``` 机器码时候，在合适的参数下，调用上面的oc_memcpy函数。
3. 改写parse.c 使得其在扫描到源代码中的 oc_memcpy 调用时候，能够生成合适的 ```X_CALL``` 机器码和参数。
4. 写一个小的例子程序，类似fac.c, 其中有oc_memcpy调用。 用该例子程序来验证parse和interp的实现。


实验四
----------

在对代码理解的基础上， 修改parse和dumpcode， 使得parse在执行的时候， 能够输出另外一个文件， 文件中包含当前被翻译的每一条C语句， 以及其翻译成的机器代码。 类似如下形式：

```
...
Line 29 putstr("factorial ");
 119 :  c70a00  PUSHAC 10
 122 :  970b00  PUSHS 11 "factorial "
 136 :  b70200  CALL 2
...
```

也即，把源代码中每一行源程序对应的汇编代码列出来。

实验五
----------

在实验三的基础上思考，如果需要实现一个针对fac.c 的源代码级别的调试器，该如何实现。 
实现如下功能：
* 能够在PC上解释执行 fac.c 的内容
* 能够单步执行fac.c 的内容
* 能够设置断点， 在执行到断点的时候停下来
* 在调试过程中， 可以查看当前的局部变量和全局变量的值
