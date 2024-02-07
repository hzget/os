
void hello() {
    unsigned int magic = 0x1234ABCD;
    __asm__ volatile("mov %0, %%eax" ::"r"(magic));
    //    __asm__ volatile("hlt;");  -- it cannot run in user mode
    __asm__ volatile("int $0x80");
    asm volatile("xchgw %bx, %bx");
}
int main(void) {
    hello();
    return 1;
}
