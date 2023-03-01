<div align="center">
  
# Timed Actions Without Using `delay()`
  
</div>

We have all seen The Arduino Blink sketch, which lights an LED for one second and then turns it off for one second.
```
void loop() {
  digitalWrite(myLED, HIGH);
  delay(1000);
  digitalWrite(myLED, LOW);
  delay(1000);
}
```
So you can obviously create timed actions using the `delay()` function.
The problem with delay() is that while your program is delaying, it isn't going to do anything else.  If you wanted your LED to blink every seconds WHILE
it was logging data from a sensor, it would fail - you would not be able to read the sensor any more often than your LED state was changed.

In this tutorial, I will examine some alternatives to `delay()` that do not interfere with other functions.  In CS, aa function that prevents other code from
running is called a blocking function (it blocks the execution of other code.)  What we want is called non-blocking behavior.

## Blink without Delay

## Functions

## Static Data

## Referenced Data

## C++ Objects

## Tables and Callbacks

## Multitasking

## Existing Libraries
