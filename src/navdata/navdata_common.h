/**
 *  \file     navdata_common.h
 *  \brief    Common navdata configuration
 *  \author   Sylvain Gaeremynck <sylvain.gaeremynck@parrot.com>
 */

#ifndef _NAVDATA_COMMON_H_
#define _NAVDATA_COMMON_H_

/*------------------------------------------ NAVDATA STRUCTURES DECLARATIONS ---------------------------------------------------------------*/

#include <float.h>
#include <inttypes.h>

typedef int32_t bool_t; 

#if defined(_MSC_VER)
	#define _ATTRIBUTE_PACKED_
	/* Asks Visual C++ to pack structures from now on*/
	#pragma pack(1)
#else
	#define _ATTRIBUTE_PACKED_  __attribute__ ((packed))
#endif

typedef enum {
  ARDRONE_FLY_MASK            = 1U << 0,  /*!< FLY MASK : (0) ardrone is landed, (1) ardrone is flying */
  ARDRONE_VIDEO_MASK          = 1U << 1,  /*!< VIDEO MASK : (0) video disable, (1) video enable */
  ARDRONE_VISION_MASK         = 1U << 2,  /*!< VISION MASK : (0) vision disable, (1) vision enable */
  ARDRONE_CONTROL_MASK        = 1U << 3,  /*!< CONTROL ALGO : (0) euler angles control, (1) angular speed control */
  ARDRONE_ALTITUDE_MASK       = 1U << 4,  /*!< ALTITUDE CONTROL ALGO : (0) altitude control inactive (1) altitude control active */
  ARDRONE_USER_FEEDBACK_START = 1U << 5,  /*!< USER feedback : Start button state */
  ARDRONE_COMMAND_MASK        = 1U << 6,  /*!< Control command ACK : (0) None, (1) one received */
  ARDRONE_CAMERA_MASK         = 1U << 7,  /*!< CAMERA MASK : (0) camera not ready, (1) Camera ready */
  ARDRONE_TRAVELLING_MASK     = 1U << 8,  /*!< Travelling mask : (0) disable, (1) enable */
  ARDRONE_USB_MASK            = 1U << 9,  /*!< USB key : (0) usb key not ready, (1) usb key ready */
  ARDRONE_NAVDATA_DEMO_MASK   = 1U << 10, /*!< Navdata demo : (0) All navdata, (1) only navdata demo */
  ARDRONE_NAVDATA_BOOTSTRAP   = 1U << 11, /*!< Navdata bootstrap : (0) options sent in all or demo mode, (1) no navdata options sent */
  ARDRONE_MOTORS_MASK  	      = 1U << 12, /*!< Motors status : (0) Ok, (1) Motors problem */
  ARDRONE_COM_LOST_MASK       = 1U << 13, /*!< Communication Lost : (1) com problem, (0) Com is ok */
  ARDRONE_SOFTWARE_FAULT      = 1U << 14, /*!< Software fault detected - user should land as quick as possible (1) */
  ARDRONE_VBAT_LOW            = 1U << 15, /*!< VBat low : (1) too low, (0) Ok */
  ARDRONE_USER_EL             = 1U << 16, /*!< User Emergency Landing : (1) User EL is ON, (0) User EL is OFF*/
  ARDRONE_TIMER_ELAPSED       = 1U << 17, /*!< Timer elapsed : (1) elapsed, (0) not elapsed */
  ARDRONE_MAGNETO_NEEDS_CALIB = 1U << 18, /*!< Magnetometer calibration state : (0) Ok, no calibration needed, (1) not ok, calibration needed */
  ARDRONE_ANGLES_OUT_OF_RANGE = 1U << 19, /*!< Angles : (0) Ok, (1) out of range */
  ARDRONE_WIND_MASK 		  = 1U << 20, /*!< WIND MASK: (0) ok, (1) Too much wind */
  ARDRONE_ULTRASOUND_MASK     = 1U << 21, /*!< Ultrasonic sensor : (0) Ok, (1) deaf */
  ARDRONE_CUTOUT_MASK         = 1U << 22, /*!< Cutout system detection : (0) Not detected, (1) detected */
  ARDRONE_PIC_VERSION_MASK    = 1U << 23, /*!< PIC Version number OK : (0) a bad version number, (1) version number is OK */
  ARDRONE_ATCODEC_THREAD_ON   = 1U << 24, /*!< ATCodec thread ON : (0) thread OFF (1) thread ON */
  ARDRONE_NAVDATA_THREAD_ON   = 1U << 25, /*!< Navdata thread ON : (0) thread OFF (1) thread ON */
  ARDRONE_VIDEO_THREAD_ON     = 1U << 26, /*!< Video thread ON : (0) thread OFF (1) thread ON */
  ARDRONE_ACQ_THREAD_ON       = 1U << 27, /*!< Acquisition thread ON : (0) thread OFF (1) thread ON */
  ARDRONE_CTRL_WATCHDOG_MASK  = 1U << 28, /*!< CTRL watchdog : (1) delay in control execution (> 5ms), (0) control is well scheduled */
  ARDRONE_ADC_WATCHDOG_MASK   = 1U << 29, /*!< ADC Watchdog : (1) delay in uart2 dsr (> 5ms), (0) uart2 is good */
  ARDRONE_COM_WATCHDOG_MASK   = 1U << 30, /*!< Communication Watchdog : (1) com problem, (0) Com is ok */
  //#if defined(__ARMCC_VERSION)
  ARDRONE_EMERGENCY_MASK      = (int)0x80000000  /*!< Emergency landing : (0) no emergency, (1) emergency */
  //#else
  //ARDRONE_EMERGENCY_MASK      = 1U << 31  /*!< Emergency landing : (0) no emergency, (1) emergency */
  //#endif
} def_ardrone_state_mask_t;


