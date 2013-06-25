.model flat
.code

interlocked_add PROC
	mov eax, [esp + 4]
	mov ecx, [esp + 8]
	lock add [eax], ecx
	ret
interlocked_add ENDP

END