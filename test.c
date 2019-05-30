#include <stdio.h>
#include <stdlib.h>
#include <sys/filio.h>
#include <string.h>
int kbhit()
{
 int i;
 ioctl(0, FIONREAD, &i);
 return i; /* return a count of chars available to read */
}
int main()
{
 int i = 0;
 char c, slc[80];
 system("stty raw -echo");
//  printf("enter 'q' to quit \n");
 for (;c!='q';i++) {
    if (kbhit()) {
        c=getchar();
        slc[i]=c;
    //    printf("\n got %c, on iteration %d",c, i);
    }
}
slc[i]='\0';
printf("%s%d\n", slc, strcmp(slc, "\rq"));
 system("stty cooked echo");
}//"[Aq"