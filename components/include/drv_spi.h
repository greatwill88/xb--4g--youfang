/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */
#ifndef _DRV_SPI_H_
#define _DRV_SPI_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_GENERAL_SPI_FLASH_MOUNT_POINT "/extn"
#define FLASH_START_ADDR 0x00000000
#define FLASH_MOUNT_SIZE (0x08000000 - 0x1000)
#define FLASH_EB_SIZE 0x1000
#define FLASH_PB_SIZE 0x100

typedef enum
{
    SPI_I2C_CS0 = 0,
    SPI_I2C_CS1,
    SPI_I2C_CS2,
    SPI_I2C_CS3,
} drvSpiCsSel;

typedef enum
{
    SPI_CPOL_LOW = 0,
    SPI_CPOL_HIGH,
} drvSpiCpolPol;

typedef enum
{
    SPI_CPHA_1Edge,
    SPI_CPHA_2Edge,
} drvSpiCphaPol;

typedef enum
{
    SPI_CS_ACTIVE_HIGH,
    SPI_CS_ACTIVE_LOW,
} drvSpiCsPol;

typedef enum
{
    SPI_DI_0 = 0,
    SPI_DI_1,
    SPI_DI_2,
} drvSpiInputSel;

typedef enum
{
    RX_TRIGGER_1_BYTE,
    RX_TRIGGER_4_BYTE,
    RX_TRIGGER_8_BYTE,
    RX_TRIGGER_12_BYTE,
} drvSpiRxTrigger;

typedef enum
{
    TX_TRIGGER_1_EMPTY,
    TX_TRIGGER_4_EMPTY,
    TX_TRIGGER_8_EMPTY,
    TX_TRIGGER_12_EMPTY,
} drvSpiTxTrigger;

typedef enum
{
    SPI_CLK_CTRL,
    SPI_DO_CTRL,
    SPI_CS0_CTRL,
    SPI_CS1_CTRL,
    SPI_CS2_CTRL,
} drvSpiPinCtrl;

typedef enum
{
    SPI_TRIGGER_1_DATA,
    SPI_TRIGGER_4_DATA,
    SPI_TRIGGER_8_DATA,
    SPI_TRIGGER_12_DATA,
} drvSpithreshold;

typedef struct
{
    uint32_t rxOvf : 1;
    uint32_t txTh : 1;
    uint32_t txDmaDone : 1;
    uint32_t rxTh : 1;
    uint32_t rxDmaDone : 1;
    drvSpithreshold Tx_Rthreshold;
    drvSpithreshold Rx_Tthreshold;
} drvSpiIrq;

typedef enum
{
    SPI_CTRL,
    INPUT_CTRL,
    FORCE_0_CTRL,
    FROCE_1_CTRL,
} drvSpiPinState;

typedef enum
{
    SPI_DIRECT_POLLING = 0,
    SPI_DIRECT_IRQ,
    SPI_DMA_POLLING,
    SPI_DMA_IRQ,
} drvSpiTransferMode;

typedef struct
{
    bool inputEn;
    uint8_t oe_delay;
    uint32_t name;
    uint32_t framesize;
    uint32_t baud;
    drvSpiCsPol cs_polarity0;
    drvSpiCsPol cs_polarity1;
    drvSpiCpolPol cpol;
    drvSpiCphaPol cpha;
    drvSpiInputSel input_sel;
    drvSpiTransferMode transmode;
} drvSpiConfig_t;

struct drvSpiMaster;

typedef void (*callback)(drvSpiIrq cause);

typedef struct drvSpiMaster drvSpiMaster_t;

typedef struct drvGeneralSpiFlash drvGeneralSpiFlash_t;

/**
 * @brief acquire the spi master
 *
 * @param cfg : drvSpiConfig_t structure that contains the configuration 
 				infomation for specified SPI peripheral
 * @return
 *      - (NULL)    fail
 *      - otherwise the spi master instance
 */
drvSpiMaster_t *drvSpiMasterAcquire(drvSpiConfig_t cfg);

/**
 * @brief Transmits datas with specific length throuth the SPI peripheral.
 *
 * @param d : point to spi instance
 * @param cs : cs choice of spi
 * @param sendaddr : point to senddata
 * @param len : data len to be transmitted.
 * @return
 *      - (false)    fail
 *      - (true)     success
 */
