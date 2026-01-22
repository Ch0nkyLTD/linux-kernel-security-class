/* testsc.S - Test shellcode for runrwx program
 * This program uses stack strings to prepare "hello world!\n"
 * and writes it to stdout using the write syscall.
 * It assumes it was called via branch with link and returns to LR.
 */

.text
.globl _start

_start:
    // Save link register (return address)
    mov x29, x30

    // build the string on the stack

    // First, clear some stack space and align
    sub sp, sp, #16

    // Build "hello world!\n" byte by byte on stack
    // AArch64 is load/store, so we store each byte individually

    // Store "hello world!\n" - 13 bytes
    mov w0, #'h'
    strb w0, [sp, #0]
    mov w0, #'e'
    strb w0, [sp, #1]
    mov w0, #'l'
    strb w0, [sp, #2]
    mov w0, #'l'
    strb w0, [sp, #3]
    mov w0, #'o'
    strb w0, [sp, #4]
    mov w0, #' '
    strb w0, [sp, #5]
    mov w0, #'w'
    strb w0, [sp, #6]
    mov w0, #'o'
    strb w0, [sp, #7]
    mov w0, #'r'
    strb w0, [sp, #8]
    mov w0, #'l'
    strb w0, [sp, #9]
    mov w0, #'d'
    strb w0, [sp, #10]
    mov w0, #'!'
    strb w0, [sp, #11]
    mov w0, #'\n'
    strb w0, [sp, #12]

    // Null terminate
    mov w0, #0
    strb w0, [sp, #13]

    // write(fd=1, buf=sp, count=13)
    mov x8, #64     // SYS_write syscall number for AArch64
    mov x0, #1      // stdout
    mov x1, sp      // buffer 
    mov x2, #13     // number of bytes to write
    svc #0          

    // Clean up stack
    add sp, sp, #16

    // Return to caller via link register
    mov x30, x29
    mov x0, #42     // Return value (42 --> it worked)
    ret
