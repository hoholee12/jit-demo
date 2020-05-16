# jit-demo
simply learning about how jit works

# how to cross compile to ubuntu 64bit
this is 32bit code. fyi

install following:

```
sudo dpkg --add-architecture i386
sudo apt install libc6-dev-i386
sudo apt install g++-multilib

```

# and compile it with

```
g++ main.cpp -o <whatever> -m32
```
