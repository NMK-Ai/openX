#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_8658494792274923532);
void live_err_fun(double *nom_x, double *delta_x, double *out_5030499929160793532);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_4052708836067517963);
void live_H_mod_fun(double *state, double *out_6322168194681504880);
void live_f_fun(double *state, double dt, double *out_3442237330279375227);
void live_F_fun(double *state, double dt, double *out_3171319833038800398);
void live_h_4(double *state, double *unused, double *out_5465770794080021784);
void live_H_4(double *state, double *unused, double *out_6125132464680508505);
void live_h_9(double *state, double *unused, double *out_7089745365262767215);
void live_H_9(double *state, double *unused, double *out_5883942818050917860);
void live_h_10(double *state, double *unused, double *out_8375116310952542662);
void live_H_10(double *state, double *unused, double *out_6839705878356543240);
void live_h_12(double *state, double *unused, double *out_5634293049988341889);
void live_H_12(double *state, double *unused, double *out_5504033439632914838);
void live_h_35(double *state, double *unused, double *out_7897287573482006209);
void live_H_35(double *state, double *unused, double *out_2758470407307901129);
void live_h_32(double *state, double *unused, double *out_4225622587469561841);
void live_H_32(double *state, double *unused, double *out_8773409162707534131);
void live_h_13(double *state, double *unused, double *out_8934386510755568170);
void live_H_13(double *state, double *unused, double *out_4616232273031006377);
void live_h_14(double *state, double *unused, double *out_7089745365262767215);
void live_H_14(double *state, double *unused, double *out_5883942818050917860);
void live_h_33(double *state, double *unused, double *out_4320971372356475799);
void live_H_33(double *state, double *unused, double *out_392086597330956475);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}