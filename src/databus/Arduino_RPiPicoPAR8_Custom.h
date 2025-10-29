#if defined(TARGET_RP2040) || defined(PICO_RP2350)

#ifndef _ARDUINO_RPIPICOPAR8_CUSTOM_H_
#define _ARDUINO_RPIPICOPAR8_CUSTOM_H_

#include "Arduino_DataBus.h"

class Arduino_RPiPicoPAR8_Custom : public Arduino_DataBus
{
public:
  Arduino_RPiPicoPAR8_Custom(int8_t dc, int8_t cs, int8_t wr, int8_t rd, int8_t d0, int8_t d1, int8_t d2, int8_t d3, int8_t d4, int8_t d5, int8_t d6, int8_t d7); // Constructor

  bool begin(int32_t speed = GFX_NOT_DEFINED, int8_t dataMode = GFX_NOT_DEFINED) override;
  void beginWrite() override;
  void endWrite() override;
  void writeCommand(uint8_t) override;
  void writeCommand16(uint16_t) override;
  void writeCommandBytes(uint8_t *data, uint32_t len) override;
  void write(uint8_t) override;
  void write16(uint16_t) override;
  void writeRepeat(uint16_t p, uint32_t len) override;
  void writePixels(uint16_t *data, uint32_t len) override;

  void writeC8D8(uint8_t c, uint8_t d) override;
  void writeC8D16(uint8_t c, uint16_t d) override;
  void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2) override;
  void writeBytes(uint8_t *data, uint32_t len) override;

  void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len) override;
  void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len) override;

protected:
private:
  GFX_INLINE void WRITE(uint8_t d);
  GFX_INLINE void DC_HIGH(void);
  GFX_INLINE void DC_LOW(void);
  GFX_INLINE void CS_HIGH(void);
  GFX_INLINE void CS_LOW(void);

  int8_t _dc, _cs, _wr, _rd;
  int8_t _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7;
  int8_t _dataPins[8];
  uint32_t _bitMaskLUT[256];

  
  uint32_t _dcPinMask;  ///< Bitmask
  uint32_t _csPinMask;  ///< Bitmask
  uint32_t _wrPinMask;  ///< Bitmask
  uint32_t _dataClrMask;
};

#endif // _ARDUINO_RPIPICOPAR8_CUSTOM_H_

#endif // #if defined(TARGET_RP2040) || defined(PICO_RP2350)
