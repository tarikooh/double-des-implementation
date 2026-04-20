# Double DES in C

This project is a simple educational implementation of **Double DES (Data Encryption Standard)** written in C.

It demonstrates how a block cipher can be applied twice with two different keys.

## Files

* `main.c` — main source file containing:

  * DES implementation
  * Double DES wrapper
  * Example usage in `main()`

---

## Requirements

* A C compiler such as:

  * GCC
  * Clang

## Compilation

Open a terminal and run:

```bash
gcc main.c -o double_des
```

Or with Clang:

```bash
clang main.c -o double_des
```

---

## Running the Program

After compiling, run:

```bash
./double_des
```

## What It Does

1. Encrypts a 64-bit plaintext using Double DES
2. Decrypts it back to verify correctness

## Group Members
* Tariku Temesgen                            UGR/7565/15
* Pawlos Addisu                              UGR/5732/15
* Simon Beyene                               UGR/6260/15
* Ibrahim Ali                                UGR/6991/14
* Nathnael Lule
