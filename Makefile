build:
	rm -rf ./bin/
	mkdir ./bin/
	g++ ./main.cpp ./IOUtil.cpp ./builtins.cpp -Wall -o bin/shell
	chmod +x ./bin/shell

run:
	./bin/shell

clean:
	rm -rf ./bin/
