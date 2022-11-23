#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GekkoCoreUnitTest
{
	TEST_CLASS(GekkoCoreUnitTest)
	{
		void ExecuteUntilSyscall(Gekko::GekkoCore* core)
		{
			core->StepVerbose();
		}

	public:
		
		TEST_METHOD_INITIALIZE(InitBoard)
		{
			HWInit();
			SetCurrentDirectoryA("./../../../../../../");
			Assert::IsTrue(LoadElfFile("AsmTests/boot.o"));
		}

		TEST_METHOD_CLEANUP(ShutdownBoard)
		{
			HWShutdown();
		}

		TEST_METHOD(TestCreateInstance)
		{
			Gekko::GekkoCore* core = new Gekko::GekkoCore();
			delete core;
		}

		TEST_METHOD(TestSimple)
		{
			Assert::IsTrue(LoadElfFile("AsmTests/simple.o"));
			Gekko::GekkoCore* core = new Gekko::GekkoCore();
			ExecuteUntilSyscall(core);
			delete core;
		}
	};
}
