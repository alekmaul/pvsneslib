;---------------------------------------------------------------------------------
;
;   Copyright (C) 2023
;       Alekmaul
;
;   This software is provided 'as-is', without any express or implied
;   warranty.  In no event will the authors be held liable for any
;   damages arising from the use of this software.
;
;   Permission is granted to anyone to use this software for any
;   purpose, including commercial applications, and to alter it and
;   redistribute it freely, subject to the following restrictions:
;
;   1.  The origin of this software must not be misrepresented; you
;       must not claim that you wrote the original software. If you use
;       this software in a product, an acknowledgment in the product
;       documentation would be appreciated but is not required.
;   2.  Altered source versions must be plainly marked as such, and
;       must not be misrepresented as being the original software.
;   3.  This notice may not be removed or altered from any source
;       distribution.
;
;---------------------------------------------------------------------------------

#define RLINK_BUFSIZE       64              ; Size of software receive buffer in bytes.
                                            ; UART Send/Receive hardware buffers are 128 Bytes each
.equ	UART_BASE           $21C0

.equ	UART_RHR            UART_BASE+0     ; Receive holding register
.equ	UART_THR            UART_BASE+0     ; Transmit holding register
.equ	UART_IER            UART_BASE+2     ; Interrupt enable register
.equ	UART_FCR            UART_BASE+4     ; FIFO control register
.equ	UART_LCR            UART_BASE+6     ; Line control register
.equ	UART_MCR            UART_BASE+8     ; Modem control register
.equ	UART_LSR            UART_BASE+10    ; Line status register
.equ	UART_DLL            UART_BASE+0     ; Divisor latch LSB. Acessed only when LCR[7] = 1
.equ	UART_DLM            UART_BASE+2     ; Divisor latch MSB. Acessed only when LCR[7] = 1
.equ	UART_DVID           UART_BASE+2     ; Device ID


.RAMSECTION ".reg_cons7e" BANK $7E

rlink_buf                   DSB 8           ; For data conversions and display (debug mostly)
rlink_present               DB              ;   Flag indicating presense of hardware

rlink_rcvbuf                DSB RLINK_BUFSIZE   ; Our circular network receive buffer
rlink_rdIdx                 DB              ; Receive buffer Read/Write indexes
rlink_wrIdx                 DB

.ENDS

.SECTION ".networkretro0_text" SUPERFREE

;---------------------------------------------------------------------------
; Initialize Network Adapter
; Sets boolean value cart_present to TRUE/FALSE
;void netRlinkInitialize(void)
netRlinkInitialize:
    php
    phb
    phx

    sep #$20                                                  ; 8bit A
    lda #$7e
    pha
    plb

    lda #$80                                                  ; Setup registers so we can read device ID from UART
    sta.l UART_LCR
    xor a
    sta.l UART_DLM                                            ; to detect presence of hardware
    sta.l UART_DLL                                            ; ..

    ldx #$1
    lda.l UART_DVID                                           ; hard ok if UART_DVID == 0x10
    sec
    sbc #$10
    beq +
    dex
+:
    txa
    sta.b rlink_present                                       ; 1 if retrolink is connected
    beq _rlIni

    lda #$83                                                  ; Init UART to 921600 Baud 8-N-1 no flow control
    sta.l UART_LCR
    xor a
    sta.l UART_DLM
    lda #$01
    sta.l UART_DLL
    lda #$03
    sta.l UART_LCR
    xor a
    sta.l UART_MCR
    lda #$07
    sta.l UART_FCR
    xor a
    sta.l UART_IER

_rlIni:
    plx
    plb
    plp
    rtl

;---------------------------------------------------------------------------
; Flush Buffer
; void netRlinkFlushBuffers(void)
netRlinkFlushBuffers:
;{
;    int i;
;    readIndex  = 0;         // reset read index for software receive buffer
;    writeIndex = 0;         // reset write index for software receive buffer
;    UART_FCR   = 0x07;      // Reset UART TX/RX hardware fifos
;    for(i=0; i<BUFFER_SIZE; i++) { receive_buffer[i] = 0xFF; } // Software buffer
;    return;
;}

;---------------------------------------------------------------------------
; Write Buffer
; Stores a byte in the software receive buffer
; void netRlinkWriteBuffer(u8 data)
netRlinkWriteBuffer:
;{
;    receive_buffer[writeIndex++] = data;
;    if (writeIndex == BUFFER_SIZE) { writeIndex = 0; }
;}

;---------------------------------------------------------------------------
;  Read Buffer
; Returns a byte from the software receive buffer
; u8 netRlinkReadBuffer(void)
netRlinkReadBuffer:
;{
;    u8 data = receive_buffer[readIndex++];
;    if (readIndex == BUFFER_SIZE) { readIndex = 0; }
;    return data;
;}

;---------------------------------------------------------------------------
;// Data Available
;//****************************************************************
;// Returns boolean value on data presence in the software receive buffer
;u8 NET_dataAvailable(void)
;netRlinkReadBuffer:
;{
;    return writeIndex != readIndex;
;};

;---------------------------------------------------------------------------
;// Bytes Available
;//****************************************************************
;// Return the number of bytes currently in the software recieve buffer
;u16 NET_bytesAvailable(void)
;{
;    return (writeIndex - readIndex);
;}

;---------------------------------------------------------------------------
;// Network TX Ready
;//****************************************************************
;// Returns boolean value if transmit fifo is clear to send data
;bool NET_TXReady()
;{
;    return (UART_LSR & 0x20);
;}

