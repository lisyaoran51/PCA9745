
#include <linux/spi/spidev.h>
#include "mailbox.h"
#include "gpio.h"

#define GPIO_OFFSET                              (0x00200000)
#define PERIPH_BASE_RPI                          0x20000000
#define DEV_GPIOMEM "/dev/gpiomem"

gpio_t* gpio;
int spi_fd;

static ws2811_return_t spi_init()
{
    
    static uint8_t mode;
    static uint8_t bits = 8;
    uint32_t speed = 25000000; //25mhz
    //ws2811_device_t *device = ws2811->device;
    uint32_t Base = 0x20000000; //ws2811->rpi_hw->periph_base;
    //int pinnum = ws2811->channel[0].gpionum;
	
	

    spi_fd = open("/dev/spidev0.0", O_RDWR);
    if (spi_fd < 0) {
        fprintf(stderr, "Cannot open /dev/spidev0.0. spi_bcm2835 module not loaded?\n");
        return WS2811_ERROR_SPI_SETUP;
    }
    //device->spi_fd = spi_fd;

    // SPI mode
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        return WS2811_ERROR_SPI_SETUP;
    }
    if (ioctl(spi_fd, SPI_IOC_RD_MODE, &mode) < 0)
    {
        return WS2811_ERROR_SPI_SETUP;
    }

    // Bits per word
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0)
    {
        return WS2811_ERROR_SPI_SETUP;
    }
    if (ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0)
    {
        return WS2811_ERROR_SPI_SETUP;
    }

    // Max speed Hz
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        return WS2811_ERROR_SPI_SETUP;
    }
    if (ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
    {
        return WS2811_ERROR_SPI_SETUP;
    }

    // Initialize device structure elements to not used
    // except driver_mode, spi_fd and max_count (already defined when spi_init called)
    //device->pxl_raw = NULL;
    //device->dma = NULL;
    //device->pwm = NULL;
    //device->pcm = NULL;
    //device->dma_cb = NULL;
    //device->dma_cb_addr = 0;
    //device->cm_clk = NULL;
    //device->mbox.handle = -1;

    // Set SPI-MOSI pin
    gpio = (gpio_t*)mapmem(GPIO_OFFSET + Base, sizeof(gpio_t), DEV_GPIOMEM);
    if (!gpio)
    {
        return WS2811_ERROR_SPI_SETUP;
    }
    gpio_function_set(gpio, 10, 0);	// SPI-MOSI ALT0 //pinnum

    

    return WS2811_SUCCESS;
}

static ws2811_return_t spi_transfer()
{
    int ret;
    struct spi_ioc_transfer tr;

    memset(&tr, 0, sizeof(struct spi_ioc_transfer));
    tr.tx_buf = (unsigned long)ws2811->device->pxl_raw;
    tr.rx_buf = 0;
    tr.len = PCM_BYTE_COUNT(ws2811->device->max_count, ws2811->freq);

    ret = ioctl(ws2811->device->spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        fprintf(stderr, "Can't send spi message");
        return WS2811_ERROR_SPI_TRANSFER;
    }

    return WS2811_SUCCESS;
}


int main(){
	
	
	spi_init();
	
	int i,fd;
	char wr_buf[]={0xc0}; // 0000 0011
	char rd_buf[10];;
	
	if (write(spi_fd, wr_buf, ARRAY_SIZE(wr_buf)) != ARRAY_SIZE(wr_buf))
		perror("Write Error");
	if (read(spi_fd, rd_buf, ARRAY_SIZE(rd_buf)) != ARRAY_SIZE(rd_buf))
		perror("Read Error");
	else
		for (i=0;i<ARRAY_SIZE(rd_buf);i++) {
		printf("0x%02X ", rd_buf[i]);
		if (i%2)
			printf("\n");
	}

	
	
	return 0;
}