// Define constants for gyrometers handling
typedef enum {
  GYRO_X    = 0,
  GYRO_Y    = 1,
  GYRO_Z    = 2,
  NB_GYROS  = 3
} def_gyro_t;


// Define constants for accelerometers handling
typedef enum {
  ACC_X   = 0,
  ACC_Y   = 1,
  ACC_Z   = 2,
  NB_ACCS = 3
} def_acc_t;

/**
 * \struct _velocities_t
 * \brief Velocities in uint32_t format 
 */
typedef struct _velocities_t {
  uint32_t x; /* floating point value */
  uint32_t y; /* floating point value */
  uint32_t z; /* floating point value */
} velocities_t;

// Default control loops gains TODO Put these values in flash memory
// To avoid divisions in embedded software, gains are defined as ratios where
//  - the numerator is an integer
//  - the denominator is an integer

/**
 *  \var     CTRL_DEFAULT_NUM_PQ_KP
 *  \brief   Numerator of default proportionnal gain for pitch (p) and roll (q) angular rate control loops
 */
/**
 *  \var     CTRL_DEFAULT_NUM_EA_KP
 *  \brief   Numerator of default proportionnal gain for Euler Angle control loops
 */
/**
 *  \var     CTRL_DEFAULT_NUM_EA_KI
 *  \brief   Numerator of default integral gain for Euler Angle control loops
 */

#define CTRL_DEFAULT_NUM_PQ_KP_NO_SHELL /*30000  26000 */ 40000
#define CTRL_DEFAULT_NUM_EA_KP_NO_SHELL /*7000  9000  7000 */ 8000
#define CTRL_DEFAULT_NUM_EA_KI_NO_SHELL /*4000  7000 6000 */ 7000

#define CTRL_DEFAULT_NUM_PQ_KP_SHELL /*30000 23000*/ 40000
#define CTRL_DEFAULT_NUM_EA_KP_SHELL /*9000 10000*/  9000
#define CTRL_DEFAULT_NUM_EA_KI_SHELL /*5000 9000*/   8000

/**
 *  \var     CTRL_DEFAULT_NUM_H_R
 *  \brief   Numerator of default proportionnal gain for yaw (r) angular rate control loop
 */
#define CTRL_DEFAULT_NUM_R_KP 200000

/**
 *  \var     CTRL_DEFAULT_NUM_R_KI
 *  \brief   Numerator of default integral gain for yaw control loops
 */
#define CTRL_DEFAULT_NUM_R_KI 3000

