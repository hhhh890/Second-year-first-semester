# Lab2 bomb Lab


* 实验时间：2025/10/16～2025/10/23
* 指导老师：杨哲慜
* 本次实验，我完成了所有内容。
## 目录


## 实验目的
#### 反汇编拆除所有炸弹
1. phase_1: 字符串匹配
2. phase_2: 循环
3. phase_3：跳转表
4. phase_4：递归
5. phase_5：ASCII码的运算与字符串匹配
6. phase_6：链表的查找与构造
7. secret_phase：树的递归遍历

## 实验内容

### 1. phase_1: 字符串匹配
代码：打断点反汇编结果
```py
Dump of assembler code for function phase_1:
   0x0000555555555627 <+0>:	endbr64 
   0x000055555555562b <+4>:	sub    $0x8,%rsp
   0x000055555555562f <+8>:	lea    0x1b1a(%rip),%rsi        # 0x555555557150
   0x0000555555555636 <+15>:	callq  0x555555555b2d <strings_not_equal>
   0x000055555555563b <+20>:	test   %eax,%eax
   0x000055555555563d <+22>:	jne    0x555555555644 <phase_1+29>
   0x000055555555563f <+24>:	add    $0x8,%rsp
   0x0000555555555643 <+28>:	retq   
   0x0000555555555644 <+29>:	callq  0x555555555e09 <explode_bomb>
   0x0000555555555649 <+34>:	jmp    0x55555555563f <phase_1+24>
End of assembler dump.
```
截图：
![1](image1.png)
思路：
lea    0x1b1a(%rip),%rsi将rip存储的下一行地址加上偏移量赋值到第二个参数寄存器，然后与rsp也就是我的第一题输入看看相等不，不相等就爆炸


### 2. phase_2
反汇编代码：
```py
Dump of assembler code for function phase_2:
   0x000055555555564b <+0>:	endbr64 
   0x000055555555564f <+4>:	push   %rbp
   0x0000555555555650 <+5>:	push   %rbx
   0x0000555555555651 <+6>:	sub    $0x28,%rsp
   0x0000555555555655 <+10>:	mov    %fs:0x28,%rax
   0x000055555555565e <+19>:	mov    %rax,0x18(%rsp)
   0x0000555555555663 <+24>:	xor    %eax,%eax
   0x0000555555555665 <+26>:	mov    %rsp,%rsi
   0x0000555555555668 <+29>:	callq  0x555555555e4b <read_six_numbers>
   0x000055555555566d <+34>:	cmpl   $0x0,(%rsp)
   0x0000555555555671 <+38>:	jne    0x55555555567a <phase_2+47>
   0x0000555555555673 <+40>:	cmpl   $0x1,0x4(%rsp)
   0x0000555555555678 <+45>:	je     0x55555555567f <phase_2+52>
   0x000055555555567a <+47>:	callq  0x555555555e09 <explode_bomb>
   0x000055555555567f <+52>:	mov    %rsp,%rbx
   0x0000555555555682 <+55>:	lea    0x10(%rsp),%rbp
   0x0000555555555687 <+60>:	jmp    0x555555555692 <phase_2+71>
   0x0000555555555689 <+62>:	add    $0x4,%rbx
   0x000055555555568d <+66>:	cmp    %rbp,%rbx
   0x0000555555555690 <+69>:	je     0x5555555556a3 <phase_2+88>
   0x0000555555555692 <+71>:	mov    0x4(%rbx),%eax
   0x0000555555555695 <+74>:	add    (%rbx),%eax
   0x0000555555555697 <+76>:	cmp    %eax,0x8(%rbx)
   0x000055555555569a <+79>:	je     0x555555555689 <phase_2+62>
   0x000055555555569c <+81>:	callq  0x555555555e09 <explode_bomb>
   0x00005555555556a1 <+86>:	jmp    0x555555555689 <phase_2+62>
   0x00005555555556a3 <+88>:	mov    0x18(%rsp),%rax
   0x00005555555556a8 <+93>:	sub    %fs:0x28,%rax
   0x00005555555556b1 <+102>:	jne    0x5555555556ba <phase_2+111>
   0x00005555555556b3 <+104>:	add    $0x28,%rsp
   0x00005555555556b7 <+108>:	pop    %rbx
   0x00005555555556b8 <+109>:	pop    %rbp
   0x00005555555556b9 <+110>:	retq   
   0x00005555555556ba <+111>:	callq  0x555555555280 <__stack_chk_fail@plt>
End of assembler dump.

```

思路：首先看输入，这一行  
~~~
callq  0x555555555e4b <read_six_numbers>
~~~  
可以看出输入是6个数字，然后继续读，发现第一个输入数字是0，第二个是1：  
```py
0x000055555555566d <+34>:	cmpl   $0x0,(%rsp)
0x0000555555555673 <+40>:	cmpl   $0x1,0x4(%rsp)
```  
接下来可以看到让rbx指向rsp[2]的地址,进入循环，+71到+79实现的是rbx[i+1]+rbx[i]==rbx[i+2]的判断，如果不相等就爆炸，知道rbx指向最后一个数字后面：  
```py
   0x0000555555555682 <+55>:	lea    0x10(%rsp),%rbp
   0x0000555555555687 <+60>:	jmp    0x555555555692 <phase_2+71>
   0x0000555555555689 <+62>:	add    $0x4,%rbx
   0x000055555555568d <+66>:	cmp    %rbp,%rbx
   0x0000555555555690 <+69>:	je     0x5555555556a3 <phase_2+88>
   0x0000555555555692 <+71>:	mov    0x4(%rbx),%eax
   0x0000555555555695 <+74>:	add    (%rbx),%eax
   0x0000555555555697 <+76>:	cmp    %eax,0x8(%rbx)
   0x000055555555569a <+79>:	je     0x555555555689 <phase_2+62>
```

