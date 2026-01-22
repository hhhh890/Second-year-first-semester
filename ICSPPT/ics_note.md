## CSAPP
the c programming language 

期中、期末题型差不多，40、50

---

### 第一周-9.11

A tour of computer systems

suggesting reading
chapter 1

layer of computer systems

complication编译基础
hardware organization
running the hello program

（storage devices hierarchy --- 寄存器、缓存、内存、硬盘、网盘等）

operating system 抽象简化掉很多东西 例如内存地址：虚拟内存-操作系统、硬件-物理上的内存的读写。虚拟内存就是一种简化。输入输出设备变成文件。对于计算机的抽象。
    简化之一：processes进程：正在运行的程序。感觉是一个在运行，是一个连续的，但实际上打断、context switching，managed by the operating kernel.

end of tour

阿布达尔定律
Amdahl's law

---
outline:
Bit and Btye
machine representations of numbers
suggesting reading:1.1 2.1.1

bit:0\1, binary digits二值信号

电压voltage高低表示数，0、1，高电压1，低电压0，介于之间--未定义值（应尽量避免、越快越好）。

十进制，各位含义，12345=1*10^4+2*10^3+...
二进制，各位也被赋予含义，10101
八进制...

8-bit chunks are organized as a byte

表示一个数---三种常用方式（与目的有关）
unsigned encoding（无符号编码方式）：表示自然数
two's-complement encoding（补码表示）（有符号编码方式）
floating point encoding

machine representation of numbers

int is not integer
overflow
交换律、结合律依然成立

float is not real number
overflow:与整型的溢出不同，不会导致几个正数相乘为负数，但是有可能乘出来不是一个数，结果直接变为0，上溢、下溢
不能保证结合律

以最大可能表述整数、实数

十六进制：0-9、A-F表示0-15
0xFA1D37B\0xfa1d37b
依然给各位赋予含义
很常用，2的整数次方

二、十、十六进制---要考的

---

manipulate number
布尔运算 boolean algebra 1-true 0-false
与、或、非、异或
A&B A|B ~A A^B(Xor)（看起来像相加，单位加法运算、溢出不管）
operate on bit vectors - operations applied bitwise

    在系统设计时，存状态，可以只用一个整型变量，就可以表示32种状态，flag，用与或非等处理
    representation of sets
    aj = 1,if j is in A
        & intersection
        | union
        ~ complement
        ^ symmetric difference
    
---

### 第二周-9.19

operate on bit vectors - operations applied bitwise
操作在bit上
续：
mask operations 掩码
    X & 0xFF 等，按需取用F，此为保留最后两位
    和1做异或，翻了一下
    ...
    ~0，而不是0xFFFFFFFF（32位、64位）

---
logic operations in c
    与布尔运算不同:操作在整个数上，而非bit上
    logic operators: && || !
    view 0 as false
    anything nonzero as true
    always return 0 or 1
    短路操作---例如&&，a && 5/a 先计算and符号左边的值，如果是false，且操作为 && 则不会计算右边的。||同理也有短路操作。

shift operations in c
    left shift:     x<<y    往左y位，左边溢出的抛弃，右边用0填充
        编译器很喜欢左移操作，如果整体左移一位，不考虑溢出的情况，相当于乘以2，左移n位，相当于乘以2^n。
        乘法非常慢，左移操作非常快。快得多。*10----*8+*2。也比直接乘以10快。
    right shift：   x>>y    往右y位，由右边溢出的抛弃
        logical shift   逻辑右移: 左边用0填充。例如都是正数，最高位不表示符号位。
        arithmetic shift    算术右移: 最前面代表符号位，整个数右移之后，看最高位如果是0，在最前面都填充0，最高位本来是1，在前面都填充1.
        右移操作经常用作除法，除以2等。

    <<32---0
    >>36---4
    左移右移优先级非常低，1<<2+3>>4,相当于1<<5>>4

information storage
    冯诺依曼结构

    read/write operations

    memory
        word size  --- 表示内存地址的长度 32位、64位
            64位；unfortunately, integer --- 32-bit
        data size
            long ...
            typical values int32_t 写操作系统时才会用
    
    --- virtual memory

subdivide virtual memory into more manageable units

    ...
---

Byte ordering
    eg long 32-bit --- 4 byte   约定俗成最小的地址为整个数的地址（下小、上大）；填充的位置、方向，没有约定俗成、两种不同的编码方式。小端、大端。
    0x12345678 ：最低有效位：78
    大端、小端：最低有效位，放在最大地址上还是最小地址上。
    大小端相互传递数据时可能会出现问题
    bi-Endian双端表示，默认--更改--单一
    
---

how to access an object
    占用连续的空间（大）

---

（2）

unsigned representation
赋予意义，2、4、8、...

有符号表示方式
    补码表示 Two's complement
        首位 1 例如 2^15---> -2^15 
        ...
        现在都用这种

---

conversions

    signed vs unsigned 
        explicit casting betwwen signed and unsigned  (int)x   (unsigned)y
        implicit casting via assignments and procedure calls 编译器做转换 双向 非必要不转
        无符号--->有符号
        有符号--->无符号  

    无符号常量
        0u  123456U
    
    无符号数与有符号数加减乘除---编译器做无符号与有符号之间的转换
    到底当作无符号还是有符号？

    （1）mixed -1 < 0u：false  只要有一个是无符号的，则两个都转为无符号整型 故为 -1 --->  2^(n-1) 即把最高位的符号位1转为0
    （2）i = 0; i < length -1; i++   若length = 0  length无符号整型 -1 --->变为最大的能表示的正整数 2^(n-1)
    （3）strlonger(char * s, char* t){
        return strlen(s) - strlen(t) > 0;   一般strlen都是无符号数，因为显然是正数。但是此相减，不能达到目标，只有两个长度相同时才不满足，剩下的都满足。
    }
---

### 第三周-9.26

11111...11111
2^(n-1) - 1 - 2^(n-1) = -1
注意到 -2^(n-1) 即为最高位的1

11111...11110
2^(n-1) - 1 - 2^(n-1) -1 = -2

signed vs unsigned 转换，但是长度都是一样的 

另外from short to long/int,
short int x = 12345;
int ix = (int) x;
16位 ---> 32位。正数问题不大。前面都补0，含义都一样。

short int x = -12345;
int ix = (int) x;
16位 ---> 32位。负数问题不大。前面不能补0。

两种扩展方式：
（1）zero extension：前面都补上0.（正数、无符号数可以）。有符号数+原本第一位是0，正数---前补0
（2）sign extension：负数--- 有符号数，第一位是1，前补1。

有符号整型，右移会根据符号位进行头部补充。
无符号整型，右移就是补0。

from long to short
不能保证含义不变
删除前面的二进制数，剩下的是多少就是结果。

---

Arithmetic Operations

unsigned addition: 可能会多一位，多出来的被删除掉了---溢出。 u + v - 2^w
判断两个无符号数相加是否溢出: OVERFLOW if(x + y) < x

sign addition: drop off MSB 
    positive overflow   u + v - 2^(w - 1) 正溢出 ---两个正数相加变成负数
    negative overflow   u + v + 2^(w - 1) 负溢出 ---两个负数相加变成正数

阿贝尔群
相加：交换律、结合律
存在一个数与其相加为零，包括无符号数
无符号构成了一个阿贝尔群、有符号的构成了一个阿贝尔群


TMIN  x = 1000000...0000
TMIN -x = 100000...0000

x + -y
x -y

x negative

multiplication 同理，溢出的删除
arbitrary precision 任意精度的加减乘除包

---

floating point --- 不考
IEEE规则

---

Data Format and Data Move

寄存器、内存

寄存器放在读的位置、写的位置，对应不同，是读还是写

addressing mode:
int arr[i]
arr  arr + 4   arr + i * 4
        (rb, ri, s) rb + ri * s  rb寄存器存储arr首地址 ri寄存器存储i的值 s
        Imm 立即数 偏移 真正访问的地址 Imm +  rb + ri * s
        Imm(rb, ri, s)

考试会考

260(%rcs, %rdx) 其中注意260为十进制
注意算出来的地址要去看值

---

understanding machine execution

保存程序状态

条件码寄存器
判断语句eg 


code layout
首值的前半部分会显示需要读几个字节

data layout
全局变量、常量：read\write data、read only data
局部变量、参数：stack

栈在最上面
栈顶在栈的最下面，用%rsp存储栈顶地址。

move指令：赋值。 move src, dest   move source destination
对于source的读，对于destination的写。其中destination不能是立即数，另外(memory, memory)也不支持：慢、可以通过内存和寄存器实现。

data movement

r:64位的寄存器 eg. %rax
i:32位的寄存器 eg. %eax
...
细分hh

q\l\w\b：move的长度
另外，movsbq、movzbq，把一个字节的赋值给八个字节的，两种拓展形式(sign-extended\zero-extended)

---

### 第四周-10.10

pushq 压栈
popq 出栈
%rsp 保存栈顶地址

Pointers in C

数组、指针不保存在寄存器中，保存在内存中，指针里面保存内存地址。寄存器里的值没有内存地址。

数值赋值运算

---

DATA MANIPULATION

