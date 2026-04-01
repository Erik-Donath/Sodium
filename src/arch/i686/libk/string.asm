bits 32
section .text

global memcpy
memcpy:
    push    esi
    push    edi

    mov     edi, [esp + 12]         ; dest
    mov     esi, [esp + 16]         ; src
    mov     ecx, [esp + 20]         ; n
 
    mov     eax, edi                ; return value = dest
 
    ; dword copy
    mov     edx, ecx               ; save original n
    shr     ecx, 2                  ; ecx = n / 4
    rep     movsd                   ; copy dwords
 
    ; byte remainder
    mov     ecx, edx
    and     ecx, 3                  ; ecx = n % 4
    rep     movsb
 
    pop     edi
    pop     esi
    ret

global memset
memset:
    push    edi

    mov     edi, [esp + 8]          ; s
    movzx   eax, byte [esp + 12]    ; c  (only low byte is meaningful)
    mov     ecx, [esp + 16]         ; n
 
    mov     edx, edi                ; save s for return
 
    ; Broadcast byte to all 4 bytes of EAX:
    ;   0x000000XX * 0x01010101 = 0xXXXXXXXX  (truncated to 32 bits)
    imul    eax, eax, 0x01010101
 
    ; dword fill
    push    ecx                     ; save n
    shr     ecx, 2
    rep     stosd
 
    ; byte remainder
    pop     ecx
    and     ecx, 3
    rep     stosb
 
    mov     eax, edx                ; return s
    pop     edi
    ret

global memcmp
memcmp:
    push    esi
    push    edi

    mov     esi, [esp + 12]         ; s1
    mov     edi, [esp + 16]         ; s2
    mov     ecx, [esp + 20]         ; n
 
    xor     eax, eax                ; assume equal (return 0)
    test    ecx, ecx
    jz      .done                   ; n == 0 → equal by definition
 
    repe    cmpsb                   ; compare until mismatch or ecx == 0
    je      .done                   ; ZF set → all bytes equal
 
    ; Compute difference of the two bytes that differed.
    ; After CMPSB, ESI/EDI point one past the differing byte.
    movzx   eax, byte [esi - 1]
    movzx   edx, byte [edi - 1]
    sub     eax, edx                ; signed difference fits in 32-bit
 
.done:
    pop     edi
    pop     esi
    ret

global strlen
strlen:
    push    edi
    ; [esp+8] = s
 
    mov     edi, [esp + 8]          ; s
    xor     eax, eax                ; search for 0x00
    mov     ecx, 0xFFFFFFFF         ; scan up to 4 GiB
 
    repne   scasb                   ; decrement ECX until *EDI++ == AL
 
    not     ecx                     ; ecx = bytes scanned including '\0'
    dec     ecx                     ; subtract the null terminator itself
 
    mov     eax, ecx
    pop     edi
    ret
