#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// Define a simple lookup table
typedef struct {
    char *word;
    uint8_t hex_value;
} lookup_entry;

// Lookup table for converting English words to byte values
const lookup_entry lookup_table[] = {
    {"the", 0x0},         {"of", 0x1},           {"and", 0x2},
    {"to", 0x3},          {"in", 0x4},           {"for", 0x5},
    {"is", 0x6},          {"on", 0x7},           {"that", 0x8},
    {"by", 0x9},          {"this", 0xa},         {"with", 0xb},
    {"you", 0xc},         {"it", 0xd},           {"not", 0xe},
    {"or", 0xf},          {"be", 0x10},          {"are", 0x11},
    {"from", 0x12},       {"at", 0x13},          {"as", 0x14},
    {"your", 0x15},       {"all", 0x16},         {"have", 0x17},
    {"new", 0x18},        {"more", 0x19},        {"an", 0x1a},
    {"was", 0x1b},        {"we", 0x1c},          {"will", 0x1d},
    {"home", 0x1e},       {"can", 0x1f},         {"us", 0x20},
    {"about", 0x21},      {"if", 0x22},          {"page", 0x23},
    {"my", 0x24},         {"has", 0x25},         {"search", 0x26},
    {"free", 0x27},       {"but", 0x28},         {"our", 0x29},
    {"one", 0x2a},        {"other", 0x2b},       {"do", 0x2c},
    {"no", 0x2d},         {"information", 0x2e}, {"time", 0x2f},
    {"they", 0x30},       {"site", 0x31},        {"he", 0x32},
    {"up", 0x33},         {"may", 0x34},         {"what", 0x35},
    {"which", 0x36},      {"their", 0x37},       {"news", 0x38},
    {"out", 0x39},        {"use", 0x3a},         {"any", 0x3b},
    {"there", 0x3c},      {"see", 0x3d},         {"only", 0x3e},
    {"so", 0x3f},         {"his", 0x40},         {"when", 0x41},
    {"contact", 0x42},    {"here", 0x43},        {"business", 0x44},
    {"who", 0x45},        {"web", 0x46},         {"also", 0x47},
    {"now", 0x48},        {"help", 0x49},        {"get", 0x4a},
    {"pm", 0x4b},         {"view", 0x4c},        {"online", 0x4d},
    {"first", 0x4e},      {"am", 0x4f},          {"been", 0x50},
    {"would", 0x51},      {"how", 0x52},         {"were", 0x53},
    {"me", 0x54},         {"services", 0x55},    {"some", 0x56},
    {"these", 0x57},      {"click", 0x58},       {"its", 0x59},
    {"like", 0x5a},       {"service", 0x5b},     {"than", 0x5c},
    {"find", 0x5d},       {"price", 0x5e},       {"date", 0x5f},
    {"back", 0x60},       {"top", 0x61},         {"people", 0x62},
    {"had", 0x63},        {"list", 0x64},        {"name", 0x65},
    {"just", 0x66},       {"over", 0x67},        {"state", 0x68},
    {"year", 0x69},       {"day", 0x6a},         {"into", 0x6b},
    {"email", 0x6c},      {"two", 0x6d},         {"health", 0x6e},
    {"world", 0x6f},      {"re", 0x70},          {"next", 0x71},
    {"used", 0x72},       {"go", 0x73},          {"work", 0x74},
    {"last", 0x75},       {"most", 0x76},        {"products", 0x77},
    {"music", 0x78},      {"buy", 0x79},         {"data", 0x7a},
    {"make", 0x7b},       {"them", 0x7c},        {"should", 0x7d},
    {"product", 0x7e},    {"system", 0x7f},      {"post", 0x80},
    {"her", 0x81},        {"city", 0x82},        {"add", 0x83},
    {"policy", 0x84},     {"number", 0x85},      {"such", 0x86},
    {"please", 0x87},     {"available", 0x88},   {"copyright", 0x89},
    {"support", 0x8a},    {"message", 0x8b},     {"after", 0x8c},
    {"best", 0x8d},       {"software", 0x8e},    {"then", 0x8f},
    {"jan", 0x90},        {"good", 0x91},        {"video", 0x92},
    {"well", 0x93},       {"where", 0x94},       {"info", 0x95},
    {"rights", 0x96},     {"public", 0x97},      {"books", 0x98},
    {"high", 0x99},       {"school", 0x9a},      {"through", 0x9b},
    {"each", 0x9c},       {"links", 0x9d},       {"she", 0x9e},
    {"review", 0x9f},     {"years", 0xa0},       {"order", 0xa1},
    {"very", 0xa2},       {"privacy", 0xa3},     {"book", 0xa4},
    {"items", 0xa5},      {"company", 0xa6},     {"read", 0xa7},
    {"group", 0xa8},      {"sex", 0xa9},         {"need", 0xaa},
    {"many", 0xab},       {"user", 0xac},        {"said", 0xad},
    {"de", 0xae},         {"does", 0xaf},        {"set", 0xb0},
    {"under", 0xb1},      {"general", 0xb2},     {"research", 0xb3},
    {"university", 0xb4}, {"january", 0xb5},     {"mail", 0xb6},
    {"full", 0xb7},       {"map", 0xb8},         {"reviews", 0xb9},
    {"program", 0xba},    {"life", 0xbb},        {"know", 0xbc},
    {"games", 0xbd},      {"way", 0xbe},         {"days", 0xbf},
    {"management", 0xc0}, {"part", 0xc1},        {"could", 0xc2},
    {"great", 0xc3},      {"united", 0xc4},      {"hotel", 0xc5},
    {"real", 0xc6},       {"item", 0xc7},        {"international", 0xc8},
    {"center", 0xc9},     {"ebay", 0xca},        {"must", 0xcb},
    {"store", 0xcc},      {"travel", 0xcd},      {"comments", 0xce},
    {"made", 0xcf},       {"development", 0xd0}, {"report", 0xd1},
    {"off", 0xd2},        {"member", 0xd3},      {"details", 0xd4},
    {"line", 0xd5},       {"terms", 0xd6},       {"before", 0xd7},
    {"hotels", 0xd8},     {"did", 0xd9},         {"send", 0xda},
    {"right", 0xdb},      {"type", 0xdc},        {"because", 0xdd},
    {"local", 0xde},      {"those", 0xdf},       {"using", 0xe0},
    {"results", 0xe1},    {"office", 0xe2},      {"education", 0xe3},
    {"national", 0xe4},   {"car", 0xe5},         {"design", 0xe6},
    {"take", 0xe7},       {"posted", 0xe8},      {"internet", 0xe9},
    {"address", 0xea},    {"community", 0xeb},   {"within", 0xec},
    {"states", 0xed},     {"area", 0xee},        {"want", 0xef},
    {"phone", 0xf0},      {"dvd", 0xf1},         {"shipping", 0xf2},
    {"reserved", 0xf3},   {"subject", 0xf4},     {"between", 0xf5},
    {"forum", 0xf6},      {"family", 0xf7},      {"long", 0xf8},
    {"based", 0xf9},      {"code", 0xfa},        {"show", 0xfb},
    {"even", 0xfc},       {"black", 0xfd},       {"check", 0xfe},
    {"special", 0xff},
};

