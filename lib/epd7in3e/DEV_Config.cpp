/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-02-19
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "DEV_Config.h"
#include <SPI.h>

static SPIClass * display_spi = new SPIClass(FSPI);

void GPIO_Config(void)
{    

    //display_spi->beginTransaction(SPISettings(20000000, 1, 0));

    pinMode(EPD_CS_PIN, OUTPUT);
    pinMode(EPD_RST_PIN, OUTPUT);
    pinMode(EPD_DC_PIN, OUTPUT);
    pinMode(EPD_BUSY_PIN, INPUT);

    SPI.begin();
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

}
/******************************************************************************
function:	Module Initialize, the BCM2835 library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_Module_Init(void)
{
    // gpio
    GPIO_Config();

    // spi
    // SPI.setDataMode(SPI_MODE0);
    // SPI.setBitOrder(MSBFIRST);
    // SPI.setClockDivider(SPI_CLOCK_DIV4);
    // SPI.begin();

    return 0;
}

/******************************************************************************
function:
            SPI read and write
******************************************************************************/
void DEV_SPI_WriteByte(UBYTE data)
{
    // SPI.beginTransaction(spi_settings);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    //digitalWrite(EPD_CS_PIN, GPIO_PIN_RESET);
    // REG_WRITE(GPIO_OUT_W1TC_REG, 1 << EPD_CS_PIN);

    SPI.transfer(data);

    //digitalWrite(EPD_CS_PIN, GPIO_PIN_SET);
    DEV_Digital_Write(EPD_CS_PIN, 1);

    // REG_WRITE(GPIO_OUT_W1TS_REG, 1 << EPD_CS_PIN);
    // SPI.endTransaction();
}