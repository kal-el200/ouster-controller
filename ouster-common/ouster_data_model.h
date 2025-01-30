#pragma once

/*
1. The final struct is in the end of file.
2. This struct is sent to Map from Ouster preprocessor once in 20 msec.
3. One ouster message consists of ~410 packets each one is a vertical firing of 32 / 64 / 128 rays.
4. Depending on the lidar mode 512 / 1024 / 2048 the number of sent packets will be different.
5. Sending message once in 20 msec allows the CPU usage in Map application be more steady
otherwise there will be CPU spikes which affect the total PC performance.
6. The ouster lidar has the build in bit / alert system of self inspection which affects the ouster preprocessor application state machine(will be defined soon).
*/

enum class SensorModelType
{
	SENSOR_MODEL_TYPE_OS0_32 = 0,
	SENSOR_MODEL_TYPE_OS0_64 = 1,
	SENSOR_MODEL_TYPE_OS0_128 = 2,
	SENSOR_MODEL_TYPE_OS1_32 = 3,
	SENSOR_MODEL_TYPE_OS1_64 = 4,
	SENSOR_MODEL_TYPE_OS1_128 = 5,
	SENSOR_MAX_NUM = 6
};

enum class ReturnMode
{
	RETURN_MODE_SINGLE = 0,
	RETURN_MODE_DUAL = 1
};

struct TimeStampJausStruct
{
	/* mille sec */
	unsigned short miliseconds;
	/* seconds */
	unsigned char Seconds;
	/* minutes */
	unsigned char Minutes;
	/* hour */
	unsigned char Hour;
	/* day */
	unsigned char Day;
};

struct OusterRayDataStruct
{
	/* LRF distance */
	unsigned short m_distance_first;  // distance in meters (got in millimeters from sensor, should be divided by 100 to get meters -> max range 524.00 meters )
	unsigned short m_distance_second; // distance in meters (got in millimeters from sensor, should be divided by 100 to get meters -> max range 524.00 meters )
	/* LRF intensity */
	unsigned char m_reflectivity_first;
	unsigned char m_reflectivity_second;
	unsigned int  m_nir_value;
};

typedef OusterRayDataStruct OusterRayDataArray[128];

struct OusterOneAzimuth
{
	unsigned short m_rotational_direction_azimuth;
	OusterRayDataArray m_ouster_ray_data;
	TimeStampJausStruct m_azimuth_firing_time;
};

struct OusterPacketStruct
{
	OusterOneAzimuth m_ouster_one_azimuth;
	unsigned char m_is_exists;
};

typedef OusterPacketStruct OusterPacketStructArray[500];

struct OusterPacketDyn
{
	unsigned short m_num_of_elements;  // Num of packets sent in x milliseconds
	OusterPacketStructArray m_ouster_packet_struct_array;
};

struct OusterMultilayerMaindataStructDev
{
	unsigned long m_msg_index;
	SensorModelType m_sensor_model;
	ReturnMode m_return_mode;
	OusterPacketDyn m_ouster_packet_array_dyn;
};

/* Interface data structure type definition */
struct OusterDynMessage
{
	unsigned char m_Presence_Vector_1byte;
	OusterMultilayerMaindataStructDev m_ouster_multilayer_data;
	unsigned char m_beam_altitude_angle_type;   // 0 - uniform, 1 - gradient, 2 - above horizon, 3 -below horizon
};