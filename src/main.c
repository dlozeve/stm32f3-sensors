#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
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

// Define the accelerometer as a child of the I2C1 interface
/* #define I2C_NODE DT_NODELABEL(i2c1) */
/* #define ACC_NODE DT_CHILD(I2C_NODE, lsm303dlhc_accel_19) */
// Define the accelerometer directly
#define ACC_NODE DT_INST(0, st_lis2dh)
#if DT_NODE_HAS_STATUS(ACC_NODE, okay)
#define ACC DT_LABEL(ACC_NODE)
#else
#error "Accelerometer device is not defined"
#endif

void main(void) {
  int ret;
  bool led_active = true;

  printk("Board config: %s\n", CONFIG_BOARD);

  const struct device *led_dev;
  led_dev = device_get_binding(LED);
  if (led_dev == NULL) {
    printk("Failed to initialize LED device %s\n", LED);
    return;
  }

  ret = gpio_pin_configure(led_dev, LED_PIN, GPIO_OUTPUT_ACTIVE | LED_FLAGS);
  if (ret < 0) {
    printk("Error %d: Couldn't configure LED device %s on pin %d\n", ret, LED,
           LED_PIN);
    return;
  }
  printk("Set up LED at %s on pin %d\n", LED, LED_PIN);

  const struct device *acc_dev;
  acc_dev = device_get_binding(ACC);
  if (acc_dev == NULL) {
    printk("Failed to initialize accelerometer device %s\n", ACC);
    return;
  }
  printk("Set up accelerometer %s\n", ACC);

  while (1) {
    struct sensor_value accel[3];
    sensor_sample_fetch(acc_dev);
    sensor_channel_get(acc_dev, SENSOR_CHAN_ACCEL_XYZ, accel);
    printk("Acceleration: (%f, %f, %f)\n", sensor_value_to_double(&accel[0]),
           sensor_value_to_double(&accel[1]),
           sensor_value_to_double(&accel[2]));

    gpio_pin_set(led_dev, LED_PIN, (int)led_active);
    led_active = !led_active;
    k_msleep(SLEEP_TIME_MS);
  }
}