### 3. phase_3
反汇编代码：
```py
(gdb) disas phase_3
Dump of assembler code for function phase_3:
   0x00005555555556bf <+0>:	endbr64 
   0x00005555555556c3 <+4>:	sub    $0x18,%rsp
   0x00005555555556c7 <+8>:	mov    %fs:0x28,%rax
   0x00005555555556d0 <+17>:	mov    %rax,0x8(%rsp)
   0x00005555555556d5 <+22>:	xor    %eax,%eax
   0x00005555555556d7 <+24>:	lea    0x4(%rsp),%rcx
   0x00005555555556dc <+29>:	mov    %rsp,%rdx
   0x00005555555556df <+32>:	lea    0x1cf7(%rip),%rsi        # 0x5555555573dd
   0x00005555555556e6 <+39>:	callq  0x555555555330 <__isoc99_sscanf@plt>
   0x00005555555556eb <+44>:	cmp    $0x1,%eax
   0x00005555555556ee <+47>:	jle    0x55555555570a <phase_3+75>
   0x00005555555556f0 <+49>:	cmpl   $0x7,(%rsp)
   0x00005555555556f4 <+53>:	ja     0x55555555575b <phase_3+156>
   0x00005555555556f6 <+55>:	mov    (%rsp),%eax
   0x00005555555556f9 <+58>:	lea    0x1ac0(%rip),%rdx        # 0x5555555571c0
   0x0000555555555700 <+65>:	movslq (%rdx,%rax,4),%rax
   0x0000555555555704 <+69>:	add    %rdx,%rax
   0x0000555555555707 <+72>:	notrack jmpq *%rax
   0x000055555555570a <+75>:	callq  0x555555555e09 <explode_bomb>
   0x000055555555570f <+80>:	jmp    0x5555555556f0 <phase_3+49>
   0x0000555555555711 <+82>:	mov    $0x3d2,%eax
   0x0000555555555716 <+87>:	cmp    %eax,0x4(%rsp)
   0x000055555555571a <+91>:	jne    0x55555555576e <phase_3+175>
   0x000055555555571c <+93>:	mov    0x8(%rsp),%rax
   0x0000555555555721 <+98>:	sub    %fs:0x28,%rax
   0x000055555555572a <+107>:	jne    0x555555555775 <phase_3+182>
   0x000055555555572c <+109>:	add    $0x18,%rsp
   0x0000555555555730 <+113>:	retq   
   0x0000555555555731 <+114>:	mov    $0x11f,%eax
   0x0000555555555736 <+119>:	jmp    0x555555555716 <phase_3+87>
   0x0000555555555738 <+121>:	mov    $0xb8,%eax
   0x000055555555573d <+126>:	jmp    0x555555555716 <phase_3+87>
   0x000055555555573f <+128>:	mov    $0x3b5,%eax
   0x0000555555555744 <+133>:	jmp    0x555555555716 <phase_3+87>
   0x0000555555555746 <+135>:	mov    $0x4d,%eax
   0x000055555555574b <+140>:	jmp    0x555555555716 <phase_3+87>
   0x000055555555574d <+142>:	mov    $0x270,%eax
   0x0000555555555752 <+147>:	jmp    0x555555555716 <phase_3+87>
   0x0000555555555754 <+149>:	mov    $0x31c,%eax
   0x0000555555555759 <+154>:	jmp    0x555555555716 <phase_3+87>
--Type <RET> for more, q to quit, c to continue without paging--c
   0x000055555555575b <+156>:	callq  0x555555555e09 <explode_bomb>
   0x0000555555555760 <+161>:	mov    $0x0,%eax
   0x0000555555555765 <+166>:	jmp    0x555555555716 <phase_3+87>
   0x0000555555555767 <+168>:	mov    $0x357,%eax
   0x000055555555576c <+173>:	jmp    0x555555555716 <phase_3+87>
   0x000055555555576e <+175>:	callq  0x555555555e09 <explode_bomb>
   0x0000555555555773 <+180>:	jmp    0x55555555571c <phase_3+93>
   0x0000555555555775 <+182>:	callq  0x555555555280 <__stack_chk_fail@plt>
End of assembler dump.
```  
思路：
先看要输入什么，查<__isoc99_sscanf@plt>输入的第二个参数:  
![3](image3_1.png)  
看这一行，要求 rsp[0]<=7 不然就爆炸
```py
 <+49>:	cmpl   $0x7,(%rsp)                               rsp[0]<=7
```
接下来进入核心跳转逻辑,右边的批注是对应意思：  
```cpp
<+55>:	mov    (%rsp),%eax                               ax = rsp[0];
<+58>:	lea    0x1ac0(%rip),%rdx # 0x5555555571c0        rdx =  0x31c0
<+65>:	movslq (%rdx,%rax,4),%rax                        ax = (rdx+rax*4)
<+69>:	add    %rdx,%rax                                 
<+65>:	movslq (%rdx,%rax,4),%rax                        rax +=rdx  
<+69>:	add    %rdx,%rax                                 跳转到 ->rax = <+65>+(rdx+rax*4)
<+72>:	notrack jmpq *%rax
```   
查看跳转基数，尝试跳转：  
![跳转表](image3_2.png)   
下面是记录跳转数字以及结果，答案只要任选其一即可：  
```cpp
 <+82>:	mov    $0x3d2,%eax                        结果：      ax=0x3d2    1/978
 <+114>:	mov    $0x11f,%eax                              ax=0x11f  2/287
 <+121>:	mov    $0xb8,%eax                               ax = 0xb8   3/184
 <+128>:	mov    $0x3b5,%eax                              ax = 0x3b5  4/949
<+135>:	mov    $0x4d,%eax                               5/77
<+142>:	mov    $0x270,%eax                              6/624
<+149>:	mov    $0x31c,%eax                               7/796
```  
**注意**：本题的输入顺序是正常的，但是并非所有输入都是这样
### 4. phase_4
反汇编代码：
```py
Dump of assembler code for function phase_4:
   0x00005555555557b5 <+0>:	endbr64 
   0x00005555555557b9 <+4>:	sub    $0x18,%rsp
   0x00005555555557bd <+8>:	mov    %fs:0x28,%rax
   0x00005555555557c6 <+17>:	mov    %rax,0x8(%rsp)
   0x00005555555557cb <+22>:	xor    %eax,%eax
   0x00005555555557cd <+24>:	mov    %rsp,%rcx
   0x00005555555557d0 <+27>:	lea    0x4(%rsp),%rdx
   0x00005555555557d5 <+32>:	lea    0x1c01(%rip),%rsi        # 0x5555555573dd
   0x00005555555557dc <+39>:	callq  0x555555555330 <__isoc99_sscanf@plt>
   0x00005555555557e1 <+44>:	cmp    $0x2,%eax
   0x00005555555557e4 <+47>:	jne    0x5555555557f1 <phase_4+60>
   0x00005555555557e6 <+49>:	mov    (%rsp),%eax
   0x00005555555557e9 <+52>:	sub    $0x2,%eax
   0x00005555555557ec <+55>:	cmp    $0x2,%eax
   0x00005555555557ef <+58>:	jbe    0x5555555557f6 <phase_4+65>      '''要求：rsp<=4'''
   0x00005555555557f1 <+60>:	callq  0x555555555e09 <explode_bomb>
   0x00005555555557f6 <+65>:	mov    (%rsp),%esi
   0x00005555555557f9 <+68>:	mov    $0x9,%edi        '''第一个参数传递8,第二个为热rsp[0] '''
   0x00005555555557fe <+73>:	callq  0x55555555577a <func4>
   0x0000555555555803 <+78>:	cmp    %eax,0x4(%rsp)
   0x0000555555555807 <+82>:	jne    0x55555555581e <phase_4+105>
   0x0000555555555809 <+84>:	mov    0x8(%rsp),%rax
   0x000055555555580e <+89>:	sub    %fs:0x28,%rax
   0x0000555555555817 <+98>:	jne    0x555555555825 <phase_4+112>
   0x0000555555555819 <+100>:	add    $0x18,%rsp
   0x000055555555581d <+104>:	retq   
   0x000055555555581e <+105>:	callq  0x555555555e09 <explode_bomb>
   0x0000555555555823 <+110>:	jmp    0x555555555809 <phase_4+84>
   0x0000555555555825 <+112>:	callq  0x555555555280 <__stack_chk_fail@plt>
End of assembler dump.

```  
```py
Dump of assembler code for function func4:
   0x000055555555577a <+0>:	endbr64 
   0x000055555555577e <+4>:	mov    $0x0,%eax
   0x0000555555555783 <+9>:	test   %edi,%edi
   0x0000555555555785 <+11>:	jle    0x5555555557b4 <func4+58>
   0x0000555555555787 <+13>:	push   %r12
   0x0000555555555789 <+15>:	push   %rbp
   0x000055555555578a <+16>:	push   %rbx
   0x000055555555578b <+17>:	mov    %edi,%ebx
   0x000055555555578d <+19>:	mov    %esi,%ebp
   0x000055555555578f <+21>:	mov    %esi,%eax
   0x0000555555555791 <+23>:	cmp    $0x1,%edi
   0x0000555555555794 <+26>:	je     0x5555555557af <func4+53>
   0x0000555555555796 <+28>:	lea    -0x1(%rdi),%edi
   0x0000555555555799 <+31>:	callq  0x55555555577a <func4>
   0x000055555555579e <+36>:	lea    (%rax,%rbp,1),%r12d
   0x00005555555557a2 <+40>:	lea    -0x2(%rbx),%edi
   0x00005555555557a5 <+43>:	mov    %ebp,%esi
   0x00005555555557a7 <+45>:	callq  0x55555555577a <func4>
   0x00005555555557ac <+50>:	add    %r12d,%eax
   0x00005555555557af <+53>:	pop    %rbx
   0x00005555555557b0 <+54>:	pop    %rbp
   0x00005555555557b1 <+55>:	pop    %r12
   0x00005555555557b3 <+57>:	retq   
   0x00005555555557b4 <+58>:	retq   
End of assembler dump.
  
```  
思路：先看输入,这个函数需要输入两个数字：  
~~~
callq  0x555555555330 <__isoc99_sscanf@plt>
<+44>:	cmp    $0x2,%eax
~~~  
![input](image4.png)  
继续往下读,确定第一个答案4，发现后面把参数传递进入func4，第一个参数时9，第二个是答案4  
阅读func4，发现有两个返回条件，第一个是di==0，第二个是di==1。看到函数内两次调用func4，看输入参数：  
```py
lea    -0x1(%rdi),%edi
callq  0x55555555577a <func4> """第一个参数是di-1，第二个参数是si"""
lea    (%rax,%rbp,1),%r12d """func4(di-1,si)的返回值+原来传进来的参数si"""
lea    -0x2(%rbx),%edi  """ 第二次用func4，第一个参数是di-2"""
mov    %ebp,%esi        """第二个参数是原来本函数的第二个参数si"""
callq  0x55555555577a <func4> 
add    %r12d,%eax       """func4(di-2,si)+func(di-1,si)+si"""
```  
现在知道第二个答案就是func4(9,4)，为了求出这个结果，用cpp写了一个代码，运行得到答案：  
```cpp
#include <iostream>
int func4(int a, int b) {
    if (a<=0)
    {
        return 0;
        /* code */
    }
    if (a==1)
    {
        return b;
        /* code */
    }
    return func4(a-1,b)+b+func4(a-2,b);
    
}

int main() {
    int result = func4(9,4);
    std::cout<<"when rsp[0]==4 ,Rsp[1]: " << result << std::endl;
    return 0;
}
```  
在输入时发现结果是反过来的，如果输入4 352，会早就炸了，所以结果是352 4  



