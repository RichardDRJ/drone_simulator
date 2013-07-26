#ifndef VREP_CONTROL_H
#define VREP_CONTROL_H

#include "control/control_server.h"
#include <inttypes.h>

void vrep_init(void);

void vrep_at_ref(struct control_session_data *d, uint8_t start, uint8_t select);
void vrep_at_pcmd_mag(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed, float magneto_psi, float magneto_psi_accuracy);

#endif
