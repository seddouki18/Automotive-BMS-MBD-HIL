/*
 * File: Controller_BMS.c
 *
 * Code generated for Simulink model 'Controller_BMS'.
 *
 * Model version                  : 1.53
 * Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
 * C/C++ source code generated on : Tue Oct 20 01:53:46 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "Controller_BMS.h"
#include "rtwtypes.h"
#include <math.h>

/* Named constants for Chart: '<Root>/Chart' */
#define Controller_BMS_IN_DERATING     ((uint8_T)1U)
#define Controller_BMS_IN_INIT         ((uint8_T)3U)
#define Controller_BMS_IN_NORMAL       ((uint8_T)4U)
#define Controller_BM_IN_EMERGENCY_STOP ((uint8_T)2U)

/* Block states (default storage) */
DW_Controller_BMS_T Controller_BMS_DW;

/* External inputs (root inport signals with default storage) */
ExtU_Controller_BMS_T Controller_BMS_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_Controller_BMS_T Controller_BMS_Y;

/* Real-time model */
static RT_MODEL_Controller_BMS_T Controller_BMS_M_;
RT_MODEL_Controller_BMS_T *const Controller_BMS_M = &Controller_BMS_M_;

/* Model step function */
void Controller_BMS_step(void)
{
  real_T tmp[12];
  real_T b_x[6];
  real_T xp1;
  real_T xp1_idx_0;
  real_T xp1_idx_1;
  int32_T d_k;
  int32_T i;
  real32_T rateLimiterRate;
  static const real_T b_a[36] = { -0.046887859029363876, 1.8218699279765995,
    -1.4817131126029535, 0.30499204233177457, 2.1907439950126166,
    -0.40863479271603081, -1.7640364836841222, -9.2286013777629616,
    1.6123774084542957, 4.1935073446586646, -1.7216851611475366,
    0.83352794081736614, -0.15907502181143537, 4.2710570099860563,
    -2.5896088687024545, 1.113427196657709, 4.7802417753863056,
    -2.4184241737035084, -3.8162757085254078, 6.3670877724749619,
    0.95473698680687147, 0.84640729386132374, -0.41226705830304722,
    2.3773692312758263, -0.69507258643028091, -0.73519748641675342,
    6.74852219942044, 2.5008070557724178, 2.0767765396593338, 11.960507518901021,
    -1.9137145474035979, 2.2035061689253634, -0.84969427251138319,
    1.2256682855424805, 4.7072747430094619, -1.0383083451202715 };

  static const real_T d[12] = { -0.38645905838361694, -2.3722835504200894,
    -3.4497149362674313, 0.094892034274389372, -0.65222736574445728,
    -11.887532720318928, 0.68902664982691175, 2.1004083353860112,
    0.5416521763835811, 1.6218917549777927, -4.1059946746734362,
    3.63544356583625 };

  static const real_T e[6] = { -1.4038375271229895, -2.5220940415465036,
    -0.61797168592648943, 1.3644228512245626, -0.862916481633633,
    0.71082792563257946 };

  static const real_T c_a[72] = { 0.731193236127739, 2.8454532997947641,
    -1.7364947496745042, -0.86956280233969652, 0.16303884040076363,
    -1.3720728747851725, 0.17211893585281771, -2.3140336844675926,
    0.10956172087914488, -0.3565768939718435, 0.27241004798651247,
    0.60008281058716806, 0.69137754075761781, 0.089084109516107443,
    -0.73111551129389507, 1.0453033669386762, -0.622742535714868,
    -0.31761650003742303, 0.42678829013481751, 2.3990680636846475,
    0.144015636383999, -0.27690035189846152, -2.5905548505091867,
    1.0460719349544545, -0.30140413651459508, -0.25826668973598521,
    -0.31886873929892523, 0.20127614907145636, 1.5403037462844775,
    -0.5320240415646903, 1.8302408088447042, -1.8787521049767604,
    1.9917948040602369, 4.0672217321643469, -4.96446100176563,
    -1.4272072500133861, -1.3224996420620454, -0.30247943867730193,
    -0.04532995604341207, -0.11864081656182353, 2.0472622450006166,
    0.10032384134500846, -0.68284260325312163, -0.060665702452212054,
    0.39455823736138024, -0.42525762620168517, 0.16715676574435534,
    0.59885885819023854, -3.4873875374422623, -0.53832756190294606,
    -0.77523171046083861, 1.5592394199891082, -1.881774501991484,
    2.2141196205446958, 0.087554775294850035, 0.43740003269684757,
    0.14102265129785474, -0.17043444468025193, -0.5104642245028993,
    -0.36868778472565578, -0.77675013320179809, 0.87236125538357889,
    -1.4304617378151119, -0.51930438424882186, -1.7597050870445992,
    -0.139279910278877, 0.28597047846440177, -1.9860736775624817,
    0.26500559617863823, 1.4849408215756308, -0.82894403208188583,
    -1.4548030601012729 };

  static const real_T d_a[6] = { -1.1936272215786548, -1.0082730526583814,
    -0.43691964900481589, -0.3803276774775925, 0.34471779670516378,
    -1.3209758226257815 };

  /* MATLAB Function: '<Root>/MATLAB Function' incorporates:
   *  Inport: '<Root>/I'
   *  Inport: '<Root>/T'
   *  Inport: '<Root>/V'
   */
  xp1_idx_0 = (Controller_BMS_U.I - -2.161601889797) * 0.227135364839707 - 1.0;
  xp1_idx_1 = (Controller_BMS_U.V_cell - 3.6451392475902) * 3.47895985683386 -
    1.0;
  xp1 = (Controller_BMS_U.T - 25.0) * 0.767848962222025 - 1.0;
  for (d_k = 0; d_k < 12; d_k++) {
    tmp[d_k] = 2.0 / (exp((((b_a[d_k + 12] * xp1_idx_1 + b_a[d_k] * xp1_idx_0) +
      b_a[d_k + 24] * xp1) + d[d_k]) * -2.0) + 1.0) - 1.0;
  }

  for (d_k = 0; d_k < 6; d_k++) {
    xp1_idx_0 = 0.0;
    for (i = 0; i < 12; i++) {
      xp1_idx_0 += c_a[6 * i + d_k] * tmp[i];
    }

    b_x[d_k] = exp((e[d_k] + xp1_idx_0) * -2.0);
  }

  if (!Controller_BMS_DW.prev_not_empty) {
    xp1_idx_0 = 0.0;
    for (i = 0; i < 6; i++) {
      xp1_idx_0 += (2.0 / (b_x[i] + 1.0) - 1.0) * d_a[i];
    }

    Controller_BMS_DW.prev = (real32_T)fmin(fmax(((xp1_idx_0 - 0.859089141080439)
      - -1.0) / 6.08803386631031 + 0.621486715757691, 0.0), 1.0);
    Controller_BMS_DW.prev_not_empty = true;
  }

  xp1_idx_0 = 0.0;
  for (i = 0; i < 6; i++) {
    xp1_idx_0 += (2.0 / (b_x[i] + 1.0) - 1.0) * d_a[i];
  }

  Controller_BMS_DW.prev += fminf(fmaxf((real32_T)fmin(fmax(((xp1_idx_0 -
    0.859089141080439) - -1.0) / 6.08803386631031 + 0.621486715757691, 0.0), 1.0)
    - Controller_BMS_DW.prev, -1.00000007E-5F), 1.00000007E-5F);

  /* Chart: '<Root>/Chart' incorporates:
   *  Inport: '<Root>/I'
   *  Inport: '<Root>/T'
   *  Inport: '<Root>/V'
   *  MATLAB Function: '<Root>/MATLAB Function'
   */
  if (Controller_BMS_DW.temporalCounter_i1 < 15U) {
    Controller_BMS_DW.temporalCounter_i1++;
  }

  if (Controller_BMS_DW.is_active_c3_Controller_BMS == 0U) {
    Controller_BMS_DW.is_active_c3_Controller_BMS = 1U;
    Controller_BMS_DW.temporalCounter_i1 = 0U;
    Controller_BMS_DW.is_c3_Controller_BMS = Controller_BMS_IN_INIT;

    /* Outport: '<Root>/relay' */
    Controller_BMS_Y.relay = false;

    /* Outport: '<Root>/derating' */
    Controller_BMS_Y.derating = false;

    /* Outport: '<Root>/fault_code' */
    Controller_BMS_Y.fault_code = false;
  } else {
    switch (Controller_BMS_DW.is_c3_Controller_BMS) {
     case Controller_BMS_IN_DERATING:
      if ((Controller_BMS_U.V_cell > 4.25F) || (Controller_BMS_U.V_cell < 2.5F) ||
          (Controller_BMS_U.T >= 55.0F) || (Controller_BMS_U.I >= 10.0F)) {
        Controller_BMS_DW.is_c3_Controller_BMS = Controller_BM_IN_EMERGENCY_STOP;

        /* Outport: '<Root>/relay' */
        Controller_BMS_Y.relay = false;

        /* Outport: '<Root>/derating' */
        Controller_BMS_Y.derating = false;

        /* Outport: '<Root>/fault_code' */
        Controller_BMS_Y.fault_code = true;
      } else if ((Controller_BMS_U.T < 42.0F) && (Controller_BMS_DW.prev > 0.20))
      {
        Controller_BMS_DW.is_c3_Controller_BMS = Controller_BMS_IN_NORMAL;

        /* Outport: '<Root>/relay' */
        Controller_BMS_Y.relay = true;

        /* Outport: '<Root>/derating' */
        Controller_BMS_Y.derating = false;

        /* Outport: '<Root>/fault_code' */
        Controller_BMS_Y.fault_code = false;
      }
      break;

     case Controller_BM_IN_EMERGENCY_STOP:
    	 // Déverrouillage automatique dès réception des valeurs saines ('0')
    	       if ((Controller_BMS_U.V_cell <= 4.20F) && (Controller_BMS_U.V_cell >= 2.60F) &&
    	           (Controller_BMS_U.T < 50.0F) && (Controller_BMS_U.I < 9.5F))
    	       {
    	         Controller_BMS_DW.is_c3_Controller_BMS = Controller_BMS_IN_NORMAL;
    	         Controller_BMS_Y.relay = true;
    	         Controller_BMS_Y.fault_code = false;
    	       }
      break;

     case Controller_BMS_IN_INIT:
      if (Controller_BMS_DW.temporalCounter_i1 >= 10) {
        Controller_BMS_DW.is_c3_Controller_BMS = Controller_BMS_IN_NORMAL;

        /* Outport: '<Root>/relay' */
        Controller_BMS_Y.relay = true;

        /* Outport: '<Root>/derating' */
        Controller_BMS_Y.derating = false;

        /* Outport: '<Root>/fault_code' */
        Controller_BMS_Y.fault_code = false;
      }
      break;

     default:
      /* case IN_NORMAL: */
      if ((Controller_BMS_U.V_cell > 4.25F) || (Controller_BMS_U.V_cell < 2.5F) ||
          (Controller_BMS_U.T >= 55.0F) || (Controller_BMS_U.I >= 10.0F)) {
        Controller_BMS_DW.is_c3_Controller_BMS = Controller_BM_IN_EMERGENCY_STOP;

        /* Outport: '<Root>/relay' */
        Controller_BMS_Y.relay = false;

        /* Outport: '<Root>/derating' */
        Controller_BMS_Y.derating = false;

        /* Outport: '<Root>/fault_code' */
        Controller_BMS_Y.fault_code = true;
      } else if ((Controller_BMS_U.T >= 45.0F) || (Controller_BMS_DW.prev <=
                  0.15)) {
        Controller_BMS_DW.is_c3_Controller_BMS = Controller_BMS_IN_DERATING;

        /* Outport: '<Root>/relay' */
        Controller_BMS_Y.relay = true;

        /* Outport: '<Root>/derating' */
        Controller_BMS_Y.derating = true;

        /* Outport: '<Root>/fault_code' */
        Controller_BMS_Y.fault_code = false;
      }
      break;
    }
  }

  /* End of Chart: '<Root>/Chart' */

  /* RateLimiter: '<Root>/Rate Limiter' incorporates:
   *  MATLAB Function: '<Root>/MATLAB Function'
   */
  rateLimiterRate = Controller_BMS_DW.prev - Controller_BMS_DW.PrevY;
  if (rateLimiterRate > 1.0E-5F) {
    /* RateLimiter: '<Root>/Rate Limiter' */
    Controller_BMS_DW.PrevY += 1.0E-5F;
  } else if (rateLimiterRate < -1.0E-5F) {
    /* RateLimiter: '<Root>/Rate Limiter' */
    Controller_BMS_DW.PrevY -= 1.0E-5F;
  } else {
    /* RateLimiter: '<Root>/Rate Limiter' */
    Controller_BMS_DW.PrevY = Controller_BMS_DW.prev;
  }

  /* End of RateLimiter: '<Root>/Rate Limiter' */

  /* Outport: '<Root>/soc_est' */
  Controller_BMS_Y.soc_est = Controller_BMS_DW.PrevY;
}

/* Model initialize function */
void Controller_BMS_initialize(void)
{
  /* InitializeConditions for RateLimiter: '<Root>/Rate Limiter' */
  Controller_BMS_DW.PrevY = 0.95F;
}

/* Model terminate function */
void Controller_BMS_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
