#include <windows.h>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("usage: ./dope_build_getter.exe path");
		getchar();
		return 1;
	}

	std::ifstream fileStream(argv[1], std::ios::ate | std::ios::binary);
	size_t size = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	char* buffer = new char[size];
	fileStream.read(buffer, size);

	std::string token(buffer + size - 16, 16);

	for (int i = 0; i < token.size(); i++)
		token[i] = token[i] ^ 0xd3adc0de;

	delete[] buffer;
	printf("%s", token.data());
	getchar();
	return 1;
}