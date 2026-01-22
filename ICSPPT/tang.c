#include<stdio.h>
int main (int argc, char** argv) {
char a[15];
a[14] = '\0';
printf("%s\n", a);
return 0;
}