/**
 *  \var     CTRL_DEFAULT_DEN_W
 *  \brief   Denominator of default proportionnal gain of pitch (p) roll (q) and yaw (r) angular rate control loops
 */
#define CTRL_DEFAULT_DEN_W 1024.0 //2^10

/**
 *  \var     CTRL_DEFAULT_DEN_EA
 *  \brief   Denominator of default gains for Euler Angle control loops
 */
#define CTRL_DEFAULT_DEN_EA 1024.0 //2^10

/**
 *  \var     CTRL_DEFAULT_NUM_ALT_KP
 *  \brief   Numerator of default proportionnal gain for Altitude control loop
 */
#ifdef ALT_CONTROL
#define CTRL_DEFAULT_NUM_ALT_KP 210
#else
#define CTRL_DEFAULT_NUM_ALT_KP 3000
#endif
/**
 *  \var     CTRL_DEFAULT_NUM_ALT_KI
 *  \brief   Numerator of default integral gain for Altitude control loop
 */
#ifdef ALT_CONTROL
	#define CTRL_DEFAULT_NUM_ALT_KI 100
#else
	#define CTRL_DEFAULT_NUM_ALT_KI 400
#endif
/**
 *  \var     CTRL_DEFAULT_NUM_ALT_KD
 *  \brief   Numerator of default derivative gain for Altitude control loop
 */

#ifdef ALT_CONTROL
	#define CTRL_DEFAULT_NUM_VZ_KP 100
#else
#define CTRL_DEFAULT_NUM_VZ_KP 200
#endif
/**
 *  \var     CTRL_DEFAULT_NUM_ALT_TD
 *  \brief   Numerator of default derivative time constant gain for Altitude control loop
 */
#define CTRL_DEFAULT_NUM_VZ_KI 100

/**
 *  \var     CTRL_DEFAULT_DEN_ALT
 *  \brief   Denominator of default gains for Altitude control loop
 */
#define CTRL_DEFAULT_DEN_ALT 1024.0

/**
 *  \var     CTRL_DEFAULT_NUM_HOVER_KP
 *  \brief   Numerator of default proportionnal gain for hovering control loop
 */
#define CTRL_DEFAULT_NUM_HOVER_KP_SHELL /*5000* 8000*/ 8000
#define CTRL_DEFAULT_NUM_HOVER_KP_NO_SHELL /*6000 12000 5000*/ 7000

/**
 *  \var     CTRL_DEFAULT_NUM_HOVER_KP
 *  \brief   Numerator of default proportionnal gain for hovering beacon control loop
 */
#define CTRL_DEFAULT_NUM_HOVER_B_KP_SHELL 1200
#define CTRL_DEFAULT_NUM_HOVER_B_KP_NO_SHELL 1200

/**
 *  \var     CTRL_DEFAULT_NUM_HOVER_KI
 *  \brief   Numerator of default integral gain for hovering control loop
 */
#define CTRL_DEFAULT_NUM_HOVER_KI_SHELL /*3000 10000*/ 8000
#define CTRL_DEFAULT_NUM_HOVER_KI_NO_SHELL /*3000 8000 5000*/ 6000

/**
 *  \var     CTRL_DEFAULT_NUM_HOVER_KI
 *  \brief   Numerator of default integral gain for hovering beacon control loop
 */
#define CTRL_DEFAULT_NUM_HOVER_B_KI_SHELL 500
#define CTRL_DEFAULT_NUM_HOVER_B_KI_NO_SHELL 500

/*
 *  \var     CTRL_DEFAULT_DEN_HOVER
 *  \brief   Numerator of default proportionnal gain for hovering control loop
 */
#define CTRL_DEFAULT_DEN_HOVER 32768.0 //2^15

#define CTRL_DEFAULT_NUM_HOVER_B_KP2_NO_SHELL  22937
#define CTRL_DEFAULT_NUM_HOVER_B_KI2_NO_SHELL  8192
#define CTRL_DEFAULT_NUM_HOVER_B_KD2_NO_SHELL  8000

