#pragma once

#ifndef MEOWVM
#define MEOWVM

#include <vector>
#include <unordered_map>
#include <functional>

namespace mvm{

    struct instruction {
        std::uint16_t opcode;
        std::uint64_t a;
        std::uint64_t b;
        std::uint8_t extra;
    };

    using program = std::vector<instruction>;

    class meow_vm {
    private:
        std::uint64_t rax;
        std::uint64_t rbx;
        std::uint64_t rcx;
        std::uint64_t rdx;

        std::uint64_t rsp;
        std::uint64_t rbp;
        std::uint64_t rip;

        std::uint8_t flags;

        std::uint8_t abs = 0;
        std::uint8_t reg = 1;
        std::uint8_t ptr = 2;

        bool jumped = false;

        std::vector<std::uint64_t> stack;

        std::unordered_map<std::uint16_t, std::function<void(instruction&)>>instruction_table;

        std::unordered_map<std::uint16_t, std::uint64_t*>register_table;

        void set_flags(instruction& itn) {
            std::uint64_t ra = *register_table[itn.a]; // A has to be a register
            std::uint64_t rb;         // B depends on extra;
            if (itn.extra == abs) {
                rb = itn.b;
            }
            else if (itn.extra == reg) {
                rb = *register_table[itn.b];
            }
            else if (itn.extra == ptr) {
                rb = *reinterpret_cast<std::uint64_t*>(itn.b);
            }
            else return;

            auto sa = static_cast<std::int64_t>(ra);
            auto sb = static_cast<std::int64_t>(rb);
            auto result = sa - sb;

            flags = 0;
            flags |= (ra == rb) ? true << 0 : 0;                  // ZF 
            flags |= (result < 0) ? true << 1 : 0;                      // SF
            flags |= (ra < rb) ? true << 2 : 0;                   // CF
            flags |= (((sa ^ sb) & (sa ^ result)) < 0) ? true << 3 : 0; // OF
        }

        void mov(instruction& itn){
            if (itn.extra == this->abs) {
                memcpy(register_table.at(itn.a), &itn.b, sizeof(std::uint64_t));
                return;
            }
            if (itn.extra == this->reg) {
                memcpy(register_table.at(itn.a), register_table.at(itn.b), sizeof(std::uint64_t));
                return;
            }
            if (itn.extra == this->ptr){
                memcpy(register_table.at(itn.a), reinterpret_cast<std::uint64_t*>(itn.b), sizeof(std::uint64_t));
                return;
            }
        }
        void push(instruction& itn){
            while (rsp >= stack.size()){
                stack.push_back(0);
            }
            if (itn.extra == this->abs){
                stack[rsp] = itn.a;
            }
            else if (itn.extra == this->reg){
                stack[rsp] = *register_table.at(itn.a);
            }
            else if (itn.extra == this->ptr){
                stack[rsp] = *reinterpret_cast<std::uint64_t*>(itn.a);
            }
            else{return;}
            rsp++;
        }
        void pop(instruction& itn){
            if (stack.empty()) return;

            if (itn.extra == this->abs){
                *register_table.at(itn.a) = stack.back();
            }
            else if (itn.extra == this->reg){
                *register_table.at(itn.a) = stack.back();
            }
            else if (itn.extra == this->ptr){
                *reinterpret_cast<std::uint64_t*>(itn.a) = stack.back();
            }
            else {return;}

            rsp--;
        }
        void add(instruction& itn) {
            if (itn.extra == this->abs){
                *register_table[itn.a] += itn.b;
            }
            else if (itn.extra == this->reg){
                *register_table[itn.a] += *register_table[itn.b];
            }
            else if (itn.extra == this->ptr){
                *register_table[itn.a] += *reinterpret_cast<std::uint64_t*>(itn.b);
            }

            set_flags(itn);
        }
        void sub(instruction& itn) {
            if (itn.extra == this->abs){
                *register_table[itn.a] -= itn.b;
            }
            else if (itn.extra == this->reg){
                *register_table[itn.a] -= *register_table[itn.b];
            }
            else if (itn.extra == this->ptr){
                *register_table[itn.a] -= *reinterpret_cast<std::uint64_t*>(itn.b);
            }

            set_flags(itn);
        }
        void mul(instruction& itn) {
            if (itn.extra == this->abs){
                *register_table[itn.a] *= itn.b;
            }
            else if (itn.extra == this->reg){
                *register_table[itn.a] *= *register_table[itn.b];
            }
            else if (itn.extra == this->ptr){
                *register_table[itn.a] *= *reinterpret_cast<std::uint64_t*>(itn.b);
            }

            set_flags(itn);
        }
        void div(instruction& itn) {
            if (itn.extra == this->abs){
                *register_table[itn.a] /= itn.b;
            }
            else if (itn.extra == this->reg){
                *register_table[itn.a] /= *register_table[itn.b];
            }
            else if (itn.extra == this->ptr){
                *register_table[itn.a] /= *reinterpret_cast<std::uint64_t*>(itn.b);
            }

            set_flags(itn);
        }
        void imul(instruction& itn) {
            if (itn.extra == this->abs){
                *register_table[itn.a] = static_cast<std::int64_t>(*register_table[itn.a]) * static_cast<std::int64_t>(itn.b);
            }
            else if (itn.extra == this->reg){
                *register_table[itn.a] = static_cast<std::int64_t>(*register_table[itn.a]) * static_cast<std::int64_t>(*register_table[itn.b]);
            }
            else if (itn.extra == this->ptr){
                *register_table[itn.a] = static_cast<std::int64_t>(*register_table[itn.a]) * static_cast<std::int64_t>(*register_table[*reinterpret_cast<std::uint64_t*>(itn.b)]);
            }

            set_flags(itn);
        }
        void idiv(instruction& itn) {
            if (itn.extra == this->abs){
                *register_table[itn.a] = static_cast<std::int64_t>(*register_table[itn.a]) / static_cast<std::int64_t>(itn.b);
            }
            else if (itn.extra == this->reg){
                *register_table[itn.a] = static_cast<std::int64_t>(*register_table[itn.a]) / static_cast<std::int64_t>(*register_table[itn.b]);
            }
            else if (itn.extra == this->ptr){
                *register_table[itn.a] = static_cast<std::int64_t>(*register_table[itn.a]) / static_cast<std::int64_t>(*register_table[*reinterpret_cast<std::uint64_t*>(itn.b)]);
            }

            set_flags(itn);
        }
        void cmp(instruction& itn) {
            set_flags(itn);
        }  
        void jmp(instruction& itn){
            jumped = true;
            if (itn.extra == abs) {
                rip = itn.a;
            }
            else if (itn.extra == reg){
                rip = *register_table.at(itn.a);

            } else if (itn.extra == ptr) {
                rip = *reinterpret_cast<std::uint64_t*>(itn.a);
            }
        }
        void je(instruction& itn) {
            if (flags & (1 << 0)) {
                jmp(itn);
            }
        }
        void jne(instruction& itn) {
            if (!(flags & (1 << 0))) {
                jmp(itn);
            }
        }
        void jl(instruction& itn) {
            bool sf = flags & (1 << 1);
            bool of = flags & (1 << 3);
            if (sf != of) {
                jmp(itn);
            }
        }
        void jg(instruction& itn) {
            bool zf = flags & (1 << 0);
            bool sf = flags & (1 << 1);
            bool of = flags & (1 << 3);
            if (!zf && (sf == of)) {
                jmp(itn);
            }
        }
        void ja(instruction& itn) {
            bool cf = flags & (1 << 2);
            bool zf = flags & (1 << 0);
            if (!cf && !zf) {
                jmp(itn);
            }
        }
        void jb(instruction& itn) {
            bool cf = flags & (1 << 2);
            if (cf) {
                jmp(itn);
            }
        }
        void call(instruction& itn) {
            reinterpret_cast<void(*)(instruction&, meow_vm*)>(itn.a)(itn, this);
        }

        void execute_instruction(instruction& itn){
            instruction_table.at(itn.opcode)(itn);
        }

    public:
        std::uint64_t* get_rax(){return &rax;}
        std::uint64_t* get_rbx(){return &rbx;}
        std::uint64_t* get_rcx(){return &rcx;}
        std::uint64_t* get_rdx(){return &rdx;}
        std::uint64_t* get_rsp(){return &rsp;}
        std::uint64_t* get_rbp(){return &rbp;}
        std::uint64_t* get_rip(){return &rip;}
        std::uint8_t*  get_flags(){return &flags;}

        std::vector<std::uint64_t>* get_stack(){return &stack;}

        std::function<void(instruction&)> get_mov() {return [this](instruction& itn){ mov(itn); }; }
        std::function<void(instruction&)> get_push() {return [this](instruction& itn){ push(itn); }; }
        std::function<void(instruction&)> get_pop() {return [this](instruction& itn){ pop(itn); }; }
        std::function<void(instruction&)> get_add() {return [this](instruction& itn){ add(itn); }; }
        std::function<void(instruction&)> get_sub() {return [this](instruction& itn){ sub(itn); }; }
        std::function<void(instruction&)> get_mul() {return [this](instruction& itn){ mul(itn); }; }
        std::function<void(instruction&)> get_div() {return [this](instruction& itn){ div(itn); }; }
        std::function<void(instruction&)> get_imul() {return [this](instruction& itn){ imul(itn); }; }
        std::function<void(instruction&)> get_idiv() {return [this](instruction& itn){ idiv(itn); }; }
        std::function<void(instruction&)> get_cmp() {return [this](instruction& itn){ cmp(itn); }; }
        std::function<void(instruction&)> get_jmp() {return [this](instruction& itn){ jmp(itn); }; }
        std::function<void(instruction&)> get_je() {return [this](instruction& itn){ je(itn); }; }
        std::function<void(instruction&)> get_jne() {return [this](instruction& itn){ jne(itn); }; }
        std::function<void(instruction&)> get_jl() {return [this](instruction& itn){ jl(itn); }; }
        std::function<void(instruction&)> get_jg() {return [this](instruction& itn){ jg(itn); }; }
        std::function<void(instruction&)> get_ja() {return [this](instruction& itn){ ja(itn); }; }
        std::function<void(instruction&)> get_jb() {return [this](instruction& itn){ jb(itn); }; }
        std::function<void(instruction&)> get_call() {return [this](instruction& itn){ call(itn); }; }


        void add_instruction(std::function<void(instruction&)> func_addr, std::uint16_t vm_code){
            instruction_table.emplace(vm_code, func_addr);
        }

        void add_register(std::uint64_t* reg_addr, std::uint16_t vm_code){
            register_table.emplace(vm_code, reg_addr);
        }

        void set_arithmetic(std::uint8_t new_abs, std::uint8_t new_reg, std::uint8_t new_ptr){
            this->abs = new_abs;
            this->reg = new_reg;
            this->ptr = new_ptr;
        }

        void run_program(std::vector<instruction> program){
            for (;rip < program.size(); rip++){
                if (jumped) {
                    jumped = false;
                    rip--;
                }
                execute_instruction(program[rip]);
            }
        }
    };
}

#endif