bool drvSpiWrite(drvSpiMaster_t *d, drvSpiCsSel cs, void *sendaddr, uint32_t len);

/**
 * @brief Receive the most recent received datas by the SPI peripheral.
 *
 * @param d : point to spi instance
 * @param cs : cs choice of spi
 * @param sendaddr : point to senddata address.in this case, you can specify the content of datas.
 * @param readaddr : point to readaddr address.
 * @param len : data len to be received.
 * @return
 *      - (false)    fail
 *      - (true)     success
 */
bool drvSpiRead(drvSpiMaster_t *d, drvSpiCsSel cs, void *sendaddr, void *readaddr, uint32_t len);

/**
 * @brief config irq callback
 * @param d : point to spi instance
 * @param callfunc :call back function .
 * @return : None
 */
void drvSpiSetIrqHandle(drvSpiMaster_t *d, callback callfunc);

/**
 * @brief set irq mask
 * @param d : point to spi instance
 * @param mask :point to  mask 
 * @return : None
 */
void drvSpiSetIrqMask(drvSpiMaster_t *d, drvSpiIrq *mask);

/**
 * @brief clear irq mask
 * @param d : point to spi instance
 * @return : None
 */

void drvSpiClrIrqMask(drvSpiMaster_t *d);

/**
 * @brief Enable the Tx stream mode,Used for SD/MMC SPI mode.
 * @param d : point to spi instance
 * @param stream_bit : value of stream mode
 * @param on : enable or disable stream mode
 *      - true  enable stream mode, this mode provide infinite bit stream for sending after fifo is empty,
 				all generated data have the same value, the vaule is in stream bit)
 		- false  disable stream mode
 * @return : None
 */
void drvSpiEnableStreamMode(drvSpiMaster_t *d, uint8_t stream_bit, bool on);

/**
 * @brief get the Number of data  in spi rx FIFO
 * @param d : point to spi instance
 * @return : Number of data  in spi rx FIFO
 */
uint8_t drvSpiGetRxLevel(drvSpiMaster_t *d);

/**
 * @brief get the Number of empty spot in spi tx FIFO
 * @param d : point to spi instance
 * @return : Number of empty spot in spi tx FIFO
 */
uint8_t drvSpiGetTxFree(drvSpiMaster_t *d);

/**
 * @brief config pin state of the spi peripheral
 * @param d : point to spi instance
 * @param pinctrl : where it can be 
 *      - SPI_CLK_CTRL SPI_DO_CTRL SPI_CS0_CTRL SPI_CS1_CTRL SPI_CS2_CTRL
 * @param pinstate : pin can be config as :
 *      - SPI_CTRL (ctrl by spi peripheral automaticly)
 *      - INPUT_CTRL (ctrl pin in input mode)
 *      - FORCE_0_CTRL (froced pull down)
 *      - FROCE_1_CTRL (froced pull up)
 * @return : None
 */
void drvSpiPinControl(drvSpiMaster_t *d, drvSpiPinCtrl pinctrl, drvSpiPinState pinstate);

/**
 * @brief release the spi peripheral
 * @param d : point to spi instance
 * @return : None
 */
void drvSpiMasterRelease(drvSpiMaster_t *d);

drvGeneralSpiFlash_t *drvGeneralSpiFlashOpen(uint32_t name);
void drvGeneralSpiFlashInit(uint32_t name);
void drvGeneralSpiFlashEnable(int wenable_cmd);
int drvGeneralSpiFlashFininsh(int read_status);
int drvGeneralSpiFlashRead(uint32_t offset, uint32_t size, void *pbuf);
bool drvGeneralSpiFlashWrite(drvGeneralSpiFlash_t *d, uint32_t offset, const void *data, size_t size);
bool drvGeneralSpiFlashErase(drvGeneralSpiFlash_t *d, uint32_t offset, size_t size);

int drvGeneralSpiFlashWriteTest(uint32_t offset, uint32_t size, const void *buf);
int drvGeneralSpiFlashEraseTest(uint32_t offset, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
