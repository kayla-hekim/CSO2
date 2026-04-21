.global check_passphrase
check_passphrase:
        incq    check_passphrase_calls(%rip)
        movq    passphrase(%rip), %rcx
        xorl    %eax, %eax
.L3:
        movb    (%rdi,%rax), %dl
        cmpb    (%rcx,%rax), %dl
        jne     .L4
        incq    %rax
        testb   %dl, %dl
        jne     .L3
        movl    $1, %eax
        ret
.L4:
        xorl    %eax, %eax
        ret
