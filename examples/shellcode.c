// Compile: scc --arch x64 -o shellcode shellcode.c
int main() {
    write(1, "hello\n", 6);
    return 0;
}
