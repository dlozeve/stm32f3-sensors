#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <sys/printk.h>
#include <zephyr.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 500

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED_NODE, okay)
#define LED DT_GPIO_LABEL(LED_NODE, gpios)
#define LED_PIN DT_GPIO_PIN(LED_NODE, gpios)
#define LED_FLAGS DT_GPIO_FLAGS(LED_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "led0 devicetree alias is not defined"
#define LED ""
#define LED_PIN 0
#define LED_FLAGS 0
#endif

/* #define I2C_NODE DT_NODELABEL(i2c1) */
/* //#define I2C_NODE DT_PATH(soc, i2c1, lsm303dlhc-magn_1e) */

/* #if DT_NODE_HAS_STATUS(I2C_NODE, okay) */
/* #define I2C DT_LABEL(I2C_NODE) */
/* #else */
/* #error "i2c1 devicetree label is not defined" */
/* #define I2C "" */
/* #endif */

void main(void) {
  const struct device *led_dev;
  /* const struct device *i2c_dev; */
  int ret;
  bool led_active = true;

  printk("Board config: %s\n", CONFIG_BOARD);

  led_dev = device_get_binding(LED);
  if (led_dev == NULL) {
    printk("Didn't find LED device %s\n", LED);
    return;
  }

  ret = gpio_pin_configure(led_dev, LED_PIN, GPIO_OUTPUT_ACTIVE | LED_FLAGS);
  if (ret < 0) {
    printk("Error %d: Couldn't configure LED device %s on pin %d\n", ret, LED,
           LED_PIN);
    return;
  }
  printk("Set up LED at %s on pin %d\n", LED, LED_PIN);

  /* i2c_dev = device_get_binding(I2C); */
  /* if (i2c_dev == NULL) { */
  /*   printk("Didn't find device %s\n", I2C); */
  /*   return; */
  /* } */
  printk("Success!\n");

  while (1) {
    gpio_pin_set(led_dev, LED_PIN, (int)led_active);
    led_active = !led_active;
    k_msleep(SLEEP_TIME_MS);
  }
}
