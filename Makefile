wyliczator: wyliczator.cpp
	clang++ -O3 -g -Wall -Wextra -pedantic -ansi \
			wyliczator.cpp -o wyliczator

clean:
	rm -f wyliczator
