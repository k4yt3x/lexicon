# Lexicon

A low-entropy shellcode executor that encodes shellcode bytes into common English words.

In short, many security software use the entropy of sections within binaries to determine if they contain encrypted or obfuscated code. By encoding shellcode bytes into English words, we can create a binary with very low entropy, which is less likely to be flagged by such security measures.
You can watch Will Summerhill's presentation ["Simplified Malware Development - Entropy and Other Techniques"](https://youtu.be/F6R-YPsjpVY?t=13597) to learn more.

Here's a "Hello, World!" shellcode encoded into English words:

<img width="1460" height="428" alt="image" src="https://github.com/user-attachments/assets/1508e927-9e70-4c1c-94e2-6a6e14b8c10a" />

...and this what the shellcode looks like when it's encoded and stored in the binary:

<img width="970" height="563" alt="encoded_shellcode" src="https://github.com/user-attachments/assets/221dc13f-2fe6-4ecf-af63-ee3bce9a2c2d" />

This project was originally a part of the CyberSci CTF challenge [Vector Veil](https://github.com/k4yt3x/cs2025-rgnl-vecveil). It has been removed from the challenge before the competition, but I thought it might be a useful tool for bypassing heuristic-based security measures during pentests, so I decided to release it as a standalone project.

Aside from the basic shellcode encoding/decoding functionality, this shellcode executor also features the following anti-debug/analysis techniques:

- Stores the DJB2 hash of the English words in the lookup table instead of plaintext strings.
- Detects debuggers by abusing the `SIGTRAP` signal handler.
- Obfuscates the control flow by manually overwriting the `main` function's return address with the allocated shellcode's address.
- Hides the allocation of r/w/x memory in the `_init_array`.

I might add more in the future for fun. This is what the `main` function looks like if you drag it into IDA Pro:

<img width="1342" height="702" alt="main" src="https://github.com/user-attachments/assets/55979357-b3f7-40e1-88e3-070124d553a9" />

## Usages

### Using the Built-in NASM Template

This repo contains a built-in NASM template and shellcode encoding tools that allow you to easily create a binary that's ready to be deployed. All you need to do is to write your NASM code in the `src/payload.nasm` file, then compile the project.

1. Write your own position-independent NASM shellcode program in `src/payload.nasm`.
2. Compile the project by running `make`. The shellcode will be automatically compiled, encoded, and embedded into the output `bin/lexicon` binary.

### Using Custom Shellcode

You can also prepare your own shellcode.

1. Write your shellcode in a binary file and place it at `src/payload.bin`.
2. Compile the project by running `make custom`. The shellcode will be automatically encoded and embedded into the output `bin/lexicon` binary.
