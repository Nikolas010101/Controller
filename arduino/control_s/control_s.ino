#include "/home/nikolas/Documents/GitHub/Controller/arduino/controller/controller.h"

#define VOLTAGEREADPIN 0
#define VOLTAGEWRITEPIN 3
#define SAMPLINGRATE 1000000

Measure *err_0 = createMeasure();
Measure *err_1 = createMeasure();
Measure *err_2 = createMeasure();
Measure *u_0 = createMeasure();
Measure *u_1 = createMeasure();
Measure *u_2 = createMeasure();

double ref = 1.0, systemOutput = 0.0, intervalSeconds = 0.0;
unsigned long prev = micros(), curr = micros();
char outputString[100];

void setup()
{
    Serial.begin(115200);
};

void loop()
{
    curr = micros();
    if (curr - prev >= SAMPLINGRATE)
    {
        intervalSeconds = (curr - prev) / 1e6;
        systemOutput = adc2real(analogRead(VOLTAGEREADPIN));

        // update previous and current values
        err_0->prev = err_0->curr;
        err_0->curr = ref - systemOutput;

        err_1->prev = err_1->curr;
        err_1->curr = derivative(err_0, intervalSeconds);

        err_2->prev = err_2->curr;
        err_2->curr = derivative(err_1, intervalSeconds);

        u_1->prev = u_1->curr;
        u_1->curr = -34.95902799659847 * u_0->curr + 1.6941086242955898 * err_0->curr + 0.06925596877633099 * err_1->curr; // numerically solve differential equation

        u_0->prev = u_0->curr;
        u_0->curr += riemannIntegral(u_1, intervalSeconds);

        analogWrite(VOLTAGEWRITEPIN, real2pwm(u_0->curr));

        sprintf(outputString, "Current error: %f\nControl value: %f\n----------------------------\n", err_0->curr, u_0->curr);
        Serial.print(outputString);
        prev = micros();
    }
};