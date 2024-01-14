
void hello() {
    unsigned int magic = 0x1234ABCD;
    __asm__ volatile("mov %0, %%eax" ::"r"(magic));
//    __asm__ volatile("hlt;");  -- it cannot run in user mode
}
int main(void) {
    hello();
    return 1;
}
