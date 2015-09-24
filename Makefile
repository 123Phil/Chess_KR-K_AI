all: main init test

main:
	g++ -W -Wall -O3 main.cpp KRk.cpp -o KRk

init:
	g++ -W -Wall -O3 find_init.cpp KRk.cpp -o find_init

test:
	g++ -W -Wall -O3 test.cpp KRk.cpp -o run_test

clean:
	rm -v run_test find_init KRk