左值位置--右值 ，...

leaq S,D ：load effective address move 但不把值取出来。move相当于在赋值，lea把地址取出来，不需要访问地址里面的元素。
ari and log 考试要考

xorq %rsi, %rdi
lea 作乘法

---

conditional codes
控制流

eflags
条件码 不能显式赋值 运算时副作用使之被赋值
用于实现条件分支、条件判断等

movzbl 等是否填充

---

control

jmp .L1   L1:标签。在这条指令执行完之后，执行L1标签所在指令
unconditional、direct & indirect
indirect:
    jmp *%rax       *后是一个寄存器，把rax寄存器里面的值取出来，这是一个地址，执行rax存的这个地址所在的指令
    jmp *(%rax)     通过rax寄存器拿到一个值，是一个地址，对这个地址进行访问，拿到一个值，作为地址，进行跳转


conditional jump
    such as jl, jge, jb ... 不赋值，是1跳转。与set不同之处在于不赋值。
    all direct

comp    两个操作数比较  if( a < b)
test    一个操作数判断  if( a == 0)

fall through

要不要跳转，而不是if else 跳转到哪里。


偏移量，跳转的真正地址。溢出 f8+d=5了。

---

gotos、branch

if else 与汇编指令不同  编译器变形
do-while translation

---

conditional move

满足就赋值，不满足就不赋值。
cmovge %rdx, %rax

带条件的move，好
跳转指令---慢的

算出来，满足，move
但是算出来中假设没有side effect

有副作用时不能优化，编译器优先在任何情况下不能出错。

---

switch construct

条件跳转可以实现，但很慢不靠谱

targ0等标签，要跳转执行的block的地址
把标签串在一个表格中
跳转表
targ0
targ1
...

根据op，查找地址

从0开始，没有的地址为default。头上：eg case 100 开始，先把n -100，再开始。 尾上：判断。
间接跳转。通过跳转表查出地址。跳转到相应位置。

---

以上，一个函数内部的控制流，分支、跳转、循环。

---

### 第六周-10.17

procedure call
过程调用

data flow
control flow

invoke call:
call label (direct)
call *operand(indirect)

call:
save return address (the next address) in the stack
jump to the entry of callee
call = push + jmp

return to caller:
ret:
pop return address from stack
jump to return address in caller
ret = pop + jmp

栈帧 连续的空间
stack frame structure
帧指针：%rbp（static）
栈指针：%rsp（dynamic）

帧指针、栈指针：当前正在执行的函数的上界与下界

main
main-%rsp 

old %rbp
f-%rbp
f
%f-rsp 

old %rbp
g-%rbp 
g
g-%rsp
见PPT

ret逆操作

leave指令 等效，不过硬件上可以加快


passing data
- stack
- register

passing data: arguments
前六个参数放到寄存器中
后面的所有参数放在栈上 are pushed by caller
先压最后一个，从第N个压到第七个（约定俗成），好处：方便算参数地址（从callee %rbp开始加，加n个8字节）

先把没有办法放在寄存器上的参数按序（逆）压栈，接下来再调用call的指令（压一个返回地址），再栈帧

passing data: return value
只有一个返回值
specific register to keep the return value
- %rax return the result from callee to caller


register: usage convention
想把局部变量都放在寄存器里
不能只看一个函数，否则被调用函数覆盖嗲调用函数存储在寄存器中的局部变量的值
f函数调用g函数：
caller-save register ：在调用者保存寄存器（caller-save register）的调用约定中，调用者（caller）需要保存的是 “自己在函数调用后还需要继续使用的寄存器”，而不是 “被调用者（callee）需要使用的寄存器”。
具体逻辑如下：caller-save 寄存器的核心约定是：被调用者（callee）可以随意修改这些寄存器，且无需负责保存和恢复其原始值。因此，若调用者（caller）在调用 callee 之后，还需要继续使用某个 caller-save 寄存器中的值（比如该寄存器中存放了后续逻辑需要的临时数据或变量），那么 caller 必须在调用 callee 之前，主动将该寄存器的值压入栈中保存；否则，callee 对该寄存器的修改会覆盖原始值，导致 caller 后续逻辑出错。
callee-save register ：在被调用者保存寄存器（callee-save register）的调用约定中，责任方是被调用者（callee）：被调用者在使用这些寄存器之前，必须先保存它们的原始值（通常压入栈中），并在函数返回前恢复这些值，以保证调用者（caller）在调用结束后能继续使用这些寄存器的原始数据。
具体逻辑如下：callee-save 寄存器的核心约定是：被调用者（callee）不能随意修改这些寄存器的原始值。如果 callee 需要使用某个 callee-save 寄存器（比如用它暂存局部变量或中间结果），则必须在使用前主动保存其原始值（通常通过 push 压栈），并在函数返回前通过 pop 恢复原值。
这样做的目的是：调用者（caller）可以安全地将数据存放在 callee-save 寄存器中，无需担心被 callee 修改 —— 因为 callee 会负责 “用完归还”。


