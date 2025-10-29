#if defined(TARGET_RP2040) || defined(PICO_RP2350)

#include "Arduino_RPiPicoPAR8_Custom.h"

Arduino_RPiPicoPAR8_Custom::Arduino_RPiPicoPAR8_Custom(
    int8_t dc, int8_t cs, int8_t wr, int8_t rd,
    int8_t d0, int8_t d1, int8_t d2, int8_t d3,
    int8_t d4, int8_t d5, int8_t d6, int8_t d7)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd),
      _d0(d0), _d1(d1), _d2(d2), _d3(d3),
      _d4(d4), _d5(d5), _d6(d6), _d7(d7)
{
  _dataPins[0] = d0;
  _dataPins[1] = d1;
  _dataPins[2] = d2;
  _dataPins[3] = d3;
  _dataPins[4] = d4;
  _dataPins[5] = d5;
  _dataPins[6] = d6;
  _dataPins[7] = d7;
}

bool Arduino_RPiPicoPAR8_Custom::begin(int32_t speed, int8_t dataMode)
{
  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH);
  _dcPinMask = digitalPinToBitMask(_dc);

  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    _csPinMask = digitalPinToBitMask(_cs);
  }

  pinMode(_wr, OUTPUT);
  digitalWrite(_wr, HIGH);
  _wrPinMask = digitalPinToBitMask(_wr);

  // Initialize data pins
  _dataClrMask = _wrPinMask;
  for (int i = 0; i < 8; i++)
  {
    pinMode(_dataPins[i], OUTPUT);
    _dataClrMask |= (1u << _dataPins[i]);
  }

  // Build lookup table mapping byte -> GPIO mask
  for (int val = 0; val < 256; val++)
  {
    uint32_t mask = 0;
    for (int i = 0; i < 8; i++)
    {
      if (val & (1 << i))
      {
        mask |= (1u << _dataPins[i]);
      }
    }
    _bitMaskLUT[val] = mask;
  }

  // Clear all data pins
  sio_hw->gpio_clr = _dataClrMask;

  return true;
}

void Arduino_RPiPicoPAR8_Custom::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}
void Arduino_RPiPicoPAR8_Custom::endWrite() { CS_HIGH(); }

void Arduino_RPiPicoPAR8_Custom::writeCommand(uint8_t c)
{
  DC_LOW();
  WRITE(c);
  DC_HIGH();
}

void Arduino_RPiPicoPAR8_Custom::writeCommand16(uint16_t c)
{
  DC_LOW();
  _data16.value = c;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
  DC_HIGH();
}

void Arduino_RPiPicoPAR8_Custom::writeCommandBytes(uint8_t *data, uint32_t len)
{
  DC_LOW();
  while (len--)
    WRITE(*data++);
  DC_HIGH();
}

void Arduino_RPiPicoPAR8_Custom::write(uint8_t d) { WRITE(d); }

void Arduino_RPiPicoPAR8_Custom::write16(uint16_t d)
{
  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_RPiPicoPAR8_Custom::writeRepeat(uint16_t p, uint32_t len)
{
  _data16.value = p;
  while (len--)
  {
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

void Arduino_RPiPicoPAR8_Custom::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    _data16.value = *data++;
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

void Arduino_RPiPicoPAR8_Custom::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();
  WRITE(c);
  DC_HIGH();
  WRITE(d);
}

void Arduino_RPiPicoPAR8_Custom::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();
  WRITE(c);
  DC_HIGH();
  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_RPiPicoPAR8_Custom::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();
  WRITE(c);
  DC_HIGH();
  _data16.value = d1;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
  _data16.value = d2;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_RPiPicoPAR8_Custom::writeBytes(uint8_t *data, uint32_t len)
{
  while (len--)
    WRITE(*data++);
}

void Arduino_RPiPicoPAR8_Custom::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

void Arduino_RPiPicoPAR8_Custom::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

/************ WRITE macro ************/

// GFX_INLINE void Arduino_RPiPicoPAR8_Custom::WRITE(uint8_t d)
// {
//   // Clear all data pins
//   sio_hw->gpio_clr = _dataClrMask;

//   // Set only the bits that are 1
//   for (int i = 0; i < 8; i++)
//   {
//     if (d & (1 << i))
//     {
//       sio_hw->gpio_set = (1u << _dataPins[i]);
//     }
//   }

//   // Pulse WR
//   sio_hw->gpio_clr = _wrPinMask;
//   sio_hw->gpio_set = _wrPinMask;
// }

GFX_INLINE void Arduino_RPiPicoPAR8_Custom::WRITE(uint8_t d)
{
  sio_hw->gpio_clr = _dataClrMask;
  sio_hw->gpio_set = _bitMaskLUT[d];
  sio_hw->gpio_clr = _wrPinMask;
  sio_hw->gpio_set = _wrPinMask;
}

/******** low level bit twiddling **********/

GFX_INLINE void Arduino_RPiPicoPAR8_Custom::DC_HIGH(void) { sio_hw->gpio_set = _dcPinMask; }
GFX_INLINE void Arduino_RPiPicoPAR8_Custom::DC_LOW(void) { sio_hw->gpio_clr = _dcPinMask; }
GFX_INLINE void Arduino_RPiPicoPAR8_Custom::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
    sio_hw->gpio_set = _csPinMask;
}
GFX_INLINE void Arduino_RPiPicoPAR8_Custom::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
    sio_hw->gpio_clr = _csPinMask;
}

#endif // TARGET_RP2040
