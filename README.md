# cpwn

cpwn is a C library which aims to implement all Python [pwntools](https://github.com/Gallopsled/pwntools) functionality.


## First, a word about string handling
Almost all strings in cpwn are handled by a custom dynamic string library for cpwn called `str`. The main advantage of `str` is that it is binary safe.

```c    
str* buffer1 = str_new("This\x00string is terminated by a null char");
str* buffer2 = str_new_raw("String\x00is\x00terminated\x00by\x00length", 30);

printf("%d\n", (int) str_len(buffer1)); // 4
printf("%d\n", (int) str_len(buffer2)); // 30
```


## Talk To Processes

Just like in pwntools, a Tube works like a general interface to interact with processes or sockets.
```c
// Spawn Python
Tube* p = pwn_process("/usr/bin/python");

// Recieve until prompt with a 1 second timeout
str* recvt = str_new(">>>");
pwn_recvuntil(p, recvt, 1);

// Send a payload to Python prompt
str* payload = str_new("3*(4+3)");
pwn_sendline(p, payload);

// Recieve result
str* res = pwn_recvline(p, 2); // 21\n
str_print(res);

// Free str
str_free(recvt);
str_free(payload);
str_free(res);
```


You can also interact with processes
```c
Tube* p = pwn_process("/usr/bin/sh");
pwn_interactive(p);
```


## Packing Integers
```c
str* r1 = pwn_p64(133333337, "little");
str_print(r1); // Y\x81\xf2\x07\x00\x00\x00\x00

str* r2 = pwn_p32(0xdeaddead, "little");
str_print(r2); // \xad\xde\xad\xde
```

## ELF Manipulation
```c
Elf64 *e = ELF64("./bin")

printf("%s\n", e->endian); // little
printf("%s\n", e->arch); // amd64
printf("0x%lx\n", (uint64_t)symbols(e, "print_flag")); // 0x4030f7


str *r = pwn_elf64_read(e, 1, 4);
str_print(r); // ELF
```

TODO: Resolve dynamic sections (.plt .got)

## Example CTF
```c
#include "includes/cpwn.h"

void narnia0() {
    // Spawn process
    Tube* p = pwn_process("./narnia0");

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