局部变量
编译器算好直接分配，不是一个一个分配

访问局部变量：
使用 %rbp 时：局部变量通过 %rbp 的负偏移访问（%rbp 固定，适合调试）。
不使用 %rbp 时：局部变量通过 %rsp 的正偏移访问（更高效，依赖编译器优化）。


一个例子（自行看）

---

array

数组起始地址记为x_a
eg char数组 a[n] 地址：x_a + n * 1;

char *b[5] 指向char的指针，地址占8字节

数组在c中，数组和指针是互通的，int 数组首地址可以看成int *的指针
两个指针相减，eg int*，得到int类型数据.在 C/C++ 中，两个同类型指针（如 int*）相减的结果不是字节数，而是它们之间的元素个数。
具体规则：
前提：两个指针必须指向同一个数组中的元素（或数组最后一个元素的下一个位置），否则行为是未定义的（编译可能通过，但运行结果无意义）。
计算逻辑：结果 = 两个指针所指向的元素在数组中的 “索引差”。
编译器会自动根据指针类型的大小（如 int 占 4 字节），将两个指针的地址差值（字节数）除以元素大小，得到元素个数。

p + i 地址取决于类型 p + 1 地址不一定由400888 变为400889

leaq
leaq 的本质是 “地址计算工具”，它根据源操作数的地址模式（基址 + 变址 + 比例 + 位移）计算出一个内存地址，然后直接将这个地址值（而非地址中的数据）写入目的寄存器。
地址模式格式
leaq 支持 x86-64 标准的内存地址格式，语法为：leaq 位移(%基址寄存器, 变址寄存器, 比例), 目的寄存器计算逻辑：有效地址 = 位移 + 基址寄存器值 + 变址寄存器值 × 比例其中，“比例” 只能是 1、2、4、8（对应 64 位架构下常见的数据宽度）。

用lea做乘法

---

nested array

int A[4][3]
先行后列
先列后行
通常，行优先存储，先存第一行，再第二行

a[i][j]
地址：x_a + L * (c * i + j)

---

fixed-size arrays

variable-size arrays

乘法操作非常慢

---

chap 3.9

struct、union

structures

eg 一个结构体、五个域，顺序保存的

结构体的域的访问形式
.
-> 指向结构体的指针

unions

struct按序存，（ps对齐问题）
union都存在0位置上面，相互覆盖。eg这三个元素指向同一个地方。eg用于元素互斥的地方。eg中间节点、叶子节点

eg double2bit 二进制表示是一样的，值显然是不一样的 

---

### 第七周-10.24

对齐部分笔记见 PPT 5 - array 40 - 52页

---

Understanding Pointers

---

y86

rrmoveq register to register move
irmoveq 立即数到register的move

y86 支持十五个寄存器
为什么是15个，不是16个，F表示不用寄存器 占坑

encoding representation
第一个字节 指令类型 60 61 算术\逻辑运算 加法、减法
第二个字节 rA rB 前四个bit表示第一个寄存器，后四个bit表示第二个寄存器

V 立即数值
D 内存地址

---

cmove 只支持从寄存器写到寄存器 2开头
jump 不需要寄存器

...

nop 10 
halt 特权指令 需要受限制 不能再user mode

---

CISC vs. RISC
指令集架构

CISC 复杂指令集 指令数量多 功能复杂
RISC 精简指令集 处理器实现效率的考虑 每一条指令实现时间一样 更容易实现流水线

---

Logical Design & HCL

组合电路：电路里不带状态 typical application: ALU. 无环的
时序电路：typical application: CPU

MUX 多路选择器
4-way Multiplexor

Arithmetic Logic Unit
ALU 算术逻辑运算单元
（组合电路 + 多路选择器）

---

storage

时序电路 带状态 有环
clocked registers
当时钟信号到达上升沿时，输入信号的值会被更新到输出，直到下一个上升沿再更新。


srcA 寄存器的号
valA 寄存器的值
读 类似于组合电路 不需要什么上升沿 马上得结果

写
dstW 写 哪个寄存器
valW 写 什么值
两个输入
写 时序电路 等到时钟上升沿 更新寄存器

内存

---

如何实现一个CPU

multi-stages


期中 11.21 范围：到此ppt的y86
没讲过的不考 eg浮点