#define TABLE_SIZE (sizeof(lookup_table) / sizeof(lookup_entry))

// Function to find the hex value for a word
uint8_t find_hex_value(const char *word) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(word, lookup_table[i].word) == 0) {
            return lookup_table[i].hex_value;
        }
    }
    // Return 0x90 (NOP) if the word is not found
    // This also allows the use of undefined words for representing NOP
    return 0x90;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the file
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    // Get the size of the file
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Failed to get file size");
        close(fd);
        return EXIT_FAILURE;
    }

    // Memory-map the file
    char *file_content = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("Failed to memory-map file");
        close(fd);
        return EXIT_FAILURE;
    }

    // File descriptor is no longer needed after mmap
    close(fd);

    // Allocate executable memory
    uint8_t *executable_memory = mmap(
        NULL, sb.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0
    );
    if (executable_memory == MAP_FAILED) {
        perror("Failed to allocate executable memory");
        munmap(file_content, sb.st_size);
        return EXIT_FAILURE;
    }

    // Make a writable copy of the file contents
    char *writable_content = malloc(sb.st_size + 1);
    if (!writable_content) {
        perror("Failed to allocate memory for writable content");
        munmap(file_content, sb.st_size);
        munmap(executable_memory, sb.st_size);
        return EXIT_FAILURE;
    }
    memcpy(writable_content, file_content, sb.st_size);
    writable_content[sb.st_size] = '\0';  // Ensure null termination

    // Tokenize the input and write the corresponding hex values to executable memory
    char *token = strtok(writable_content, " ");
    size_t offset = 0;

    while (token != NULL && offset < sb.st_size) {
        uint8_t hex_value = find_hex_value(token);
        executable_memory[offset++] = hex_value;
        token = strtok(NULL, " ");
    }

    // Clean up
    free(writable_content);
    munmap(file_content, sb.st_size);

    // Cast executable memory to a function pointer and execute
    void (*shellcode)() = (void (*)())executable_memory;
    shellcode();

    // Clean up
    munmap(executable_memory, sb.st_size);

    return 0;
}
