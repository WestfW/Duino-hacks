OUT=bin-stripped
CORE=./core-stripped
INC="-Icore-stripped -E"
GCC=/Applications/Arduino-1.8.10.app/Contents/Java/portable/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5/bin

$GCC/avr-gcc --version

# Sketch
$GCC/avr-g++ -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -flto -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  test_pwm.ino.cpp -o $OUT/test_pwm.ino.cpp.E


# Core
$GCC/avr-gcc -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -flto -fno-fat-lto-objects -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/wiring_analog.c -o $OUT/wiring_analog.c.E

$GCC/avr-gcc -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -flto -fno-fat-lto-objects -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/wiring_digital.c -o $OUT/wiring_digital.c.E

$GCC/avr-gcc -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -flto -fno-fat-lto-objects -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/wiring.c -o $OUT/wiring.c.E

$GCC/avr-g++ -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -flto -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR $INC  $CORE/main.cpp -o $OUT/main.cpp.E

