#ifndef MACRO_H
#define MACRO_H

#define VOLTAGE 'V'
#define CAPACITOR 'C'
#define RESISTOR 'R'
#define INFO '.'
#define SUBCKT 'X'
#define MOS 'm'

enum read_state {ToModule, ToMain, Main, Module};
enum typeofDevice {Resistor, Voltage, Capacitor, Subckt, Mos};
enum typeofVoltage {DC, AC, ACDC};
enum typeofODEMethod{BCKEULER, TRAPZOIDAL, GEAR2};

#endif
