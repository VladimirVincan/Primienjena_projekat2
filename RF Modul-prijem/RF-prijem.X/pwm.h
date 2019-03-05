/* 
 * File:   pwm.h
 * Author: Z1
 *
 * Created on March 3, 2019, 10:18 PM
 */

#ifndef PWM_H
#define	PWM_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MOT_EN1_TRIS TRISDbits.TRISD0 
#define MOT_EN2_TRIS TRISDbits.TRISD1
#define MOT_IN1_TRIS TRISDbits.TRISD2
#define MOT_IN2_TRIS TRISDbits.TRISD3
#define MOT_IN3_TRIS TRISDbits.TRISD8
#define MOT_IN4_TRIS TRISDbits.TRISD9
    
#define MOT_IN1_LAT LATDbits.LATD2
#define MOT_IN2_LAT LATDbits.LATD3
#define MOT_IN3_LAT LATDbits.LATD8
#define MOT_IN4_LAT LATDbits.LATD9

#define OCxCON_OCSIDL_HALT 0x2000
#define OCxCON_OCSIDL_CONTINUE 0x0000 
#define OCxCON_OCTSEL_TIMER3 0x0008
#define OCxCON_OCTSEL_TIMER2 0x0000
#define OCxCON_OCM_PWM_ON_FAULT_ENABLED 0x0007 
#define OCxCON_OCM_PWM_ON_FAULT_DISABLED 0x0006
    
#define PWM_MIN 0
#define PWM_MID 1249    
#define PWM_MAX 2499

#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */

