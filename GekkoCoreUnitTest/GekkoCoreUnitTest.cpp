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
