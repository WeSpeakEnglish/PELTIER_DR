#ifndef _LCD_H
#define _LCD_H



extern unsigned char DispString[17];

void ConfigureLCD(void);
void DisplayPressure(float Pressure);
unsigned char* DisplayTemperature(int16_t Pressure);
void DisplayHumidity(float* Humidity);


#endif