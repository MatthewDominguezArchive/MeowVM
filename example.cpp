#include <Windows.h>
#include <string>
#include <iostream>
#include "meow_vm.h"


namespace codes {
    std::uint16_t push = 0x62; // random values;
    std::uint16_t call = 0x88;
    std::uint16_t sub = 0x49;

    std::uint16_t rsp = 0x92;

    std::uint16_t print = 0x23;

    std::uint16_t input = 0x29;

    std::uint8_t extra_abs = 15;
    std::uint8_t extra_reg = 29;
    std::uint8_t extra_ptr = 0;
};

// top of stack is size, second to top is char buffer address
void get_input(mvm::instruction& itn, mvm::meow_vm* this_vm){
    auto& stack = *this_vm->get_stack();
    auto& strbuf_size = stack[*this_vm->get_rsp() - 1];                          // size
    auto strbuf_ptr = reinterpret_cast<char*>(stack[*this_vm->get_rsp() - 2]);   // string* 
    std::string in_string;
    std::getline(std::cin, in_string);

    if (in_string.size() < strbuf_size){
        std::memcpy(strbuf_ptr, in_string.c_str(), in_string.size());
        strbuf_ptr[in_string.size()] ='\0';
        strbuf_size = in_string.size();
    }
}

void print_str(mvm::instruction& itn, mvm::meow_vm* this_vm){
    auto& stack = *this_vm->get_stack();
    std::printf("%s", reinterpret_cast<char*>(stack[*this_vm->get_rsp() - 1]));
}

void test(){
    mvm::meow_vm vm{};

    // Add vm mvm::instructions
    vm.add_instruction(vm.get_push(), codes::push);
    vm.add_instruction(vm.get_call(), codes::call);
    vm.add_instruction(vm.get_sub(), codes::sub);

    // Add registers
    vm.add_register(vm.get_rsp(), codes::rsp);

    // Set the extra fields
    vm.set_arithmetic(codes::extra_abs, codes::extra_reg, codes::extra_ptr);

    const int size = 256;
    const char question_a[] = "What is your name: ";
    const char question_b[] = "Where were you born: ";
    const char response_part_a[] = "Hello ";
    const char response_part_b[] = " from ";
    const char response_part_c[] = ".\n";

    char input_a[size];
    char input_b[size];

    mvm::program vm_msgbx_test = {
        { codes::push,  reinterpret_cast<std::uint64_t>(&question_a), 0,        codes::extra_abs },         // push question_a
        { codes::call,  reinterpret_cast<std::uint64_t>(&print_str), 0,         codes::extra_abs },         // call print_str
        { codes::sub,   codes::rsp, 1,                                          codes::extra_abs },         // sub rsp, 1

        { codes::push,  reinterpret_cast<std::uint64_t>(&input_a), 0,           codes::extra_abs },         // push &input_a
        { codes::push,  size, 0,                                                codes::extra_abs },         // push size
        { codes::call,  reinterpret_cast<std::uint64_t>(&get_input), 0,         codes::extra_abs },         // call get_input
        { codes::sub,   codes::rsp, 1,                                          codes::extra_abs },         // sub rsp, 2

        { codes::push,  reinterpret_cast<std::uint64_t>(&question_b), 0,        codes::extra_abs },         // push question_b
        { codes::call,  reinterpret_cast<std::uint64_t>(&print_str), 0,         codes::extra_abs },         // call print_str
        { codes::sub,   codes::rsp, 1,                                          codes::extra_abs },         // sub rsp, 1

        { codes::push,  reinterpret_cast<std::uint64_t>(&input_b), 0,           codes::extra_abs },         // push &input_b
        { codes::push,  size, 0,                                                codes::extra_abs },         // push size
        { codes::call,  reinterpret_cast<std::uint64_t>(&get_input), 0,         codes::extra_abs },         // call get_input
        { codes::sub,   codes::rsp, 1,                                          codes::extra_abs },         // sub rsp, 2

        { codes::push,  reinterpret_cast<std::uint64_t>(&response_part_a), 0,   codes::extra_abs },         // push &response_part_a
        { codes::call,  reinterpret_cast<std::uint64_t>(&print_str), 0,         codes::extra_abs },         // call print_str
        { codes::sub,   codes::rsp, 1,                                          codes::extra_abs },         // sub rsp, 1

        { codes::push,  reinterpret_cast<std::uint64_t>(&input_a), 0,           codes::extra_abs },         // &input_a
        { codes::call,  reinterpret_cast<std::uint64_t>(&print_str), 0,         codes::extra_abs },         // call print_str
        { codes::sub,   codes::rsp, 1,                                          codes::extra_abs },         // sub rsp, 1

        { codes::push,  reinterpret_cast<std::uint64_t>(&response_part_b), 0,   codes::extra_abs },         // push &response_part_b
        { codes::call,  reinterpret_cast<std::uint64_t>(&print_str), 0,         codes::extra_abs },         // call print_str
        { codes::sub,   codes::rsp, 1,                                          codes::extra_abs },         // sub rsp, 1

        { codes::push,  reinterpret_cast<std::uint64_t>(&input_b), 0,           codes::extra_abs },         // &input_a
        { codes::call,  reinterpret_cast<std::uint64_t>(&print_str), 0,         codes::extra_abs },         // call print_str
        { codes::sub,   codes::rsp, 1,                                          codes::extra_abs },         // sub rsp, 1

        { codes::push,  reinterpret_cast<std::uint64_t>(&response_part_c), 0,   codes::extra_abs },         // push &response_part_c
        { codes::call,  reinterpret_cast<std::uint64_t>(&print_str), 0,         codes::extra_abs },         // call print_str
        { codes::sub,   codes::rsp, 1,                                          codes::extra_abs },         // sub rsp, 1
    };

    vm.run_program(vm_msgbx_test);
}

int main() {

    test();

    return 0;
}
