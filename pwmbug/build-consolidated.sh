OUT=bin-consolidated
CORE=./core-cons
INC="-Icore-cons"
LTO="-flto"
#LTO=""
GCC=/Applications/Arduino-1.8.10.app/Contents/Java/portable/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5/bin

$GCC/avr-gcc --version
set -x

# Sketch
#$GCC/avr-g++ -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing $LTO -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/main.cpp -o $OUT/main.cpp.o
$GCC/avr-g++ -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing $LTO -mmcu=atmega2560 $CORE/main.cpp -o $OUT/main.cpp.o

# Core
#$GCC/avr-gcc -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections $LTO -fno-fat-lto-objects -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/wiring_all.c -o $OUT/wiring_all.c.o
$GCC/avr-gcc -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections $LTO -fno-fat-lto-objects -mmcu=atmega2560  $CORE/wiring_all.c -o $OUT/wiring_all.c.o

#link
$GCC/avr-gcc -w -Os -g $LTO -fuse-linker-plugin -Wl,--gc-sections,--relax -mmcu=atmega2560 -o $OUT/test_pwm.ino.elf $OUT/main.cpp.o $OUT/wiring_all.c.o -L$OUT -lm

$GCC/avr-objcopy -O ihex -R .eeprom $OUT/test_pwm.ino.elf $OUT/test_pwm.ino.hex

$GCC/avr-size $OUT/test_pwm.ino.elf

$GCC/avr-objdump -SC  $OUT/test_pwm.ino.elf >$OUT/test_pwm.ino.lss
