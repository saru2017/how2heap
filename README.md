# how2heap


(https://github.com/shellphish/how2heap)[!shellphishのhow2hea]を見ながらheap exploitationの勉強。

## 001_first_fit.c

やってることは単純で、mallocを2回して1回目のやつをfreeしてもう1回mallocすると1回目のやつが再利用されるというやつ。
当たり前だと思ったけどfreeすると最初に入れた```this is A!```が壊れるのは少し面白かった。

fastbinが未だに良く分からない。

```c:001_first_fit.c
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
```


```
wani:~/2019/how2heap$ gcc 001_first_fit.c
wani:~/2019/how2heap$ ./a
このプログラムはglibcがどういう風にメモリを割り当てるかを見せるためのものであり、攻撃をデモするものではありません。
glibcは空いているchunkを見つけるためにfirst-fitアルゴリズムを用います。
もし十分大きな空きchunkがあった場合、mallocはそのchunkを選択します。.
これは「use-after-free」の状況でexploitに利用できます。
2つのバッファをmallocします。これらのバッファはfastbinになる必要が無いです。十分大きく取ることができます。
saru: fastbinってなんだ？？？
c: char* a = malloc(512);
c: char* b = malloc(256);
c: char* c;
1st malloc(512): 0x20048390
2nd malloc(256): 0x20048598
0x20048598 - 0x20048390 = 520
こんな感じでmallocを続けることができます。
mallocしたaの領域に"this is A!"という文字列を書き込みましょう。
c: strcpy(a, "this is A!");
最初に割り当てられた0x20048390はthis is A!を指し示しています。
で、この最初のやつをfreeします。
c: free(a);
これ以上free他にfreeする必要はないです。もしわれわれが512バイト以下の領域をmallocしようとした場合、そのアドレスは0x20048390になるはず。
なので500バイトmallocしてみます。
c: c = malloc(500);
3rd malloc(500): 0x20048390
最初のmallocポイントの0x20048390の指してる文字列は▒▒ ▒▒
saru: 壊れてる???
今度はcに別の文字列"this is C!"を突っ込んでみます。
c: strcpy(c, "this is C!");
3回目のmallocがさしてる0x20048390に入ってる文字列はthis is C!
1回目のmallocがさしてる0x20048390に入ってる文字列はthis is C!
つまりmallocで再利用したデータは、3番目のデータが保持されていることになります。
wani:~/2019/how2heap$
```
