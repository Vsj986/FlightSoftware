//
//  QLocate.hpp
//  QLocate
//
//  Created by Kyle Krol (kpk63@cornell.edu) on 3/04/18.
//  Pathfinder for Autonomous Navigation
//  Cornell University
// 
// Updated 12/23/2019
// Refactored on 1/25/2020
//
#include "QLocate.hpp"
#ifndef DESKTOP
#include <Arduino.h>
#endif
using namespace Devices;

#ifdef DEBUG_ENABLED
#define qdebug_print(fmt, ...)\
    do{Serial.printf(fmt, __VA_ARGS__);} while(0)
#else
#define qdebug_print(fmt, ...)\
    do{} while(0)
#endif

#define CHECK_PORT_AVAILABLE() \
    if (!port->available()){    \
        return PORT_UNAVAILABLE;}

/*! QLocate implementation */
#ifdef DESKTOP
using F = std::string;
QLocate::QLocate(const std::string &name) : Device(name) {}
#else
QLocate::QLocate(const std::string &name, HardwareSerial *const port, int timeout)
    : Device(name), port(port), timeout(timeout){}
#endif

bool QLocate::setup()
{
#ifndef DESKTOP
    // Configure QLocate port settings
    this->port->begin(19200);
    // Initialize class variables
    this->port->setTimeout(timeout);
#endif
    return true;
}

int QLocate::query_is_functional_1()
{
    return sendCommand("AT\r");
}

int QLocate::get_is_functional()
{
    return consume(F("0\r"));
}

int QLocate::query_config_1()
{
    return sendCommand("AT&F0\r");
}

int QLocate::query_config_2()
{
#ifndef DESKTOP
    CHECK_PORT_AVAILABLE()
#endif
    // Disable flow control, disable DTR, disable echo, 
    // set numeric responses, and
    // disable "RING" alerts
    return sendCommand("AT&K0;&D0;E0;V0;+SBDMTA=0\r");
}

int QLocate::query_config_3()
{
    int status = consume(F("AT&K0;&D0;E0;V0;+SBDMTA=0\r0\r"));
    // Clear QLocate MO and MT buffers
    return (status != OK) ? status : sendCommand("AT+SBDD2\r");
}

int QLocate::get_config()
{
    return consume(F("0\r\n0\r"));
}

int QLocate::query_sbdwb_1(int len)
{
    // Check the length of the message.
    if (len <= 0 || len > MAX_MSG_SIZE)
        return WRONG_LENGTH;
#ifndef DESKTOP
    port->clear();
    if (port->printf("AT+SBDWB=%d\r", len) == 0)
        return WRITE_FAIL;
#endif
    return OK;
}

int QLocate::query_sbdwb_2(char const *c, int len)
{
    int status = consume(F("READY\r\n"));
    if (status != OK)
        return status;

    // Write binary data to QLocate
#ifndef DESKTOP
    if ( (size_t)len != port->write(c, len) )
        return WRITE_FAIL;
    uint16_t s = checksum(c, len);
    if ( port->write((uint8_t)(s >> 8u)) != 1 )
        return WRITE_FAIL;
    if ( port->write((uint8_t)s) != 1 )
        return WRITE_FAIL;
    // WARNING: this method blocks
    port->flush();
#endif
    return OK;
}

int QLocate::get_sbdwb()
{
#ifdef DESKTOP
    return OK;
#else
    // If it is a timeout, then port will not be available anyway
    CHECK_PORT_AVAILABLE()
    char buf[6];
    memset(buf, 0, 6);
    // expect to read <sbdwb status>\r\n0\r
    int len = port->readBytes(buf, 5);

#ifdef DEBUG_ENABLED
    Serial.print("        > get_SBDWB=");
    for (int i = 0; i < len; i++)
        Serial.print(buf[i], HEX);
    Serial.println("\n        > return=" + String(*buf));
#endif
    // SBDWB status is returned in buf[0]
    int status = buf[0] - '0';

    if (len < 3)
        return UNKNOWN;

    if (buf[1] != '\r' || buf[2] != '\n' || status > 3)
        return UNEXPECTED_RESPONSE;

    return status;
#endif
}

// Parses the result buffer of sbdix into sbdix_r
int QLocate::parse_ints(char const *c, int *i)
{
    int res = sscanf(c, "%d, %d, %d, %d, %d, %d\r", i, i + 1, i + 2, i + 3, i + 4, i + 5);
    if (res == 6)
        return OK;

#ifdef DEBUG_ENABLED
    Serial.printf("parse_ints: unexpected response %d\n", status);
#endif
    return UNEXPECTED_RESPONSE;
}

int QLocate::query_sbdix_1()
{
    // Request sbdix session
    return sendCommand("AT+SBDIX\r");
}

// Requires 15 seconds
int QLocate::get_sbdix()
{
#ifdef DESKTOP
    return OK;
#else
    CHECK_PORT_AVAILABLE()
    // Parse SBDIX output
    char buf[75];
    memset (buf, 0, 75);
    port->readBytesUntil('\n', buf, 74);
    // Clear SBDIX buffer before writing to it
    memset(sbdix_r, 0, 6*sizeof(int));
    return parse_ints(buf + 8, sbdix_r);
#endif
}

int QLocate::query_sbdrb_1()
{
    return sendCommand("AT+SBDRB\r");
}

