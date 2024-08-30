# Lexikon

A low-entropy shellcode executor achieved by encoding shellcode bytes into common English words.

When you're doing pentesting, there may come a time when you will need to execute malicious code on a target machine, where malicious executable files will be detected and blocked by NGFWs, EDRs, etc. This is where this too may come in handy. The main lexikon shellcode executor is rather simple and can be unobfuscated. This binary, by itself, does nothing harmful. The real malicious payload would be encoded into English words, and thus, appear to have very low entropy in the eyes of sandboxes and EDRs, allowing the file to pass through.

There might be more efficient ways to achieve this. I made this because it randomly came into my mind on a random Thursday afternoon. Perhaps this could be inspiration for something greater.

## Usages

1. Compile your shellcode into a binary file. An example is available at `examples/shellcode.c`. This example code can be compiled by Binary Ninja's [SCC](https://scc.binary.ninja/).
2. Use the encoder at `tools/encoder.py` to encode the shellcode bytes into a sequence of English words. Save its output to a text file. We will use `dict.txt` as an example here.
3. Build the Lexikon binary and execute the shellcode by running `lexikon dict.txt`.
