#include "util/data_options.h"
#include "control/control_server.h"

void print_control_init(struct data_options *d);

void print_at_ref(struct control_session_data *d, uint8_t start, uint8_t select);

void print_at_pcmd(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed);

void print_at_pcmd_mag(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed, float magneto_psi, float magneto_psi_accuracy);