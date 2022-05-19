ciw: ciw.o Expression.o Lexer.o Parser.o Statement.o SymbolTable.o
	g++ -Wall -Wextra -Wpedantic -s -O2 -o ciw.exe ciw.o Expression.o Lexer.o Parser.o Statement.o SymbolTable.o

clean:
	rm *.o ciw.exe

ciw.o: ciw.cpp
	g++ -Wall -Wextra -Wpedantic -O2 -c ciw.cpp

Expression.o: Expression.cpp
	g++ -Wall -Wextra -Wpedantic -O2 -c Expression.cpp

Lexer.o: Lexer.cpp
	g++ -Wall -Wextra -Wpedantic -O2 -c Lexer.cpp

Parser.o: Parser.cpp
	g++ -Wall -Wextra -Wpedantic -O2 -c Parser.cpp

Statement.o: Statement.cpp
	g++ -Wall -Wextra -Wpedantic -O2 -c Statement.cpp

SymbolTable.o: SymbolTable.cpp
	g++ -Wall -Wextra -Wpedantic -O2 -c SymbolTable.cpp
