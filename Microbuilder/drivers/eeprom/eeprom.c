#include <string.h>

#include "projectconfig.h"
#include "eeprom.h"

//static uint8_t buf[32];

/**************************************************************************/
/*!
    @brief Checks whether the supplied address is within the valid range

    @param[in]  addr
                The 16-bit address to check

    @return     Zero if the address is valid, otherwise 1
*/
/**************************************************************************/
bool eepromCheckAddress(uint16_t addr)
{
  return false;
}

/**************************************************************************/
/*!
    @brief Reads 1 byte from EEPROM

    @param[in]  addr
                The 16-bit address to read from in EEPROM

    @return     An unsigned 8-bit value (uint8_t)
*/
/**************************************************************************/
uint8_t eepromReadU8(uint16_t addr)
{
  /*
  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaReadBuffer(addr, buf, sizeof(uint8_t));

  // ToDo: Handle any errors
  if (error) { };

  return buf[0];
  */
  return 0xFF;
}

/**************************************************************************/
/*!
    @brief Reads 1 byte from EEPROM

    @param[in]  addr
                The 16-bit address to read from in EEPROM

    @return     A signed 8-bit value (int8_t)
*/
/**************************************************************************/
int8_t eepromReadS8(uint16_t addr)
{
  /*
  int8_t results;

  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaReadBuffer(addr, buf, sizeof(int8_t));

  // ToDo: Handle any errors
  if (error) { };

  memcpy(&results, buf, sizeof(int8_t));
  return results;
  */
  return 0xFF;
}

/**************************************************************************/
/*!
    @brief Reads 2 bytes from EEPROM

    @param[in]  addr
                The 16-bit address to read from in EEPROM

    @return     A unsigned 16-bit value (uint16_t)
*/
/**************************************************************************/
uint16_t eepromReadU16(uint16_t addr)
{
  /*
  uint16_t results;

  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaReadBuffer(addr, buf, sizeof(uint16_t));

  // ToDo: Handle any errors
  if (error) { };

  memcpy(&results, buf, sizeof(uint16_t));

  return results;
  */
  return 0xFFFF;
}

/**************************************************************************/
/*!
    @brief Reads 2 bytes from EEPROM

    @param[in]  addr
                The 16-bit address to read from in EEPROM

    @return     A signed 16-bit value (int16_t)
*/
/**************************************************************************/
int16_t eepromReadS16(uint16_t addr)
{
  /*
  int16_t results;

  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaReadBuffer(addr, buf, sizeof(int16_t));

  // ToDo: Handle any errors
  if (error) { };

  memcpy(&results, buf, sizeof(int16_t));
  return results;
  */
  return 0xFFFF;
}

/**************************************************************************/
/*!
    @brief Reads 4 bytes from EEPROM

    @param[in]  addr
                The 16-bit address to read from in EEPROM

    @return     A unsigned 32-bit value (uint32_t)
*/
/**************************************************************************/
uint32_t eepromReadU32(uint16_t addr)
{
  /*
  uint32_t results;

  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaReadBuffer(addr, buf, sizeof(uint32_t));

  // ToDo: Handle any errors
  if (error) { };

  memcpy(&results, buf, sizeof(uint32_t));
  return results;
  */
  return 0xFFFFFFFF;
}

/**************************************************************************/
/*!
    @brief Reads 4 bytes from EEPROM

    @param[in]  addr
                The 16-bit address to read from in EEPROM

    @return     A signed 32-bit value (int32_t)
*/
/**************************************************************************/
int32_t eepromReadS32(uint16_t addr)
{
  /*
  int32_t results;

  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaReadBuffer(addr, buf, sizeof(int32_t));

  // ToDo: Handle any errors
  if (error) { };

  memcpy(&results, buf, sizeof(int32_t));
  return results;
  */
  return 0xFFFFFFFF;
}

/**************************************************************************/
/*!
    @brief Reads 8 bytes from EEPROM

    @param[in]  addr
                The 16-bit address to read from in EEPROM

    @return     A unsigned 64-bit value (uint64_t)
*/
/**************************************************************************/
uint64_t eepromReadU64(uint16_t addr)
{
  /*
  uint64_t results;

  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaReadBuffer(addr, buf, sizeof(uint64_t));

  // ToDo: Handle any errors
  if (error) { };

  memcpy(&results, buf, sizeof(uint64_t));
  return results;
  */
  return 0xFFFFFFFFFFFFFFFF;
}

