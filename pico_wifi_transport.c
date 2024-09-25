#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include <uxr/client/profile/transport/custom/custom_transport.h>

struct micro_ros_agent_locator
{
    ip_addr_t address;
    int port;
};

struct transport_buffer
{
    uint8_t *buf;
    bool packet_received;
};

void usleep(uint64_t us)
{
    sleep_us(us);
}

int clock_gettime(clockid_t unused, struct timespec *tp)
{
    uint64_t m = time_us_64();
    tp->tv_sec = m / 1000000;
    tp->tv_nsec = (m % 1000000) * 1000;
    return 0;
}

static struct udp_pcb *pcb;

bool pico_wifi_transport_open(struct uxrCustomTransport *transport)
{
    struct micro_ros_agent_locator *locator = (struct micro_ros_agent_locator *)transport->args;

    pcb = udp_new();

    err_t bind_error = udp_bind(pcb, IP_ADDR_ANY, locator->port);

    bind_error = udp_connect(pcb, &locator->address, locator->port);

    return bind_error == ERR_OK;
}

bool pico_wifi_transport_close(struct uxrCustomTransport *transport)
{
    udp_disconnect(pcb);
    return true;
}

size_t pico_wifi_transport_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len, uint8_t *errcode)
{

    struct micro_ros_agent_locator *locator = (struct micro_ros_agent_locator *)transport->args;

    // err_t connect_error = udp_connect(pcb, locator->address, locator->port);
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

    memcpy(p->payload, buf, p->len);

    /*
    char *req = (char *)p->payload;
    char *ipa = ipaddr_ntoa((ip_addr_t *)&p->payload);
    memset(req, 0, len + 1);
    memcpy(req, buf, len);
    snprintf(req, len + 1, "%d\n", buf);
    printf("Req content %d\n\n", p->payload);
    printf("Pay len %d --- Size of buf %d\n\n", len, sizeof(buf));
    printf("LOCATOR: IP -- %s ; PORT -- %d\n", ipaddr_ntoa(&locator->address), locator->port);
    err_t er = udp_sendto(pcb, p, &locator->address, locator->port);
    */

    err_t er = udp_send(pcb, p);
    // err_t er = udp_sendto(pcb, p, &locator->address, locator->port);
    pbuf_free(p);

    if (er != ERR_OK)
    {
        printf("Writing error");
        return 0;
    }
    // struct udp_pcb *tpcb = udp_new();
    /*
    struct micro_ros_agent_locator *locator = (struct micro_ros_agent_locator *)transport->args;
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    char *req = (char *)p->payload;
    memset(req, 0, len + 1);
    snprintf(req, len, "%d\n", 100);
    err_t er = udp_sendto(pcb, p, &locator->address, locator->port);
    pbuf_free(p);

    if (er != ERR_OK)
    {
        printf("Erreur: %d", er);
    }
    else
    {
        return len + 1;
    }
    */

    return len;
}

static void udp_recv_callback(void *arg, struct udp_pcb *pcb,
                              struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    struct transport_buffer *transport_buffer = (struct transport_buffer *)arg;
    memcpy(transport_buffer->buf, p->payload, p->len);
    pbuf_free(p);
    // printf("MSG RCV");
    transport_buffer->packet_received = true;
    // return;
}

size_t pico_wifi_transport_read(struct uxrCustomTransport *transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode)
{
    (void)errcode;

    struct transport_buffer transport_buffer = {buf, false};

    // uint32_t start_time = millis();
    uint64_t start_time_us = time_us_64();
    int64_t elapsed_time_us = timeout * 1000 - (time_us_64() - start_time_us);
    // printf("ELAPSED TIME : %" PRId64 "\n", elapsed_time_us);
    // printf("TIMEOUT : %d,  --  PACKET : %d BOOL : %d \n\n", timeout, transport_buffer.packet_received, true);
    while (!transport_buffer.packet_received && elapsed_time_us > 0)
    {
        udp_recv(pcb, udp_recv_callback, &transport_buffer);
        sleep_ms(1);
        elapsed_time_us = timeout * 1000 - (time_us_64() - start_time_us);
    }

    /*
        if (transport_buffer.packet_received)
        {
            printf("PACKET RECEIVED");
        }
    */

    // printf("Good");

    // printf("Received : %s",(char * )p->payload);

    return (elapsed_time_us < 0) ? 0 : len;
}