### 5. phase_5
反汇编代码：
```py
Dump of assembler code for function phase_5:
   0x000055555555582a <+0>:	endbr64 
   0x000055555555582e <+4>:	push   %rbx
   0x000055555555582f <+5>:	sub    $0x10,%rsp
   0x0000555555555833 <+9>:	mov    %rdi,%rbx
   0x0000555555555836 <+12>:	mov    %fs:0x28,%rax
   0x000055555555583f <+21>:	mov    %rax,0x8(%rsp)
   0x0000555555555844 <+26>:	xor    %eax,%eax
   0x0000555555555846 <+28>:	callq  0x555555555b0c <string_length>
   0x000055555555584b <+33>:	cmp    $0x6,%eax
   0x000055555555584e <+36>:	jne    0x5555555558a5 <phase_5+123>
   0x0000555555555850 <+38>:	mov    $0x0,%eax
   0x0000555555555855 <+43>:	lea    0x1984(%rip),%rcx        # 0x5555555571e0 <array.0>
   0x000055555555585c <+50>:	movzbl (%rbx,%rax,1),%edx
   0x0000555555555860 <+54>:	and    $0xf,%edx
   0x0000555555555863 <+57>:	movzbl (%rcx,%rdx,1),%edx
   0x0000555555555867 <+61>:	mov    %dl,0x1(%rsp,%rax,1)
   0x000055555555586b <+65>:	add    $0x1,%rax
   0x000055555555586f <+69>:	cmp    $0x6,%rax
   0x0000555555555873 <+73>:	jne    0x55555555585c <phase_5+50>
   0x0000555555555875 <+75>:	movb   $0x0,0x7(%rsp)
   0x000055555555587a <+80>:	lea    0x1(%rsp),%rdi
   0x000055555555587f <+85>:	lea    0x1928(%rip),%rsi        # 0x5555555571ae
   0x0000555555555886 <+92>:	callq  0x555555555b2d <strings_not_equal>
   0x000055555555588b <+97>:	test   %eax,%eax
   0x000055555555588d <+99>:	jne    0x5555555558ac <phase_5+130>
   0x000055555555588f <+101>:	mov    0x8(%rsp),%rax
   0x0000555555555894 <+106>:	sub    %fs:0x28,%rax
   0x000055555555589d <+115>:	jne    0x5555555558b3 <phase_5+137>
   0x000055555555589f <+117>:	add    $0x10,%rsp
   0x00005555555558a3 <+121>:	pop    %rbx
   0x00005555555558a4 <+122>:	retq   
   0x00005555555558a5 <+123>:	callq  0x555555555e09 <explode_bomb>
   0x00005555555558aa <+128>:	jmp    0x555555555850 <phase_5+38>
   0x00005555555558ac <+130>:	callq  0x555555555e09 <explode_bomb>
   0x00005555555558b1 <+135>:	jmp    0x55555555588f <phase_5+101>
   0x00005555555558b3 <+137>:	callq  0x555555555280 <__stack_chk_fail@plt>
End of assembler dump.
```

10 4  15 5  6  7
1010  0100  1111  0101  0110  0111
j     d     o     e     f     g
"maduiersnfotvbylSo you
 think you can stop the bomb with ctrl-c, do you?"
   0x0000555555555846 <+28>:	callq  0x555555555b0c <string_length>
思路：先看输入string_length返回6，6个符号的字符串。再看关键匹配项strings_not_equal，先把这个要相等的字符串找出来：  
![目标](image5_2.png)  
得到“oilers”，再回过头来看前面，发现<+50>到<+73>是循环，而rax充当for循环的循环计数器i，我们输入的答案是存在rsp[0:5]上面的，看循环逻辑，rbx存着rsp[0],然后rsp[rax]&0xf得到的值作为索引从<array.0>里面找到对应的字符，再赋值回rsp[rax]里面，现在看<array.0>里面存了什么：  
![array.0](image5_1.png)  
发现是字符串"maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"  
现在可以找“oilers”每个字母对应的索引以及二进制转换，再在ASCII码表里面找到对应小写字母，其实大写字母也应该可以。  
~~~
索引：10       4     15    5      6    7
二进制：1010  0100  1111  0101  0110  0111
小写字母：j     d     o     e     f     g
~~~


### 6. phase_6
思路：由于phase_6太长了，讲解每一行就写进反汇编代码里面了。<read_six_numbers>可见读入6个数字。在有颜色的while大循环里面，r14是循环计数器，一直到6，先取rsp[r14],rbx是第二个循环计数器，rbx=r14,跳到第二个循环里面，判断rsp[r14]是否与rsp[rbx]相等，出来后还要判断rsp[r14]<=6，所以这6个数必须是1、2、3、4、5、6，顺序看后面逻辑。   
进入第二个循环，先看 <node1>：  
![node](image6_1.png)  
发现是链表，node5后面没有了，但是通过链表后面8个字节存储的是下一个指针的地址查到node6.  
看第二个大循环，遍历rsp[0:5],然后进入小循环，取出node(rsp[i])，开一个数组,头的地址在rsp+0x20，存的是按照rsp里面数字顺序对应的节点的地址  
接着开一段新的内存rbx，作为数组，存的是rsp+0x20开始的数组里面节点地址对应的值，就是前4个字节存的值。  
<+234>这行开始又是一个循环，要rbx[i]< rbx[i+1]。由此我们回来看node，前4个字节存的值从小到大排列一下，最后得到了结果1 4 3 6 5 2
```cpp 
6：Dump of assembler code for function phase_6:
   0x00000000000018b8 <+0>:	endbr64 
   0x00000000000018bc <+4>:	push   %r14
   0x00000000000018be <+6>:	push   %r13
   0x00000000000018c0 <+8>:	push   %r12
   0x00000000000018c2 <+10>:	push   %rbp
   0x00000000000018c3 <+11>:	push   %rbx
   0x00000000000018c4 <+12>:	sub    $0x60,%rsp
   0x00000000000018c8 <+16>:	mov    %fs:0x28,%rax
   0x00000000000018d1 <+25>:	mov    %rax,0x58(%rsp)
   0x00000000000018d6 <+30>:	xor    %eax,%eax
   0x00000000000018d8 <+32>:	mov    %rsp,%r13        #(*r13)=rsp[0]
   0x00000000000018db <+35>:	mov    %r13,%rsi  
   0x00000000000018de <+38>:	callq  0x1e4b <read_six_numbers>
   0x00000000000018e3 <+43>:	mov    $0x1,%r14d       #r14 = 1;
   0x00000000000018e9 <+49>:	mov    %rsp,%r12        #*r12 = rsp[0];
   0x00000000000018ec <+52>:	jmp    0x1916 <phase_6+94>    while(eax<6)//in 94 
   0x00000000000018ee <+54>:	callq  0x1e09 <explode_bomb>
   0x00000000000018f3 <+59>:	jmp    0x1925 <phase_6+109>   #while(j<5)
   0x00000000000018f5 <+61>:	add    $0x1,%rbx           #rbx:j+=1;
   0x00000000000018f9 <+65>:	cmp    $0x5,%ebx           #if(k>5){break;}->86
   0x00000000000018fc <+68>:	jg     0x190e <phase_6+86>
   0x00000000000018fe <+70>:	mov    (%r12,%rbx,4),%eax  #eax = rsp[j]
   0x0000000000001902 <+74>:	cmp    %eax,0x0(%rbp)      #if(eax:rsp[j]==rsp[i]){bomb;}
   0x0000000000001905 <+77>:	jne    0x18f5 <phase_6+61>    #whileEnd
   0x0000000000001907 <+79>:	callq  0x1e09 <explode_bomb>
   0x000000000000190c <+84>:	jmp    0x18f5 <phase_6+61>
   0x000000000000190e <+86>:	add    $0x1,%r14           #r14 +=1;
   0x0000000000001912 <+90>:	add    $0x4,%r13           #r13+=4
   0x0000000000001916 <+94>:	mov    %r13,%rbp           #*rbp=*r13=rsp[j];
   0x0000000000001919 <+97>:	mov    0x0(%r13),%eax      #eax=rsp[j]
   0x000000000000191d <+101>:	sub    $0x1,%eax           #eax-=1
   0x0000000000001920 <+104>:	cmp    $0x5,%eax           
   0x0000000000001923 <+107>:	ja     0x18ee <phase_6+54>  #eax<=5  => rsp[]<=6
   0x0000000000001925 <+109>:	cmp    $0x5,%r14d          #if(r14d>5)->120;r14d is i;
   0x0000000000001929 <+113>:	jg     0x1930 <phase_6+120>    
   0x000000000000192b <+115>:	mov    %r14,%rbx              #j = i;
   0x000000000000192e <+118>:	jmp    0x18fe <phase_6+70>    #else{->70}    while end
   0x0000000000001930 <+120>:	mov    $0x0,%esi              #si = 0;
   0x0000000000001935 <+125>:	mov    (%rsp,%rsi,4),%ecx     # do{   #cx = rsp[si]
   0x0000000000001938 <+128>:	mov    $0x1,%eax              #eax = 1
   0x000000000000193d <+133>:	lea    0x38dc(%rip),%rdx        # 0x5220 <node1> #rdx=node1
   0x0000000000001944 <+140>:	cmp    $0x1,%ecx                 #if(cx>1){
   0x0000000000001947 <+143>:	jle    0x1954 <phase_6+156>         do
   0x0000000000001949 <+145>:	mov    0x8(%rdx),%rdx            #rdx=rdx+0x8; rdx=node++
   0x000000000000194d <+149>:	add    $0x1,%eax                 #eax+=1
   0x0000000000001950 <+152>:	cmp    %ecx,%eax                 # while(eax!=cx)->145
   0x0000000000001952 <+154>:	jne    0x1949 <phase_6+145>      }
   0x0000000000001954 <+156>:	mov    %rdx,0x20(%rsp,%rsi,8)    #rsp+si*8+0x20 = rdx //开数组新地址，存的是按照rsp里面数字顺序对应的节点的地址
   0x0000000000001959 <+161>:	add    $0x1,%rsi                  #rsi+=1
   0x000000000000195d <+165>:	cmp    $0x6,%rsi                 #  }while(rsi<6)
   0x0000000000001961 <+169>:	jne    0x1935 <phase_6+125>
   0x0000000000001963 <+171>:	mov    0x20(%rsp),%rbx           #rbx=rsp+20=newnode1 
   0x0000000000001968 <+176>:	mov    0x28(%rsp),%rax           #rax = new2
   0x000000000000196d <+181>:	mov    %rax,0x8(%rbx)            #rbx -> rax    new1->new2
   0x0000000000001971 <+185>:	mov    0x30(%rsp),%rdx           #rdx = new3
   0x0000000000001976 <+190>:	mov    %rdx,0x8(%rax)            #rax  ->rdx    n2->n3
   0x000000000000197a <+194>:	mov    0x38(%rsp),%rax           #rax = new4 
   0x000000000000197f <+199>:	mov    %rax,0x8(%rdx)            #rdx->new4     n3->n4
   0x0000000000001983 <+203>:	mov    0x40(%rsp),%rdx           #rdx=new5
   0x0000000000001988 <+208>:	mov    %rdx,0x8(%rax)            #n4->n5
   0x000000000000198c <+212>:	mov    0x48(%rsp),%rax
   0x0000000000001991 <+217>:	mov    %rax,0x8(%rdx)            #n5->n6
   0x0000000000001995 <+221>:	movq   $0x0,0x8(%rax)            #n6->null
   0x000000000000199d <+229>:	mov    $0x5,%ebp              #bp=5
   0x00000000000019a2 <+234>:	jmp    0x19ad <phase_6+245>   #do{
   0x00000000000019a4 <+236>:	mov    0x8(%rbx),%rbx         rbx=rbx->next
   0x00000000000019a8 <+240>:	sub    $0x1,%ebp              #bp=bp-1
   0x00000000000019ab <+243>:	je     0x19be <phase_6+262>   do:
   0x00000000000019ad <+245>:	mov    0x8(%rbx),%rax         #ax=n1->next
   0x00000000000019b1 <+249>:	mov    (%rax),%eax            #ax=*rax
   0x00000000000019b3 <+251>:	cmp    %eax,(%rbx)            #*rbx<=ax}// 升序
   0x00000000000019b5 <+253>:	jle    0x19a4 <phase_6+236>      #while(bp==0)
   0x00000000000019b7 <+255>:	callq  0x1e09 <explode_bomb>
   0x00000000000019bc <+260>:	jmp    0x19a4 <phase_6+236>
   0x00000000000019be <+262>:	mov    0x58(%rsp),%rax           
   0x00000000000019c3 <+267>:	sub    %fs:0x28,%rax
   0x00000000000019cc <+276>:	jne    0x19db <phase_6+291>
   0x00000000000019ce <+278>:	add    $0x60,%rsp
   0x00000000000019d2 <+282>:	pop    %rbx
   0x00000000000019d3 <+283>:	pop    %rbp
   0x00000000000019d4 <+284>:	pop    %r12
   0x00000000000019d6 <+286>:	pop    %r13
   0x00000000000019d8 <+288>:	pop    %r14
   0x00000000000019da <+290>:	retq   
   0x00000000000019db <+291>:	callq  0x1280 <__stack_chk_fail@plt>
End of assembler dump.

```


