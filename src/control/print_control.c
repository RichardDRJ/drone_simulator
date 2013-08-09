#include "control/print_control.h"
#include "util/error.h"
#include <stdio.h>
#include <unistd.h>

void print_control_init(struct data_options *d)
{
    d->at_ref = print_at_ref;
    d->at_pcmd_mag = print_at_pcmd_mag;
    d->at_pcmd = print_at_pcmd;
}

void print_at_ref(struct control_session_data *d, uint8_t start, uint8_t select)
{

}

void print_at_pcmd(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed)
{
    print_at_pcmd_mag(d, control, roll, pitch, vert_speed, ang_speed, 0.0f, 0.0f);
}

void print_at_pcmd_mag(struct control_session_data *d, uint32_t control, float roll, float pitch, float vert_speed, float ang_speed, float magneto_psi, float magneto_psi_accuracy)
{
    printf("roll: %f\n", roll * d->max_roll);
    printf("pitch: %f\n", -pitch * d->max_pitch);
    printf("vspeed: %f\n", vert_speed * d->max_vert_speed / 1000.0f);
    printf("aspeed: %f\n", ang_speed * d->max_ang_speed);
}