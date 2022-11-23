#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Debug
{
	void Halt(const char* text, ...)
	{
		va_list arg;
		char buf[0x1000] = { 0, };

		va_start(arg, text);
		vsprintf_s(buf, sizeof(buf), text, arg);
		va_end(arg);

		Logger::WriteMessage(buf);
		Assert::Fail();
	}

	void Report(const char* text, ...)
	{
		va_list arg;
		char buf[0x1000] = { 0, };

		va_start(arg, text);
		vsprintf_s(buf, sizeof(buf), text, arg);
		va_end(arg);

		Logger::WriteMessage(buf);
	}
}
