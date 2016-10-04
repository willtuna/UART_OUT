all: uart_exe.cpp
	@echo "947894 Kuang"
	g++ -I mavlink/ -Wall -g uart_exe.cpp -o uart_exe.out -lrt
local_uart: uart_exe.cpp
	arm-linux-gnueabihf-g++ -I mavlink/ -Wall -g uart_exe.cpp -o uart_exe.out -lrt
local_share: share_mem_test_read.cpp
	arm-linux-gnueabihf-g++ -I mavlink/ -Wall -g share_mem_test_read.cpp -o share_mem_test_read.out -lrt

