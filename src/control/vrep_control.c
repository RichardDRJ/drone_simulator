#include "control/vrep_control.h"
#include "util/error.h"
#include "libs/vrep/extApi.h"
#include "libs/vrep/extApiPlatform.h"
#include <stdio.h>

static simxInt client_id;
extern pthread_mutex_t vrep_mutex;

void vrep_control_init(struct data_options *d, simxInt id)
{
    client_id = id;

    d->at_ref = vrep_at_ref;
    d->at_pcmd_mag = vrep_at_pcmd_mag;
    d->at_pcmd = vrep_at_pcmd;
}

void vrep_at_ref(struct control_session_data *d, uint8_t start, uint8_t select)
{

}

void vrep_at_pcmd(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed)
{
    vrep_at_pcmd_mag(d, control, roll, pitch, vert_speed, ang_speed, 0.0f, 0.0f);
}

void vrep_at_pcmd_mag(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed, float magneto_psi, float magneto_psi_accuracy)
{
	pthread_mutex_lock(&vrep_mutex);
    simxSetFloatSignal(client_id, "QCRoll", roll * d->max_roll, simx_opmode_oneshot);
    simxSetFloatSignal(client_id, "QCPitch", -pitch * d->max_pitch, simx_opmode_oneshot);
    simxSetFloatSignal(client_id, "QCVSpeed", vert_speed * d->max_vert_speed / 1000.0f, simx_opmode_oneshot);
    simxSetFloatSignal(client_id, "QCASpeed", -ang_speed * d->max_ang_speed, simx_opmode_oneshot);
    pthread_mutex_unlock(&vrep_mutex);
}