### 7. secret_phase
反汇编代码：
```cpp
(gdb) disas phase_defused
Dump of assembler code for function phase_defused:
   0x0000555555555fca <+0>:	endbr64 
   0x0000555555555fce <+4>:	sub    $0x88,%rsp
   0x0000555555555fd5 <+11>:	mov    %fs:0x28,%rax
   0x0000555555555fde <+20>:	mov    %rax,0x78(%rsp)
   0x0000555555555fe3 <+25>:	xor    %eax,%eax
   0x0000555555555fe5 <+27>:	mov    $0x1,%edi
   0x0000555555555fea <+32>:	callq  0x555555555d16 <send_msg>
   0x0000555555555fef <+37>:	cmpl   $0x6,0x372e(%rip)        # 0x555555559724 <num_input_strings>
   0x0000555555555ff6 <+44>:	je     0x555555556014 <phase_defused+74>
   0x0000555555555ff8 <+46>:	mov    0x78(%rsp),%rax
   0x0000555555555ffd <+51>:	sub    %fs:0x28,%rax
   0x0000555555556006 <+60>:	jne    0x555555556093 <phase_defused+201>
   0x000055555555600c <+66>:	add    $0x88,%rsp
   0x0000555555556013 <+73>:	retq   
   0x0000555555556014 <+74>:	lea    0xc(%rsp),%rcx
   0x0000555555556019 <+79>:	lea    0x8(%rsp),%rdx
   0x000055555555601e <+84>:	lea    0x10(%rsp),%r8
   0x0000555555556023 <+89>:	lea    0x13fd(%rip),%rsi        # 0x555555557427
   0x000055555555602a <+96>:	lea    0x383b(%rip),%rdi        # 0x55555555986c <input_strings+300>
--Type <RET> for more, q to quit, c to continue without paging--c
   0x0000555555556031 <+103>:	mov    $0x0,%eax
   0x0000555555556036 <+108>:	callq  0x555555555330 <__isoc99_sscanf@plt>
   0x000055555555603b <+113>:	cmp    $0x3,%eax
   0x000055555555603e <+116>:	je     0x55555555605a <phase_defused+144>
   0x0000555555556040 <+118>:	lea    0x12a1(%rip),%rdi        # 0x5555555572e8
   0x0000555555556047 <+125>:	callq  0x555555555250 <puts@plt>
   0x000055555555604c <+130>:	lea    0x12c5(%rip),%rdi        # 0x555555557318
   0x0000555555556053 <+137>:	callq  0x555555555250 <puts@plt>
   0x0000555555556058 <+142>:	jmp    0x555555555ff8 <phase_defused+46>
   0x000055555555605a <+144>:	lea    0x10(%rsp),%rdi
   0x000055555555605f <+149>:	lea    0x13ca(%rip),%rsi        # 0x555555557430
   0x0000555555556066 <+156>:	callq  0x555555555b2d <strings_not_equal>
   0x000055555555606b <+161>:	test   %eax,%eax
   0x000055555555606d <+163>:	jne    0x555555556040 <phase_defused+118>
   0x000055555555606f <+165>:	lea    0x1212(%rip),%rdi        # 0x555555557288
   0x0000555555556076 <+172>:	callq  0x555555555250 <puts@plt>
   0x000055555555607b <+177>:	lea    0x122e(%rip),%rdi        # 0x5555555572b0
   0x0000555555556082 <+184>:	callq  0x555555555250 <puts@plt>
   0x0000555555556087 <+189>:	mov    $0x0,%eax
   0x000055555555608c <+194>:	callq  0x555555555a21 <secret_phase>
   0x0000555555556091 <+199>:	jmp    0x555555556040 <phase_defused+118>
   0x0000555555556093 <+201>:	callq  0x555555555280 <__stack_chk_fail@plt>
End of assembler dump.

```
```cpp
(gdb) disas secret_phase
Dump of assembler code for function secret_phase:
   0x0000555555555a21 <+0>:	endbr64 
   0x0000555555555a25 <+4>:	push   %rbx
   0x0000555555555a26 <+5>:	callq  0x555555555e90 <read_line>
   0x0000555555555a2b <+10>:	mov    %rax,%rdi
   0x0000555555555a2e <+13>:	mov    $0xa,%edx
   0x0000555555555a33 <+18>:	mov    $0x0,%esi
   0x0000555555555a38 <+23>:	callq  0x555555555310 <strtol@plt>
   0x0000555555555a3d <+28>:	mov    %eax,%ebx
   0x0000555555555a3f <+30>:	sub    $0x1,%eax
   0x0000555555555a42 <+33>:	cmp    $0x3e8,%eax
   0x0000555555555a47 <+38>:	ja     0x555555555a6f <secret_phase+78>
   0x0000555555555a49 <+40>:	mov    %ebx,%esi
   0x0000555555555a4b <+42>:	lea    0x36ee(%rip),%rdi        # 0x555555559140 <n1>
   0x0000555555555a52 <+49>:	callq  0x5555555559e0 <fun7>
   0x0000555555555a57 <+54>:	cmp    $0x2,%eax
   0x0000555555555a5a <+57>:	jne    0x555555555a76 <secret_phase+85>
   0x0000555555555a5c <+59>:	lea    0x1725(%rip),%rdi        # 0x555555557188
   0x0000555555555a63 <+66>:	callq  0x555555555250 <puts@plt>
   0x0000555555555a68 <+71>:	callq  0x555555555fca <phase_defused>
   0x0000555555555a6d <+76>:	pop    %rbx
   0x0000555555555a6e <+77>:	retq   
   0x0000555555555a6f <+78>:	callq  0x555555555e09 <explode_bomb>
   0x0000555555555a74 <+83>:	jmp    0x555555555a49 <secret_phase+40>
   0x0000555555555a76 <+85>:	callq  0x555555555e09 <explode_bomb>
   0x0000555555555a7b <+90>:	jmp    0x555555555a5c <secret_phase+59>
End of assembler dump.
```

