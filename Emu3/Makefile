emu: ClockController.o Core.o MemoryController.o KeyBoard.o Screen.o Speed.o main.o
	x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -std=c++17 -O2 -o emu ClockController.o Core.o MemoryController.o KeyBoard.o Screen.o Speed.o main.o -luser32 -lgdi32 -lgdiplus -lopengl32 -lSHlwapi -ldwmapi -lstdc++fs -lwinmm

clean:
	rm *.o emu.exe

ClockController.o: ../Base/ClockController.cpp
	x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -std=c++17 -O2 -c ../Base/ClockController.cpp

Core.o: ../Base/Core.cpp
	x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -std=c++17 -O2 -c ../Base/Core.cpp

MemoryController.o: ../Base/MemoryController.cpp
	x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -std=c++17 -O2 -c ../Base/MemoryController.cpp

KeyBoard.o: KeyBoard.cpp
	x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -std=c++17 -O2 -c KeyBoard.cpp

Screen.o: Screen.cpp
	x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -std=c++17 -O2 -c Screen.cpp

Speed.o: Speed.cpp
	x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -std=c++17 -O2 -c Speed.cpp

main.o: main.cpp
	x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -std=c++17 -O2 -c main.cpp