#define CTRL_DEFAULT_NUM_HOVER_B_KP2_SHELL  22937
#define CTRL_DEFAULT_NUM_HOVER_B_KI2_SHELL  8192
#define CTRL_DEFAULT_NUM_HOVER_B_KD2_SHELL  8000
/* Timeout for mayday maneuvers*/
static const int32_t MAYDAY_TIMEOUT[] = {
    1000,  // ARDRONE_ANIM_PHI_M30_DEG
    1000,  // ARDRONE_ANIM_PHI_30_DEG
    1000,  // ARDRONE_ANIM_THETA_M30_DEG
    1000,  // ARDRONE_ANIM_THETA_30_DEG
    1000,  // ARDRONE_ANIM_THETA_20DEG_YAW_200DEG
    1000,  // ARDRONE_ANIM_THETA_20DEG_YAW_M200DEG
    5000,  // ARDRONE_ANIM_TURNAROUND
    5000,  // ARDRONE_ANIM_TURNAROUND_GODOWN
    2000,  // ARDRONE_ANIM_YAW_SHAKE
    5000,  // ARDRONE_ANIM_YAW_DANCE
    5000,  // ARDRONE_ANIM_PHI_DANCE
    5000,  // ARDRONE_ANIM_THETA_DANCE
    5000,  // ARDRONE_ANIM_VZ_DANCE
    5000,  // ARDRONE_ANIM_WAVE
    5000,  // ARDRONE_ANIM_PHI_THETA_MIXED
    5000,  // ARDRONE_ANIM_DOUBLE_PHI_THETA_MIXED
    15,  // ARDRONE_ANIM_FLIP_AHEAD
    15,  // ARDRONE_ANIM_FLIP_BEHIND
    15,  // ARDRONE_ANIM_FLIP_LEFT
    15,  // ARDRONE_ANIM_FLIP_RIGHT
};

#define NAVDATA_SEQUENCE_DEFAULT  1

#define NAVDATA_HEADER  0x55667788

#define NAVDATA_MAX_SIZE 4096
#define NAVDATA_MAX_CUSTOM_TIME_SAVE 20

/* !!! Warning !!! - changing the value below would break compatibility with older applications
 * DO NOT CHANGE THIS  */
#define NB_NAVDATA_DETECTION_RESULTS 4


/**
 * @brief Tags identifying navdata blocks in a Navdata UDP packet
 * This tag is stored in the first two bytes of any navdata block (aka 'option').
 */

#define NAVDATA_OPTION_DEMO(STRUCTURE,NAME,TAG)  TAG = 0,
#define NAVDATA_OPTION(STRUCTURE,NAME,TAG)       TAG ,
#define NAVDATA_OPTION_CKS(STRUCTURE,NAME,TAG)   NAVDATA_NUM_TAGS, TAG = 0xFFFF

typedef enum _navdata_tag_t {
	#include "navdata_keys.h"
} navdata_tag_t;

#define NAVDATA_OPTION_MASK(option) ( 1 << (option) )
#define NAVDATA_OPTION_FULL_MASK    ((1<<NAVDATA_NUM_TAGS)-1)


typedef struct _navdata_option_t {
  uint16_t  tag;
  uint16_t  size;
  uint8_t   data[1];
} navdata_option_t;


/**
 * @brief Navdata structure sent over the network.
 */
typedef struct _navdata_t {
  uint32_t    header;			/*!< Always set to NAVDATA_HEADER */
  uint32_t    ardrone_state;    /*!< Bit mask built from def_ardrone_state_mask_t */
  uint32_t    sequence;         /*!< Sequence number, incremented for each sent packet */
  bool_t      vision_defined;

  navdata_option_t  options[1];
}_ATTRIBUTE_PACKED_ navdata_t;


/**
 * All navdata options can be extended (new values AT THE END) except navdata_demo whose size must be constant across versions
 * New navdata options may be added, but must not be sent in navdata_demo mode unless requested by navdata_options.
 */

/*----------------------------------------------------------------------------*/
/**
 * @brief Minimal navigation data for all flights.
 */
