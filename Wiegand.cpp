#include "Wiegand.h"

unsigned long WIEGAND::_cardTempHigh=0;
unsigned long WIEGAND::_cardTemp=0;
unsigned long WIEGAND::_lastWiegand=0;
unsigned long WIEGAND::_sysTick=0;
unsigned long WIEGAND::_code=0;
int 		  WIEGAND::_bitCount=0;	
int			  WIEGAND::_wiegandType=0;

WIEGAND::WIEGAND()
{
}

unsigned long WIEGAND::getCode()
{
	return _code;
}

int WIEGAND::getWiegandType()
{
	return _wiegandType;
}

bool WIEGAND::available()
{
	return DoWiegandConversion();
}

void WIEGAND::begin()
{
	_lastWiegand = 0;
	_cardTempHigh = 0;
	_cardTemp = 0;
	_code = 0;
	_wiegandType = 0;
	_bitCount = 0;  
	_sysTick=millis();
	pinMode(D0Pin, INPUT);					// Set D0 pin as input
	pinMode(D1Pin, INPUT);					// Set D1 pin as input
	attachInterrupt(0, ReadD0, FALLING);	// Hardware interrupt - high to low pulse
	attachInterrupt(1, ReadD1, FALLING);	// Hardware interrupt - high to low pulse
}

void WIEGAND::ReadD0 ()
{
	_bitCount++;				// Increament bit count for Interrupt connected to D0
	if (_bitCount>31)			// If bit count more than 31, process high bits
	{
		_cardTempHigh |= ((0x80000000 & _cardTemp)>>31);	//	shift value to high bits
		_cardTempHigh <<= 1;
		_cardTemp <<=1;
	}
	else
	{
		_cardTemp <<= 1;		// D0 represent binary 0, so just left shift card data
	}
	_lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}

void WIEGAND::ReadD1()
{
	_bitCount ++;				// Increment bit count for Interrupt connected to D1
	if (_bitCount>31)			// If bit count more than 31, process high bits
	{
		_cardTempHigh |= ((0x80000000 & _cardTemp)>>31);	// shift value to high bits
		_cardTempHigh <<= 1;
		_cardTemp |= 1;
		_cardTemp <<=1;
	}
	else
	{
		_cardTemp |= 1;			// D1 represent binary 1, so OR card data with 1 then
		_cardTemp <<= 1;		// left shift card data
	}
	_lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}

unsigned long WIEGAND::GetCardId (unsigned long *codehigh, unsigned long *codelow, char bitlength)
{
	unsigned long cardID=0;

	if (bitlength==26)								// EM tag
		cardID = (*codelow & 0x1FFFFFE) >>1;

	if (bitlength==34)								// Mifare 
	{
		*codehigh = *codehigh & 0x03;				// only need the 2 LSB of the codehigh
		*codehigh <<= 30;							// shift 2 LSB to MSB		
		*codelow >>=1;
		cardID = *codehigh | *codelow;
	}
	return cardID;
}

bool WIEGAND::DoWiegandConversion ()
{
	unsigned long cardID;
	
	_sysTick=millis();
	if ((_sysTick - _lastWiegand) > 25)								// if no more signal coming through after 25ms
	{
		if ((_bitCount==26) || (_bitCount==34) || (_bitCount==8)) 	// bitCount for keypress=8, Wiegand 26=26, Wiegand 34=34
		{
			_cardTemp >>= 1;			// shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
			if (_bitCount>32)			// bit count more than 32 bits, shift high bits right to make adjustment
				_cardTempHigh >>= 1;	

			if((_bitCount==26) || (_bitCount==34))		// wiegand 26 or wiegand 34
			{
				cardID = GetCardId (&_cardTempHigh, &_cardTemp, _bitCount);
				_wiegandType=_bitCount;
				_bitCount=0;
				_cardTemp=0;
				_cardTempHigh=0;
				_code=cardID;
				return true;				
			}
			else if (_bitCount==8)		// keypress wiegand
			{
				// 8-bit Wiegand keyboard data, high nibble is the "NOT" of low nibble
				// eg if key 1 pressed, data=E1 in binary 11100001 , high nibble=1110 , low nibble = 0001 
				char highNibble = (_cardTemp & 0xf0) >>4;
				char lowNibble = (_cardTemp & 0x0f);
				_wiegandType=_bitCount;					
				_bitCount=0;
				_cardTemp=0;
				_cardTempHigh=0;
				
				if (lowNibble == (~highNibble & 0x0f))		// check if low nibble matches the "NOT" of high nibble.
				{
					if (lowNibble==0x0b)					// ENT pressed
					{
						_code=0x0d;							
					}
					else if (lowNibble==0x0a)				// ESC pressed
					{
						_code=0x1b;							
					}
					else
					{
						_code=(int)lowNibble;				// 0 - 9 keys
					}
					return true;
				}
			}
		}
		else
		{
			// well time over 25 ms and bitCount !=8 , !=26, !=34 , must be noise or nothing then.
			_lastWiegand=_sysTick;
			_bitCount=0;			
			_cardTemp=0;
			_cardTempHigh=0;
			return false;
		}	
	}
	else
		return false;
}