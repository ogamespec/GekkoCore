# AsmTests

Testing the execution of various Gekko instructions.

The testing environment is a stripped-down version of BootROM (boot.s), which is mapped to address 0xfff0_0000 (like on the real system).

Boot makes the minimum required CPU setup and goes to the unit test entry point (0x8130_0000), which corresponds to the GameCube IPL (BS2).

At address 0x8000_0100 there is a "canary" that contains the result of the self-test (0x55aa - good, 0xaa55 - bad).

The execution of the unit test is interrupted by the `sc` instruction, after which the host checks the canary in memory and outputs the result in the test framework.

The unit tests loaded at address 0x8130_0000 are always different.

---

Тестирование выполнения различных инструкций Gekko.

Среда тестирования представляет собой урезанную версию BootROM (boot.s), который отображается на адрес 0xfff0_0000 (как на реальной системе).

Boot производит минимальную необходимую настройку процессора и переходит на точку входа юнит-теста (0x8130_0000), которая соответствует GameCube IPL (BS2).

По адресу 0x8000_0100 находится "канарейка", в которой содержится результат самотестирования (0x55aa - хорошо, 0xaa55 - плохо).

Выполнение юнит-теста прерывается инструкцией `sc`, после чего хост проверяет канарейку в памяти и выводит результат в тестовом фреймворке.

Юнит-тесты загружаемые по адресу 0x8130_0000 всегда разные.