typedef struct _navdata_demo_t {
  uint16_t    tag;					  /*!< Navdata block ('option') identifier */
  uint16_t    size;					  /*!< set this to the size of this structure */

  uint32_t    ctrl_state;             /*!< Flying state (landed, flying, hovering, etc.) defined in CTRL_STATES enum. */
  uint32_t    vbat_flying_percentage; /*!< battery voltage filtered (mV) */

  uint32_t   theta;                  /*!< UAV's pitch in milli-degrees */ /* floating point value */
  uint32_t   phi;                    /*!< UAV's roll  in milli-degrees */ /* floating point value */
  uint32_t   psi;                    /*!< UAV's yaw   in milli-degrees */ /* floating point value */

  int32_t     altitude;               /*!< UAV's altitude in centimeters */

  uint32_t   vx;                     /*!< UAV's estimated linear velocity */ /* floating point value */
  uint32_t   vy;                     /*!< UAV's estimated linear velocity */ /* floating point value */
  uint32_t   vz;                     /*!< UAV's estimated linear velocity */ /* floating point value */

  uint32_t    num_frames;			  /*!< streamed frame index */ // Not used -> To integrate in video stage.

  // Camera parameters compute by detection
  uint32_t    detection_camera_rot[9];   /*!<  Deprecated ! Don't use ! */
  uint32_t detection_camera_trans[3]; /*!<  Deprecated ! Don't use ! */
  uint32_t	  detection_tag_index;    /*!<  Deprecated ! Don't use ! */

  uint32_t	  detection_camera_type;  /*!<  Type of tag searched in detection */

  // Camera parameters compute by drone
  uint32_t drone_camera_rot[9];		  /*!<  Deprecated ! Don't use ! */
  uint32_t drone_camera_trans[3];	  /*!<  Deprecated ! Don't use ! */
}_ATTRIBUTE_PACKED_ navdata_demo_t;



/*----------------------------------------------------------------------------*/
/**
 * @brief Last navdata option that *must* be included at the end of all navdata packets
 * + 6 bytes
 */
typedef struct _navdata_cks_t {
  uint16_t  tag;
  uint16_t  size;

  // Checksum for all navdatas (including options)
  uint32_t  cks;
}_ATTRIBUTE_PACKED_ navdata_cks_t;


/*----------------------------------------------------------------------------*/
/**
 * @brief Timestamp
 * + 6 bytes
 */
typedef struct _navdata_time_t {
  uint16_t  tag;
  uint16_t  size;

  uint32_t  time;  /*!< 32 bit value where the 11 most significant bits represents the seconds, and the 21 least significant bits are the microseconds. */
}_ATTRIBUTE_PACKED_ navdata_time_t;



/*----------------------------------------------------------------------------*/
/**
 * @brief Raw sensors measurements
 */
typedef struct _navdata_raw_measures_t {
  uint16_t  tag;
  uint16_t  size;

  // +12 bytes
  uint16_t  raw_accs[NB_ACCS];    // filtered accelerometers
  int16_t   raw_gyros[NB_GYROS];  // filtered gyrometers
  int16_t   raw_gyros_110[2];     // gyrometers  x/y 110 deg/s
  uint32_t  vbat_raw;             // battery voltage raw (mV)
  uint16_t  us_debut_echo;
  uint16_t  us_fin_echo;
  uint16_t  us_association_echo;
  uint16_t  us_distance_echo;
  uint16_t  us_courbe_temps;
  uint16_t  us_courbe_valeur;
  uint16_t  us_courbe_ref;
  uint16_t  flag_echo_ini;
  // TODO:   uint16_t  frame_number; // from ARDrone_Magneto
  uint16_t  nb_echo;
  uint32_t  sum_echo;
  int32_t   alt_temp_raw;
  int16_t   gradient;
}_ATTRIBUTE_PACKED_ navdata_raw_measures_t;

// split next struc into magneto_navdata_t and pressure_navdata_t
typedef struct _navdata_pressure_raw_t {
  uint16_t   tag;
  uint16_t   size;

  int32_t   up;
  int16_t   ut;
  int32_t   Temperature_meas;
  int32_t   Pression_meas;
}_ATTRIBUTE_PACKED_ navdata_pressure_raw_t;

