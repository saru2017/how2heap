#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{

  printf("まず、2つのバッファをmallocします。これらのバッファはfastbinになる必要が無い。なので十分大きく取ることができる。\n");
  printf("(saru: fastbinってなんだ？？？)\n\n");

  char* a = malloc(512);
  char* b = malloc(256);
  char* c;

  printf("c: char* a = malloc(512);\n");
  printf("c: char* b = malloc(256);\n");
  printf("c: char* c;\n\n");
  
  
  printf("1st malloc(512): %p\n", a);
  printf("2nd malloc(256): %p\n", b);
  printf("%p - %p = %d\n\n", b, a, b - a);

  printf("mallocしたaの領域に\"this is A!\"という文字列を書き込む。\n\n");
  strcpy(a, "this is A!");
  printf("c: strcpy(a, \"this is A!\");\n\n");

  printf("1回目に割り当てられた%pは%sを指し示している。\n", a, a);

  printf("で、この最初のやつをfreeする。\n\n");
  free(a);
  printf("c: free(a);\n\n");

  printf("1回目に割り当てられた%pは%sを指し示している。\n", a, a);
  printf("(saru: 壊れてる？)\n");

  
  printf("もし次に512バイト以下の領域をmallocしようとした場合、そのアドレスは%pになるはず。\n", a); 
  printf("なので500バイトmallocしてみる。\n\n");
  
  c = malloc(500);
  printf("c: c = malloc(500);\n\n");
  
  printf("3rd malloc(500): %p\n", c);
  printf("3回目に割り当てられた%pは%sを指し示している。\n", a, a);

  
  printf("今度はcに別の文字列\"this is C!\"を突っ込んでみる。\n\n");
  
  strcpy(c, "this is C!");
  printf("c: strcpy(c, \"this is C!\");\n\n");
  
  printf("3回目のmallocがさしてる%pに入ってる文字列は%s\n", c, c);
  printf("1回目のmallocがさしてる%pに入ってる文字列は%s\n", a, a);
  printf("つまりmallocで再利用したデータは、3番目のデータが保持されていることになる。\n");
}
