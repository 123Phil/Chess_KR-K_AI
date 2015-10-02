all: main main_find main_test

main: main.cpp play.cpp play.h heuristic.cpp heuristic.h move.cpp move.h helper.cpp helper.h
	g++ -W -Wall -O3 main.cpp play.cpp heuristic.cpp move.cpp helper.cpp -o main

main_find: main_find_init.cpp play.cpp play.h heuristic.cpp heuristic.h move.cpp move.h helper.cpp helper.h
	g++ -W -Wall -O3 main_find_init.cpp play.cpp heuristic.cpp move.cpp helper.cpp -o main_find
	
main_test:  main_run_test.cpp play.cpp play.h heuristic.cpp heuristic.h move.cpp move.h helper.cpp helper.h
	g++ -W -Wall -O3 main_run_test.cpp play.cpp heuristic.cpp move.cpp helper.cpp -o main_test

clean:
	rm -v main main_find main_test