typedef struct _navdata_magneto_t {
  uint16_t   tag;
  uint16_t   size;

  int16_t   	mx;
  int16_t   	my;
  int16_t   	mz;
  uint32_t magneto_raw[3];       // magneto in the body frame, in mG
  uint32_t magneto_rectified[3];
  uint32_t magneto_offset[3];
  uint32_t 	heading_unwrapped; /* floating point value */
  uint32_t 	heading_gyro_unwrapped; /* floating point value */
  uint32_t 	heading_fusion_unwrapped; /* floating point value */
  char 			magneto_calibration_ok;
  uint32_t      magneto_state;
  uint32_t 	magneto_radius; /* floating point value */
  uint32_t     error_mean; /* floating point value */
  uint32_t     error_var; /* floating point value */

}_ATTRIBUTE_PACKED_ navdata_magneto_t;

typedef struct _navdata_wind_speed_t {
  uint16_t   tag;
  uint16_t   size;

  uint32_t wind_speed;			// estimated wind speed [m/s] /* floating point value */
  uint32_t wind_angle;			// estimated wind direction in North-East frame [rad] e.g. if wind_angle is pi/4, wind is from South-West to North-East /* floating point value */
  uint32_t wind_compensation_theta; /* floating point value */
  uint32_t wind_compensation_phi; /* floating point value */
  uint32_t state_x1; /* floating point value */
  uint32_t state_x2; /* floating point value */
  uint32_t state_x3; /* floating point value */
  uint32_t state_x4; /* floating point value */
  uint32_t state_x5; /* floating point value */
  uint32_t state_x6; /* floating point value */
  uint32_t magneto_debug1; /* floating point value */
  uint32_t magneto_debug2; /* floating point value */
  uint32_t magneto_debug3; /* floating point value */
}_ATTRIBUTE_PACKED_ navdata_wind_speed_t;

typedef struct _navdata_kalman_pressure_t{
  uint16_t   tag;
  uint16_t   size;

  uint32_t offset_pressure; /* floating point value */
  uint32_t est_z; /* floating point value */
  uint32_t est_zdot; /* floating point value */
  uint32_t est_bias_PWM; /* floating point value */
  uint32_t est_biais_pression; /* floating point value */
  uint32_t offset_US; /* floating point value */
  uint32_t prediction_US; /* floating point value */
  uint32_t cov_alt; /* floating point value */
  uint32_t cov_PWM; /* floating point value */
  uint32_t cov_vitesse; /* floating point value */
  bool_t    bool_effet_sol;
  uint32_t somme_inno; /* floating point value */
  bool_t    flag_rejet_US;
  uint32_t u_multisinus; /* floating point value */
  uint32_t gaz_altitude; /* floating point value */
  bool_t    Flag_multisinus;
  bool_t    Flag_multisinus_debut;
}_ATTRIBUTE_PACKED_ navdata_kalman_pressure_t;

// TODO: depreciated struct ? remove it ?
typedef struct navdata_zimmu_3000_t {
uint16_t   tag;
uint16_t   size;

	int32_t vzimmuLSB;
	uint32_t vzfind; /* floating point value */

}_ATTRIBUTE_PACKED_ navdata_zimmu_3000_t;

typedef struct _navdata_phys_measures_t {
  uint16_t   tag;
  uint16_t   size;

  uint32_t   accs_temp; /* floating point value */
  uint16_t    gyro_temp;
  uint32_t   phys_accs[NB_ACCS]; /* floating point value */
  uint32_t   phys_gyros[NB_GYROS]; /* floating point value */
  uint32_t    alim3V3;              // 3.3volt alim [LSB]
  uint32_t    vrefEpson;            // ref volt Epson gyro [LSB]
  uint32_t    vrefIDG;              // ref volt IDG gyro [LSB]
}_ATTRIBUTE_PACKED_ navdata_phys_measures_t;


typedef struct _navdata_gyros_offsets_t {
  uint16_t   tag;
  uint16_t   size;

  uint32_t offset_g[NB_GYROS]; /* floating point value */
}_ATTRIBUTE_PACKED_ navdata_gyros_offsets_t;


typedef struct _navdata_euler_angles_t {
  uint16_t   tag;
  uint16_t   size;

  uint32_t   theta_a; /* floating point value */
  uint32_t   phi_a; /* floating point value */
}_ATTRIBUTE_PACKED_ navdata_euler_angles_t;


