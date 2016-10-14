#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include "serial_port.cpp"
//#include "editor_serial_port.cpp"




/*#include "mavlink_msg_attitude_target.h"
#include "mavlink_msg_vfr_hud.h"
#include "mavlink_msg_timesync.h"
#include "mavlink_msg_attitude.h"
#include "mavlink_msg_highres_imu.h"
#include "mavlink_msg_servo_output_raw.h"
#include "mavlink_msg_command_ack.h"
#include "mavlink_msg_heartbeat.h"
#include "mavlink_msg_position_target_local_ned.h"
#include "mavlink_msg_command_long.h"
#include "mavlink_msg_sys_status.h"
#include "mavlink_msg_battery_status.h"
#include "mavlink_msg_system_time.h"
*/


struct current_message {

	int sysid;
	int compid;

	mavlink_heartbeat_t heartbeat;
	mavlink_local_position_ned_t local_position_ned;
	mavlink_attitude_target_t attitude_target;
	mavlink_vfr_hud_t vfr_hud;
	mavlink_timesync_t timesync;
	mavlink_attitude_t attitude;
	mavlink_highres_imu_t highres_imu;
	mavlink_servo_output_raw_t servo_output_raw;
	mavlink_gps_raw_int_t gps_raw_int;
	mavlink_command_ack_t command_ack;
	mavlink_position_target_local_ned_t position_target_local_ned;
	mavlink_command_long_t command_long;
	mavlink_sys_status_t sys_status;
	mavlink_battery_status_t battery_status;
	mavlink_system_time_t system_time;
	mavlink_statustext_t statustext;

};

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
	int16_t temperature;// Temperature of the battery in centi - degrees celsius.INT16_MAX for unknown temperature.
	uint16_t voltages[10]; // Battery voltage of cells, in millivolts(1 = 1 millivolt).Cells above the valid cell count for this battery should have the UINT16_MAX value.
	int16_t current_battery;	// Battery current, in 10 * milliamperes(1 = 10 milliampere), -1: autopilot does not measure the current
	int32_t current_consumed; // Consumed charge, in milliampere hours(1 = 1 mAh), -1 : autopilot does not provide mAh consumption estimate
	int32_t energy_consumed; // Consumed energy, in 100 * Joules(intergrated U*I*dt) (1 = 100 Joule), -1 : autopilot does not provide energy consumption estimate
	int8_t battery_remaining; // Remaining battery energy : (0 % : 0, 100 % : 100), -1 : autopilot does not estimate the remaining battery
	int32_t lon;//	Longitude (WGS84), in degrees * 1E7
	int32_t lat;// Latitude (WGS84), in degrees * 1E7	
    int8_t satellites_visible;// number of satellite_visible if unknown set to 255
};

fixed_size::fixed_size():
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
for(int i=0;i<10;++i)
		voltages[i]=-1;
}






