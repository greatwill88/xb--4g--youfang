#ifndef __NWY_PWM_H__
#define __NWY_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define NAME_PWM_1  "PWM1"

typedef struct nwyPwm nwy_pwm_t;

nwy_pwm_t* nwy_pwm_init(char *name,uint32_t freq,uint32_t duty);
bool nwy_pwm_start(nwy_pwm_t *d);
bool nwy_pwm_stop(nwy_pwm_t *d);
int nwy_pwm_deinit(nwy_pwm_t *d);


#ifdef __cplusplus
   }
#endif

#endif // __NWY_PWM_H__

