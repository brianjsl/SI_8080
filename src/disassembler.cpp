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

int Disassembler8080Op(unsigned char* codeBuffer, int pc) {
	int opBytes = 1;
	auto code = &(codeBuffer[pc]);

	cout << format("{:04x} ", pc);

	switch (*code) {
	case 0x00:
		cout << setw(8) << left << format("{:02x}", *code) << "NOP";
		break;
	case 0x01:
		cout << setw(8) << left << format("{:02x} {:02x} {:02x} ", code[0], code[2], code[1]) << "LXI" << format("B,#{:02x}{:02x}", code[2], code[1]);
		break;
	}
	cout << endl;
	return opBytes;
}

int main(int argc, char* argv[]) {
	unsigned char buffer[3] = {0x01, 0xd3, 0x17};
	Disassembler8080Op(buffer, 0);
	return 0;
}