int main(int argc, char const *argv[])
{
/*****************************************************************************************/
/*                                   setup the uart port                                 */
/*****************************************************************************************/
	Serial_Port serial_port;// this is default uart_name /dev/ttyAMA0 , baudrate 57600
    serial_port.start();


/*****************************************************************************************/
/*                             open and setup the share memory                           */
/*****************************************************************************************/
	fixed_size *ptr;
    int fd;// file descriptor
    int size ; // size need to chop in memory


	fd = shm_open("share_data", O_CREAT | O_RDWR, 0666);
	if (fd == -1)
		printf("shm_open error\n");

	size = sizeof(fixed_size);
	ftruncate(fd, size);// open up the enough size of shared memory
	ptr = (fixed_size*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


/*****************************************************************************************/
/*                         Receive and decode the MAVLink message                        */
/*****************************************************************************************/
	mavlink_message_t	msgrcv;
	current_message current;
	int rcv_success = 0;
	int sysid(0),compid(0);



	mavlink_message_t msg_send;
	uint8_t confirm = 0;
	uint8_t arm = 1;
	int rcv_count =0;
	
	
	
	while (1){
		//int have_new_data = 0;

		rcv_success = serial_port.read_message(msgrcv);// not sure it would wipe out the msgrcv or not

		if(rcv_success)
		{
				sysid = msgrcv.sysid;
				compid = msgrcv.compid;
				//printf("sysid :%d \n compid: %d\n", msgrcv.sysid,msgrcv.compid);
				switch(msgrcv.msgid)
				{
				case MAVLINK_MSG_ID_LOCAL_POSITION_NED:
				{
					printf("find local_position_ned  !!! \n");
					mavlink_msg_local_position_ned_decode(&msgrcv, &(current.local_position_ned));
					ptr->vx = current.local_position_ned.vx;
					ptr->vy = current.local_position_ned.vy;
					ptr->vz = current.local_position_ned.vz;
				//	ptr->afx = current.local_position_ned.afx;
				//	ptr->afy = current.local_position_ned.afy;
				//	ptr->afz = current.local_position_ned.afz;
					break;
				}

				case MAVLINK_MSG_ID_ATTITUDE_TARGET:
				{
					mavlink_msg_attitude_target_decode(&msgrcv, &(current.attitude_target));
					ptr->body_roll_rate = current.attitude_target.body_roll_rate;
					ptr->body_pitch_rate = current.attitude_target.body_pitch_rate;
					ptr->body_yaw_rate = current.attitude_target.body_yaw_rate;
					break;
				}

				case MAVLINK_MSG_ID_VFR_HUD:
				{
					mavlink_msg_vfr_hud_decode(&msgrcv, &(current.vfr_hud));
					//ptr->airspeed = current.vfr_hud.airspeed;
					//ptr->groundspeed = current.vfr_hud.groundspeed;
					ptr->heading = current.vfr_hud.heading;
					ptr->throttle = current.vfr_hud.throttle;
					ptr->alt = current.vfr_hud.alt;
					ptr->climb = current.vfr_hud.climb;
					break;
				}

				case MAVLINK_MSG_ID_TIMESYNC:
				{
					mavlink_msg_timesync_decode(&msgrcv, &(current.timesync));
					//do nothing
					break;
				}

				case MAVLINK_MSG_ID_ATTITUDE:
				{
					mavlink_msg_attitude_decode(&msgrcv, &(current.attitude));
					//do nothing
					break;
				}

				case MAVLINK_MSG_ID_HIGHRES_IMU:
				{
					mavlink_msg_highres_imu_decode(&msgrcv, &(current.highres_imu));
					ptr->xacc = current.highres_imu.xacc;
					ptr->yacc = current.highres_imu.yacc;
					ptr->zacc = current.highres_imu.zacc;
					ptr->xgyro = current.highres_imu.xgyro;
					ptr->ygyro = current.highres_imu.ygyro;
					ptr->zgyro = current.highres_imu.zgyro;
					ptr->pressure_alt = current.highres_imu.pressure_alt;
					break;
				}

				case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW:
				{
					mavlink_msg_servo_output_raw_decode(&msgrcv, &(current.servo_output_raw));
					//do nothing
					break;
				}

				case MAVLINK_MSG_ID_GPS_RAW_INT:
				{
					mavlink_msg_gps_raw_int_decode(&msgrcv, &(current.gps_raw_int));
					
					ptr->groundspeed = current.gps_raw_int.vel;
					ptr->lon = current.gps_raw_int.lon;
					ptr->lat = current.gps_raw_int.lat;
					ptr ->satellites_visible= current.gps_raw_int.satellites_visible;
					
					//do nothing
					break;
				}

				case MAVLINK_MSG_ID_COMMAND_ACK:
				{
					mavlink_msg_command_ack_decode(&msgrcv, &(current.command_ack));
					//do nothing
					break;
				}

				case MAVLINK_MSG_ID_HEARTBEAT:
				{
					mavlink_msg_heartbeat_decode(&msgrcv, &(current.heartbeat));
					ptr->base_mode = current.heartbeat.base_mode;
					//do nothing
					break;
				}

				case MAVLINK_MSG_ID_POSITION_TARGET_LOCAL_NED:
				{
					mavlink_msg_position_target_local_ned_decode(&msgrcv, &(current.position_target_local_ned));
					printf("local x: %f\n local y: %f\n local z: %f\n", current.position_target_local_ned.x, current.position_target_local_ned.y, current.position_target_local_ned.z);
					//do nothing
					break;
				}

				case MAVLINK_MSG_ID_COMMAND_LONG:
				{
					mavlink_msg_command_long_decode(&msgrcv, &(current.command_long));
					//do nothing
					//printf("Receive command_long from pixhawk !!\n");
					break;
				}

				case MAVLINK_MSG_ID_SYS_STATUS:
				{
					mavlink_msg_sys_status_decode(&msgrcv, &(current.sys_status));
					//do nothing
					break;
				}

				case MAVLINK_MSG_ID_BATTERY_STATUS:
				{
					mavlink_msg_battery_status_decode(&msgrcv, &(current.battery_status));
					ptr->temperature = current.battery_status.temperature;
					for (int i = 0; i < 10; ++i){
						ptr->voltages[i] = current.battery_status.voltages[i];
					}
					ptr->current_battery = current.battery_status.current_battery;
					ptr->current_consumed = current.battery_status.current_consumed;
					ptr->energy_consumed = current.battery_status.energy_consumed;
					ptr->battery_remaining = current.battery_status.battery_remaining;
					break;
				}

				case MAVLINK_MSG_ID_SYSTEM_TIME:
				{
					mavlink_msg_system_time_decode(&msgrcv, &(current.system_time));
					break;
				}
				case MAVLINK_MSG_ID_STATUSTEXT:
				{
					mavlink_msg_statustext_decode(&msgrcv, &(current.statustext));
					printf("MSG: %s\n", current.statustext.text);
				}
				default:
				{
					printf("Getting unexpected message, msgrcv.msgid = %d\n", msgrcv.msgid);
				}
			}// end of switch
		
		rcv_count++;
		}//end of if rcv



	if(rcv_count > 1000 && confirm  == 0 ){
    		mavlink_msg_command_long_pack( 0 , 0, &msg_send, sysid , compid ,MAV_CMD_DO_SET_MODE , confirm , MAV_MODE_AUTO_DISARMED , 0, 0, 0, 0, 0, 0);
    		printf("Write %d bytes\n",serial_port.write_message(msg_send));
		    printf("DISARM Executed !!\n");
			confirm++;
			arm = 0;
	}
	
	if(confirm >0 && arm == 0 )sleep(10);
	if(rcv_count > 1000 && confirm >0 && arm == 0){
    		mavlink_msg_command_long_pack( 0 , 0, &msg_send, sysid , compid , MAV_CMD_NAV_TAKEOFF, confirm , 5 , 0, 0,0,ptr-> lat,ptr-> lon,ptr-> alt);
    		printf("Write %d bytes\n",serial_port.write_message(msg_send));
		    printf("Takeoff Executed \n!!\n");
			arm =1;
     }

}// end of while

	return 0;
}