/**************************************************************************/
/*!
    @brief Reads 8 bytes from EEPROM

    @param[in]  addr
                The 16-bit address to read from in EEPROM

    @return     A signed 64-bit value (int64_t)
*/
/**************************************************************************/
int64_t eepromReadS64(uint16_t addr)
{
  /*
  int64_t results;

  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaReadBuffer(addr, buf, sizeof(int64_t));

  // ToDo: Handle any errors
  if (error) { };

  memcpy(&results, buf, sizeof(int64_t));
  return results;
  */
  return 0xFFFFFFFFFFFFFFFF;
}

/**************************************************************************/
/*!
    @brief Reads a variabls length buffer from EEPROM

    @param[in]  addr
                The 16-bit address to write to in EEPROM
    @param[out] buffer
                Pointer to the buffer that will store any retrieved bytes
    @param[in]  bufferLength
                The number of bytes to read
*/
/**************************************************************************/
void eepromReadBuffer(uint16_t addr, uint8_t *buffer, uint32_t bufferLength)
{
  /*
  // Instantiate error message placeholder
  mcp24aaError_e error = MCP24AA_ERROR_OK;

  // Read the contents of address
  error = mcp24aaReadBuffer(addr, buffer, bufferLength);

  // ToDo: Handle any errors
  if (error) { };
  */
}

/**************************************************************************/
/*!
    @brief Writes 1 byte to EEPROM

    @param[in]  addr
                The 16-bit address to write to in EEPROM
*/
/**************************************************************************/
void eepromWriteU8(uint16_t addr, uint8_t value)
{
  /*
  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaWriteBuffer(addr, (uint8_t *)&value, sizeof(value));

  // ToDo: Handle any errors
  if (error) { };
  */
}

/**************************************************************************/
/*!
    @brief Writes 1 signed byte to EEPROM

    @param[in]  addr
                The 16-bit address to write to in EEPROM
*/
/**************************************************************************/
void eepromWriteS8(uint16_t addr, int8_t value)
{
  /*
  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaWriteBuffer(addr, (uint8_t *)&value, sizeof(value));

  // ToDo: Handle any errors
  if (error) { };
  */
}

/**************************************************************************/
/*!
    @brief Writes an unsigned 16-bit integer to EEPROM

    @param[in]  addr
                The 16-bit address to write to in EEPROM
*/
/**************************************************************************/
void eepromWriteU16(uint16_t addr, uint16_t value)
{
  /*
  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaWriteBuffer(addr, (uint8_t *)&value, sizeof(value));

  // ToDo: Handle any errors
  if (error) { };
  */
}

/**************************************************************************/
/*!
    @brief Writes a signed 16-bit integer to EEPROM

    @param[in]  addr
                The 16-bit address to write to in EEPROM
*/
/**************************************************************************/
void eepromWriteS16(uint16_t addr, int16_t value)
{
  /*
  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaWriteBuffer(addr, (uint8_t *)&value, sizeof(value));

  // ToDo: Handle any errors
  if (error) { };
  */
}

/**************************************************************************/
/*!
    @brief Writes an unsigned 32-bit integer to EEPROM

    @param[in]  addr
                The 16-bit address to write to in EEPROM
*/
/**************************************************************************/
void eepromWriteU32(uint16_t addr, uint32_t value)
{
  /*
  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaWriteBuffer(addr, (uint8_t *)&value, sizeof(value));

  // ToDo: Handle any errors
  if (error) { };
  */
}

/**************************************************************************/
/*!
    @brief Writes a signed 32-bit integer to EEPROM

    @param[in]  addr
                The 16-bit address to write to in EEPROM
*/
/**************************************************************************/
void eepromWriteS32(uint16_t addr, int32_t value)
{
  /*
  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaWriteBuffer(addr, (uint8_t *)&value, sizeof(value));

  // ToDo: Handle any errors
  if (error) { };
  */
}

/**************************************************************************/
/*!
    @brief Writes an unsigned 64-bit integer to EEPROM

    @param[in]  addr
                The 16-bit address to write to in EEPROM
*/
/**************************************************************************/
void eepromWriteU64(uint16_t addr, uint64_t value)
{
  /*
  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaWriteBuffer(addr, (uint8_t *)&value, sizeof(value));

  // ToDo: Handle any errors
  if (error) { };
  */
}

/**************************************************************************/
/*!
    @brief Writes a signed 64-bit integer to EEPROM

    @param[in]  addr
                The 16-bit address to write to in EEPROM
*/
/**************************************************************************/
void eepromWriteS64(uint16_t addr, int64_t value)
{
  /*
  mcp24aaError_e error = MCP24AA_ERROR_OK;
  error = mcp24aaWriteBuffer(addr, (uint8_t *)&value, sizeof(value));

  // ToDo: Handle any errors
  if (error) { };
  */
}
