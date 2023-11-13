#ifndef KEYPAD_GPIO_H
#define KEYPAD_GPIO_H



void turnKeypadRowOn(int keypadRowPin);

void turnKeypadRowOff(int keypadRowPin);

bool isKeypadColumnOn(int keypadColumnPin);



#endif // KEYPAD_GPIO_H