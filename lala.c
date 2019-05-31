#include <stdio.h>


int main()
{
  char *str = "Mr. Fiddle Tim went to the mall. Mr. Kurdt was there. Mrs. Love was there also. "
    "They said hi and ate ice cream together." ;

  char res[800] = { 0 };
  sscanf( str, "%800[^.,]", res ) ;
  puts( res ) ;
}