.global _codigoASM
.global _retornoASM
.data
_codigoASM:
.byte 0

.text
_retornoASM:
	xor %al, %al
	inb $0x64, %al
	movb %al, _codigoASM
	ret
