## What is WTCC?

WTCC stands for `Way Too Complex Calculator` and is the result of a school asignment to write a program that can do math with two numbers.

## What can it do?

WTCC is planed to be, as the name says, *a way to complex calculator*.

Currently you'd have a hard time using it like a calculator because i have not yet implimented text parsing and the running program has to be hard coded.
*coming soon: propper calculator action*

But apart from being able to add and subtract numbers WTCC aims to have a bit more functionality... maybe a bit too much functionality.

### Features?

- [x] Parsing of equations and syntax
- [x] Basic arithmatic
- [x] Function calls *(no function definition yet)*
- [x] Variables *(no variable definition yet)*
- [ ] Comparison
- [ ] IO functions

## What is the end goal?

When complete <sup>*enough*</sup> WTCC should be able to run a propper program. Since it still a calculator computing a mandelbrot set should be a good goal.

## How do i compile it?

You can either run `make` directly to build the project or run `buildLinux.sh`.

The output binary is `wtcc`.

### I run Windows, how do i compile it?

To compile this code under Windows you need MSYS2 with gcc and g++.

Similar to linux you can either run `make EXECUTABLE=wtcc.exe` or run `buildWindows.bat`.

The output binary is `wtcc.exe`.

## License and warranty?

This code is licensed under GPL3 and thus i give no warranty of the code compiling, working, giving accurate numbers and not burning your house down.