#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_159602887683425670) {
   out_159602887683425670[0] = delta_x[0] + nom_x[0];
   out_159602887683425670[1] = delta_x[1] + nom_x[1];
   out_159602887683425670[2] = delta_x[2] + nom_x[2];
   out_159602887683425670[3] = delta_x[3] + nom_x[3];
   out_159602887683425670[4] = delta_x[4] + nom_x[4];
   out_159602887683425670[5] = delta_x[5] + nom_x[5];
   out_159602887683425670[6] = delta_x[6] + nom_x[6];
   out_159602887683425670[7] = delta_x[7] + nom_x[7];
   out_159602887683425670[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6799422043649238232) {
   out_6799422043649238232[0] = -nom_x[0] + true_x[0];
   out_6799422043649238232[1] = -nom_x[1] + true_x[1];
   out_6799422043649238232[2] = -nom_x[2] + true_x[2];
   out_6799422043649238232[3] = -nom_x[3] + true_x[3];
   out_6799422043649238232[4] = -nom_x[4] + true_x[4];
   out_6799422043649238232[5] = -nom_x[5] + true_x[5];
   out_6799422043649238232[6] = -nom_x[6] + true_x[6];
   out_6799422043649238232[7] = -nom_x[7] + true_x[7];
   out_6799422043649238232[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_824948957987322433) {
   out_824948957987322433[0] = 1.0;
   out_824948957987322433[1] = 0.0;
   out_824948957987322433[2] = 0.0;
   out_824948957987322433[3] = 0.0;
   out_824948957987322433[4] = 0.0;
   out_824948957987322433[5] = 0.0;
   out_824948957987322433[6] = 0.0;
   out_824948957987322433[7] = 0.0;
   out_824948957987322433[8] = 0.0;
   out_824948957987322433[9] = 0.0;
   out_824948957987322433[10] = 1.0;
   out_824948957987322433[11] = 0.0;
   out_824948957987322433[12] = 0.0;
   out_824948957987322433[13] = 0.0;
   out_824948957987322433[14] = 0.0;
   out_824948957987322433[15] = 0.0;
   out_824948957987322433[16] = 0.0;
   out_824948957987322433[17] = 0.0;
   out_824948957987322433[18] = 0.0;
   out_824948957987322433[19] = 0.0;
   out_824948957987322433[20] = 1.0;
   out_824948957987322433[21] = 0.0;
   out_824948957987322433[22] = 0.0;
   out_824948957987322433[23] = 0.0;
   out_824948957987322433[24] = 0.0;
   out_824948957987322433[25] = 0.0;
   out_824948957987322433[26] = 0.0;
   out_824948957987322433[27] = 0.0;
   out_824948957987322433[28] = 0.0;
   out_824948957987322433[29] = 0.0;
   out_824948957987322433[30] = 1.0;
   out_824948957987322433[31] = 0.0;
   out_824948957987322433[32] = 0.0;
   out_824948957987322433[33] = 0.0;
   out_824948957987322433[34] = 0.0;
   out_824948957987322433[35] = 0.0;
   out_824948957987322433[36] = 0.0;
   out_824948957987322433[37] = 0.0;
   out_824948957987322433[38] = 0.0;
   out_824948957987322433[39] = 0.0;
   out_824948957987322433[40] = 1.0;
   out_824948957987322433[41] = 0.0;
   out_824948957987322433[42] = 0.0;
   out_824948957987322433[43] = 0.0;
   out_824948957987322433[44] = 0.0;
   out_824948957987322433[45] = 0.0;
   out_824948957987322433[46] = 0.0;
   out_824948957987322433[47] = 0.0;
   out_824948957987322433[48] = 0.0;
   out_824948957987322433[49] = 0.0;
   out_824948957987322433[50] = 1.0;
   out_824948957987322433[51] = 0.0;
   out_824948957987322433[52] = 0.0;
   out_824948957987322433[53] = 0.0;
   out_824948957987322433[54] = 0.0;
   out_824948957987322433[55] = 0.0;
   out_824948957987322433[56] = 0.0;
   out_824948957987322433[57] = 0.0;
   out_824948957987322433[58] = 0.0;
   out_824948957987322433[59] = 0.0;
   out_824948957987322433[60] = 1.0;
   out_824948957987322433[61] = 0.0;
   out_824948957987322433[62] = 0.0;
   out_824948957987322433[63] = 0.0;
   out_824948957987322433[64] = 0.0;
   out_824948957987322433[65] = 0.0;
   out_824948957987322433[66] = 0.0;
   out_824948957987322433[67] = 0.0;
   out_824948957987322433[68] = 0.0;
   out_824948957987322433[69] = 0.0;
   out_824948957987322433[70] = 1.0;
   out_824948957987322433[71] = 0.0;
   out_824948957987322433[72] = 0.0;
   out_824948957987322433[73] = 0.0;
   out_824948957987322433[74] = 0.0;
   out_824948957987322433[75] = 0.0;
   out_824948957987322433[76] = 0.0;
   out_824948957987322433[77] = 0.0;
   out_824948957987322433[78] = 0.0;
   out_824948957987322433[79] = 0.0;
   out_824948957987322433[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_3529301216258866967) {
   out_3529301216258866967[0] = state[0];
   out_3529301216258866967[1] = state[1];
   out_3529301216258866967[2] = state[2];
   out_3529301216258866967[3] = state[3];
   out_3529301216258866967[4] = state[4];
   out_3529301216258866967[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_3529301216258866967[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_3529301216258866967[7] = state[7];
   out_3529301216258866967[8] = state[8];
}
void F_fun(double *state, double dt, double *out_153009162267755836) {
   out_153009162267755836[0] = 1;
   out_153009162267755836[1] = 0;
   out_153009162267755836[2] = 0;
   out_153009162267755836[3] = 0;
   out_153009162267755836[4] = 0;
   out_153009162267755836[5] = 0;
   out_153009162267755836[6] = 0;
   out_153009162267755836[7] = 0;
   out_153009162267755836[8] = 0;
   out_153009162267755836[9] = 0;
   out_153009162267755836[10] = 1;
   out_153009162267755836[11] = 0;
   out_153009162267755836[12] = 0;
   out_153009162267755836[13] = 0;
   out_153009162267755836[14] = 0;
   out_153009162267755836[15] = 0;
   out_153009162267755836[16] = 0;
   out_153009162267755836[17] = 0;
   out_153009162267755836[18] = 0;
   out_153009162267755836[19] = 0;
   out_153009162267755836[20] = 1;
   out_153009162267755836[21] = 0;
   out_153009162267755836[22] = 0;
   out_153009162267755836[23] = 0;
   out_153009162267755836[24] = 0;
   out_153009162267755836[25] = 0;
   out_153009162267755836[26] = 0;
   out_153009162267755836[27] = 0;
   out_153009162267755836[28] = 0;
   out_153009162267755836[29] = 0;
   out_153009162267755836[30] = 1;
   out_153009162267755836[31] = 0;
   out_153009162267755836[32] = 0;
   out_153009162267755836[33] = 0;
   out_153009162267755836[34] = 0;
   out_153009162267755836[35] = 0;
   out_153009162267755836[36] = 0;
   out_153009162267755836[37] = 0;
   out_153009162267755836[38] = 0;
   out_153009162267755836[39] = 0;
   out_153009162267755836[40] = 1;
   out_153009162267755836[41] = 0;
   out_153009162267755836[42] = 0;
   out_153009162267755836[43] = 0;
   out_153009162267755836[44] = 0;
   out_153009162267755836[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_153009162267755836[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_153009162267755836[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_153009162267755836[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_153009162267755836[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_153009162267755836[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_153009162267755836[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_153009162267755836[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_153009162267755836[53] = -9.8000000000000007*dt;
   out_153009162267755836[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_153009162267755836[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_153009162267755836[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_153009162267755836[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_153009162267755836[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_153009162267755836[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_153009162267755836[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_153009162267755836[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_153009162267755836[62] = 0;
   out_153009162267755836[63] = 0;
   out_153009162267755836[64] = 0;
   out_153009162267755836[65] = 0;
   out_153009162267755836[66] = 0;
   out_153009162267755836[67] = 0;
   out_153009162267755836[68] = 0;
   out_153009162267755836[69] = 0;
   out_153009162267755836[70] = 1;
   out_153009162267755836[71] = 0;
   out_153009162267755836[72] = 0;
   out_153009162267755836[73] = 0;
   out_153009162267755836[74] = 0;
   out_153009162267755836[75] = 0;
   out_153009162267755836[76] = 0;
   out_153009162267755836[77] = 0;
   out_153009162267755836[78] = 0;
   out_153009162267755836[79] = 0;
   out_153009162267755836[80] = 1;
}
void h_25(double *state, double *unused, double *out_6336809938858143189) {
   out_6336809938858143189[0] = state[6];
}
void H_25(double *state, double *unused, double *out_319614515787025721) {
   out_319614515787025721[0] = 0;
   out_319614515787025721[1] = 0;
   out_319614515787025721[2] = 0;
   out_319614515787025721[3] = 0;
   out_319614515787025721[4] = 0;
   out_319614515787025721[5] = 0;
   out_319614515787025721[6] = 1;
   out_319614515787025721[7] = 0;
   out_319614515787025721[8] = 0;
}
void h_24(double *state, double *unused, double *out_8531840257113345752) {
   out_8531840257113345752[0] = state[4];
   out_8531840257113345752[1] = state[5];
}
void H_24(double *state, double *unused, double *out_4301937667184793619) {
   out_4301937667184793619[0] = 0;
   out_4301937667184793619[1] = 0;
   out_4301937667184793619[2] = 0;
   out_4301937667184793619[3] = 0;
   out_4301937667184793619[4] = 1;
   out_4301937667184793619[5] = 0;
   out_4301937667184793619[6] = 0;
   out_4301937667184793619[7] = 0;
   out_4301937667184793619[8] = 0;
   out_4301937667184793619[9] = 0;
   out_4301937667184793619[10] = 0;
   out_4301937667184793619[11] = 0;
   out_4301937667184793619[12] = 0;
   out_4301937667184793619[13] = 0;
   out_4301937667184793619[14] = 1;
   out_4301937667184793619[15] = 0;
   out_4301937667184793619[16] = 0;
   out_4301937667184793619[17] = 0;
}
void h_30(double *state, double *unused, double *out_434025287492207747) {
   out_434025287492207747[0] = state[4];
}
void H_30(double *state, double *unused, double *out_6597075825704591034) {
   out_6597075825704591034[0] = 0;
   out_6597075825704591034[1] = 0;
   out_6597075825704591034[2] = 0;
   out_6597075825704591034[3] = 0;
   out_6597075825704591034[4] = 1;
   out_6597075825704591034[5] = 0;
   out_6597075825704591034[6] = 0;
   out_6597075825704591034[7] = 0;
   out_6597075825704591034[8] = 0;
}
void h_26(double *state, double *unused, double *out_5435698555153053392) {
   out_5435698555153053392[0] = state[7];
}
void H_26(double *state, double *unused, double *out_337239548323286183) {
   out_337239548323286183[0] = 0;
   out_337239548323286183[1] = 0;
   out_337239548323286183[2] = 0;
   out_337239548323286183[3] = 0;
   out_337239548323286183[4] = 0;
   out_337239548323286183[5] = 0;
   out_337239548323286183[6] = 0;
   out_337239548323286183[7] = 1;
   out_337239548323286183[8] = 0;
}
void h_27(double *state, double *unused, double *out_7108260867190562782) {
   out_7108260867190562782[0] = state[3];
}
void H_27(double *state, double *unused, double *out_4422312513904166123) {
   out_4422312513904166123[0] = 0;
   out_4422312513904166123[1] = 0;
   out_4422312513904166123[2] = 0;
   out_4422312513904166123[3] = 1;
   out_4422312513904166123[4] = 0;
   out_4422312513904166123[5] = 0;
   out_4422312513904166123[6] = 0;
   out_4422312513904166123[7] = 0;
   out_4422312513904166123[8] = 0;
}
void h_29(double *state, double *unused, double *out_4489175435771333084) {
   out_4489175435771333084[0] = state[1];
}
void H_29(double *state, double *unused, double *out_7107307170018983218) {
   out_7107307170018983218[0] = 0;
   out_7107307170018983218[1] = 1;
   out_7107307170018983218[2] = 0;
   out_7107307170018983218[3] = 0;
   out_7107307170018983218[4] = 0;
   out_7107307170018983218[5] = 0;
   out_7107307170018983218[6] = 0;
   out_7107307170018983218[7] = 0;
   out_7107307170018983218[8] = 0;
}
void h_28(double *state, double *unused, double *out_8798708701926243742) {
   out_8798708701926243742[0] = state[0];
}
void H_28(double *state, double *unused, double *out_2024908152949452644) {
   out_2024908152949452644[0] = 1;
   out_2024908152949452644[1] = 0;
   out_2024908152949452644[2] = 0;
   out_2024908152949452644[3] = 0;
   out_2024908152949452644[4] = 0;
   out_2024908152949452644[5] = 0;
   out_2024908152949452644[6] = 0;
   out_2024908152949452644[7] = 0;
   out_2024908152949452644[8] = 0;
}
void h_31(double *state, double *unused, double *out_7183720145107809723) {
   out_7183720145107809723[0] = state[8];
}
void H_31(double *state, double *unused, double *out_288968553910065293) {
   out_288968553910065293[0] = 0;
   out_288968553910065293[1] = 0;
   out_288968553910065293[2] = 0;
   out_288968553910065293[3] = 0;
   out_288968553910065293[4] = 0;
   out_288968553910065293[5] = 0;
   out_288968553910065293[6] = 0;
   out_288968553910065293[7] = 0;
   out_288968553910065293[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_159602887683425670) {
  err_fun(nom_x, delta_x, out_159602887683425670);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6799422043649238232) {
  inv_err_fun(nom_x, true_x, out_6799422043649238232);
}
void car_H_mod_fun(double *state, double *out_824948957987322433) {
  H_mod_fun(state, out_824948957987322433);
}
void car_f_fun(double *state, double dt, double *out_3529301216258866967) {
  f_fun(state,  dt, out_3529301216258866967);
}
void car_F_fun(double *state, double dt, double *out_153009162267755836) {
  F_fun(state,  dt, out_153009162267755836);
}
void car_h_25(double *state, double *unused, double *out_6336809938858143189) {
  h_25(state, unused, out_6336809938858143189);
}
void car_H_25(double *state, double *unused, double *out_319614515787025721) {
  H_25(state, unused, out_319614515787025721);
}
void car_h_24(double *state, double *unused, double *out_8531840257113345752) {
  h_24(state, unused, out_8531840257113345752);
}
void car_H_24(double *state, double *unused, double *out_4301937667184793619) {
  H_24(state, unused, out_4301937667184793619);
}
void car_h_30(double *state, double *unused, double *out_434025287492207747) {
  h_30(state, unused, out_434025287492207747);
}
void car_H_30(double *state, double *unused, double *out_6597075825704591034) {
  H_30(state, unused, out_6597075825704591034);
}
void car_h_26(double *state, double *unused, double *out_5435698555153053392) {
  h_26(state, unused, out_5435698555153053392);
}
void car_H_26(double *state, double *unused, double *out_337239548323286183) {
  H_26(state, unused, out_337239548323286183);
}
void car_h_27(double *state, double *unused, double *out_7108260867190562782) {
  h_27(state, unused, out_7108260867190562782);
}
void car_H_27(double *state, double *unused, double *out_4422312513904166123) {
  H_27(state, unused, out_4422312513904166123);
}
void car_h_29(double *state, double *unused, double *out_4489175435771333084) {
  h_29(state, unused, out_4489175435771333084);
}
void car_H_29(double *state, double *unused, double *out_7107307170018983218) {
  H_29(state, unused, out_7107307170018983218);
}
void car_h_28(double *state, double *unused, double *out_8798708701926243742) {
  h_28(state, unused, out_8798708701926243742);
}
void car_H_28(double *state, double *unused, double *out_2024908152949452644) {
  H_28(state, unused, out_2024908152949452644);
}
void car_h_31(double *state, double *unused, double *out_7183720145107809723) {
  h_31(state, unused, out_7183720145107809723);
}
void car_H_31(double *state, double *unused, double *out_288968553910065293) {
  H_31(state, unused, out_288968553910065293);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
