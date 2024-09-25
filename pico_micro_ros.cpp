#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

// ros2 general
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rcl/error_handling.h>
#include <rclc/executor.h>

// ros2 messages
#include <rmw_microros/rmw_microros.h> //necessary?
#include <std_msgs/msg/int32.h>

#include "pico_wifi_transport.h"
#include "hardware/adc.h" //necessary?

// Network settings
char ssid[] = "ssid";
char pass[] = "pass";
char agent_ip[] = "192.168.131.22";     // host computer running the micro_ros_agent node (or snap or docker agent)
int agent_port = 8888;

#define UART_ID uart0
#define BAUD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// use these as uart0 pins
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int counter;

// microros
rcl_publisher_t publisher_counter;
std_msgs__msg__Int32 msg_counter = {
    .data = 0
};


// RX interrupt handler
void on_uart_rx()
{
    if (uart_is_readable(UART_ID))
    {
        uint8_t ch = uart_getc(UART_ID);
        uart_putc(UART_ID, ch);
    }
}


// ROS timer callback function
void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
    counter++;
    msg_counter.data = counter;
    rcl_publish(&publisher_counter, &msg_counter, NULL);
}


int main()
{
    // connect to wifi and microros agent
    set_microros_wifi_transports(ssid, pass, agent_ip, agent_port);
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn on FIFO: receive buffer
    // we probably do't need this, as we are using an interrupt to handle each byte
    uart_set_fifo_enabled(UART_ID, true);
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);   


    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " Hello, UART!\n");    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart


    // Micro ROS
    // ROS client Library timer, allocater, executer and support
    rcl_timer_t timer;
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    rcl_init_options_init(&init_options, allocator);
    const size_t ros_domain_id = 0;
    rcl_init_options_set_domain_id(&init_options, ros_domain_id);       // example, how to change domain id
    rclc_support_t support;
    rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);
    rclc_executor_t executor;
    rcl_node_t node;
    const char * node_name = "pico_node";
    const char * node_namespace = "";

    rclc_node_init_default(&node, node_name, node_namespace, &support);

    // Wait for agent successful ping for 2 minutes.
    const int timeout_ms = 1000;
    const uint8_t attempts = 120;
    rmw_uros_ping_agent(timeout_ms, attempts);
    uart_puts(UART_ID, "Connected to micro-ros agent!\n");

    rclc_publisher_init_best_effort(
        &publisher_counter,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "counter");

    rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(1000), // 1sec
        timer_callback);

    rclc_executor_init(&executor, &support.context, 2, &allocator);
    rclc_executor_add_timer(&executor, &timer);


    while (true)
    {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1000));

        // do something else
        // ...
    }
}

