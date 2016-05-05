#ifndef INCLUDE_WII_H_
#define INCLUDE_WII_H_
/*
 * File  : wiinunchuk.h V0.9
 * Author: Tim Teatro
 * Date  : Feb 2012
 *
 * https://github.com/timtro/wiinunchuck-h
 * Description:
 *
 *   Library to set up and poll a Wii nunchuk with Arduino. There are
 * many libraries available to do this, none of which I really liked.
 * I was fond of Tod Kurt's, but his was incomplete as it did not work
 * with knockoff nunchuks, it did not consider the least significant
 * bits of accelerometer data and didn't have any advanced functions
 * for processing the data such as calculating pitch and roll angles.
 *
 *
 * Provides functions:
 *  void nunchuk_setpowerpins()
 *  void nunchuk_init()
 *  int nunchuk_get_data()
 *  void nunchuk_calibrate_joy()
 *  inline unsigned int nunchuk_zbutton()
 *  inline unsigned int nunchuk_cbutton()
 *  inline int nunchuk_joy_x()
 *  inline int nunchuk_cjoy_x()
 *  inline int nunchuk_cjoy_y()
 *  inline uint16_t nunchuk_accelx()
 *  inline uint16_t nunchuk_accely()
 *  inline uint16_t nunchuk_accelz()
 *  inline int nunchuk_caccelx()
 *  inline int nunchuk_caccely()
 *  inline int nunchuk_caccelz()
 *  inline int nunchuk_joyangle()
 *  inline int nunchuk_rollangle()
 *  inline int nunchuk_pitchangle()
 *  void nunchuk_calibrate_accelxy()
 *  void nunchuk_calibrate_accelz()
 *
 * This library is inspired by the work of Tod E. Kurt,
 *	(http://todbot.com/blog/bionicarduino/)
 *
 * (c) 2012 by Tim Teatro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//#if (ARDUINO >= 100)
//#include <Arduino.h>
//#else
//#include <WProgram.h>
//#endif


//https://bitbucket.org/mitov/visuino-libraries/src/e56ec5eafe8c550b56d26cbb527da63d58b5fa82/Mitov_Wii_Controller.h?fileviewer=file-view-default

#define M_PI 3.141592653589793238462643

//
// These are suitable defaults for most nunchuks, including knockoffs.
// If you intend to use the same nunchuk all the time and demand accu-
// racy, it is worth your time to measure these on your own.
//     If you may want to use various nunchuks, you may want to
// calibrate using functions
//   nunchuk_calibrate_joy()
//   nunchuk_calibrate_accelxy()
//   nunchuk_calibrate_accelz()
//
#define DEFAULT_CENTRE_JOY_X 124
#define DEFAULT_CENTRE_JOY_Y 132
#define ACCEL_ZEROX 490
#define ACCEL_ZEROY 500
#define ACCEL_ZEROZ 525

class WiiNunchucks {
public:
	WiiNunchucks(){
		debugf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

	}
	//
	//
	// Initialize and join the I2C bus, and tell the nunchuk we're
	// talking to it. This function will work both with Nintendo
	// nunchuks, or knockoffs.
	//
	// See http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1264805255
	//
	void init()
	{
//		Wire.begin();
//		delay(1);
//		Wire.beginTransmission(0x52);  // device address
//		Wire.write((uint8_t)0xF0);  // 1st initialisation register
//		Wire.write((uint8_t)0x55);  // 1st initialisation value
//		Wire.endTransmission();
//		delay(1);
//		Wire.beginTransmission(0x52);
//		Wire.write((uint8_t)0xFB);  // 2nd initialisation register
//		Wire.write((uint8_t)0x00);  // 2nd initialisation value
//		Wire.endTransmission();
//		delay(1);


//		Wire.write (0x40);		  // sends memory address
//		  Wire.write (0x00);		  // sends sent a zero.
//		  Wire.endTransmission ();


		///////////

		byte cnt;
		debugf("************************************* statrting init");

		Wire.begin();

		// init controller
		delay(1);
		Wire.beginTransmission(0x52);      // device address
		Wire.write(0xF0);                    // 1st initialisation register
		Wire.write(0x55);                    // 1st initialisation value
		Wire.endTransmission();
		delay(1);
		Wire.beginTransmission(0x52);
		Wire.write(0xFB);                    // 2nd initialisation register
		Wire.write(0x00);                    // 2nd initialisation value
		Wire.endTransmission();
		delay(1);

		// read the extension type from the register block
		Wire.beginTransmission(0x52);
		Wire.write(0xFA);                    // extension type register
		Wire.endTransmission();
		Wire.beginTransmission(0x52);
		Wire.requestFrom(0x52, 6);               // request data from controller
		for (cnt = 0; cnt < 6; cnt++) {
		   if (Wire.available()) {
		       ctrlr_type[cnt] = Wire.read(); // Should be 0x0000 A420 0101 for Classic Controller, 0x0000 A420 0000 for nunchuck
//		       debugf("wire - %i", ctrlr_type[cnt]);
		   }
		}
//		 debugf("wire 1- %i", ctrlr_type[4]);
		Wire.endTransmission();
		delay(1);

		// send the crypto key (zeros), in 3 blocks of 6, 6 & 4.
		Wire.beginTransmission(0x52);
		Wire.write(0xF0);                    // crypto key command register
		Wire.write(0xAA);                    // sends crypto enable notice
		Wire.endTransmission();
		delay(1);
		Wire.beginTransmission(0x52);
		Wire.write(0x40);                    // crypto key data address
		for (cnt = 0; cnt < 6; cnt++) {
		   Wire.write(0x00);                    // sends 1st key block (zeros)
		}
		Wire.endTransmission();
		Wire.beginTransmission(0x52);
		Wire.write(0x40);                    // sends memory address
		for (cnt = 6; cnt < 12; cnt++) {
		   Wire.write(0x00);                    // sends 2nd key block (zeros)
		}
		Wire.endTransmission();
		Wire.beginTransmission(0x52);
		Wire.write(0x40);                    // sends memory address
		for (cnt = 12; cnt < 16; cnt++) {
		   Wire.write(0x00);                    // sends 3rd key block (zeros)
		}
		Wire.endTransmission();
		delay(1);
		// end device init
		/////////////////
		//
		// Set default calibration centres:
		//
		joy_zerox = DEFAULT_CENTRE_JOY_X;
		joy_zeroy = DEFAULT_CENTRE_JOY_Y;
		accel_zerox = ACCEL_ZEROX;
		accel_zeroy = ACCEL_ZEROY;
		accel_zeroz = ACCEL_ZEROZ;
	}

	String getDirection() {
		String ret;
		if (nunchuk_get_data() == 1) {
			char buf[100];
//			printf("x=%s", String(Float(nunchuk_joy_x())));
			sprintf(buf, "x=%d,y=%d, cx=%i, cy=%i,cbtn=%i,zbtn=%i,angle=%i", nunchuk_joy_x(),
					nunchuk_joy_y(), nunchuk_cjoy_x(),
					nunchuk_cjoy_y(), nunchuk_cbutton(), nunchuk_zbutton(), nunchuk_joyangle());
			ret = String(buf);
//			delete buf;
		}
		return ret;
	}

	//
	//
	// T.T.
	// Standard nunchuks use a byte-wise encryption using bit-wise XOR
	// with 0x17. This function decodes a byte.
	//
	// This function is not needed since the way we initialize the nunchuk
	// does not XOR encrypt the bits.
	//
//	static inline char nunchuk_decode_byte (char x)
//	{
//		x = (x ^ 0x17) + 0x17;
//		return x;
//	}

	static void nunchuk_send_request()
	{
		Wire.beginTransmission(0x52);// transmit to device 0x52
		Wire.write((uint8_t)0x00);// sends one byte
		Wire.endTransmission();// stop transmitting
	}

	//
	//
	// Gets data from the nunchuk and packs it into the nunchuk_buff byte
	// aray. That array will be processed by other functions to extract
	// the data from the sensors and analyse.
	//
	int nunchuk_get_data()
	{
		int cnt=0;
		  // Request six bytes from the chuck.
		Wire.requestFrom (0x52, 6);
		while (Wire.available ())
		{
		  // receive byte as an integer
			nunchuk_buf[cnt] = Wire.read();
			cnt++;
		}

		Wire.beginTransmission(0x52);// transmit to device 0x52
		Wire.write((uint8_t)0x00);// sends one byte
		Wire.endTransmission();// stop transmitting

		if (cnt >= 5)
			{
			return 1;   // success
			}
		return 0; // failure
	}

	// Calibrate joystick so that we read the centre position as (0,0).
	// Otherwise, we use the default values from the header.

	void nunchuk_calibrate_joy()
	{
		debugf("!!!!!!!!!!!!!!!!!!!!!!!!!!ßjoy_x=%i, joy_y=%i", joy_x, joy_y);
		joy_zerox = joy_x;
		joy_zeroy = joy_y;
	}

	// Returns c and z button states: 1=pressed, 0=not
	// The state is in the two least significant bits of the 6th byte.
	// In the data, a 1 is unpressed and 0 is pressed, so this will be
	// reversed. These functions use a bitwise AND to determine the value
	// and then the () ? true : false; conditional structure to pass out
	// the appropriate state.

	unsigned int nunchuk_zbutton()
	{
		return ((nunchuk_buf[5] >> 0) & 1) ? 0 : 1;
	}

	unsigned int nunchuk_cbutton()
	{
		return ((nunchuk_buf[5] >> 1) & 1) ? 0 : 1;
	}

	// Returns the raw x and y values of the the joystick, cast as ints.
	int nunchuk_joy_x()
	{
		return (float)nunchuk_buf[0] ;
	}

	int nunchuk_joy_y()
	{
		return (float)nunchuk_buf[1];
	}

	//
	//
	// Return calibrated x and y values of the joystick.
	//
	uint8_t nunchuk_cjoy_x()
	{
		return nunchuk_buf[0] - joy_zerox;
	}

	uint8_t nunchuk_cjoy_y()
	{
		return nunchuk_buf[1] - joy_zeroy;
	}

	//
	//
	// Returns the raw 10-bit values from the 3-axis accelerometer sensor.
	// Of the six bytes recieved in a data payload from the nunchuk, bytes
	// 2, 3 and 4 are the most significant 8 bits of each 10-bit reading.
	// The final two bits are stored in the 6th bit along with the states
	// of the c and z button. These functions take the most significant
	// 8-bits and stacks it into a 16 bit unsigned integer, and then tacks
	// on the least significant bits from the 6th byte of the data
	// payload.
	//
	// Load the most sig digits into a blank 16-bit unsigned int leaving
	// two bits in the bottom ( via a 2-bit shift, << 2) for the least sig
	// bits:
	// 	0x0000 | nunchuk_buff[*] << 2
	// Add to the above, the least sig bits. The code for x:
	// 	nunchuk_buf[5] & B00001100
	// for example selects the 3rd and 4th bits from the 6th byte of the
	// payload to be concatinated with nunchuk_buff[2] to complete the 10-
	// bit datum for a given axis.
	//
	 uint16_t nunchuk_accelx()
	{
		return (  0x0000 | ( nunchuk_buf[2] << 2 ) +
			( ( nunchuk_buf[5] & B00001100 ) >> 2 )  );
	}
	//
	 uint16_t nunchuk_accely()
	{
		return (  0x0000 ^ ( nunchuk_buf[3] << 2 ) +
			( ( nunchuk_buf[5] & B00110000 ) >> 4 )  );
	}

	uint16_t nunchuk_accelz()
	{
		return (  0x0000 ^ ( nunchuk_buf[4] << 2 ) +
			( ( nunchuk_buf[5] & B11000000 ) >> 6 )  );
	}

	//
	//
	// Returns the x,y and z accelerometer values with calibration values
	// subtracted.

	int nunchuk_caccelx()
		{
			return (int)(nunchuk_accelx() - accel_zerox);
		}

	int nunchuk_caccely()
		{
			return (int)(nunchuk_accely() - accel_zeroy);
		}

	int nunchuk_caccelz()
		{
			return (int)(nunchuk_accelz() - accel_zeroz);
		}

	//
	//
	// Returns joystick angle in degrees. It uses the ratio of calibrated
	// x and y potentiometer readings to find the angle, zero being direct
	// right (positive x) and measured counter-clockwise from there.
	//
	// If the atan2 function returns a negative angle, it is rotated back
	// into a positive angle. For those unfamiliar, the atan2 function
	// is a more inteligent atan function which quadrant the vector <x,y>
	// is in, and returns the appropriate angle.
	//
	int nunchuk_joyangle()
	{
		double theta;
		theta = atan2( nunchuk_cjoy_y(), nunchuk_cjoy_x() );
		while (theta < 0) theta += 2*M_PI;
		return (int)(theta * 180/M_PI);
	}

	//
	//
	// Returns roll angle in degrees. Under the assumption that the
	// only acceleration detected by the accelerometer is acceleration due
	// to gravity, this function uses the ratio of the x and z
	// accelerometer readings to gauge pitch. This only works while the
	// nunchuk is being held still or at constant velocity with zero ext-
	// ernal force.
	//
	int nunchuk_rollangle()
	{
		return (int) (  atan2( (double) nunchuk_caccelx(),
			(double) nunchuk_caccelz() ) * 180 / M_PI  );
	}

	//
	//
	// Returns pitch angle in degrees. Under the assumption that the
	// only acceleration detected by the accelerometer is acceleration due
	// to gravity, this function uses the ratio of the y and z
	// accelerometer readings to gauge pitch.  This only works while the
	// nunchuk is being held still or at constant velocity with zero ext-
	// ernal force.
	//
	int nunchuk_pitchangle()
	{
		return (int) (  atan2( (double) nunchuk_caccely(),
			(double)nunchuk_caccelz() ) * 180 / M_PI  );
	}

	//
	//
	// Because gravity pulls down on the z-accelerometer while the nunchuk
	// is upright, we need to calibrate {x,y} and {z} separately. Execute
	// this function while the nunchuk is known to be upright and then
	// execute nunchuk_calibrate_accelz() when the nunchuk is on its side.
	//
	void nunchuk_calibrate_accelxy()
	{
		accel_zerox = nunchuk_accelx();
		accel_zeroy = nunchuk_accely();
	}

	//
	//
	// See documentation for nunchuk_calibrate_xy()
	//
	void nunchuk_calibrate_accelz()
	{
		accel_zeroz = nunchuk_accelz();
	}
private:
	//
		// Global vars are kept to a minimum.
		//
		uint8_t ctrlr_type[6];   // Used externally?
		uint8_t nunchuk_buf[6];  // Keeps data payload from nunchuk
		// Accelerometer values and callibration centres:
		uint16_t accel_zerox, accel_zeroy, accel_zeroz;
		// Joystick values and calibration centres:
		int joy_x, joy_y, joy_zerox, joy_zeroy;
};



// EOF

#endif /* INCLUDE_WII_H_ */
