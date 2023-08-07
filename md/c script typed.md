# C script != C




## syntax
---
```Cpp

<1>main(); // run main thread

thread hello_world(int *x,int y){
    lock(); //locks thread
    *x += y;
    unlock(); //unlocks thread
}

thread main(){
    int x = 2;
    u0 t = <1>hello_world(&x, 2); // create thread and set its priority to 1 (bytecode execution speed, higher = faster)
    int hello = 2;
    hello +=2 ;
    wait(t); // wait for thread to finish

}

```