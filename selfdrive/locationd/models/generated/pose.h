#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_6215768011470258474);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6516201049276311805);
void pose_H_mod_fun(double *state, double *out_8843316986321445925);
void pose_f_fun(double *state, double dt, double *out_1388079773596962594);
void pose_F_fun(double *state, double dt, double *out_1987774110883025382);
void pose_h_4(double *state, double *unused, double *out_6417975175098302508);
void pose_H_4(double *state, double *unused, double *out_3690798601282365190);
void pose_h_10(double *state, double *unused, double *out_933754340625772488);
void pose_H_10(double *state, double *unused, double *out_2496885064021374014);
void pose_h_13(double *state, double *unused, double *out_7412978871522735544);
void pose_H_13(double *state, double *unused, double *out_478524775950032389);
void pose_h_14(double *state, double *unused, double *out_1424794695951564614);
void pose_H_14(double *state, double *unused, double *out_6773587033577737486);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}