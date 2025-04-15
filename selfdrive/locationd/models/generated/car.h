#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_2404323163298862836);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8674924215746766711);
void car_H_mod_fun(double *state, double *out_7483312979707705837);
void car_f_fun(double *state, double dt, double *out_3592507306981724774);
void car_F_fun(double *state, double dt, double *out_1048406091213510053);
void car_h_25(double *state, double *unused, double *out_398477380036345276);
void car_H_25(double *state, double *unused, double *out_1770242868089608542);
void car_h_24(double *state, double *unused, double *out_6615193940643128455);
void car_H_24(double *state, double *unused, double *out_8424448326197794299);
void car_h_30(double *state, double *unused, double *out_8291416874920868635);
void car_H_30(double *state, double *unused, double *out_1640903920946368472);
void car_h_26(double *state, double *unused, double *out_5254630823476535950);
void car_H_26(double *state, double *unused, double *out_1971260450784447682);
void car_h_27(double *state, double *unused, double *out_1341987232938007711);
void car_H_27(double *state, double *unused, double *out_533859390854056439);
void car_h_29(double *state, double *unused, double *out_7550921877979985330);
void car_H_29(double *state, double *unused, double *out_2247222117723607472);
void car_h_28(double *state, double *unused, double *out_4378773284382340396);
void car_H_28(double *state, double *unused, double *out_283591846158281221);
void car_h_31(double *state, double *unused, double *out_6372357846314005660);
void car_H_31(double *state, double *unused, double *out_1800888829966568970);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}