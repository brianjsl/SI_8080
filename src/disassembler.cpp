#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <format>
#include <stdexcept>
#include <fstream>
using std::vector;
using std::cout;
using std::format;
using std::endl;
using std::setw;
using std::left;
using std::string;
using std::ostream;
using std::runtime_error;
using std::ifstream;
using std::streampos;

class FileOpenError : public std::exception {
public:
	const char* what() const throw () {
		return "Error Opening File";
	}
};

class FileReadError : public std::exception {
public:
	const char* what() const throw () {
		return "Error Reading File";
	}
};

ostream& FormatCode(unsigned char* code, int codeLen) {
	string instructions;
	switch (codeLen) {
	case 1:
		instructions = format("{:02x}", code[0]);
		break;
	case 2:
		instructions = format("{:02x} {:02x}", code[0], code[1]);
		break;
	case 3:
		instructions = format("{:02x} {:02x} {:02x}", code[0], code[1], code[2]);
		break;
	default:
		throw runtime_error("Invalid code length");
	}
	return cout << left << setw(10) << instructions;
}

void MOV(unsigned char *code, char c1, char c2) {
	FormatCode(code, 1) << setw(8) << "MOV" << format("{},{}", c1, c2);
}

//Accumulator operations
void AccOp(unsigned char* code, string op, char reg) {
	FormatCode(code, 1) << setw(8) << op << format("{}", reg);
}

