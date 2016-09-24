#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "mavlink_types.h"
using namespace std;

class fixed_size // acutally this is not queue
{
public:
	fixed_size();
	float vx; //X velocity in NED frame in meter / s
	float vy; //Y velocity in NED frame in meter / s
	float vz; //Z velocity in NED frame in meter / s
	float afx; //X acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N
	float afy; //Y acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N
	float afz; //Z acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N
	float body_roll_rate; //	Body roll rate in radians per second
	float body_pitch_rate; //	Body pitch rate in radians per second
	float body_yaw_rate; //	Body yaw rate in radians per second
	float airspeed; //	Current airspeed in m / s
	float groundspeed; // Current ground speed in m / s
	uint16_t heading;	// Current heading in degrees, in compass units(0..360, 0 = north)
	uint16_t throttle; // Current throttle setting in integer percent, 0 to 100
	float alt;	// Current altitude(MSL), in meters
	float climb; // Current climb rate in meters / second
	float xacc; //	X acceleration(m / s ^ 2)
	float yacc; // Y acceleration(m / s ^ 2)
	float zacc; // Z acceleration(m / s ^ 2)
	float xgyro; // Angular speed around X axis(rad / sec)
	float ygyro; // Angular speed around Y axis(rad / sec)
	float zgyro; // Angular speed around Z axis(rad / sec)
	float pressure_alt; // Altitude calculated from pressure
		int16_t temperature;	// Temperature of the battery in centi - degrees celsius.INT16_MAX for unknown temperature.
		uint16_t voltages[10]; // Battery voltage of cells, in millivolts(1 = 1 millivolt).Cells above the valid cell count for this battery should have the UINT16_MAX value.
		int16_t current_battery;	// Battery current, in 10 * milliamperes(1 = 10 milliampere), -1: autopilot does not measure the current
		int32_t current_consumed; // Consumed charge, in milliampere hours(1 = 1 mAh), -1 : autopilot does not provide mAh consumption estimate
		int32_t energy_consumed; // Consumed energy, in 100 * Joules(intergrated U*I*dt) (1 = 100 Joule), -1 : autopilot does not provide energy consumption estimate
		int8_t battery_remaining; // Remaining battery energy : (0 % : 0, 100 % : 100), -1 : autopilot does not estimate the remaining battery

};

fixed_size::fixed_size() :
vx(-1),
vy(-1),
vz(-1),
afx(-1),
afy(-1),
afz(-1),
body_roll_rate(-1),
body_pitch_rate(-1),
body_yaw_rate(-1),
airspeed(-1),
heading(-1),
throttle(-1),
alt(-1),
climb(-1),
xacc(-1),
yacc(-1),
zacc(-1),
xgyro(-1),
ygyro(-1),
zgyro(-1),
pressure_alt(-1),
temperature(-1),
current_battery(-1),
current_consumed(-1),
energy_consumed(-1),
battery_remaining(-1){
	for (int i = 0; i<10; ++i)
		voltages[i] = -1;
}

int main(int argc, char const *argv[]) {
  int fd;
  int size;
  fixed_size *ptr;

  fd = shm_open("share_data", O_RDWR, 0666);
  if (fd == -1)
	  printf("ERROR: Uart_exe hasn't opened yet.\n");

  size = sizeof(fixed_size);
  ftruncate(fd, size);// open up the enough size of shared memory
  ptr = (fixed_size*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  while (1){

	  cout << "vx= " << ptr->vx;
	  cout << "vy= " << ptr->vy;
	  cout << "vz= " << ptr->vz;
	  cout << "afx= " << ptr->afx;
	  cout << "afy= " << ptr->afy;
	  cout << "afz(-1)= " << ptr->afz;
	  cout << "body_roll_rate= " << ptr->body_roll_rate;
	  cout << "body_pitch_rate= " << ptr->body_pitch_rate;
	  cout << "body_yaw_rate= " << ptr->body_yaw_rate;
	  cout << "airspeed= " << ptr->airspeed;
	  cout << "heading= " << ptr->heading;
	  cout << "throttle= " << ptr->throttle;
	  cout << "alt= " << ptr->alt;
	  cout << "climb= " << ptr->climb;
	  cout << "xacc= " << ptr->xacc;
	  cout << "yacc= " << ptr->yacc;
	  cout << "zacc= " << ptr->zacc;
	  cout << "xgyro= " << ptr->xgyro;
	  cout << "ygyro= " << ptr->ygyro;
	  cout << "zgyro= " << ptr->zgyro;
	  cout << "pressure_alt= " << ptr->pressure_alt;
	  cout << "temperature= " << ptr->temperature;
	  cout << "current_battery= " << ptr->current_battery;
	  cout << "current_consumed= " << ptr->current_consumed;
	  cout << "energy_consumed= " << ptr->energy_consumed;
	  cout << "battery_remaining= " << ptr->battery_remaining;
	  sleep(30000);
	  cout << "\033[2J\033[1;1H";
  }
 
  while(1)
  printf("Here is the world %s", ptr);

  return 0;
}
