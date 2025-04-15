#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.13.2                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_6215768011470258474) {
   out_6215768011470258474[0] = delta_x[0] + nom_x[0];
   out_6215768011470258474[1] = delta_x[1] + nom_x[1];
   out_6215768011470258474[2] = delta_x[2] + nom_x[2];
   out_6215768011470258474[3] = delta_x[3] + nom_x[3];
   out_6215768011470258474[4] = delta_x[4] + nom_x[4];
   out_6215768011470258474[5] = delta_x[5] + nom_x[5];
   out_6215768011470258474[6] = delta_x[6] + nom_x[6];
   out_6215768011470258474[7] = delta_x[7] + nom_x[7];
   out_6215768011470258474[8] = delta_x[8] + nom_x[8];
   out_6215768011470258474[9] = delta_x[9] + nom_x[9];
   out_6215768011470258474[10] = delta_x[10] + nom_x[10];
   out_6215768011470258474[11] = delta_x[11] + nom_x[11];
   out_6215768011470258474[12] = delta_x[12] + nom_x[12];
   out_6215768011470258474[13] = delta_x[13] + nom_x[13];
   out_6215768011470258474[14] = delta_x[14] + nom_x[14];
   out_6215768011470258474[15] = delta_x[15] + nom_x[15];
   out_6215768011470258474[16] = delta_x[16] + nom_x[16];
   out_6215768011470258474[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6516201049276311805) {
   out_6516201049276311805[0] = -nom_x[0] + true_x[0];
   out_6516201049276311805[1] = -nom_x[1] + true_x[1];
   out_6516201049276311805[2] = -nom_x[2] + true_x[2];
   out_6516201049276311805[3] = -nom_x[3] + true_x[3];
   out_6516201049276311805[4] = -nom_x[4] + true_x[4];
   out_6516201049276311805[5] = -nom_x[5] + true_x[5];
   out_6516201049276311805[6] = -nom_x[6] + true_x[6];
   out_6516201049276311805[7] = -nom_x[7] + true_x[7];
   out_6516201049276311805[8] = -nom_x[8] + true_x[8];
   out_6516201049276311805[9] = -nom_x[9] + true_x[9];
   out_6516201049276311805[10] = -nom_x[10] + true_x[10];
   out_6516201049276311805[11] = -nom_x[11] + true_x[11];
   out_6516201049276311805[12] = -nom_x[12] + true_x[12];
   out_6516201049276311805[13] = -nom_x[13] + true_x[13];
   out_6516201049276311805[14] = -nom_x[14] + true_x[14];
   out_6516201049276311805[15] = -nom_x[15] + true_x[15];
   out_6516201049276311805[16] = -nom_x[16] + true_x[16];
   out_6516201049276311805[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_8843316986321445925) {
   out_8843316986321445925[0] = 1.0;
   out_8843316986321445925[1] = 0.0;
   out_8843316986321445925[2] = 0.0;
   out_8843316986321445925[3] = 0.0;
   out_8843316986321445925[4] = 0.0;
   out_8843316986321445925[5] = 0.0;
   out_8843316986321445925[6] = 0.0;
   out_8843316986321445925[7] = 0.0;
   out_8843316986321445925[8] = 0.0;
   out_8843316986321445925[9] = 0.0;
   out_8843316986321445925[10] = 0.0;
   out_8843316986321445925[11] = 0.0;
   out_8843316986321445925[12] = 0.0;
   out_8843316986321445925[13] = 0.0;
   out_8843316986321445925[14] = 0.0;
   out_8843316986321445925[15] = 0.0;
   out_8843316986321445925[16] = 0.0;
   out_8843316986321445925[17] = 0.0;
   out_8843316986321445925[18] = 0.0;
   out_8843316986321445925[19] = 1.0;
   out_8843316986321445925[20] = 0.0;
   out_8843316986321445925[21] = 0.0;
   out_8843316986321445925[22] = 0.0;
   out_8843316986321445925[23] = 0.0;
   out_8843316986321445925[24] = 0.0;
   out_8843316986321445925[25] = 0.0;
   out_8843316986321445925[26] = 0.0;
   out_8843316986321445925[27] = 0.0;
   out_8843316986321445925[28] = 0.0;
   out_8843316986321445925[29] = 0.0;
   out_8843316986321445925[30] = 0.0;
   out_8843316986321445925[31] = 0.0;
   out_8843316986321445925[32] = 0.0;
   out_8843316986321445925[33] = 0.0;
   out_8843316986321445925[34] = 0.0;
   out_8843316986321445925[35] = 0.0;
   out_8843316986321445925[36] = 0.0;
   out_8843316986321445925[37] = 0.0;
   out_8843316986321445925[38] = 1.0;
   out_8843316986321445925[39] = 0.0;
   out_8843316986321445925[40] = 0.0;
   out_8843316986321445925[41] = 0.0;
   out_8843316986321445925[42] = 0.0;
   out_8843316986321445925[43] = 0.0;
   out_8843316986321445925[44] = 0.0;
   out_8843316986321445925[45] = 0.0;
   out_8843316986321445925[46] = 0.0;
   out_8843316986321445925[47] = 0.0;
   out_8843316986321445925[48] = 0.0;
   out_8843316986321445925[49] = 0.0;
   out_8843316986321445925[50] = 0.0;
   out_8843316986321445925[51] = 0.0;
   out_8843316986321445925[52] = 0.0;
   out_8843316986321445925[53] = 0.0;
   out_8843316986321445925[54] = 0.0;
   out_8843316986321445925[55] = 0.0;
   out_8843316986321445925[56] = 0.0;
   out_8843316986321445925[57] = 1.0;
   out_8843316986321445925[58] = 0.0;
   out_8843316986321445925[59] = 0.0;
   out_8843316986321445925[60] = 0.0;
   out_8843316986321445925[61] = 0.0;
   out_8843316986321445925[62] = 0.0;
   out_8843316986321445925[63] = 0.0;
   out_8843316986321445925[64] = 0.0;
   out_8843316986321445925[65] = 0.0;
   out_8843316986321445925[66] = 0.0;
   out_8843316986321445925[67] = 0.0;
   out_8843316986321445925[68] = 0.0;
   out_8843316986321445925[69] = 0.0;
   out_8843316986321445925[70] = 0.0;
   out_8843316986321445925[71] = 0.0;
   out_8843316986321445925[72] = 0.0;
   out_8843316986321445925[73] = 0.0;
   out_8843316986321445925[74] = 0.0;
   out_8843316986321445925[75] = 0.0;
   out_8843316986321445925[76] = 1.0;
   out_8843316986321445925[77] = 0.0;
   out_8843316986321445925[78] = 0.0;
   out_8843316986321445925[79] = 0.0;
   out_8843316986321445925[80] = 0.0;
   out_8843316986321445925[81] = 0.0;
   out_8843316986321445925[82] = 0.0;
   out_8843316986321445925[83] = 0.0;
   out_8843316986321445925[84] = 0.0;
   out_8843316986321445925[85] = 0.0;
   out_8843316986321445925[86] = 0.0;
   out_8843316986321445925[87] = 0.0;
   out_8843316986321445925[88] = 0.0;
   out_8843316986321445925[89] = 0.0;
   out_8843316986321445925[90] = 0.0;
   out_8843316986321445925[91] = 0.0;
   out_8843316986321445925[92] = 0.0;
   out_8843316986321445925[93] = 0.0;
   out_8843316986321445925[94] = 0.0;
   out_8843316986321445925[95] = 1.0;
   out_8843316986321445925[96] = 0.0;
   out_8843316986321445925[97] = 0.0;
   out_8843316986321445925[98] = 0.0;
   out_8843316986321445925[99] = 0.0;
   out_8843316986321445925[100] = 0.0;
   out_8843316986321445925[101] = 0.0;
   out_8843316986321445925[102] = 0.0;
   out_8843316986321445925[103] = 0.0;
   out_8843316986321445925[104] = 0.0;
   out_8843316986321445925[105] = 0.0;
   out_8843316986321445925[106] = 0.0;
   out_8843316986321445925[107] = 0.0;
   out_8843316986321445925[108] = 0.0;
   out_8843316986321445925[109] = 0.0;
   out_8843316986321445925[110] = 0.0;
   out_8843316986321445925[111] = 0.0;
   out_8843316986321445925[112] = 0.0;
   out_8843316986321445925[113] = 0.0;
   out_8843316986321445925[114] = 1.0;
   out_8843316986321445925[115] = 0.0;
   out_8843316986321445925[116] = 0.0;
   out_8843316986321445925[117] = 0.0;
   out_8843316986321445925[118] = 0.0;
   out_8843316986321445925[119] = 0.0;
   out_8843316986321445925[120] = 0.0;
   out_8843316986321445925[121] = 0.0;
   out_8843316986321445925[122] = 0.0;
   out_8843316986321445925[123] = 0.0;
   out_8843316986321445925[124] = 0.0;
   out_8843316986321445925[125] = 0.0;
   out_8843316986321445925[126] = 0.0;
   out_8843316986321445925[127] = 0.0;
   out_8843316986321445925[128] = 0.0;
   out_8843316986321445925[129] = 0.0;
   out_8843316986321445925[130] = 0.0;
   out_8843316986321445925[131] = 0.0;
   out_8843316986321445925[132] = 0.0;
   out_8843316986321445925[133] = 1.0;
   out_8843316986321445925[134] = 0.0;
   out_8843316986321445925[135] = 0.0;
   out_8843316986321445925[136] = 0.0;
   out_8843316986321445925[137] = 0.0;
   out_8843316986321445925[138] = 0.0;
   out_8843316986321445925[139] = 0.0;
   out_8843316986321445925[140] = 0.0;
   out_8843316986321445925[141] = 0.0;
   out_8843316986321445925[142] = 0.0;
   out_8843316986321445925[143] = 0.0;
   out_8843316986321445925[144] = 0.0;
   out_8843316986321445925[145] = 0.0;
   out_8843316986321445925[146] = 0.0;
   out_8843316986321445925[147] = 0.0;
   out_8843316986321445925[148] = 0.0;
   out_8843316986321445925[149] = 0.0;
   out_8843316986321445925[150] = 0.0;
   out_8843316986321445925[151] = 0.0;
   out_8843316986321445925[152] = 1.0;
   out_8843316986321445925[153] = 0.0;
   out_8843316986321445925[154] = 0.0;
   out_8843316986321445925[155] = 0.0;
   out_8843316986321445925[156] = 0.0;
   out_8843316986321445925[157] = 0.0;
   out_8843316986321445925[158] = 0.0;
   out_8843316986321445925[159] = 0.0;
   out_8843316986321445925[160] = 0.0;
   out_8843316986321445925[161] = 0.0;
   out_8843316986321445925[162] = 0.0;
   out_8843316986321445925[163] = 0.0;
   out_8843316986321445925[164] = 0.0;
   out_8843316986321445925[165] = 0.0;
   out_8843316986321445925[166] = 0.0;
   out_8843316986321445925[167] = 0.0;
   out_8843316986321445925[168] = 0.0;
   out_8843316986321445925[169] = 0.0;
   out_8843316986321445925[170] = 0.0;
   out_8843316986321445925[171] = 1.0;
   out_8843316986321445925[172] = 0.0;
   out_8843316986321445925[173] = 0.0;
   out_8843316986321445925[174] = 0.0;
   out_8843316986321445925[175] = 0.0;
   out_8843316986321445925[176] = 0.0;
   out_8843316986321445925[177] = 0.0;
   out_8843316986321445925[178] = 0.0;
   out_8843316986321445925[179] = 0.0;
   out_8843316986321445925[180] = 0.0;
   out_8843316986321445925[181] = 0.0;
   out_8843316986321445925[182] = 0.0;
   out_8843316986321445925[183] = 0.0;
   out_8843316986321445925[184] = 0.0;
   out_8843316986321445925[185] = 0.0;
   out_8843316986321445925[186] = 0.0;
   out_8843316986321445925[187] = 0.0;
   out_8843316986321445925[188] = 0.0;
   out_8843316986321445925[189] = 0.0;
   out_8843316986321445925[190] = 1.0;
   out_8843316986321445925[191] = 0.0;
   out_8843316986321445925[192] = 0.0;
   out_8843316986321445925[193] = 0.0;
   out_8843316986321445925[194] = 0.0;
   out_8843316986321445925[195] = 0.0;
   out_8843316986321445925[196] = 0.0;
   out_8843316986321445925[197] = 0.0;
   out_8843316986321445925[198] = 0.0;
   out_8843316986321445925[199] = 0.0;
   out_8843316986321445925[200] = 0.0;
   out_8843316986321445925[201] = 0.0;
   out_8843316986321445925[202] = 0.0;
   out_8843316986321445925[203] = 0.0;
   out_8843316986321445925[204] = 0.0;
   out_8843316986321445925[205] = 0.0;
   out_8843316986321445925[206] = 0.0;
   out_8843316986321445925[207] = 0.0;
   out_8843316986321445925[208] = 0.0;
   out_8843316986321445925[209] = 1.0;
   out_8843316986321445925[210] = 0.0;
   out_8843316986321445925[211] = 0.0;
   out_8843316986321445925[212] = 0.0;
   out_8843316986321445925[213] = 0.0;
   out_8843316986321445925[214] = 0.0;
   out_8843316986321445925[215] = 0.0;
   out_8843316986321445925[216] = 0.0;
   out_8843316986321445925[217] = 0.0;
   out_8843316986321445925[218] = 0.0;
   out_8843316986321445925[219] = 0.0;
   out_8843316986321445925[220] = 0.0;
   out_8843316986321445925[221] = 0.0;
   out_8843316986321445925[222] = 0.0;
   out_8843316986321445925[223] = 0.0;
   out_8843316986321445925[224] = 0.0;
   out_8843316986321445925[225] = 0.0;
   out_8843316986321445925[226] = 0.0;
   out_8843316986321445925[227] = 0.0;
   out_8843316986321445925[228] = 1.0;
   out_8843316986321445925[229] = 0.0;
   out_8843316986321445925[230] = 0.0;
   out_8843316986321445925[231] = 0.0;
   out_8843316986321445925[232] = 0.0;
   out_8843316986321445925[233] = 0.0;
   out_8843316986321445925[234] = 0.0;
   out_8843316986321445925[235] = 0.0;
   out_8843316986321445925[236] = 0.0;
   out_8843316986321445925[237] = 0.0;
   out_8843316986321445925[238] = 0.0;
   out_8843316986321445925[239] = 0.0;
   out_8843316986321445925[240] = 0.0;
   out_8843316986321445925[241] = 0.0;
   out_8843316986321445925[242] = 0.0;
   out_8843316986321445925[243] = 0.0;
   out_8843316986321445925[244] = 0.0;
   out_8843316986321445925[245] = 0.0;
   out_8843316986321445925[246] = 0.0;
   out_8843316986321445925[247] = 1.0;
   out_8843316986321445925[248] = 0.0;
   out_8843316986321445925[249] = 0.0;
   out_8843316986321445925[250] = 0.0;
   out_8843316986321445925[251] = 0.0;
   out_8843316986321445925[252] = 0.0;
   out_8843316986321445925[253] = 0.0;
   out_8843316986321445925[254] = 0.0;
   out_8843316986321445925[255] = 0.0;
   out_8843316986321445925[256] = 0.0;
   out_8843316986321445925[257] = 0.0;
   out_8843316986321445925[258] = 0.0;
   out_8843316986321445925[259] = 0.0;
   out_8843316986321445925[260] = 0.0;
   out_8843316986321445925[261] = 0.0;
   out_8843316986321445925[262] = 0.0;
   out_8843316986321445925[263] = 0.0;
   out_8843316986321445925[264] = 0.0;
   out_8843316986321445925[265] = 0.0;
   out_8843316986321445925[266] = 1.0;
   out_8843316986321445925[267] = 0.0;
   out_8843316986321445925[268] = 0.0;
   out_8843316986321445925[269] = 0.0;
   out_8843316986321445925[270] = 0.0;
   out_8843316986321445925[271] = 0.0;
   out_8843316986321445925[272] = 0.0;
   out_8843316986321445925[273] = 0.0;
   out_8843316986321445925[274] = 0.0;
   out_8843316986321445925[275] = 0.0;
   out_8843316986321445925[276] = 0.0;
   out_8843316986321445925[277] = 0.0;
   out_8843316986321445925[278] = 0.0;
   out_8843316986321445925[279] = 0.0;
   out_8843316986321445925[280] = 0.0;
   out_8843316986321445925[281] = 0.0;
   out_8843316986321445925[282] = 0.0;
   out_8843316986321445925[283] = 0.0;
   out_8843316986321445925[284] = 0.0;
   out_8843316986321445925[285] = 1.0;
   out_8843316986321445925[286] = 0.0;
   out_8843316986321445925[287] = 0.0;
   out_8843316986321445925[288] = 0.0;
   out_8843316986321445925[289] = 0.0;
   out_8843316986321445925[290] = 0.0;
   out_8843316986321445925[291] = 0.0;
   out_8843316986321445925[292] = 0.0;
   out_8843316986321445925[293] = 0.0;
   out_8843316986321445925[294] = 0.0;
   out_8843316986321445925[295] = 0.0;
   out_8843316986321445925[296] = 0.0;
   out_8843316986321445925[297] = 0.0;
   out_8843316986321445925[298] = 0.0;
   out_8843316986321445925[299] = 0.0;
   out_8843316986321445925[300] = 0.0;
   out_8843316986321445925[301] = 0.0;
   out_8843316986321445925[302] = 0.0;
   out_8843316986321445925[303] = 0.0;
   out_8843316986321445925[304] = 1.0;
   out_8843316986321445925[305] = 0.0;
   out_8843316986321445925[306] = 0.0;
   out_8843316986321445925[307] = 0.0;
   out_8843316986321445925[308] = 0.0;
   out_8843316986321445925[309] = 0.0;
   out_8843316986321445925[310] = 0.0;
   out_8843316986321445925[311] = 0.0;
   out_8843316986321445925[312] = 0.0;
   out_8843316986321445925[313] = 0.0;
   out_8843316986321445925[314] = 0.0;
   out_8843316986321445925[315] = 0.0;
   out_8843316986321445925[316] = 0.0;
   out_8843316986321445925[317] = 0.0;
   out_8843316986321445925[318] = 0.0;
   out_8843316986321445925[319] = 0.0;
   out_8843316986321445925[320] = 0.0;
   out_8843316986321445925[321] = 0.0;
   out_8843316986321445925[322] = 0.0;
   out_8843316986321445925[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_1388079773596962594) {
   out_1388079773596962594[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_1388079773596962594[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_1388079773596962594[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_1388079773596962594[3] = dt*state[12] + state[3];
   out_1388079773596962594[4] = dt*state[13] + state[4];
   out_1388079773596962594[5] = dt*state[14] + state[5];
   out_1388079773596962594[6] = state[6];
   out_1388079773596962594[7] = state[7];
   out_1388079773596962594[8] = state[8];
   out_1388079773596962594[9] = state[9];
   out_1388079773596962594[10] = state[10];
   out_1388079773596962594[11] = state[11];
   out_1388079773596962594[12] = state[12];
   out_1388079773596962594[13] = state[13];
   out_1388079773596962594[14] = state[14];
   out_1388079773596962594[15] = state[15];
   out_1388079773596962594[16] = state[16];
   out_1388079773596962594[17] = state[17];
}
void F_fun(double *state, double dt, double *out_1987774110883025382) {
   out_1987774110883025382[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1987774110883025382[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1987774110883025382[2] = 0;
   out_1987774110883025382[3] = 0;
   out_1987774110883025382[4] = 0;
   out_1987774110883025382[5] = 0;
   out_1987774110883025382[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1987774110883025382[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1987774110883025382[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_1987774110883025382[9] = 0;
   out_1987774110883025382[10] = 0;
   out_1987774110883025382[11] = 0;
   out_1987774110883025382[12] = 0;
   out_1987774110883025382[13] = 0;
   out_1987774110883025382[14] = 0;
   out_1987774110883025382[15] = 0;
   out_1987774110883025382[16] = 0;
   out_1987774110883025382[17] = 0;
   out_1987774110883025382[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_1987774110883025382[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_1987774110883025382[20] = 0;
   out_1987774110883025382[21] = 0;
   out_1987774110883025382[22] = 0;
   out_1987774110883025382[23] = 0;
   out_1987774110883025382[24] = 0;
   out_1987774110883025382[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_1987774110883025382[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_1987774110883025382[27] = 0;
   out_1987774110883025382[28] = 0;
   out_1987774110883025382[29] = 0;
   out_1987774110883025382[30] = 0;
   out_1987774110883025382[31] = 0;
   out_1987774110883025382[32] = 0;
   out_1987774110883025382[33] = 0;
   out_1987774110883025382[34] = 0;
   out_1987774110883025382[35] = 0;
   out_1987774110883025382[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1987774110883025382[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1987774110883025382[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1987774110883025382[39] = 0;
   out_1987774110883025382[40] = 0;
   out_1987774110883025382[41] = 0;
   out_1987774110883025382[42] = 0;
   out_1987774110883025382[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1987774110883025382[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_1987774110883025382[45] = 0;
   out_1987774110883025382[46] = 0;
   out_1987774110883025382[47] = 0;
   out_1987774110883025382[48] = 0;
   out_1987774110883025382[49] = 0;
   out_1987774110883025382[50] = 0;
   out_1987774110883025382[51] = 0;
   out_1987774110883025382[52] = 0;
   out_1987774110883025382[53] = 0;
   out_1987774110883025382[54] = 0;
   out_1987774110883025382[55] = 0;
   out_1987774110883025382[56] = 0;
   out_1987774110883025382[57] = 1;
   out_1987774110883025382[58] = 0;
   out_1987774110883025382[59] = 0;
   out_1987774110883025382[60] = 0;
   out_1987774110883025382[61] = 0;
   out_1987774110883025382[62] = 0;
   out_1987774110883025382[63] = 0;
   out_1987774110883025382[64] = 0;
   out_1987774110883025382[65] = 0;
   out_1987774110883025382[66] = dt;
   out_1987774110883025382[67] = 0;
   out_1987774110883025382[68] = 0;
   out_1987774110883025382[69] = 0;
   out_1987774110883025382[70] = 0;
   out_1987774110883025382[71] = 0;
   out_1987774110883025382[72] = 0;
   out_1987774110883025382[73] = 0;
   out_1987774110883025382[74] = 0;
   out_1987774110883025382[75] = 0;
   out_1987774110883025382[76] = 1;
   out_1987774110883025382[77] = 0;
   out_1987774110883025382[78] = 0;
   out_1987774110883025382[79] = 0;
   out_1987774110883025382[80] = 0;
   out_1987774110883025382[81] = 0;
   out_1987774110883025382[82] = 0;
   out_1987774110883025382[83] = 0;
   out_1987774110883025382[84] = 0;
   out_1987774110883025382[85] = dt;
   out_1987774110883025382[86] = 0;
   out_1987774110883025382[87] = 0;
   out_1987774110883025382[88] = 0;
   out_1987774110883025382[89] = 0;
   out_1987774110883025382[90] = 0;
   out_1987774110883025382[91] = 0;
   out_1987774110883025382[92] = 0;
   out_1987774110883025382[93] = 0;
   out_1987774110883025382[94] = 0;
   out_1987774110883025382[95] = 1;
   out_1987774110883025382[96] = 0;
   out_1987774110883025382[97] = 0;
   out_1987774110883025382[98] = 0;
   out_1987774110883025382[99] = 0;
   out_1987774110883025382[100] = 0;
   out_1987774110883025382[101] = 0;
   out_1987774110883025382[102] = 0;
   out_1987774110883025382[103] = 0;
   out_1987774110883025382[104] = dt;
   out_1987774110883025382[105] = 0;
   out_1987774110883025382[106] = 0;
   out_1987774110883025382[107] = 0;
   out_1987774110883025382[108] = 0;
   out_1987774110883025382[109] = 0;
   out_1987774110883025382[110] = 0;
   out_1987774110883025382[111] = 0;
   out_1987774110883025382[112] = 0;
   out_1987774110883025382[113] = 0;
   out_1987774110883025382[114] = 1;
   out_1987774110883025382[115] = 0;
   out_1987774110883025382[116] = 0;
   out_1987774110883025382[117] = 0;
   out_1987774110883025382[118] = 0;
   out_1987774110883025382[119] = 0;
   out_1987774110883025382[120] = 0;
   out_1987774110883025382[121] = 0;
   out_1987774110883025382[122] = 0;
   out_1987774110883025382[123] = 0;
   out_1987774110883025382[124] = 0;
   out_1987774110883025382[125] = 0;
   out_1987774110883025382[126] = 0;
   out_1987774110883025382[127] = 0;
   out_1987774110883025382[128] = 0;
   out_1987774110883025382[129] = 0;
   out_1987774110883025382[130] = 0;
   out_1987774110883025382[131] = 0;
   out_1987774110883025382[132] = 0;
   out_1987774110883025382[133] = 1;
   out_1987774110883025382[134] = 0;
   out_1987774110883025382[135] = 0;
   out_1987774110883025382[136] = 0;
   out_1987774110883025382[137] = 0;
   out_1987774110883025382[138] = 0;
   out_1987774110883025382[139] = 0;
   out_1987774110883025382[140] = 0;
   out_1987774110883025382[141] = 0;
   out_1987774110883025382[142] = 0;
   out_1987774110883025382[143] = 0;
   out_1987774110883025382[144] = 0;
   out_1987774110883025382[145] = 0;
   out_1987774110883025382[146] = 0;
   out_1987774110883025382[147] = 0;
   out_1987774110883025382[148] = 0;
   out_1987774110883025382[149] = 0;
   out_1987774110883025382[150] = 0;
   out_1987774110883025382[151] = 0;
   out_1987774110883025382[152] = 1;
   out_1987774110883025382[153] = 0;
   out_1987774110883025382[154] = 0;
   out_1987774110883025382[155] = 0;
   out_1987774110883025382[156] = 0;
   out_1987774110883025382[157] = 0;
   out_1987774110883025382[158] = 0;
   out_1987774110883025382[159] = 0;
   out_1987774110883025382[160] = 0;
   out_1987774110883025382[161] = 0;
   out_1987774110883025382[162] = 0;
   out_1987774110883025382[163] = 0;
   out_1987774110883025382[164] = 0;
   out_1987774110883025382[165] = 0;
   out_1987774110883025382[166] = 0;
   out_1987774110883025382[167] = 0;
   out_1987774110883025382[168] = 0;
   out_1987774110883025382[169] = 0;
   out_1987774110883025382[170] = 0;
   out_1987774110883025382[171] = 1;
   out_1987774110883025382[172] = 0;
   out_1987774110883025382[173] = 0;
   out_1987774110883025382[174] = 0;
   out_1987774110883025382[175] = 0;
   out_1987774110883025382[176] = 0;
   out_1987774110883025382[177] = 0;
   out_1987774110883025382[178] = 0;
   out_1987774110883025382[179] = 0;
   out_1987774110883025382[180] = 0;
   out_1987774110883025382[181] = 0;
   out_1987774110883025382[182] = 0;
   out_1987774110883025382[183] = 0;
   out_1987774110883025382[184] = 0;
   out_1987774110883025382[185] = 0;
   out_1987774110883025382[186] = 0;
   out_1987774110883025382[187] = 0;
   out_1987774110883025382[188] = 0;
   out_1987774110883025382[189] = 0;
   out_1987774110883025382[190] = 1;
   out_1987774110883025382[191] = 0;
   out_1987774110883025382[192] = 0;
   out_1987774110883025382[193] = 0;
   out_1987774110883025382[194] = 0;
   out_1987774110883025382[195] = 0;
   out_1987774110883025382[196] = 0;
   out_1987774110883025382[197] = 0;
   out_1987774110883025382[198] = 0;
   out_1987774110883025382[199] = 0;
   out_1987774110883025382[200] = 0;
   out_1987774110883025382[201] = 0;
   out_1987774110883025382[202] = 0;
   out_1987774110883025382[203] = 0;
   out_1987774110883025382[204] = 0;
   out_1987774110883025382[205] = 0;
   out_1987774110883025382[206] = 0;
   out_1987774110883025382[207] = 0;
   out_1987774110883025382[208] = 0;
   out_1987774110883025382[209] = 1;
   out_1987774110883025382[210] = 0;
   out_1987774110883025382[211] = 0;
   out_1987774110883025382[212] = 0;
   out_1987774110883025382[213] = 0;
   out_1987774110883025382[214] = 0;
   out_1987774110883025382[215] = 0;
   out_1987774110883025382[216] = 0;
   out_1987774110883025382[217] = 0;
   out_1987774110883025382[218] = 0;
   out_1987774110883025382[219] = 0;
   out_1987774110883025382[220] = 0;
   out_1987774110883025382[221] = 0;
   out_1987774110883025382[222] = 0;
   out_1987774110883025382[223] = 0;
   out_1987774110883025382[224] = 0;
   out_1987774110883025382[225] = 0;
   out_1987774110883025382[226] = 0;
   out_1987774110883025382[227] = 0;
   out_1987774110883025382[228] = 1;
   out_1987774110883025382[229] = 0;
   out_1987774110883025382[230] = 0;
   out_1987774110883025382[231] = 0;
   out_1987774110883025382[232] = 0;
   out_1987774110883025382[233] = 0;
   out_1987774110883025382[234] = 0;
   out_1987774110883025382[235] = 0;
   out_1987774110883025382[236] = 0;
   out_1987774110883025382[237] = 0;
   out_1987774110883025382[238] = 0;
   out_1987774110883025382[239] = 0;
   out_1987774110883025382[240] = 0;
   out_1987774110883025382[241] = 0;
   out_1987774110883025382[242] = 0;
   out_1987774110883025382[243] = 0;
   out_1987774110883025382[244] = 0;
   out_1987774110883025382[245] = 0;
   out_1987774110883025382[246] = 0;
   out_1987774110883025382[247] = 1;
   out_1987774110883025382[248] = 0;
   out_1987774110883025382[249] = 0;
   out_1987774110883025382[250] = 0;
   out_1987774110883025382[251] = 0;
   out_1987774110883025382[252] = 0;
   out_1987774110883025382[253] = 0;
   out_1987774110883025382[254] = 0;
   out_1987774110883025382[255] = 0;
   out_1987774110883025382[256] = 0;
   out_1987774110883025382[257] = 0;
   out_1987774110883025382[258] = 0;
   out_1987774110883025382[259] = 0;
   out_1987774110883025382[260] = 0;
   out_1987774110883025382[261] = 0;
   out_1987774110883025382[262] = 0;
   out_1987774110883025382[263] = 0;
   out_1987774110883025382[264] = 0;
   out_1987774110883025382[265] = 0;
   out_1987774110883025382[266] = 1;
   out_1987774110883025382[267] = 0;
   out_1987774110883025382[268] = 0;
   out_1987774110883025382[269] = 0;
   out_1987774110883025382[270] = 0;
   out_1987774110883025382[271] = 0;
   out_1987774110883025382[272] = 0;
   out_1987774110883025382[273] = 0;
   out_1987774110883025382[274] = 0;
   out_1987774110883025382[275] = 0;
   out_1987774110883025382[276] = 0;
   out_1987774110883025382[277] = 0;
   out_1987774110883025382[278] = 0;
   out_1987774110883025382[279] = 0;
   out_1987774110883025382[280] = 0;
   out_1987774110883025382[281] = 0;
   out_1987774110883025382[282] = 0;
   out_1987774110883025382[283] = 0;
   out_1987774110883025382[284] = 0;
   out_1987774110883025382[285] = 1;
   out_1987774110883025382[286] = 0;
   out_1987774110883025382[287] = 0;
   out_1987774110883025382[288] = 0;
   out_1987774110883025382[289] = 0;
   out_1987774110883025382[290] = 0;
   out_1987774110883025382[291] = 0;
   out_1987774110883025382[292] = 0;
   out_1987774110883025382[293] = 0;
   out_1987774110883025382[294] = 0;
   out_1987774110883025382[295] = 0;
   out_1987774110883025382[296] = 0;
   out_1987774110883025382[297] = 0;
   out_1987774110883025382[298] = 0;
   out_1987774110883025382[299] = 0;
   out_1987774110883025382[300] = 0;
   out_1987774110883025382[301] = 0;
   out_1987774110883025382[302] = 0;
   out_1987774110883025382[303] = 0;
   out_1987774110883025382[304] = 1;
   out_1987774110883025382[305] = 0;
   out_1987774110883025382[306] = 0;
   out_1987774110883025382[307] = 0;
   out_1987774110883025382[308] = 0;
   out_1987774110883025382[309] = 0;
   out_1987774110883025382[310] = 0;
   out_1987774110883025382[311] = 0;
   out_1987774110883025382[312] = 0;
   out_1987774110883025382[313] = 0;
   out_1987774110883025382[314] = 0;
   out_1987774110883025382[315] = 0;
   out_1987774110883025382[316] = 0;
   out_1987774110883025382[317] = 0;
   out_1987774110883025382[318] = 0;
   out_1987774110883025382[319] = 0;
   out_1987774110883025382[320] = 0;
   out_1987774110883025382[321] = 0;
   out_1987774110883025382[322] = 0;
   out_1987774110883025382[323] = 1;
}
void h_4(double *state, double *unused, double *out_6417975175098302508) {
   out_6417975175098302508[0] = state[6] + state[9];
   out_6417975175098302508[1] = state[7] + state[10];
   out_6417975175098302508[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_3690798601282365190) {
   out_3690798601282365190[0] = 0;
   out_3690798601282365190[1] = 0;
   out_3690798601282365190[2] = 0;
   out_3690798601282365190[3] = 0;
   out_3690798601282365190[4] = 0;
   out_3690798601282365190[5] = 0;
   out_3690798601282365190[6] = 1;
   out_3690798601282365190[7] = 0;
   out_3690798601282365190[8] = 0;
   out_3690798601282365190[9] = 1;
   out_3690798601282365190[10] = 0;
   out_3690798601282365190[11] = 0;
   out_3690798601282365190[12] = 0;
   out_3690798601282365190[13] = 0;
   out_3690798601282365190[14] = 0;
   out_3690798601282365190[15] = 0;
   out_3690798601282365190[16] = 0;
   out_3690798601282365190[17] = 0;
   out_3690798601282365190[18] = 0;
   out_3690798601282365190[19] = 0;
   out_3690798601282365190[20] = 0;
   out_3690798601282365190[21] = 0;
   out_3690798601282365190[22] = 0;
   out_3690798601282365190[23] = 0;
   out_3690798601282365190[24] = 0;
   out_3690798601282365190[25] = 1;
   out_3690798601282365190[26] = 0;
   out_3690798601282365190[27] = 0;
   out_3690798601282365190[28] = 1;
   out_3690798601282365190[29] = 0;
   out_3690798601282365190[30] = 0;
   out_3690798601282365190[31] = 0;
   out_3690798601282365190[32] = 0;
   out_3690798601282365190[33] = 0;
   out_3690798601282365190[34] = 0;
   out_3690798601282365190[35] = 0;
   out_3690798601282365190[36] = 0;
   out_3690798601282365190[37] = 0;
   out_3690798601282365190[38] = 0;
   out_3690798601282365190[39] = 0;
   out_3690798601282365190[40] = 0;
   out_3690798601282365190[41] = 0;
   out_3690798601282365190[42] = 0;
   out_3690798601282365190[43] = 0;
   out_3690798601282365190[44] = 1;
   out_3690798601282365190[45] = 0;
   out_3690798601282365190[46] = 0;
   out_3690798601282365190[47] = 1;
   out_3690798601282365190[48] = 0;
   out_3690798601282365190[49] = 0;
   out_3690798601282365190[50] = 0;
   out_3690798601282365190[51] = 0;
   out_3690798601282365190[52] = 0;
   out_3690798601282365190[53] = 0;
}
void h_10(double *state, double *unused, double *out_933754340625772488) {
   out_933754340625772488[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_933754340625772488[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_933754340625772488[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_2496885064021374014) {
   out_2496885064021374014[0] = 0;
   out_2496885064021374014[1] = 9.8100000000000005*cos(state[1]);
   out_2496885064021374014[2] = 0;
   out_2496885064021374014[3] = 0;
   out_2496885064021374014[4] = -state[8];
   out_2496885064021374014[5] = state[7];
   out_2496885064021374014[6] = 0;
   out_2496885064021374014[7] = state[5];
   out_2496885064021374014[8] = -state[4];
   out_2496885064021374014[9] = 0;
   out_2496885064021374014[10] = 0;
   out_2496885064021374014[11] = 0;
   out_2496885064021374014[12] = 1;
   out_2496885064021374014[13] = 0;
   out_2496885064021374014[14] = 0;
   out_2496885064021374014[15] = 1;
   out_2496885064021374014[16] = 0;
   out_2496885064021374014[17] = 0;
   out_2496885064021374014[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_2496885064021374014[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_2496885064021374014[20] = 0;
   out_2496885064021374014[21] = state[8];
   out_2496885064021374014[22] = 0;
   out_2496885064021374014[23] = -state[6];
   out_2496885064021374014[24] = -state[5];
   out_2496885064021374014[25] = 0;
   out_2496885064021374014[26] = state[3];
   out_2496885064021374014[27] = 0;
   out_2496885064021374014[28] = 0;
   out_2496885064021374014[29] = 0;
   out_2496885064021374014[30] = 0;
   out_2496885064021374014[31] = 1;
   out_2496885064021374014[32] = 0;
   out_2496885064021374014[33] = 0;
   out_2496885064021374014[34] = 1;
   out_2496885064021374014[35] = 0;
   out_2496885064021374014[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_2496885064021374014[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_2496885064021374014[38] = 0;
   out_2496885064021374014[39] = -state[7];
   out_2496885064021374014[40] = state[6];
   out_2496885064021374014[41] = 0;
   out_2496885064021374014[42] = state[4];
   out_2496885064021374014[43] = -state[3];
   out_2496885064021374014[44] = 0;
   out_2496885064021374014[45] = 0;
   out_2496885064021374014[46] = 0;
   out_2496885064021374014[47] = 0;
   out_2496885064021374014[48] = 0;
   out_2496885064021374014[49] = 0;
   out_2496885064021374014[50] = 1;
   out_2496885064021374014[51] = 0;
   out_2496885064021374014[52] = 0;
   out_2496885064021374014[53] = 1;
}
void h_13(double *state, double *unused, double *out_7412978871522735544) {
   out_7412978871522735544[0] = state[3];
   out_7412978871522735544[1] = state[4];
   out_7412978871522735544[2] = state[5];
}
void H_13(double *state, double *unused, double *out_478524775950032389) {
   out_478524775950032389[0] = 0;
   out_478524775950032389[1] = 0;
   out_478524775950032389[2] = 0;
   out_478524775950032389[3] = 1;
   out_478524775950032389[4] = 0;
   out_478524775950032389[5] = 0;
   out_478524775950032389[6] = 0;
   out_478524775950032389[7] = 0;
   out_478524775950032389[8] = 0;
   out_478524775950032389[9] = 0;
   out_478524775950032389[10] = 0;
   out_478524775950032389[11] = 0;
   out_478524775950032389[12] = 0;
   out_478524775950032389[13] = 0;
   out_478524775950032389[14] = 0;
   out_478524775950032389[15] = 0;
   out_478524775950032389[16] = 0;
   out_478524775950032389[17] = 0;
   out_478524775950032389[18] = 0;
   out_478524775950032389[19] = 0;
   out_478524775950032389[20] = 0;
   out_478524775950032389[21] = 0;
   out_478524775950032389[22] = 1;
   out_478524775950032389[23] = 0;
   out_478524775950032389[24] = 0;
   out_478524775950032389[25] = 0;
   out_478524775950032389[26] = 0;
   out_478524775950032389[27] = 0;
   out_478524775950032389[28] = 0;
   out_478524775950032389[29] = 0;
   out_478524775950032389[30] = 0;
   out_478524775950032389[31] = 0;
   out_478524775950032389[32] = 0;
   out_478524775950032389[33] = 0;
   out_478524775950032389[34] = 0;
   out_478524775950032389[35] = 0;
   out_478524775950032389[36] = 0;
   out_478524775950032389[37] = 0;
   out_478524775950032389[38] = 0;
   out_478524775950032389[39] = 0;
   out_478524775950032389[40] = 0;
   out_478524775950032389[41] = 1;
   out_478524775950032389[42] = 0;
   out_478524775950032389[43] = 0;
   out_478524775950032389[44] = 0;
   out_478524775950032389[45] = 0;
   out_478524775950032389[46] = 0;
   out_478524775950032389[47] = 0;
   out_478524775950032389[48] = 0;
   out_478524775950032389[49] = 0;
   out_478524775950032389[50] = 0;
   out_478524775950032389[51] = 0;
   out_478524775950032389[52] = 0;
   out_478524775950032389[53] = 0;
}
void h_14(double *state, double *unused, double *out_1424794695951564614) {
   out_1424794695951564614[0] = state[6];
   out_1424794695951564614[1] = state[7];
   out_1424794695951564614[2] = state[8];
}
void H_14(double *state, double *unused, double *out_6773587033577737486) {
   out_6773587033577737486[0] = 0;
   out_6773587033577737486[1] = 0;
   out_6773587033577737486[2] = 0;
   out_6773587033577737486[3] = 0;
   out_6773587033577737486[4] = 0;
   out_6773587033577737486[5] = 0;
   out_6773587033577737486[6] = 1;
   out_6773587033577737486[7] = 0;
   out_6773587033577737486[8] = 0;
   out_6773587033577737486[9] = 0;
   out_6773587033577737486[10] = 0;
   out_6773587033577737486[11] = 0;
   out_6773587033577737486[12] = 0;
   out_6773587033577737486[13] = 0;
   out_6773587033577737486[14] = 0;
   out_6773587033577737486[15] = 0;
   out_6773587033577737486[16] = 0;
   out_6773587033577737486[17] = 0;
   out_6773587033577737486[18] = 0;
   out_6773587033577737486[19] = 0;
   out_6773587033577737486[20] = 0;
   out_6773587033577737486[21] = 0;
   out_6773587033577737486[22] = 0;
   out_6773587033577737486[23] = 0;
   out_6773587033577737486[24] = 0;
   out_6773587033577737486[25] = 1;
   out_6773587033577737486[26] = 0;
   out_6773587033577737486[27] = 0;
   out_6773587033577737486[28] = 0;
   out_6773587033577737486[29] = 0;
   out_6773587033577737486[30] = 0;
   out_6773587033577737486[31] = 0;
   out_6773587033577737486[32] = 0;
   out_6773587033577737486[33] = 0;
   out_6773587033577737486[34] = 0;
   out_6773587033577737486[35] = 0;
   out_6773587033577737486[36] = 0;
   out_6773587033577737486[37] = 0;
   out_6773587033577737486[38] = 0;
   out_6773587033577737486[39] = 0;
   out_6773587033577737486[40] = 0;
   out_6773587033577737486[41] = 0;
   out_6773587033577737486[42] = 0;
   out_6773587033577737486[43] = 0;
   out_6773587033577737486[44] = 1;
   out_6773587033577737486[45] = 0;
   out_6773587033577737486[46] = 0;
   out_6773587033577737486[47] = 0;
   out_6773587033577737486[48] = 0;
   out_6773587033577737486[49] = 0;
   out_6773587033577737486[50] = 0;
   out_6773587033577737486[51] = 0;
   out_6773587033577737486[52] = 0;
   out_6773587033577737486[53] = 0;
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

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_6215768011470258474) {
  err_fun(nom_x, delta_x, out_6215768011470258474);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6516201049276311805) {
  inv_err_fun(nom_x, true_x, out_6516201049276311805);
}
void pose_H_mod_fun(double *state, double *out_8843316986321445925) {
  H_mod_fun(state, out_8843316986321445925);
}
void pose_f_fun(double *state, double dt, double *out_1388079773596962594) {
  f_fun(state,  dt, out_1388079773596962594);
}
void pose_F_fun(double *state, double dt, double *out_1987774110883025382) {
  F_fun(state,  dt, out_1987774110883025382);
}
void pose_h_4(double *state, double *unused, double *out_6417975175098302508) {
  h_4(state, unused, out_6417975175098302508);
}
void pose_H_4(double *state, double *unused, double *out_3690798601282365190) {
  H_4(state, unused, out_3690798601282365190);
}
void pose_h_10(double *state, double *unused, double *out_933754340625772488) {
  h_10(state, unused, out_933754340625772488);
}
void pose_H_10(double *state, double *unused, double *out_2496885064021374014) {
  H_10(state, unused, out_2496885064021374014);
}
void pose_h_13(double *state, double *unused, double *out_7412978871522735544) {
  h_13(state, unused, out_7412978871522735544);
}
void pose_H_13(double *state, double *unused, double *out_478524775950032389) {
  H_13(state, unused, out_478524775950032389);
}
void pose_h_14(double *state, double *unused, double *out_1424794695951564614) {
  h_14(state, unused, out_1424794695951564614);
}
void pose_H_14(double *state, double *unused, double *out_6773587033577737486) {
  H_14(state, unused, out_6773587033577737486);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
