
```cpp
58 farm <mid_farm> 48 89 c7 end_farm 
48 89 e add_xy   
48 89 c movq rax ->
48 89 f
89 c
89 d6
89 ca
0000000000401c9e <start_farm>:
  401c9e:	b8 01 00 00 00       	mov    $0x1,%eax
  401ca3:	c3                   	retq   

0000000000401ca4 <addval_202>:
  401ca4:	8d 87 4c 48 89 c7    	lea    -0x3876b7b4(%rdi),%eax
  401caa:	c3                   	retq   

0000000000401cab <setval_177>:
  401cab:	c7 07 d8 90 90 90    	movl   $0x909090d8,(%rdi)
  401cb1:	c3                   	retq   

0000000000401cb2 <setval_321>:
  401cb2:	c7 07 68 89 c7 90    	movl   $0x90c78968,(%rdi)
  401cb8:	c3                   	retq   

0000000000401cb9 <getval_198>:
  401cb9:	b8 5b 14 78 90       	mov    $0x9078145b,%eax
  401cbe:	c3                   	retq   

0000000000401cbf <setval_336>:
  401cbf:	c7 07 a1 58 c3 0a    	movl   $0xac358a1,(%rdi)
  401cc5:	c3                   	retq   

0000000000401cc6 <setval_266>:
  401cc6:	c7 07 48 89 c7 91    	movl   $0x91c78948,(%rdi)
  401ccc:	c3                   	retq   

0000000000401ccd <getval_275>:
  401ccd:	b8 75 48 89 c7       	mov    $0xc7894875,%eax
  401cd2:	c3                   	retq   

0000000000401cd3 <getval_209>:
  401cd3:	b8 58 90 90 c3       	mov    $0xc3909058,%eax
  401cd8:	c3                   	retq   


0000000000401cd9 <mid_farm>:
  401cd9:	b8 01 00 00 00       	mov    $0x1,%eax
  401cde:	c3                   	retq   

0000000000401cdf <add_xy>:
  401cdf:	48 8d 04 37          	lea    (%rdi,%rsi,1),%rax
  401ce3:	c3                   	retq   

0000000000401ce4 <addval_496>:
  401ce4:	8d 87 48 89 e0 90    	lea    -0x6f1f76b8(%rdi),%eax
  401cea:	c3                   	retq   

0000000000401ceb <addval_358>:
  401ceb:	8d 87 89 d6 00 d2    	lea    -0x2dff2977(%rdi),%eax
  401cf1:	c3                   	retq   

0000000000401cf2 <setval_425>:
  401cf2:	c7 07 8b c1 90 c3    	movl   $0xc390c18b,(%rdi)
  401cf8:	c3                   	retq   

0000000000401cf9 <getval_271>:
  401cf9:	b8 99 ca 90 c3       	mov    $0xc390ca99,%eax
  401cfe:	c3                   	retq   

0000000000401cff <setval_189>:
  401cff:	c7 07 89 ca 20 d2    	movl   $0xd220ca89,(%rdi)
  401d05:	c3                   	retq   

0000000000401d06 <setval_298>:
  401d06:	c7 07 89 d6 a4 c0    	movl   $0xc0a4d689,(%rdi)
  401d0c:	c3                   	retq   

0000000000401d0d <setval_315>:
  401d0d:	c7 07 89 c1 30 c9    	movl   $0xc930c189,(%rdi)
  401d13:	c3                   	retq   

0000000000401d14 <setval_378>:
  401d14:	c7 07 81 c1 08 c0    	movl   $0xc008c181,(%rdi)
  401d1a:	c3                   	retq   

0000000000401d1b <addval_359>:
  401d1b:	8d 87 8d ca 08 d2    	lea    -0x2df73573(%rdi),%eax
  401d21:	c3                   	retq   

0000000000401d22 <addval_488>:
  401d22:	8d 87 89 c1 38 c9    	lea    -0x36c73e77(%rdi),%eax
  401d28:	c3                   	retq   

0000000000401d29 <setval_142>:
  401d29:	c7 07 8b c1 38 c0    	movl   $0xc038c18b,(%rdi)
  401d2f:	c3                   	retq   

0000000000401d30 <setval_107>:
  401d30:	c7 07 09 ca 90 c3    	movl   $0xc390ca09,(%rdi)
  401d36:	c3                   	retq   

0000000000401d37 <getval_461>:
  401d37:	b8 8d d6 38 c0       	mov    $0xc038d68d,%eax
  401d3c:	c3                   	retq   

0000000000401d3d <addval_392>:
  401d3d:	8d 87 89 d6 08 c0    	lea    -0x3ff72977(%rdi),%eax
  401d43:	c3                   	retq   

0000000000401d44 <getval_241>:
  401d44:	b8 ff 8b c1 c3       	mov    $0xc3c18bff,%eax
  401d49:	c3                   	retq   

0000000000401d4a <setval_253>:
  401d4a:	c7 07 a9 d6 38 c0    	movl   $0xc038d6a9,(%rdi)
  401d50:	c3                   	retq   

0000000000401d51 <getval_348>:
  401d51:	b8 89 d6 84 d2       	mov    $0xd284d689,%eax
  401d56:	c3                   	retq   

0000000000401d57 <setval_230>:
  401d57:	c7 07 89 ca 28 d2    	movl   $0xd228ca89,(%rdi)
  401d5d:	c3                   	retq   

0000000000401d5e <getval_401>:
  401d5e:	b8 58 89 e0 90       	mov    $0x90e08958,%eax
  401d63:	c3                   	retq   

0000000000401d64 <getval_438>:
  401d64:	b8 89 ca 78 db       	mov    $0xdb78ca89,%eax
  401d69:	c3                   	retq   

0000000000401d6a <setval_251>:
  401d6a:	c7 07 48 89 e0 c7    	movl   $0xc7e08948,(%rdi)
  401d70:	c3                   	retq   

0000000000401d71 <addval_111>:
  401d71:	8d 87 c8 89 e0 90    	lea    -0x6f1f7638(%rdi),%eax
  401d77:	c3                   	retq   

0000000000401d78 <addval_294>:
  401d78:	8d 87 68 89 e0 90    	lea    -0x6f1f7698(%rdi),%eax
  401d7e:	c3                   	retq   

0000000000401d7f <setval_465>:
  401d7f:	c7 07 8d d6 08 c0    	movl   $0xc008d68d,(%rdi)
  401d85:	c3                   	retq   

0000000000401d86 <addval_180>:
  401d86:	8d 87 48 89 e0 91    	lea    -0x6e1f76b8(%rdi),%eax
  401d8c:	c3                   	retq   

0000000000401d8d <getval_386>:
  401d8d:	b8 89 ca 90 c3       	mov    $0xc390ca89,%eax
  401d92:	c3                   	retq   

0000000000401d93 <setval_480>:
  401d93:	c7 07 f4 48 89 e0    	movl   $0xe08948f4,(%rdi)
  401d99:	c3                   	retq   

0000000000401d9a <setval_426>:
  401d9a:	c7 07 81 ca 20 d2    	movl   $0xd220ca81,(%rdi)
  401da0:	c3                   	retq   

0000000000401da1 <addval_134>:
  401da1:	8d 87 88 c1 38 c9    	lea    -0x36c73e78(%rdi),%eax
  401da7:	c3                   	retq   

0000000000401da8 <setval_295>:
  401da8:	c7 07 89 c1 90 c3    	movl   $0xc390c189,(%rdi)
  401dae:	c3                   	retq   

0000000000401daf <setval_223>:
  401daf:	c7 07 89 d6 18 d2    	movl   $0xd218d689,(%rdi)
  401db5:	c3                   	retq   

0000000000401db6 <addval_167>:
  401db6:	8d 87 48 89 e0 c7    	lea    -0x381f76b8(%rdi),%eax
  401dbc:	c3                   	retq   

0000000000401dbd <end_farm>:
  401dbd:	b8 01 00 00 00       	mov    $0x1,%eax
  401dc2:	c3                   	retq   
```