    .global _start
    .text
_start:
    // x0 = argc   (ldr x0, [sp])
    ldr x0, [sp]

    // x1 = argv (pointer to argv[0]) -> sp + 8
    add x1, sp, #8

    // x2 = envp = argv + (argc + 1)
    // temp: x3 = argc
    ldr x3, [sp]       // x3 = argc
    add x3, x3, #1     // argc + 1
    lsl x3, x3, #3     // *8  (pointer size)
    add x2, x1, x3     // envp = x1 + (argc+1)*8

    // Call main(argc, argv, envp)
    bl main

    // exit(return_value_in_x0)
    mov x8, #93        // __NR_exit
    svc #0

