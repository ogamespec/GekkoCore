#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GekkoCoreUnitTest
{
	TEST_CLASS(GekkoCoreUnitTest)
	{
	public:
		
		TEST_METHOD_INITIALIZE(InitBoard)
		{
			HWInit();
		}

		TEST_METHOD_CLEANUP(ShutdownBoard)
		{
			HWShutdown();
		}

		TEST_METHOD(TestMethod1)
		{
			Gekko::GekkoCore* core = new Gekko::GekkoCore();
			delete core;
		}
	};
}

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
