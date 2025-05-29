#include "epd.h"
#include "Debug.h"

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(10);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_SPI_WriteByte(Reg);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_SPI_WriteByte(Data);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void ReadBusy(void)
{
    UBYTE busy;
    Debug("e-Paper busy\r\n");
    while (!DEV_Digital_Read(EPD_BUSY_PIN))
    {
        DEV_Delay_ms(1);
    }
    Debug("e-Paper busy release\r\n");
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void TurnOnDisplay(void)
{
    SendCommand(0x04); // POWER ON
    ReadBusy();

    SendCommand(0x12); // display refresh
    SendData(0x00);
    ReadBusy();

    SendCommand(0x02); // POWER_OFF
    SendData(0x00);
    ReadBusy();
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void Init(void)
{
    DEV_Module_Init();
    Reset();
    DEV_Delay_ms(20);
    ReadBusy();

    SendCommand(0xAA); // CMDH
    SendData(0x49);
    SendData(0x55);
    SendData(0x20);
    SendData(0x08);
    SendData(0x09);
    SendData(0x18);

    SendCommand(0x01);
    SendData(0x3F);
    SendData(0x00);
    SendData(0x32);
    SendData(0x2A);
    SendData(0x0E);
    SendData(0x2A);

    SendCommand(0x00);
    SendData(0x5F);
    SendData(0x69);

    SendCommand(0x03);
    SendData(0x00);
    SendData(0x54);
    SendData(0x00);
    SendData(0x44);

    SendCommand(0x05);
    SendData(0x40);
    SendData(0x1F);
    SendData(0x1F);
    SendData(0x2C);

    SendCommand(0x06);
    SendData(0x6F);
    SendData(0x1F);
    SendData(0x1F);
    SendData(0x22);

    SendCommand(0x08);
    SendData(0x6F);
    SendData(0x1F);
    SendData(0x1F);
    SendData(0x22);

    SendCommand(0x13); // IPC
    SendData(0x00);
    SendData(0x04);

    SendCommand(0x30);
    SendData(0x3C);

    SendCommand(0x41); // TSE
    SendData(0x00);

    SendCommand(0x50);
    SendData(0x3F);

    SendCommand(0x60);
    SendData(0x02);
    SendData(0x00);

    SendCommand(0x61);
    SendData(0x03);
    SendData(0x20);
    SendData(0x01);
    SendData(0xE0);

    SendCommand(0x82);
    SendData(0x1E);

    SendCommand(0x84);
    SendData(0x00);

    SendCommand(0x86); // AGID
    SendData(0x00);

    SendCommand(0xE3);
    SendData(0x2F);

    SendCommand(0xE0); // CCSET
    SendData(0x00);

    SendCommand(0xE6); // TSSET
    SendData(0x00);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void Clear(UBYTE color)
{

    SendCommand(0x10);
    for (int i = 0; i < WIDTH / 2; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            SendData((color << 4) | color);
        }
    }

    TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void Display(UBYTE *Image)
{
    UDOUBLE i, j, Width, Height;
    int imgSize = WIDTH * HEIGHT / 2;

    SendCommand(0x10);
    for (j = 0; j < imgSize; j++)
    {
        SendData(Image[j]);
    }

    TurnOnDisplay();
}

void DisplayHalfScreen(const UBYTE *blackimage, const UBYTE *ryimage)
{
    UBYTE Data_Black, Data_RY, Data;
    UDOUBLE i, j, Width, Height;
    Width = (WIDTH % 8 == 0) ? (WIDTH / 8) : (WIDTH / 8 + 1);
    Height = HEIGHT;

    SendCommand(0x10);
    for (j = 0; j < Height / 2; j++)
    {
        for (i = 0; i < Width; i++)
        {
            Data_Black = blackimage[i + j * Width];
            Data_RY = ryimage[i + j * Width]; // red or yellow
            for (UBYTE k = 0; k < 8; k++)
            {
                if ((Data_RY & 0x80) == 0x00)
                {
                    Data = 0x04; // red
                }
                else if ((Data_Black & 0x80) == 0x00)
                {
                    Data = 0x00; // black
                }
                else
                {
                    Data = 0x03; // white
                }
                Data = (Data << 4) & 0xFF;
                Data_Black = (Data_Black << 1) & 0xFF;
                Data_RY = (Data_RY << 1) & 0xFF;
                k += 1;

                if ((Data_RY & 0x80) == 0x00)
                {
                    Data |= 0x04; // red
                }
                else if ((Data_Black & 0x80) == 0x00)
                {
                    Data |= 0x00; // black
                }
                else
                {
                    Data |= 0x03; // white
                }
                Data_Black = (Data_Black << 1) & 0xFF;
                Data_RY = (Data_RY << 1) & 0xFF;
                SendData(Data);
            }
        }
    }

    for (j = 0; j < Height / 2; j++)
    {
        for (i = 0; i < Width; i++)
        {
            for (UBYTE k = 0; k < 4; k++)
            {
                SendData(Data);
            }
        }
    }

    TurnOnDisplay();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void Sleep(void)
{
    SendCommand(0x07); // DEEP_SLEEP
    SendData(0XA5);
    DEV_Delay_ms(10);
    DEV_Digital_Write(EPD_RST_PIN, 0); // Reset
}
