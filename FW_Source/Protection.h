/*
 * Protection.h
 *
 *  Created on: 2022. 12. 28.
 *      Author: king
 */

#ifndef PROTECTION_H_
#define PROTECTION_H_

void tp_process(void);
void cp_process(void);
void vp_process(void);
void pf_process(void);

void Auto_Release(void);
void Auto_Release_vp(void);
void Auto_Release_cp(void);
void Auto_Release_tp(void);

#endif /* PROTECTION_H_ */
