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
	int base_mode;
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
	int32_t lon;//	Longitude (WGS84), in degrees * 1E7
	int32_t lat;// Latitude (WGS84), in degrees * 1E7	
    int8_t satellites_visible;// number of satellite_visible if unknown set to 255
		
};

fixed_size::fixed_size() :
base_mode(-1),
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
battery_remaining(-1),
lon(-1),//	Longitude (WGS84), in degrees * 1E7
lat(-1),// Latitude (WGS84), in degrees * 1E7	
satellites_visible(-1)// number of satellite_visible if unknown set to 255
{
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

else
		printf("Open SUccess !\n");

  size = sizeof(fixed_size);
  ftruncate(fd, size);// open up the enough size of shared memory
  ptr = (fixed_size*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

 //for(int i=0; i<10 ; ++i){
  while (1){
    printf("MAV_MODE_FLAG : %d\n",ptr->base_mode );
		printf("vx= %f\n",ptr->vx);
	  printf("vy= %f\n", ptr->vy);
	  printf("vz= %f\n", ptr->vz);
	  printf("afx= %f\n", ptr->afx);
	  printf("afy= %f\n", ptr->afy);
	  printf("afz= %f\n", ptr->afz);
	  printf("body_roll_rate= %f\n", ptr->body_roll_rate);
	  printf("body_pitch_rate= %f\n", ptr->body_pitch_rate);
	  printf("body_yaw_rate= %f\n", ptr->body_yaw_rate);
	  printf("airspeed= %f\n", ptr->airspeed);
    printf("groundspeed= %f\n", ptr->groundspeed);
     //printf("heading= %f", ptr->heading);
	  printf("throttle= %d", ptr->throttle);
	  printf("alt= %f\n", ptr->alt);
	  printf("climb= %f\n", ptr->climb);
	  printf("xacc= %f\n", ptr->xacc);
	  printf("yacc= %f\n", ptr->yacc);
	  printf("zacc= %f\n", ptr->zacc);
	  printf("xgyro= %f\n", ptr->xgyro);
	  printf("ygyro= %f\n", ptr->ygyro);
	  printf("zgyro= %f\n", ptr->zgyro);
	  printf("pressure_alt= %f\n", ptr->pressure_alt);
	  //printf("temperature= %f\n", ptr->temperature);
	  //printf("current_battery=%f\n", ptr->current_battery);
	  //printf("current_consumed= %f\n", ptr->current_consumed);
	  //printf("energy_consumed= %f\n", ptr->energy_consumed);
	  //printf("battery_remaining= %f\n", ptr->battery_remaining);
	  printf("GPS: longitude:%d\n",ptr->lon);
	  printf("GPS: latitude: %d\n",ptr->lat);
	  printf("Number of GPS : %d\n",ptr->satellites_visible);
	  usleep(200000);

	//  printf("\033[2J\033[1;1H");
  //}
 
 }
 // while(1)
  printf("Here is the world ");

  return 0;
}