int QLocate::get_sbdrb()
{

    CHECK_PORT_AVAILABLE()
    if ( should_wait_one_cycle() )
        return PORT_UNAVAILABLE;

    size_t msg_size = 256 * port->read() + port->read();
    if (msg_size > MAX_MSG_SIZE)
        return WRONG_LENGTH;

    // make sure port has our message + 2 byte checksum
    if ( (size_t)port->available() != msg_size + 2)
        return UNEXPECTED_RESPONSE;

    // we definitely have the correct number of bytes at port
    memset(mt_message, 0, MAX_MSG_SIZE+1);
    port->readBytes(mt_message, msg_size);

    // get the checksum
    uint16_t msg_checksum = 256*port->read() + port->read();

    // check the checksum
    if ( msg_checksum == checksum(mt_message, msg_size) )
        return OK;

    return BAD_CHECKSUM;


//#ifndef DESKTOP
//    CHECK_PORT_AVAILABLE()
//
//    if ( should_wait_one_cycle() )
//        return PORT_UNAVAILABLE;
//
//    unsigned char sbuf[3];
//    memset(sbuf, 0, 3);
//    // Read the message size
//    if (2 != port->readBytes(sbuf, 2))
//        return WRONG_LENGTH;
//    size_t size;
//    size = (sbuf[0u] << 8u) | sbuf[1u]; // highest order byte first
//    // Read the message
//    memset(mt_message, 0, MAX_MSG_SIZE);
//
//    size_t actual = port->readBytes(mt_message, size);
//#ifdef DEBUG_ENABLED
//    Serial.print("        > get_SBDRB=");
//    for (size_t i = 0; i < size; i++)
//        Serial.print(mt_message[i], HEX);
//    Serial.println("\n        > return=" + String(*mt_message));
//#endif
//    if (actual != size)
//    {
//#ifdef DEBUG_ENABLED
//        Serial.printf("Expected msg size: %d, actual size: %d", size, actual);
//#endif
//        return UNEXPECTED_RESPONSE;
//    }
//    // Read the checksum
//    memset(sbuf, 0, 3);
//    if (2 != port->readBytes(sbuf, 2))
//        return UNEXPECTED_RESPONSE;
//    uint16_t s = (sbuf[0u] << 8u) | sbuf[1u];
//    // Verify checksum
//    if ( s != checksum(mt_message, size))
//        return BAD_CHECKSUM;
//#endif
//    return OK;
}

bool QLocate::should_wait_one_cycle()
{
    did_wait_one_cycle = ~did_wait_one_cycle;
    // if we return true then that means we have not waited a cycle
    return did_wait_one_cycle;
}

int QLocate::consume(const String& expected)
{
    CHECK_PORT_AVAILABLE()
    // Edge case where we get unlucky and data is coming in right as we're checking for availability
    if ( should_wait_one_cycle() )
        return PORT_UNAVAILABLE;

    size_t expected_len = expected.length();
    // Make sure that the number of bytes at port == number of bytes expected
    if ( (size_t)port->available() < expected_len )
    {
        qdebug_print("consume failed with wrong length\n");
        return WRONG_LENGTH;
    }
    // If we have reached here, then we are certain that port->available() >= expected_len

    // read the bytes at port into rx_buf
    char rx_buf[expected_len + 1];
    port->readBytes(rx_buf, expected_len);
    rx_buf[expected_len] = 0;

    // Compare rx_buf with expected
    if( ! strncmp(rx_buf, expected.c_str(), expected_len) )
        return OK;

    qdebug_print("Consume expected %s but received %s\n",expected, rx_buf);
    return UNEXPECTED_RESPONSE;
}

// Read the data at port and make sure it matches expected
//int QLocate::consume(const String& expected)
//{
//#ifdef DESKTOP
//    return OK;
//#else
//    // Return if nothing at the port
//    CHECK_PORT_AVAILABLE()
//
//    size_t expectLength = expected.length();
//    char buf[expectLength + 1];
//    memset(buf, 0, expectLength+1);
//
//    // Read current data at port
//    size_t len = port->readBytes(buf, expectLength);
//
//#ifdef DEBUG_ENABLED
//    Serial.printf("Consumed[");
//    for (int i = 0; i < len; i++)
//    {
//        if (buf[i] == '\r')
//            Serial.printf("\\r");
//        else if (buf[i] == '\n')
//            Serial.printf("\\n");
//        else
//            Serial.printf("%c", buf[i]);
//    }
//    Serial.printf("]\n");
//    Serial.flush();
//#endif
//    port->clear();
//    // Did not read enough bytes
//    if (len != expectLength)
//        return WRONG_LENGTH;
//    // Data read does not match expected data
//    if ( ! expected.equals(String(buf)) )
//    {
//#ifdef DEBUG_ENABLED
//        Serial.printf("Consume expected: %s", expected);
//#endif
//        return UNEXPECTED_RESPONSE;
//    }
//    return OK;
//#endif
//}

int QLocate::sendCommand(const char *cmd)
{
#ifdef DESKTOP
return OK;
#else
    port->clear();
    // port->print returns the number of characters printed
    return (port->print(F(cmd)) != 0) ? OK : WRITE_FAIL;
#endif
}

// Calculate checksum
uint16_t QLocate::checksum(char const *c, size_t len)
{
    uint16_t checksum = 0;
    for (size_t i = 0; i < len; ++i)
    {
        checksum += (uint16_t)c[i];
    }
    return checksum;
//    short s = 0;
//    char const *const cf = c + len;
//    while (c < cf)
//        s += *(c++);
//
//    return s;
}

void QLocate::disable()
{
    // Do nothing; we really don't want to disable Quake
}
