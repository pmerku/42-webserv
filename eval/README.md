make -j && cd build

for mac:
	./not-apache -c -f ../eval/provided/test_mac.conf
for windows
	./not-apache -c -f ../eval/provided/test.conf

in another terminal, cd to eval directory and run:
	./test http://localhost:8080