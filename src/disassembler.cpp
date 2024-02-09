#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <format>
using std::vector;
using std::cout;
using std::format;
using std::endl;
using std::setw;
using std::left;
using std::string;
using std::ostream;

ostream& FormatCode(unsigned char* code, int codeLen) {
	string instructions;
	if (codeLen == 1) {
		instructions = format("{:02x}", code[0]);
	}
	else if (codeLen == 3) {
		instructions = format("{:02x} {:02x} {:02x} ", code[0], code[2], code[1]);
	}
	return cout << setw(8) << left << instructions;
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
		FormatCode(code, 3) << setw(7) <<  "LXI" << format("B,#${:02x}{:02x}", code[2], code[1]);
		opBytes = 3;
		break;
	case 0x02:
		//(BC) <- A
		FormatCode(code, 1) << "STAX" << "B";
		break;
	case 0x03:
		FormatCode(code, 1) << "INX" << "B";
		break;
	case 0x04:
		FormatCode(code, 1) << "INR" << "B";
		break;
	}
	cout << endl;
	return opBytes;
}

int main(int argc, char* argv[]) {
	unsigned char buffer[3] = {0x01, 0x18, 0x17};
	Disassembler8080Op(buffer, 0);
	return 0;
}
