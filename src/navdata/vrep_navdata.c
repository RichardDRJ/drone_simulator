#include "navdata/navdata_common.h"
#include "navdata/vrep_navdata.h"

static simxInt client_id;
extern pthread_mutex_t vrep_mutex;

void vrep_navdata_init(struct data_options *d, simxInt id)
{
    client_id = id;
    d->fill_navdata_demo = vrep_fill_navdata_demo;
}

void vrep_fill_navdata_demo(navdata_demo_t *demo)
{
    demo->tag = NAVDATA_DEMO_TAG;
    demo->ctrl_state = 0;
    demo->vbat_flying_percentage = 0xFFFFFFFF;

    simxGetFloatSignal(client_id, "theta", (float*)&demo->theta, simx_opmode_oneshot_wait);
    simxGetFloatSignal(client_id, "phi", (float*)&demo->phi, simx_opmode_oneshot_wait);
    simxGetFloatSignal(client_id, "psi", (float*)&demo->psi, simx_opmode_oneshot_wait);
    simxGetFloatSignal(client_id, "altitude", (float*)&demo->altitude, simx_opmode_oneshot_wait);

    simxGetFloatSignal(client_id, "vx", (float*)&demo->vx, simx_opmode_oneshot_wait);
    simxGetFloatSignal(client_id, "vy", (float*)&demo->vy, simx_opmode_oneshot_wait);
    simxGetFloatSignal(client_id, "vz", (float*)&demo->vz, simx_opmode_oneshot_wait);

    demo->num_frames = 0;

    demo->size = sizeof(navdata_demo_t);

    printf("theta: %f\nphi: %f\npsi: %f\naltitude: %f\nvx: %f\nvy: %f\nvz: %f\n\n", *(float*)&demo->theta, *(float*)&demo->phi, *(float*)&demo->psi, *(float*)&demo->altitude, *(float*)&demo->vx, *(float*)&demo->vy, *(float*)&demo->vz);
}
