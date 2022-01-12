#pragma once

//CLS
void op_00E0(void);
//RET
void op_00EE(void);
//JP addr
void op_1nnn(void);
//CALL addr
void op_2nnn(void);
//SE Vx, byte
void op_3xnn(void);
//SNE Vx, byte
void op_4xnn(void);
//SE Vx, Vy
void op_5xy0(void);
//LD Vx, byte
void op_6xnn(void);
//ADD Vx, byte
void op_7xnn(void);
//LD Vx, Vy
void op_8xy0(void);
//OR Vx, Vy
void op_8xy1(void);
//AND Vx, Vy
void op_8xy2(void);
//XOR Vx, Vy
void op_8xy3(void);
//ADD Vx, Vy
void op_8xy4(void);
//SUB Vx, Vy
void op_8xy5(void);
//SHR Vx {, Vy}
void op_8xy6(void);
//SUBN Vx, Vy
void op_8xy7(void);
//SHL Vx {, Vy}
void op_8xyE(void);
//SNE Vx, Vy
void op_9xy0(void);
//LD I, addr
void op_Annn(void);
//JP V0, addr
void op_Bnnn(void);
//RND Vx, byte
void op_Cxnn(void);
//DRW Vx, Vy, nibble
void op_Dxyn(void);
//SKP Vx
void op_Ex9E(void);
//SKNP Vx
void op_ExA1(void);
//LD Vx, DT
void op_Fx07(void);
//LD Vx, K
void op_Fx0A(void);
//LD DT, Vx
void op_Fx15(void);
//LD ST, Vx
void op_Fx18(void);
//ADD I, Vx
void op_Fx1E(void);
//LD F, Vx
void op_Fx29(void);
//LD B, Vx
void op_Fx33(void);
//LD [I], Vx
void op_Fx55(void);
//LD Vx, [I]
void op_Fx65(void);