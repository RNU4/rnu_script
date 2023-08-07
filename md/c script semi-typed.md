# Cemi script

## rules
semi types
only 3 types float, int and "string"(u8)

auto sets types for varibels which can be manually set

structs is a list of pointers
## syntax
---
```Cpp
import("std")
<1>main() // run main thread

struct string{
    *bytes,
    len,
    size
}

void string:new(s){
    len := strlen(s)
    str := malloc(sizeof(string))
    str.bytes = malloc(len)
    memcpy(str.bytes,s,len)
    str.len = len
    str.size = len
}

thread hello_world(*x,y){
    lock() //locks thread
    *x += y
    unlock() //unlocks thread
}

thread main(){
    x := 2
    pointer := std:malloc(32)
    t := <1>hello_world(&x, 2) // create thread and set its priority to 1 (bytecode execution speed, higher = faster)
    // t contains the thread pointer
    

    hello := 2f
    hello += 2.2
    wait(t) // wait for thread to finish
    str := string:new("hello")//malloc(sizeof(string))
    
    str = str:copy(2,str:len())
    println(str)
    
    #bytecode{
        push_num,1,
        push_num,42,
        push_num,31,
        add_signed,
        set_pointer,
        halt,
    }

}

```