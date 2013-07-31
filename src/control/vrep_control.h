#ifndef VREP_CONTROL_H
#define VREP_CONTROL_H

#include "control/control_server.h"
#include "util/data_options.h"
#include <inttypes.h>
#include "libs/vrep/extApi.h"

void vrep_control_init(struct data_options *d, simxInt id);

void vrep_at_ref(struct control_session_data *d, uint8_t start, uint8_t select);
void vrep_at_pcmd_mag(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed, float magneto_psi, float magneto_psi_accuracy);
void vrep_at_pcmd(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed);

#endif
