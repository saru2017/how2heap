#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  printf("このプログラムはglibcがどういう風にメモリを割り当てるかを見せるためのものであり、攻撃をデモするものではありません。\n");
  printf("glibcは空いているchunkを見つけるためにfirst-fitアルゴリズムを用います。\n");
  printf("もし十分大きな空きchunkがあった場合、mallocはそのchunkを選択します。.\n");
  printf("これは「use-after-free」の状況でexploitに利用できます。\n");

  printf("2つのバッファをmallocします。これらのバッファはfastbinになる必要が無いです。十分大きく取ることができます。\n");
  printf("saru: fastbinってなんだ？？？\n");
  char* a = malloc(512);
  char* b = malloc(256);
  char* c;

  printf("c: char* a = malloc(512);\n");
  printf("c: char* b = malloc(256);\n");
  printf("c: char* c;\n");
  
  
  printf("1st malloc(512): %p\n", a);
  printf("2nd malloc(256): %p\n", b);
  printf("%p - %p = %d\n", b, a, b - a);
  printf("こんな感じでmallocを続けることができます。\n");
  printf("mallocしたaの領域に\"this is A!\"という文字列を書き込みましょう。\n");
  strcpy(a, "this is A!");
  printf("c: strcpy(a, \"this is A!\");\n");
  printf("最初に割り当てられた%pは%sを指し示しています。\n", a, a);

  printf("で、この最初のやつをfreeします。\n");
  free(a);
  printf("c: free(a);\n");

  printf("これ以上free他にfreeする必要はないです。もしわれわれが512バイト以下の領域をmallocしようとした場合、そのアドレスは%pになるはず。\n", a); 
  printf("なので500バイトmallocしてみます。\n");
  c = malloc(500);
  printf("c: c = malloc(500);\n");
  printf("3rd malloc(500): %p\n", c);
  printf("最初のmallocポイントの%pの指してる文字列は%s\n", a, a);
  printf("saru: 壊れてる???\n");

  printf("今度はcに別の文字列\"this is C!\"を突っ込んでみます。\n");
  strcpy(c, "this is C!");
  printf("c: strcpy(c, \"this is C!\");\n");
  printf("3回目のmallocがさしてる%pに入ってる文字列は%s\n", c, c);
  printf("1回目のmallocがさしてる%pに入ってる文字列は%s\n", a, a);
  printf("つまりmallocで再利用したデータは、3番目のデータが保持されていることになります。\n");
}
