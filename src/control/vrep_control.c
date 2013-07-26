#include "control/vrep_control.h"
#include "util/error.h"
#include "libs/vrep/extApi.h"
#include "libs/vrep/extApiPlatform.h"

static simxInt client_id;

void vrep_init(void)
{
    simxInt ret = simxStart("127.0.0.1", 20000, 1, 0, 1000, 5);
    if(ret == -1)
        error("Could not connect to vrep");

    client_id = ret;
}

void vrep_at_ref(struct control_session_data *d, uint8_t start, uint8_t select)
{

}

void vrep_at_pcmd_mag(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed, float magneto_psi, float magneto_psi_accuracy)
{
   simxSetFloatSignal(client_id, "QCRoll", roll * d->max_roll, simx_opmode_oneshot);
   simxSetFloatSignal(client_id, "QCPitch", pitch * d->max_pitch, simx_opmode_oneshot);
   simxSetFloatSignal(client_id, "QCVSpeed", vert_speed * d->max_vert_speed / 1000.0f, simx_opmode_oneshot);
   simxSetFloatSignal(client_id, "QCASpeed", ang_speed * d->max_ang_speed, simx_opmode_oneshot);
}
