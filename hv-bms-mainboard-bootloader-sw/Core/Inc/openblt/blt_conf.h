#ifndef BLT_CONF_H
#define BLT_CONF_H

/****************************************************************************************
*   C P U   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/* External crystal frequency (if you only use HSI = 8 MHz, just leave at 8000). */
#define BOOT_CPU_XTAL_SPEED_KHZ (16000)

/* System clock frequency */
#define BOOT_CPU_SYSTEM_SPEED_KHZ (180000)

/* STM32C0 is little endian. */
#define BOOT_CPU_BYTE_ORDER_MOTOROLA (0)

/* Optional hook before jumping to user application. */
#define BOOT_CPU_USER_PROGRAM_START_HOOK (0)

/****************************************************************************************
*   C O M M U N I C A T I O N   I N T E R F A C E   C O N F I G U R A T I O N
****************************************************************************************/
/* CAN communication */
#define BOOT_COM_CAN_ENABLE (1)
#define BOOT_COM_CAN_BAUDRATE (1000000) /* pick what matches your bus */
#define BOOT_COM_CAN_TX_MSG_ID (0x21u)  /* to be modified when ids defined */
#define BOOT_COM_CAN_TX_MAX_DATA (8)
#define BOOT_COM_CAN_RX_MSG_ID (0x22u) /* to be modified when ids defined */
#define BOOT_COM_CAN_RX_MAX_DATA (8)
#define BOOT_COM_CAN_CHANNEL_INDEX (0)

/* UART (RS232) – enabled, by default is disabled as CAN already active */
#define BOOT_COM_RS232_ENABLE (0)
#define BOOT_COM_RS232_BAUDRATE (115200)
#define BOOT_COM_RS232_TX_MAX_DATA (64)
#define BOOT_COM_RS232_RX_MAX_DATA (64)
#define BOOT_COM_RS232_CHANNEL_INDEX (0)

/****************************************************************************************
*   B A C K D O O R   E N T R Y   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_BACKDOOR_HOOKS_ENABLE (0)

/****************************************************************************************
*   N O N - V O L A T I L E   M E M O R Y   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/* STMF767ZI has up to 2MB Flash. But we are using the MCU stm32f767vgtx!!!!!! */
#define BOOT_NVM_HOOKS_ENABLE (0)
#define BOOT_NVM_SIZE_KB (1024)
#define BOOT_NVM_CHECKSUM_HOOKS_ENABLE (0)

/****************************************************************************************
*   W A T C H D O G   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_COP_HOOKS_ENABLE (0)

/****************************************************************************************
*   S E E D / K E Y   S E C U R I T Y   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_XCP_SEED_KEY_ENABLE (0)

#endif /* BLT_CONF_H */
       /*********************************** end of blt_conf.h *********************************/
