# how2heap


[shellphishのhow2heap](https://github.com/shellphish/how2heap)を見ながらheap exploitationの勉強。

## 001_first_fit.c

やってることは単純で、mallocを2回して1回目のやつをfreeしてもう1回mallocすると1回目のやつが再利用されるというやつ。
当たり前だと思ったけどfreeすると最初に入れた```this is A!```が壊れるのは少し面白かった。

glibcがどういう風にmallocでメモリを割り当てるかを見せている。
glibcは空いているchunkを見つけるためにfirst-fitアルゴリズムを用いているらしい。
たぶんその名の通り、一番最初にサイズがfitする空間を割り当てている。
これは「use-after-free」をしているプログラムでのexploitに利用できるらしい。

fastbinが未だに良く分からない。

```c:001_first_fit.c
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
```


```
wani:~/2019/how2heap$ ./a
まず、2つのバッファをmallocします。これらのバッファはfastbinになる必要が無い。なので十分大きく取ることができる。
(saru: fastbinってなんだ？？？)

c: char* a = malloc(512);
c: char* b = malloc(256);
c: char* c;

1st malloc(512): 0x20048390
2nd malloc(256): 0x20048598
0x20048598 - 0x20048390 = 520

mallocしたaの領域に"this is A!"という文字列を書き込む。

c: strcpy(a, "this is A!");

1回目に割り当てられた0x20048390はthis is A!を指し示している。
で、この最初のやつをfreeする。

c: free(a);

1回目に割り当てられた0x20048390は▒▒ ▒▒ を指し示している。
(saru: 壊れてる？)
もし次に512バイト以下の領域をmallocしようとした場合、そのアドレスは0x20048390になるはず。
なので500バイトmallocしてみる。

c: c = malloc(500);

3rd malloc(500): 0x20048390
3回目に割り当てられた0x20048390は▒▒ ▒▒ を指し示している。
今度はcに別の文字列"this is C!"を突っ込んでみる。

c: strcpy(c, "this is C!");

3回目のmallocがさしてる0x20048390に入ってる文字列はthis is C!
1回目のmallocがさしてる0x20048390に入ってる文字列はthis is C!
つまりmallocで再利用したデータは、3番目のデータが保持されていることになる。
wani:~/2019/how2heap$
```


## 002_fastbin_dup.c

単純なfastbinを使ったdouble-free attackのデモ。
これをやってfastbinというのはmallocで前にfreeしたのと同じ領域をmallocした場合、free listから返すということを指すのだということが判明。

```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
  printf("まず、3つのバッファをmallocする。\n\n");

  int *a = malloc(8);
  int *b = malloc(8);
  int *c = malloc(8);

  printf("c: int *a = malloc(8);\n");
  printf("c: int *b = malloc(8);\n");
  printf("c: int *c = malloc(8);\n\n");

  
  printf("1st malloc(8): %p\n", a);
  printf("2nd malloc(8): %p\n", b);
  printf("3rd malloc(8): %p\n\n", c);

  printf("1個目のバッファをfree\n");

  free(a);
  printf("free(a);\n\n");

  printf("もし1個目のバッファ%pをもう一度freeすると、%pはfree listのトップなのでプログラムがクラッシュする場合がある。\n\n", a, a);

  free(a);
  printf("free(a);\n\n");
  
  printf("2個目のバッファ%pをfreeする。\n\n", b);
  free(b);
  printf("free(b);\n\n");

  printf("現段階のfee listは [ %p, %p, %p ]。となる。もしここでmallocを2回呼ぶと%pが2回返ってくる。\n\n", b, a, a, a);
  printf("1st malloc(8): %p\n", malloc(8));
  printf("2nd malloc(8): %p\n", malloc(8));
  printf("3rd malloc(8): %p\n", malloc(8));
}
```

```
saru@lucifen:~/tmp$ ./a.out
まず、3つのバッファをmallocする。

c: int *a = malloc(8);
c: int *b = malloc(8);
c: int *c = malloc(8);

1st malloc(8): 0x561e7e8c4670
2nd malloc(8): 0x561e7e8c4690
3rd malloc(8): 0x561e7e8c46b0

1個目のバッファをfree
free(a);

もし1個目のバッファ0x561e7e8c4670をもう一度freeすると、0x561e7e8c4670はfree listのトップなのでプログラムがクラッシュする場合がある。

free(a);

2個目のバッファ0x561e7e8c4690をfreeする。

free(b);

現段階のfee listは [ 0x561e7e8c4690, 0x561e7e8c4670, 0x561e7e8c4670 ]。となる。もしここでmallocを2回呼ぶと0x561e7e8c4670が2回返ってくる。

1st malloc(8): 0x561e7e8c4690
2nd malloc(8): 0x561e7e8c4670
3rd malloc(8): 0x561e7e8c4670
saru@lucifen:~/tmp$
```

## 003_fastbin_dup.c

今度はa, b, aの順でfreeしてみる。

```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
  printf("まず、3つのバッファをmallocする。\n\n");

  int *a = malloc(8);
  int *b = malloc(8);
  int *c = malloc(8);

  printf("c: int *a = malloc(8);\n");
  printf("c: int *b = malloc(8);\n");
  printf("c: int *c = malloc(8);\n\n");

  
  printf("1st malloc(8): %p\n", a);
  printf("2nd malloc(8): %p\n", b);
  printf("3rd malloc(8): %p\n\n", c);

  printf("1個目のバッファをfree\n");

  free(a);
  printf("free(a);\n\n");

  
  printf("2個目のバッファ%pをfreeする。\n", b);
  free(b);
  printf("free(b);\n\n");

  printf("そうすると今度は1個目のバッファ%pがfree listの先頭なのでfreeすることが可能になる。\n", a);

  free(a);
  printf("free(a);\n\n");

  printf("現段階のfee listは [ %p, %p, %p ]。となる。もしここでmallocを2回呼ぶと%pが2回返ってくる。\n\n", a, b, a, a);
  printf("1st malloc(8): %p\n", malloc(8));
  printf("2nd malloc(8): %p\n", malloc(8));
  printf("3rd malloc(8): %p\n", malloc(8));
}
```


```bash-statement
saru@lucifen:~/tmp$ ./a.out
まず、3つのバッファをmallocする。

c: int *a = malloc(8);
c: int *b = malloc(8);
c: int *c = malloc(8);

1st malloc(8): 0x5557dfd17670
2nd malloc(8): 0x5557dfd17690
3rd malloc(8): 0x5557dfd176b0

1個目のバッファをfree
free(a);

2個目のバッファ0x5557dfd17690をfreeする。
free(b);

そうすると今度は1個目のバッファ0x5557dfd17670がfree listの先頭なのでfreeすることが可能になる。
free(a);

現段階のfee listは [ 0x5557dfd17670, 0x5557dfd17690, 0x5557dfd17670 ]。となる。もしここでmallocを2回呼ぶと0x5557dfd17670が2回返ってくる。

1st malloc(8): 0x5557dfd17670
2nd malloc(8): 0x5557dfd17690
3rd malloc(8): 0x5557dfd17670
saru@lucifen:~/tmp$
```

