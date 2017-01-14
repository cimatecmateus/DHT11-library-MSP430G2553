
#ifndef CONFIG_H_
#define CONFIG_H_

#define RED_LED BIT0   // Pin P1.0
#define GREEN_LED BIT6      // Pin P1.6
#define RXD BIT1            // Pin P1.1
#define TXD BIT2            // Pin P1.2

void configureClock(void);
void initializePORT1(void);

#endif /* CONFIG_H_ */
