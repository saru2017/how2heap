# how2heap


(https://github.com/shellphish/how2heap)[!shellphishのhow2hea]を見ながらheap exploitationの勉強。

## 001_first_fit.c

やってることは単純で、mallocを2回して1回目のやつをfreeしてもう1回mallocすると1回目のやつが再利用されるというやつ。
当たり前だと思ったけどfreeすると最初に入れた```this is A!```が壊れるのは少し面白かった。

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