typedef struct _navdata_references_t {
  uint16_t   tag;
  uint16_t   size;

  int32_t   ref_theta;
  int32_t   ref_phi;
  int32_t   ref_theta_I;
  int32_t   ref_phi_I;
  int32_t   ref_pitch;
  int32_t   ref_roll;
  int32_t   ref_yaw;
  int32_t   ref_psi;

  uint32_t vx_ref; /* floating point value */
	uint32_t vy_ref; /* floating point value */
	uint32_t theta_mod; /* floating point value */
	uint32_t phi_mod; /* floating point value */

	uint32_t k_v_x; /* floating point value */
	uint32_t k_v_y; /* floating point value */
	uint32_t  k_mode;

	uint32_t ui_time; /* floating point value */
	uint32_t ui_theta; /* floating point value */
	uint32_t ui_phi; /* floating point value */
	uint32_t ui_psi; /* floating point value */
	uint32_t ui_psi_accuracy; /* floating point value */
	int32_t ui_seq;

}_ATTRIBUTE_PACKED_ navdata_references_t;


typedef struct _navdata_trims_t {
  uint16_t   tag;
  uint16_t   size;

  uint32_t angular_rates_trim_r; /* floating point value */
  uint32_t euler_angles_trim_theta; /* floating point value */
  uint32_t euler_angles_trim_phi; /* floating point value */
}_ATTRIBUTE_PACKED_ navdata_trims_t;

typedef struct _navdata_rc_references_t {
  uint16_t   tag;
  uint16_t   size;

  int32_t    rc_ref_pitch;
  int32_t    rc_ref_roll;
  int32_t    rc_ref_yaw;
  int32_t    rc_ref_gaz;
  int32_t    rc_ref_ag;
}_ATTRIBUTE_PACKED_ navdata_rc_references_t;


typedef struct _navdata_pwm_t {
  uint16_t   tag;
  uint16_t   size;

  uint8_t     motor1;
  uint8_t     motor2;
  uint8_t     motor3;
  uint8_t     motor4;
  uint8_t	  sat_motor1;
  uint8_t	  sat_motor2;
  uint8_t	  sat_motor3;
  uint8_t	  sat_motor4;
  uint32_t   gaz_feed_forward; /* floating point value */
  uint32_t   gaz_altitude; /* floating point value */
  uint32_t   altitude_integral; /* floating point value */
  uint32_t   vz_ref; /* floating point value */
  int32_t     u_pitch;
  int32_t     u_roll;
  int32_t     u_yaw;
  uint32_t   yaw_u_I; /* floating point value */
  int32_t     u_pitch_planif;
  int32_t     u_roll_planif;
  int32_t     u_yaw_planif;
  uint32_t   u_gaz_planif; /* floating point value */
  uint16_t    current_motor1;
  uint16_t    current_motor2;
  uint16_t    current_motor3;
  uint16_t    current_motor4;
	//WARNING: new navdata (FC 26/07/2011)
	uint32_t 	altitude_prop; /* floating point value */
	uint32_t 	altitude_der; /* floating point value */
}_ATTRIBUTE_PACKED_ navdata_pwm_t;


typedef struct _navdata_altitude_t {
  uint16_t   tag;
  uint16_t   size;

  int32_t   altitude_vision;
  uint32_t altitude_vz; /* floating point value */
  int32_t   altitude_ref;
  int32_t   altitude_raw;

	uint32_t	obs_accZ; /*floating point value */
	uint32_t 	obs_alt; /* floating point value */
	uint32_t obs_x[3];
	uint32_t 		obs_state;
	uint32_t est_vb[2];
	uint32_t 		est_state ;

}_ATTRIBUTE_PACKED_ navdata_altitude_t;


typedef struct _navdata_vision_raw_t {
  uint16_t   tag;
  uint16_t   size;

  uint32_t vision_tx_raw; /* floating point value */
  uint32_t vision_ty_raw; /* floating point value */
  uint32_t vision_tz_raw; /* floating point value */
}_ATTRIBUTE_PACKED_ navdata_vision_raw_t;


