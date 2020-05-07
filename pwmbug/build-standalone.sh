OUT=bin-standalone
CORE=./core
INC="-Icore"
GCC=/Applications/Arduino-1.8.10.app/Contents/Java/portable/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5/bin

$GCC/avr-gcc --version
set -x

# Sketch
$GCC/avr-g++ -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  test_pwm.ino.cpp -o $OUT/test_pwm.ino.cpp.o


# Core
$GCC/avr-gcc -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/wiring_analog.c -o $OUT/wiring_analog.c.o

$GCC/avr-gcc -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/wiring_digital.c -o $OUT/wiring_digital.c.o

$GCC/avr-gcc -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/wiring.c -o $OUT/wiring.c.o

$GCC/avr-g++ -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/main.cpp -o $OUT/main.cpp.o

# Core Lbrary
$GCC/avr-gcc-ar rcs $OUT/core.a $OUT/wiring_analog.c.o
$GCC/avr-gcc-ar rcs $OUT/core.a $OUT/wiring_digital.c.o
$GCC/avr-gcc-ar rcs $OUT/core.a $OUT/wiring.c.o
$GCC/avr-gcc-ar rcs $OUT/core.a $OUT/main.cpp.o


# Link
$GCC/avr-gcc -w -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections,--relax -mmcu=atmega2560 -o $OUT/test_pwm.ino.elf $OUT/test_pwm.ino.cpp.o $OUT/core.a -L$OUT -lm

$GCC/avr-objcopy -O ihex -R .eeprom $OUT/test_pwm.ino.elf $OUT/test_pwm.ino.hex

$GCC/avr-size $OUT/test_pwm.ino.elf

$GCC/avr-objdump -SC  $OUT/test_pwm.ino.elf >$OUT/test_pwm.ino.lss
