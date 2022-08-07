# stm32h7_wireless
This project aim to implement wireless driver of AP6212, porting lwip TCP/IP stack with freertos, hope to reduce memory usage by cut wireless driver(wiced sdk), lwip stack.

## rtos source code
Get freertos kernel source code using git submodule:
```
git submodule add https://github.com/FreeRTOS/FreeRTOS-Kernel.git ./lib
```
## port freertos
- Compiler related porting<br>