```cpp
(gdb) disas fun7
Dump of assembler code for function fun7:
   0x00005555555559e0 <+0>:	endbr64 
   0x00005555555559e4 <+4>:	test   %rdi,%rdi
   0x00005555555559e7 <+7>:	je     0x555555555a1b <fun7+59>  #di!=null
   0x00005555555559e9 <+9>:	sub    $0x8,%rsp
   0x00005555555559ed <+13>:	mov    (%rdi),%edx   #dx=rdi
   0x00005555555559ef <+15>:	cmp    %esi,%edx     #si>dx   ->31  si<dx  ->44
   0x00005555555559f1 <+17>:	jg     0x5555555559ff <fun7+31>
   0x00005555555559f3 <+19>:	mov    $0x0,%eax        #eax=0  
   0x00005555555559f8 <+24>:	jne    0x555555555a0c <fun7+44>  
   0x00005555555559fa <+26>:	add    $0x8,%rsp        # rsp+=8寄存器返回
   0x00005555555559fe <+30>:	retq   
   0x00005555555559ff <+31>:	mov    0x8(%rdi),%rdi      #rdi=rdi->left.v
   0x0000555555555a03 <+35>:	callq  0x5555555559e0 <fun7>  #func7
   0x0000555555555a08 <+40>:	add    %eax,%eax           #eax+=eax
   0x0000555555555a0a <+42>:	jmp    0x5555555559fa <fun7+26>  #->26,ret
   0x0000555555555a0c <+44>:	mov    0x10(%rdi),%rdi           #rdi=rdi->right.v
   0x0000555555555a10 <+48>:	callq  0x5555555559e0 <fun7>     #func7
   0x0000555555555a15 <+53>:	lea    0x1(%rax,%rax,1),%eax     #ax=ax+ax+1
   0x0000555555555a19 <+57>:	jmp    0x5555555559fa <fun7+26>
   0x0000555555555a1b <+59>:	mov    $0xffffffff,%eax
   0x0000555555555a20 <+64>:	retq   
End of assembler dump.
```

