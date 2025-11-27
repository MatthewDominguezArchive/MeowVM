# MeowVM
Single header include 🤩

## How to use

### Create a meowvm
Create an instance of mvm::meow by default initializing the class.
```cpp
mvm::meow_vm vm{};
```
### Language instructions and call instruction
Before the use of an instruction, you must add that instruction.
```cpp
vm.add_instruction(vm.get_mov(), 0x10);
```
Now in our virtualized code, whenever we want to use the mov instruction we can just type 0x10. This leaves to a lot of ambiguous codes, so feel free to either use pre-processor definitions like #define mov 0x10. This is a great and simple way of doing it, but the issue is that if you have multiple meowvms, you should have different vm_codes (ex: 0x10 in this case) for each instruction. So you might rather use a namespace, enum, or struct to contain your definitions for each instruction.

To create a custon instruction, you can do something like this:
```cpp
// THIS PROGRAM WILL PRINT 11
# define print_value_vm_code 0x12

void print_value(mvm::instruction& itn){
  std::printf("%llu", itn.a);
}

int main(){
    mvm::meow_vm vm{};

    // Add vm mvm::instructions
    vm.add_instruction(&print_value, print_value_vm_code);
    

    vm.run_program(
        {
            {
                print_value_vm_code, 11, 0, 0
            }
        }
    );

    return 0;
}
```
*Note: arithmetic types are default initialized to {0, 1, 2}*
There are many limitations to this. One being that because we don't have a pointer to the class that the instruction is being ran in, you can't access registers, the stack, etc. So, in the case that you need that information, you are better to use the **call** instruction.
```cpp
// THIS PROGRAM WILL PRINT 54
# define mov_vm_code 0x13
# define call_vm_code 0x199
# define rax_vm_code 0x99

void print_rax(mvm::instruction& itn, mvm::meow_vm* this_vm){
    std::printf("%llu", *this_vm->get_rax());
}

int main(){
    mvm::meow_vm vm{};

    vm.add_instruction(vm.get_mov(), mov_vm_code);
    vm.add_instruction(vm.get_call(), call_vm_code);
    
    vm.add_register(vm.get_rax(), rax_vm_code);
    

    vm.run_program(
        {
            {
                mov_vm_code, rax_vm_code, 54, 0
            },
            {
                call_vm_code, reinterpret_cast<std::uint64_t>(&print_rax), 0, 0
            }
        }
    );

    return 0;
}
```

### Language Syntax
The language is based on x86 instruction set. Including these instructions: mov, push, pop, add, sub, mul, div, imul, idiv, cmp, jmp, je, jne, jl, jg, ja, jb, call. But gives the user the freedom to create their own instructions if they like.

Each instruction is of type:
```cpp
struct instruction {
    std::uint16_t opcode;
    std::uint64_t a;
    std::uint64_t b;
    std::uint8_t extra;
};
```

Since the project is open source, any function can be understood by reading. Here are the key points.
```asm
mov rax, 10
```
Can be translated to:
```asm
mov_vmc, rax_vmc, 10, abs_vmc
```
Obviously using the vm_codes instead of the actual instructions and registers, but the final value is telling the mov instruction that the value in field b is an absolute value. If it was a pointer to an absolute value, you can use the arithmetic code of ptr. Lastly if it's a register, you can use the arithmetic code of reg. These vm_codes can be defined using the set_arithmetic() method. The default value is {0, 1, 2}.

This logic applies to all instructions in the default instruction set. For exersize, heres one more
```asm
push rcx
```
Can be translated to:
```asm
push_vmc, rcx_vmc, 0, reg_vmc
```
So the 0 in the b field, can be trashed. If you would like you can set this to zero, or to any random value because this field isn't accessed.



This post is the thing that gave me the idea for this project: [Credit](https://medium.com/%40112piyushsingh/hiding-code-like-a-spy-vm-based-code-obfuscation-in-c-with-runtime-decryption-ccb24c53b7d6)
