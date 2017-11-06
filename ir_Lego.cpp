#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//                               L       GGGG
//                               L      G
//                               L      G  GG
//                               L      G   G
//                               LLLLL   GGG
//==============================================================================

#define Lego_BITS 16

#define Lego_HDR_MARK 156
#define Lego_HDR_SPACE 1014
#define Lego_BIT_MARK 156
#define Lego_ONE_SPACE 546
#define Lego_ZERO_SPACE 260
//#define Lego_RPT_LENGTH 60000

//+=============================================================================
#if DECODE_LEGO
bool  IRrecv::decodeLEGO (decode_results *results)
{
	long  data   = 0;
    int   offset = 1; // Skip first space


	// Check we have the right amount of data
    if (irparams.rawlen < (2 * Lego_BITS) + 1 )  return false ;

    // Initial mark/space
    if (!MATCH_MARK(results->rawbuf[offset++], Lego_HDR_MARK))  return false ;
    if (!MATCH_SPACE(results->rawbuf[offset++], Lego_HDR_SPACE))  return false ;

    // data bits
    for (int i = 0;  i < Lego_BITS;  i++) {
        if (!MATCH_MARK(results->rawbuf[offset++], Lego_BIT_MARK))  return false ;

        if      (MATCH_SPACE(results->rawbuf[offset], Lego_ONE_SPACE))
        {
			data = (data << 1) | 1 ;
        }
        else if (MATCH_SPACE(results->rawbuf[offset], Lego_ZERO_SPACE))
		{
        	data = (data << 1) | 0 ;
		}
        else                                                           return false ;
        offset++;
    }

    // Stop bit
    if (!MATCH_MARK(results->rawbuf[offset], Lego_BIT_MARK))   return false ;


    // until now we have a valid parsing of the infrared protocol. Now we need to check the LRC.
    /**
     * The lego pf ir protocol is devided into 3 nibbles and one LRC nibble.
     * The lrc is calculated by following formular: 0xF xor nibble1 xor nibble2 xor nibble3
     */

    // get LRC
    // we need 4th nibble -> use 0xF as bitmask and copy data bitwise and bitshift to get 4bit value
    int lrc = 0xF & data;

    // get nibble3
    int nibble3 = (0xF0 & data) >> 4;

    // get nibble2
    int nibble2 = (0xF00 & data) >> 8;

    // get nibble1
    int nibble1 = (0xF000 & data) >> 12;

    int lrcCheck = 0xF ^nibble1 ^ nibble2 ^ nibble3;

    if (lrcCheck != lrc)
    {
//    	Serial.println ("\nlrcCheck failed");
    	return false;
    }

    // Success
    results->bits        = Lego_BITS;
    results->value       = data;
    results->decode_type = LEGO;
    return true;
}
#endif

//+=============================================================================
/*#if SEND_Lego
void  IRsend::sendLG (unsigned long data,  int nbits)
{
    // Set IR carrier frequency
    enableIROut(38);

    // Header
    mark(Lego_HDR_MARK);
    space(Lego_HDR_SPACE);
    mark(Lego_BIT_MARK);

    // Data
    for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
        if (data & mask) {
            space(Lego_ONE_SPACE);
            mark(Lego_BIT_MARK);
        } else {
            space(Lego_ZERO_SPACE);
            mark(Lego_BIT_MARK);
        }
    }
    space(0);  // Always end with the LED off
}
#endif
*/
