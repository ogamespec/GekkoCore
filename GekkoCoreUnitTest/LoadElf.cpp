#include "pch.h"

void LoadElfImage(uint8_t* image, size_t image_size)
{

}

bool LoadElfFile(const char* filename)
{
	FILE* f;
	fopen_s(&f, filename, "rb");
	if (!f)
		return false;

	fseek(f, 0, SEEK_END);
	auto size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t* image = new uint8_t[size];

	fread(image, 1, size, f);
	fclose(f);

	LoadElfImage(image, size);

	return true;
}