typedef struct _navdata_vision_t {
  uint16_t   tag;
  uint16_t   size;

  uint32_t   vision_state;
  int32_t    vision_misc;
  uint32_t  vision_phi_trim; /* floating point value */
  uint32_t  vision_phi_ref_prop; /* floating point value */
  uint32_t  vision_theta_trim; /* floating point value */
  uint32_t  vision_theta_ref_prop; /* floating point value */

  int32_t    new_raw_picture;
  uint32_t  theta_capture; /* floating point value */
  uint32_t  phi_capture; /* floating point value */
  uint32_t  psi_capture; /* floating point value */
  int32_t    altitude_capture;
  uint32_t   time_capture;     // time in TSECDEC format (see config.h)
  velocities_t body_v;

  uint32_t  delta_phi; /* floating point value */
  uint32_t  delta_theta; /* floating point value */
  uint32_t  delta_psi; /* floating point value */

	uint32_t  gold_defined;
	uint32_t  gold_reset;
	uint32_t gold_x; /* floating point value */
	uint32_t gold_y; /* floating point value */
}_ATTRIBUTE_PACKED_ navdata_vision_t;


typedef struct _navdata_watchdog_t {
  uint16_t   tag;
  uint16_t   size;

  // +4 bytes
  int32_t    watchdog;
}_ATTRIBUTE_PACKED_ navdata_watchdog_t;

typedef struct _navdata_adc_data_frame_t {
  uint16_t  tag;
  uint16_t  size;

  uint32_t  version;
  uint8_t   data_frame[32];
}_ATTRIBUTE_PACKED_ navdata_adc_data_frame_t;

typedef struct _navdata_video_stream_t {
  uint16_t  tag;
  uint16_t  size;

  uint8_t 	quant;			// quantizer reference used to encode frame [1:31]
  uint32_t	frame_size;		// frame size (bytes)
  uint32_t	frame_number;	// frame index
  uint32_t	atcmd_ref_seq;  // atmcd ref sequence number
  uint32_t	atcmd_mean_ref_gap;	// mean time between two consecutive atcmd_ref (ms)
  uint32_t atcmd_var_ref_gap; /* floating point value */
  uint32_t	atcmd_ref_quality; // estimator of atcmd link quality

  // drone2
  uint32_t  out_bitrate;     // measured out throughput from the video tcp socket
  uint32_t  desired_bitrate; // last frame size generated by the video encoder

  // misc temporary data
  int32_t  data1;
  int32_t  data2;
  int32_t  data3;
  int32_t  data4;
  int32_t  data5;

  // queue usage
  uint32_t tcp_queue_level;
  uint32_t fifo_queue_level;

}_ATTRIBUTE_PACKED_ navdata_video_stream_t;

typedef enum
{
	NAVDATA_HDVIDEO_STORAGE_FIFO_IS_FULL = (1<<0),
	NAVDATA_HDVIDEO_USBKEY_IS_PRESENT   = (1<<8),
	NAVDATA_HDVIDEO_USBKEY_IS_RECORDING = (1<<9),
	NAVDATA_HDVIDEO_USBKEY_IS_FULL      = (1<<10)
}_navdata_hdvideo_states_t;


typedef struct _navdata_hdvideo_stream_t {
  uint16_t  tag;
  uint16_t  size;

  uint32_t hdvideo_state;
  uint32_t storage_fifo_nb_packets;
  uint32_t storage_fifo_size;
  uint32_t usbkey_size;         /*! USB key in kbytes - 0 if no key present */
  uint32_t usbkey_freespace;    /*! USB key free space in kbytes - 0 if no key present */
  uint32_t frame_number;        /*! 'frame_number' PaVE field of the frame starting to be encoded for the HD stream */
  uint32_t usbkey_remaining_time; /*! time in seconds */

}_ATTRIBUTE_PACKED_ navdata_hdvideo_stream_t;


typedef struct _navdata_games_t {
  uint16_t  tag;
  uint16_t  size;
  uint32_t  double_tap_counter;
  uint32_t  finish_line_counter;
}_ATTRIBUTE_PACKED_ navdata_games_t;

typedef struct _navdata_wifi_t {
  uint16_t  tag;
  uint16_t  size;
  uint32_t link_quality;
}_ATTRIBUTE_PACKED_  navdata_wifi_t;

#if defined(_MSC_VER)
	/* Go back to default packing policy */
	#pragma pack()
#endif

#endif // _NAVDATA_COMMON_H_

