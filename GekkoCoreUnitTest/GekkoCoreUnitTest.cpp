#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GekkoCoreUnitTest
{
	TEST_CLASS(GekkoCoreUnitTest)
	{
		void ExecuteUntilSyscall(Gekko::GekkoCore* core)
		{
			while (true)
			{
				core->StepVerbose();

				if (core->regs.pc == (uint32_t)Gekko::Exception::EXCEPTION_SYSTEM_CALL)
				{
					break;
				}
			}
		}

		uint32_t GetCanary()
		{
			uint32_t val;
			Gekko::SixtyBus_ReadWord(UnitTestCanaryAddress, &val);
			return val;
		}

	public:
		
		TEST_METHOD_INITIALIZE(InitBoard)
		{
			HWInit();
			SetCurrentDirectoryA("./../../../../../../");
			Assert::IsTrue(LoadElfFile("AsmTests/boot.elf"));
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
			Assert::IsTrue(LoadElfFile("AsmTests/simple.elf"));
			Gekko::GekkoCore* core = new Gekko::GekkoCore();
			ExecuteUntilSyscall(core);
			Assert::IsTrue(GetCanary() == UnitTestOK);
			delete core;
		}
	};
}