思路：用objdump -d反汇编所有发现有个秘密关卡。先看进入条件:  
![in](image.png)  
进入后读输入,@plt的函数是库函数，所以用man strtol查询功能，发现输入要转成一个数字，往下读发现要求要小于0x3e8，输入参数di = <n1>，si=输入。先看看<n1>：  
![n1](image_se.png)  
发现<n1>是树，指代关系如下：  
```py 
(gdb) x/60wx   0x555555559140
0x555555559140 <n1>:    0x00000024      0x00000000      0x55559160      0x00005555  ->n21
0x555555559150 <n1+16>: 0x55559180      0x00005555      0x00000000      0x00000000  ->n22
0x555555559160 <n21>:   0x00000008      0x00000000      0x555591e0      0x00005555  ->n31
0x555555559170 <n21+16>:        0x555591a0      0x00005555      0x00000000      0x00000000   ->32
0x555555559180 <n22>:   0x00000032      0x00000000      0x555591c0      0x00005555        ->n33
0x555555559190 <n22+16>:        0x55559200      0x00005555      0x00000000      0x00000000   ->n34
0x5555555591a0 <n32>:   0x00000016      0x00000000      0x555590b0      0x00005555
0x5555555591b0 <n32+16>:        0x55559070      0x00005555      0x00000000      0x00000000
0x5555555591c0 <n33>:   0x0000002d      0x00000000      0x55559010      0x00005555
0x5555555591d0 <n33+16>:        0x555590d0      0x00005555      0x00000000      0x00000000
0x5555555591e0 <n31>:   0x00000006      0x00000000      0x55559030      0x00005555
0x5555555591f0 <n31+16>:        0x55559090      0x00005555      0x00000000      0x00000000
0x555555559200 <n34>:   0x0000006b      0x00000000      0x55559050      0x00005555
0x555555559210 <n34+16>:        0x555590f0      0x00005555      0x00000000      0x00000000
```  
现在读func7，重点看这几行,发现对于参数1，也就是<ni>的值如果大于我输入的数，就跳到31行，内存取的是左子树地址，如果小于，先将之前的返回值清空为0，然后跳转到44行，内存取右子树地址，递归，如果等于，返回0。   
```py
<+15>:	cmp    %esi,%edx 
<+17>:	jg     0x5555555559ff <fun7+31>
<+19>:	mov    $0x0,%eax 
<+24>:	jne    0x555555555a0c <fun7+44> 
```  
由于最后要返回到secret_phase的结果为2，所以要上一个是左子树返回eax+=eax，上上个要返回1，只有右子树会返回ax*2+1,所以上上个返回0，找到目标<n32>:   0x00000016，结果就是22
## 实验结果
![final](imagefinal.png)  
![score](score.png)  


