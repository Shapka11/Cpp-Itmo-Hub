# адреса в памяти для data
lui x2, 0x0
addi x2, x2, 0x100
lui x3, 0x0
addi x3, x3, 0x500

addi x6, x6, 0x400
add x4, x3, x6
add x5, x4, x6

add x7, x6, x5
 
# данные для push в memory
addi x10, x10, 0x1
addi x11, x11, 0x2
addi x12, x12, 0x3
addi x13, x13, 0x4

addi x14, x14, 0x5
addi x15, x15, 0x6
addi x16, x16, 0x7
addi x17, x17, 0x8
addi x18, x18, 0x9
addi x19, x19, 0x10
addi x20, x20, 0x11
addi x21, x21, 0x12
addi x22, x22, 0x13
addi x23, x23, 0x14
addi x24, x24, 0x1E    


add x14, x14, x10
bltu x14, x24, -4

# push в memory
sw x10, 0x0(x2)
sw x11, 0x0(x3)
sw x12, 0x0(x4)
sw x13, 0x0(x5)

sw x24 0x0(x7) # вытеснение
# 6 промахов, 10 попаданий


sw x15, 0x0(x3) # вытеснение
sw x16, 0x0(x4)
sw x17, 0x0(x5)
sw x14, 0x0(x2)
sw x18, 0x0(x2)
sw x19, 0x0(x3)
sw x20, 0x0(x4)
sw x21, 0x0(x5)
sw x22, 0x0(x2)
sw x23, 0x0(x3)
sw x24, 0x0(x4)

ecall