;---------------------------------------------------------------------------
;// Network RX Ready
;//****************************************************************
;/ Returns boolean value if there is data in hardware receive fifo
;bool NET_RXReady()
;{
;    return (UART_LSR & 0x01);
;}

;---------------------------------------------------------------------------
;// Network Send
;//****************************************************************
;// Sends a single byte
;void NET_sendByte(u8 data)
;{
;    while(!NET_TXReady());
;    UART_RHR = data;
;    return;
;}

;---------------------------------------------------------------------------
;// Read Byte
;//****************************************************************
;// Returns a single byte from the hardware UART receive buffer directly
;u8 NET_readByte(void)
;{
;    return UART_RHR;
;}

;---------------------------------------------------------------------------
;// Network Send Message                                         **
;//****************************************************************
;// Sends a string of ascii
;void NET_sendMessage(char *str)
;{
;  int i=0;
;  int length=0;
;  length = strlen(str);
;  char data[128];
;  strncpy(data, str, length);
;  data[length] = '\0';
;  while (i<length) { NET_sendByte(data[i]); i++; }
;}

;---------------------------------------------------------------------------
;// Enter Monitor Mode                                           **
;//****************************************************************
;void NET_enterMonitorMode(void)
;{
;    NET_flushBuffers();
;    NET_sendMessage("C0.0.0.0/0\n");
;    while (!NET_RXReady() || NET_readByte() != '>') {}
;}

;---------------------------------------------------------------------------
;// Exit Monitor Mode                                            **
;//****************************************************************
;void NET_exitMonitorMode(void)
;{
;    NET_sendMessage("QU\n");
;    while (!NET_RXReady() || NET_readByte() != '>') {}
;    NET_flushBuffers();
;}

;---------------------------------------------------------------------------
;// Allow Connections                                            **
;//****************************************************************
;// Allows inbound TCP connections on port 5364
;void NET_allowConnections(void)
;{
;    UART_MCR = 0x08;
;    while(UART_MCR != 0x08);
;    return;
;}

;---------------------------------------------------------------------------
;// Block Connections                                            **
;//****************************************************************
;// Drops any current connection and blocks future inbound connections
;void NET_BlockConnections(void)
;{
;    UART_MCR = 0x00;
;    while(UART_MCR != 0x00);
;    return;
;}

;---------------------------------------------------------------------------
;// Reboot Adapter                                               **
;//****************************************************************
;// Reboots Xpico and waits until its back up before returning
;void NET_resetAdapter(void)
;{
;    NET_enterMonitorMode();
;    NET_sendMessage("RS\n");
;    while(1)
;    {
;        while(!NET_RXReady());
;        u8 byte = NET_readByte();
;        if(byte == 'D') { break; }
;    }
;    return;
;}

;---------------------------------------------------------------------------
;// Connect                                                      **
;//****************************************************************
;// Make an outbound TCP connection to supplied DNS/IP
;void NET_connect(int x, int y, char *str)
;{
;    NET_sendByte('C');
;    NET_sendMessage(str);
;    NET_sendByte(0x0A);;;

;    while(!NET_RXReady());
;    u8 byte = NET_readByte();
;    switch(byte)
;    {
;        case 'C': // Connected
;            consoleDrawText(x, y, "Connected:"); consoleDrawText(x+11, y, str);
;            break;
;        case 'N': // Host Unreachable
;            consoleDrawText(x, y, "Error: Host unreachable");
;            NET_flushBuffers();
;            break;
;    }
;}

;---------------------------------------------------------------------------
;// Print IP                                                     **
;//****************************************************************
;// Prints IP address of cartridge adapter
;void NET_printIP(int x, int y)
;{
;    NET_enterMonitorMode();
;    NET_sendMessage("NC\n"); // Send command to get network information
;    while(1)
;    {
;        while(!NET_RXReady());
;        u8 byte = NET_readByte();
;        if(byte == 'G') { break; }
;        if ((byte >= '0' && byte <= '9') || byte == '.' || byte == '1')
;        {
;;           sprintf(str, "%c", byte); consoleDrawText(x, y, str); x++;
;        }
;    }
;    NET_exitMonitorMode();
;}

;---------------------------------------------------------------------------
;// Print MAC Address                                            **
;//****************************************************************
;// Prints MAC address of cartridge hardware (Xpico)
;void NET_printMAC(int x, int y)
;{
;    int i;

;    NET_enterMonitorMode();
;    NET_sendMessage("GM\n");
;    for(i=1; i<22; i++)
;    {
;        while(!NET_RXReady());
;        u8 byte = NET_readByte();
;        if(i>4) { sprintf(str, "%c", byte); consoleDrawText(x, y, str); x++; }
;    }
;    NET_exitMonitorMode();
;}

;---------------------------------------------------------------------------
;// Ping IP Address                                              **
;//****************************************************************
;// Only accepts an IP address to ping
;void NET_pingIP(int x, int y, int ping_count, char *ip)
;{
;    int ping_counter = 0;
;    int byte_count = 0;
;    int tmp = x;;

;    consoleDrawText(x, y, "Pinging:"); x+=8;

;    NET_enterMonitorMode();
;    NET_sendMessage("PI ");
;    NET_sendMessage(ip);
;    NET_sendMessage("\n");

;    while(1)
;    {
;        while(!NET_RXReady());
;        u8 byte = NET_readByte();
;        byte_count++;
;        if(byte_count > 2)
;        {
;            sprintf(str, "%c", byte);
;            consoleDrawText(x, y, str); x++;
;            if(byte == '\n') { x=tmp; y++; ping_counter++; }
;            if(ping_counter >= ping_count+1) { break; }
;        }
;    }
;    NET_exitMonitorMode();
;}

.ENDS