## 遇到的问题及解决办法

1. 遇到问题：拆炸弹的时候一行行运行看不到运行到哪里很麻烦

   解决方案：用下面这几个命令行就能看到框框同步运行
   ~~~
      (gdb)layout asm
      (gdb)tui disable  # 关闭所有 TUI 窗口，返回普通命令行模式
      (gdb) layout src   # 切换回源代码窗口（或其他布局）
      (gdb) layout split # 切换回分屏模式（源代码 + 汇编）
      (gdb) layout next  # 循环切换布局模式，直到关闭汇编窗口
   ~~~
2. 遇到问题：要防止炸弹爆炸导致扣分，还要运行到指定函数前面停下来让我一行行运行知道汇编语言的运行逻辑

   解决方案：用下面几行打断点和删断点
   ~~~
   b phase_
   break explode_bomb
   delete breakpoint 2

   b *<adr>
   ~~~

3. 遇到问题：要查一些地址或寄存器的值

   解决办法：用下面一些命令：
   ~~~
   x/s <address>
   x/wx $rsp + 4  //会 =value
   print (char*)$rax
   print $eax
   ~~~

## 实验心得
0基础，学汇编，拆完炸弹感觉汇编已经能看懂了，寄存器和内存相应功能和使用也了解了一些，还学会了gdb的调试工具。
（谈谈本次实验中，有哪些收获）

## 其他备注

无