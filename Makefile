build:
	mkdir bin -p
	gcc process_generator.c -o bin/process_generator.out
	gcc clk.c -o bin/clk.out
	gcc scheduler.c -o bin/scheduler.out
	gcc process.c -o bin/process.out
	gcc test_generator.c -o bin/test_generator.out

clean:
	rm -f bin/*.out  processes.txt

all: clean build

generate: 
	./bin/test_generator.out

runSJF:
	./bin/test_generator.out
	./bin/process_generator.out processes.txt -sch 1 -q 2

runHPF:
	./bin/test_generator.out
	./bin/process_generator.out processes.txt -sch 2 -q 2

runRR:
	./bin/test_generator.out
	./bin/process_generator.out processes.txt -sch 3 -q 2

runMLFQ:
	./bin/test_generator.out
	./bin/process_generator.out processes.txt -sch 4 -q 2

debugSJF:
	./bin/test_generator.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes bin/process_generator.out processes.txt -sch 1 -q 2 -d

debugHPF:
	./bin/test_generator.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes bin/process_generator.out processes.txt -sch 2 -q 2 -d

debugRR:
	./bin/test_generator.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes bin/process_generator.out processes.txt -sch 3 -q 2 -d

debugMLFQ:
	./bin/test_generator.out
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes bin/process_generator.out processes.txt -sch 4 -q 2 -d

runTestSJF:
	./bin/process_generator.out testcase.txt -sch 1 -q 2

runTestHPF:
	./bin/process_generator.out testcase.txt -sch 2 -q 2

runTestRR:
	./bin/process_generator.out testcase.txt -sch 3 -q 2

runTestMLFQ:
	./bin/process_generator.out testcase.txt -sch 4 -q 5