int Disassembler8080Op(unsigned char* codeBuffer, int pc) {
	int opBytes = 1;
	auto code = &(codeBuffer[pc]);

	cout << format("{:04x} ", pc);

	switch (*code) {
	case 0x00:
		FormatCode(code, 1) << "NOP";
		break;
	case 0x01:
		//B <- code[2], C <- code[1]
		FormatCode(code, 3) << setw(8) << "LXI" << format("B,#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0x02:
		//(BC) <- A
		FormatCode(code, 1) << setw(8) << "STAX" << "B";
		break;
	case 0x03:
		//(BC) <- (BC) + 1
		FormatCode(code, 1) << setw(8) << "INX" << "B";
		break;
	case 0x04:
		//B <- (B+1)
		FormatCode(code, 1) << setw(8) << "INR" << "B";
		break;
	case 0x05:
		//B <- (B-1)
		FormatCode(code, 1) << setw(8) << "DCR" << "B";
		break;
	case 0x06:
		//B <- byte 2
		FormatCode(code, 2) << setw(8) << "MVI" << format("B,#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0x07:
		//A <- A << 1, bit0 =	prev bit 7, CY = prev bit 7
		FormatCode(code, 1) << setw(8) << "RLC";
		break;
	case 0x08:
		break;
	case 0x09:
		//HL <- (HL) + (BC)
		FormatCode(code, 1) << setw(8) << "DAD" << "B";
		break;
	case 0x0a:
		//A <- (BC)
		FormatCode(code, 1) << setw(8) << "LDAX" << "B";
		break;
	case 0x0b:
		//(BC) <- (BC-1)
		FormatCode(code, 1) << setw(8) << "DCX" << "B";
		break;
	case 0x0c:
		//C <- (C+1)
		FormatCode(code, 1) << setw(8) << "INR" << "C";
		break;
	case 0x0d:
		//C <- (C-1)
		FormatCode(code, 1) << setw(8) << "DCR" << "C";
		break;
	case 0x0e:
		//C <- byte 2
		FormatCode(code, 2) << setw(8) << format("C,#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0x0f:
		//A <- A>>1, bit7 = CY = prev bit 0
		FormatCode(code, 1) << setw(8) << "RRC";
		break;
	case 0x10:
		break;
	case 0x11:
		//D <- byte 3, E <- Byte 2
		FormatCode(code, 3) << setw(8) << "LXI" << format("D,#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0x12:
		//DE <- A
		FormatCode(code, 1) << setw(1) << "STAX" << "D";
		break;
	case 0x13:
		//DE <- (DE+1)
		FormatCode(code, 1) << setw(8) << "INC" << "D";
		break;
	case 0x14:
		//D <- (D+1)
		FormatCode(code, 1) << setw(8) << "INR" << "D";
		break;
	case 0x15:
		//D <- (D-1)
		FormatCode(code, 1) << setw(8) << "DCR" << "D";
		break;
	case 0x16:
		//D<- byte 2
		FormatCode(code, 2) << setw(8) << "MVI" << format("D,#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0x17:
		//A <- A<<1, bit0 = CY = prev bit 7
		FormatCode(code, 1) << setw(8) << "RAL";
		break;
	case 0x18:
		break;
	case 0x19:
		//(HL) <= (HL) + (DE)
		FormatCode(code, 1) << setw(8) << "DAD" << "D";
		break;
	case 0x1a:
		//A <- (DE)
		FormatCode(code, 1) << setw(8) << "LDAX" << "D";
		break;
	case 0x1b:
		//(DE) <- (DE-1)
		FormatCode(code, 1) << setw(8) << "DCX" << "D";
		break;
	case 0x1c:
		//E <- (E+1)
		FormatCode(code, 1) << setw(8) << "INR" << "E";
		break;
	case 0x1d:
		//E <- (E-1)
		FormatCode(code, 1) << setw(8) << "DCR" << "E";
		break;
	case 0x1e:
		//E <- byte 2
		FormatCode(code, 2) << setw(8) << "MVI" << format("E, #${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0x1f:
		//A = A>>1, bit 7 = CY = prev bit 0
		FormatCode(code, 1) << setw(8) << "RAR";
		break;
	case 0x20:
		break;
	case 0x21:
		//H <- byte 3, L <- byte 2
		FormatCode(code, 3) << setw(8) << "LXI" << format("H,#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0x22:
		//(adr) <- L, (adr) <- H
		FormatCode(code, 3) << setw(8) << "SHLD" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0x23:
		//(HL) <- (HL+1)
		FormatCode(code, 1) << setw(8) << "INX" << "H";
		break;
	case 0x24:
		//H <- (H+1)
		FormatCode(code, 1) << setw(8) << "INR" << "H";
		break;
	case 0x25:
		//H <- (H-1)
		FormatCode(code, 1) << setw(8) << "DCR" << "H";
		break;
	case 0x26:
		//H <- byte 2
		FormatCode(code, 2) << setw(8) << "MVI" << format("H,#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0x27:
		//decimal adjusted accumulator
		FormatCode(code, 1) << setw(8) << "DAA";
		break;
	case 0x28:
		break;
	case 0x29:
		//(HL) <- (HL) + (HI)
		FormatCode(code, 1) << setw(8) << "DAD" << "H";
		break;
	case 0x2a:
		//L <- (adr), H <- (adr + 1)
		FormatCode(code, 3) << setw(8) << "LHLD" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0x2b:
		//(HL) <- (HL-1)
		FormatCode(code, 1) << setw(8) << "DCX" << "H";
		break;
	case 0x2c:
		//L <- (L+1)
		FormatCode(code, 1) << setw(8) << "INR" << "L";
		break;
	case 0x2d:
		//L <- (L-1)
		FormatCode(code, 1) << setw(8) << "DCR" << "L";
		break;
	case 0x2e:
		FormatCode(code, 2) << setw(8) << "MVI" << format("L,#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0x2f:
		//A <- !(A)
		FormatCode(code, 1) << setw(8) << "CMA";
		break;
	case 0x30:
		break;
	case 0x31:
		//SP.high <- byte 3 , SP.low <- byte 2
		FormatCode(code, 3) << setw(8) << "LXI" << format("SP,#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0x32:
		//(adr) <- (A)
		FormatCode(code, 3) << setw(8) << "STA" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0x33:
		//(SP) <- (SP+1)
		FormatCode(code, 1) << setw(8) << "INX" << "SP";
		break;
	case 0x34:
		//(HL) <-(HL+1)
		FormatCode(code, 1) << setw(8) << "INR" << "M";
		break;
	case 0x35:
		//(HL) <-(HL-1)
		FormatCode(code, 1) << setw(8) << "DCR" << "M";
		break;
	case 0x36:
		//(HL) <- byte 2
		FormatCode(code, 2) << setw(8) << "MVI" << format("M,#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0x37:
		//CY = 1
		FormatCode(code, 1) << setw(8) << "STC";
		break;
	case 0x38:
		break;
	case 0x39:
		//(HL) <- (HL+SP)
		FormatCode(code, 1) << setw(8) << "DAD" << "SP";
		break;
	case 0x3a:
		//A<- (adr)
		FormatCode(code, 3) << setw(8) << "LDA" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0x3b:
		//SP <- (SP-1)
		FormatCode(code, 1) << setw(8) << "DCX" << "SP";
		break;
	case 0x3c:
		//A <- (A+1)
		FormatCode(code, 1) << setw(8) << "INR" << "A";
		break;
	case 0x3d:
		//A <- (A-1)
		FormatCode(code, 1) << setw(8) << "DCR" << "A";
		break;
	case 0x3e:
		//A<- byte 2
		FormatCode(code, 2) << setw(8) << "MVI" << format("A,#${:02x}", code[1]);
		opBytes = 2;
	case 0x3f:
		//CY=!(CY) reset carry bit
		FormatCode(code, 1) << setw(8) << "CMC";
		break;
	case 0x40:
		MOV(code, 'B','B');
		break;
	case 0x41:
		MOV(code, 'B', 'C');
		break;
	case 0x42:
		MOV(code, 'B', 'D');
		break;
	case 0x43:
		MOV(code, 'B', 'E');
		break;
	case 0x44:
		MOV(code, 'B', 'H');
		break;
	case 0x45:
		MOV(code, 'B', 'L');
		break;
	case 0x46:
		MOV(code, 'B', 'M');
		break;
	case 0x47:
		MOV(code, 'B', 'A');
		break;
	case 0x48:
		MOV(code, 'C', 'B');
		break;
	case 0x49:
		MOV(code, 'C', 'C');
		break;
	case 0x4a:
		MOV(code, 'C', 'D');
		break;
	case 0x4b:
		MOV(code, 'C', 'E');
		break;
	case 0x4c:
		MOV(code, 'C', 'H');
		break;
	case 0x4d:
		MOV(code, 'C', 'L');
		break;
	case 0x4e:
		MOV(code, 'C', 'M');
		break;
	case 0x4f:
		MOV(code, 'C', 'A');
		break;
	case 0x50:
		MOV(code, 'D', 'B');
		break;
	case 0x51:
		MOV(code, 'D', 'C');
		break;
	case 0x52:
		MOV(code, 'D', 'D');
		break;
	case 0x53:
		MOV(code, 'D', 'E');
		break;
	case 0x54:
		MOV(code, 'D', 'H');
		break;
	case 0x55:
		MOV(code, 'D', 'L');
		break;
	case 0x56:
		MOV(code, 'D', 'M');
		break;
	case 0x57:
		MOV(code, 'D', 'A');
		break;
	case 0x58:
		MOV(code, 'E', 'B');
		break;
	case 0x59:
		MOV(code, 'E', 'C');
		break;
	case 0x5a:
		MOV(code, 'E', 'D');
		break;
	case 0x5b:
		MOV(code, 'E', 'E');
		break;
	case 0x5c:
		MOV(code, 'E', 'H');
		break;
	case 0x5d:
		MOV(code, 'E', 'L');
		break;
	case 0x5e:
		MOV(code, 'E', 'M');
		break;
	case 0x5f:
		MOV(code, 'E', 'A');
		break;
	case 0x60:
		MOV(code, 'H', 'B');
		break;
	case 0x61:
		MOV(code, 'H', 'C');
		break;
	case 0x62:
		MOV(code, 'H', 'D');
		break;
	case 0x63:
		MOV(code, 'H', 'E');
		break;
	case 0x64:
		MOV(code, 'H', 'H');
		break;
	case 0x65:
		MOV(code, 'H', 'L');
		break;
	case 0x66:
		MOV(code, 'H', 'M');
		break;
	case 0x67:
		MOV(code, 'H', 'A');
		break;
	case 0x68:
		MOV(code, 'L', 'B');
		break;
	case 0x69:
		MOV(code, 'L', 'C');
		break;
	case 0x6a:
		MOV(code, 'L', 'D');
		break;
	case 0x6b:
		MOV(code, 'L', 'E');
		break;
	case 0x6c:
		MOV(code, 'L', 'H');
		break;
	case 0x6d:
		MOV(code, 'L', 'L');
		break;
	case 0x6e:
		MOV(code, 'L', 'M');
		break;
	case 0x70:
		MOV(code, 'M', 'B');
		break;
	case 0x71:
		MOV(code, 'M', 'C');
		break;
	case 0x72:
		MOV(code, 'M', 'D');
		break;
	case 0x73:
		MOV(code, 'M', 'E');
		break;
	case 0x74:
		MOV(code, 'M', 'H');
		break;
	case 0x75:
		MOV(code, 'M', 'L');
		break;
	case 0x76:
		//Halt (CPU Stop)
		FormatCode(code, 1) << setw(8) << "HLT";
		break;
	case 0x77:
		MOV(code, 'M', 'A');
		break;
	case 0x78:
		MOV(code, 'A', 'B');
		break;
	case 0x79:
		MOV(code, 'A', 'C');
		break;
	case 0x7a:
		MOV(code, 'A', 'D');
		break;
	case 0x7b:
		MOV(code, 'A', 'E');
		break;
	case 0x7c:
		MOV(code, 'A', 'H');
		break;
	case 0x7d:
		MOV(code, 'A', 'L');
		break;
	case 0x7e:
		MOV(code, 'A', 'M');
		break;
	case 0x7f:
		MOV(code, 'A', 'A');
		break;
	case 0x80:
		AccOp(code, "ADD", 'B');
		break;
	case 0x81:
		AccOp(code, "ADD", 'C');
		break;
	case 0x82:
		AccOp(code, "ADD", 'D');
		break;
	case 0x83:
		AccOp(code, "ADD", 'E');
		break;
	case 0x84:
		AccOp(code, "ADD", 'H');
		break;
	case 0x85:
		AccOp(code, "ADD", 'L');
		break;
	case 0x86:
		AccOp(code, "ADD", 'M');
		break;
	case 0x87:
		AccOp(code, "ADD", 'A');
		break;
	case 0x88:
		//Add carry
		AccOp(code, "ADC", 'B');
		break;
	case 0x89:
		AccOp(code, "ADC", 'C');
		break;
	case 0x8a:
		AccOp(code, "ADC", 'D');
		break;
	case 0x8b:
		AccOp(code, "ADC", 'E');
		break;
	case 0x8c:
		AccOp(code, "ADC", 'H');
		break;
	case 0x8d:
		AccOp(code, "ADC", 'L');
		break;
	case 0x8e:
		AccOp(code, "ADC", 'M');
		break;
	case 0x8f:
		AccOp(code, "ADC", 'A');
		break;
	case 0x90:
		AccOp(code, "SUB", 'B');
		break;
	case 0x91:
		AccOp(code, "SUB", 'C');
		break;
	case 0x92:
		AccOp(code, "SUB", 'D');
		break;
	case 0x93:
		AccOp(code, "SUB", 'E');
		break;
	case 0x94:
		AccOp(code, "SUB", 'H');
		break;
	case 0x95:
		AccOp(code, "SUB", 'L');
		break;
	case 0x96:
		AccOp(code, "SUB", 'M');
		break;
	case 0x97:
		AccOp(code, "SUB", 'A');
		break;
	case 0x98:
		//sub with carry
		AccOp(code, "SBB", 'B');
		break;
	case 0x99:
		AccOp(code, "SBB", 'C');
		break;
	case 0x9a:
		AccOp(code, "SBB", 'D');
		break;
	case 0x9b:
		AccOp(code, "SBB", 'E');
		break;
	case 0x9c:
		AccOp(code, "SBB", 'H');
		break;
	case 0x9d:
		AccOp(code, "SBB", 'L');
		break;
	case 0x9e:
		AccOp(code, "SBB", 'M');
		break;
	case 0x9f:
		AccOp(code, "SBB", 'A');
		break;
	case 0xa0:
		//and operator
		AccOp(code, "ANA", 'B');
		break;
	case 0xa1:
		AccOp(code, "ANA", 'C');
		break;
	case 0xa2:
		AccOp(code, "ANA", 'D');
		break;
	case 0xa3:
		AccOp(code, "ANA", 'E');
		break;
	case 0xa4:
		AccOp(code, "ANA", 'H');
		break;
	case 0xa5:
		AccOp(code, "ANA", 'L');
		break;
	case 0xa6:
		AccOp(code, "ANA", 'M');
		break;
	case 0xa7:
		AccOp(code, "ANA", 'A');
		break;
	case 0xa8:
		//xor
		AccOp(code, "XRA", 'B');
		break;
	case 0xa9:
		AccOp(code, "XRA", 'C');
		break;
	case 0xaa:
		AccOp(code, "XRA", 'D');
		break;
	case 0xab:
		AccOp(code, "XRA", 'E');
		break;
	case 0xac:
		AccOp(code, "XRA", 'H');
		break;
	case 0xad:
		AccOp(code, "XRA", 'L');
		break;
	case 0xae:
		AccOp(code, "XRA", 'M');
		break;
	case 0xaf:
		AccOp(code, "XRA", 'A');
		break;
	case 0xb0:
		//or 
		AccOp(code, "ORA", 'B');
		break;
	case 0xb1:
		AccOp(code, "ORA", 'C');
		break;
	case 0xb2:
		AccOp(code, "ORA", 'D');
		break;
	case 0xb3:
		AccOp(code, "ORA", 'E');
		break;
	case 0xb4:
		AccOp(code, "ORA", 'H');
		break;
	case 0xb5:
		AccOp(code, "ORA", 'L');
		break;
	case 0xb6:
		AccOp(code, "ORA", 'M');
		break;
	case 0xb7:
		AccOp(code, "ORA", 'A');
		break;
	case 0xb8:
		//cmp (A-B)
		AccOp(code, "CMP", 'B');
		break;
	case 0xb9:
		AccOp(code, "CMP", 'C');
		break;
	case 0xba:
		AccOp(code, "CMP", 'D');
		break;
	case 0xbb:
		AccOp(code, "CMP", 'E');
		break;
	case 0xbc:
		AccOp(code, "CMP", 'H');
		break;
	case 0xbd:
		AccOp(code, "CMP", 'L');
		break;
	case 0xbe:
		AccOp(code, "CMP", 'M');
		break;
	case 0xbf:
		AccOp(code, "CMP", 'A');
		break;
	case 0xc0:
		//if NZ, RET
		FormatCode(code, 1) << setw(8) << "RNZ";
		break;
	case 0xc1:
		//C <- (sp), B <- (sp + 1), sp <- (sp + 2)
		FormatCode(code, 1) << setw(8) << "POP" << "B";
		break;
	case 0xc2:
		//if NZ, CALL adr
		FormatCode(code, 3) << setw(8) << "JNZ" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xc3:
		//PC <- (addr)
		FormatCode(code, 3) << setw(8) << "JMP" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xc4:
		//If NZ CALL addr
		FormatCode(code, 3) << setw(8) << "CNZ" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xc5:
		//(sp-2) <- C; (sp-1) <- B; sp <- sp-2
		FormatCode(code, 1) << setw(8) << "PUSH" << "B";
		break;
	case 0xc6:
		// A <- (A + byte 2)
		FormatCode(code, 2) << setw(8) << "ADI" << format("#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0xc7:
		// CALL $0
		FormatCode(code, 1) << setw(8) << "RST" << 0;
		break;
	case 0xc8:
		// if Z, RET
		FormatCode(code, 1) << setw(8) << "RZ";
		break;
	case 0xc9:
		// 	PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2
		FormatCode(code, 1) << setw(8) << "RET";
		break;
	case 0xca:
		// IF Z, PC <- addr
		FormatCode(code, 3) << setw(8) << "JZ" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xcb:
		break;
	case 0xcc:
		// If Z, CALL (addr)
		FormatCode(code, 3) << setw(8) << "CZ" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xcd:
		// (SP-1)<-PC.hi;(SP-2)<-PC.lo;SP<-SP-2;PC=adr
		FormatCode(code, 3) << setw(8) << "CALL" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xce:
		// A <- A + data + CY
		FormatCode(code, 2) << setw(8) << "ACI" << format("#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0xcf:
		// CALL $8
		FormatCode(code, 1) << setw(8) << "RST" << 1;
		break;
	case 0xd0:
		// IF NCY, RET
		FormatCode(code, 1) << setw(8) << "RNC";
		break;
	case 0xd1:
		// E <- (sp); D <- (sp+1); sp <- sp+2
		FormatCode(code, 1) << setw(8) << "POP" << "D";
		break;
	case 0xd2:
		// if NCY, PC<-adr
		FormatCode(code, 3) << setw(8) << "JNC" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xd3:
		// IO OUT
		FormatCode(code, 2) << setw(8) << "OUT" << format("#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0xd4:
		//If NCY, CALL adr
		FormatCode(code, 3) << setw(8) << "CNC" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xd5:
		// (sp-2)<-E; (sp-1)<-D; sp <- sp - 2
		FormatCode(code, 1) << setw(8) << "PUSH" << "D";
		break;
	case 0xd6:
		// A <- A - byte 2
		FormatCode(code, 2) << setw(8) << "SUI" << format("#${:02x}", code[1]);
		break;
	case 0xd7:
		// CALL $10
		FormatCode(code, 1) << setw(8) << "RST" << "2";
		break;
	case 0xd8:
		// if CY, RET
		FormatCode(code, 1) << setw(8) << "RC";
		break;
	case 0xd9:
		break;
	case 0xda:
		// if CY, PC < -adr
		FormatCode(code, 3) << setw(8) << "JC" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xdb:
		//Read from Input 
		FormatCode(code, 2) << setw(8) << "IN" << format("#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0xdc:
		//If CY, CALL adr (conditional call)
		FormatCode(code, 3) << setw(8) << "CC" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xdd:
		break;
	case 0xde:
		FormatCode(code, 2) << setw(8) << "SBI" << format("#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0xdf:
		// CALL $18
		FormatCode(code, 1) << setw(8) << "RST" << "3";
		break;
	case 0xe0:
		// if PO, RET
		FormatCode(code, 1) << setw(8) << "RPO";
		break;
	case 0xe1:
		// L <- (sp); H <- (sp+1); sp <- sp+2
		FormatCode(code, 1) << setw(8) << "POP" << "H";
		break;
	case 0xe2:
		//if PO, PC <- adr
		FormatCode(code, 3) << setw(8) << "JPO" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xe3:
		//L <-> (SP); H <-> (SP+1)
		FormatCode(code, 1) << setw(8) << "XTHL";
		break;
	case 0xe4:
		// if PO, CALL adr
		FormatCode(code, 3) << setw(8) << "CPO" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xe5:
		//(sp-2)<-L; (sp-1)<-H; sp <- sp - 2
		FormatCode(code, 1) << setw(8) << "PUSH" << "H";
		break;
	case 0xe6:
		// A <- A & data (byte 1)
		FormatCode(code, 2) << setw(8) << "ANI" << format("#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0xe7:
		// CALL $20
		FormatCode(code, 1) << setw(8) << "RST" << 4;
		break;
	case 0xe8:
		// if PE, RET
		FormatCode(code, 1) << setw(8) << "RPE";
		break;
	case 0xe9:
		// PC.hi <- H; PC.lo <- L
		FormatCode(code, 1) << setw(8) << "PCHL";
		break;
	case 0xea:
		// if PE, PC <- adr
		FormatCode(code, 3) << setw(8) << "JPE" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xeb:
		// H <-> D; L <-> E
		FormatCode(code, 1) << setw(8) << "XCHG";
		break;
	case 0xec:
		// if PE, CALL adr
		FormatCode(code, 3) << setw(8) << "CPE" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xed:
		break;
	case 0xee:
		// A <- A ^ data
		FormatCode(code, 2) << setw(8) << "XRI" << format("#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0xef:
		// CALL $28
		FormatCode(code, 1) << setw(8) << "RST" << 5;
		break;
	case 0xf0:
		// if P, RET
		FormatCode(code, 1) << setw(8) << "RP";
		break;
	case 0xf1:
		// flags <- (sp); A <- (sp+1); sp <- sp+2
		FormatCode(code, 1) << setw(8) << "POP" << "PSW";
		break;
	case 0xf2:
		// if P=1 PC <- adr
		FormatCode(code, 3) << setw(8) << "JP" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xf3:
		// Disable Interrupts
		FormatCode(code, 1) << setw(8) << "DI";
		break;
	case 0xf4:
		// if P, PC <- adr (conditional move to pc)
		FormatCode(code, 3) << setw(8) << "CP" << format("#${:02x}{:02x}", code[2], code[1]);
		break;
	case 0xf5:
		// (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2
		FormatCode(code, 1) << setw(8) << "PUSH" << "PCW";
		break;
	case 0xf6:
		// A <- A | data (byte 1)
		FormatCode(code, 2) << setw(8) << "ORI" << format("#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0xf7:
		// CALL $30
		FormatCode(code, 1) << setw(8) << "RST" << 6;
		break;
	case 0xf8:
		// if M, RET
		FormatCode(code, 1) << setw(8) << "RM";
		break;
	case 0xf9:
		// SP <= HL
		FormatCode(code, 1) << setw(8) << "SPHL";
		break;
	case 0xfa:
		// if M, PC <- adr
		FormatCode(code, 3) << setw(8) << "JM" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xfb:
		// Enable Interrupts
		FormatCode(code, 1) << setw(8) << "EI";
		break;
	case 0xfc:
		// if M, CALL adr
		FormatCode(code, 3) << setw(8) << "CM" << format("#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0xfd:
		break;
	case 0xfe:
		// A - data
		FormatCode(code, 2) << setw(8) << "CPI" << format("#${:02x}", code[1]);
		opBytes = 2;
		break;
	case 0xff:
		// CALL $38
		FormatCode(code, 1) << setw(8) << "RST" << 7;
		break;
	}
	cout << endl;
	return opBytes;
}

int main(int argc, char* argv[]) {
	ifstream file(argv[1], ifstream::binary);
	if (!file.good()) {
		throw FileOpenError();
		exit(1);
	}
	file.seekg(0, file.end);
	size_t fsize = file.tellg();
	file.seekg(0, file.beg);

	unsigned char* buffer = new unsigned char[fsize];
	file.read((char*)buffer, fsize);

	if (!file.good()) {
		throw FileReadError();
		exit(1);
	}

	file.close();

	Disassembler8080Op(buffer, 0);
	int pc = 0;

	while (pc < fsize) {
		pc += Disassembler8080Op(buffer, pc);
	}

	delete[] buffer;
	return 0;
}
