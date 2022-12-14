/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     pinmux.h
 * @brief    Header file for the pinmux
 * @version  V1.0
 * @date     23. August 2017
 ******************************************************************************/
#ifndef HOBBIT1_2_PINMUX_H
#define HOBBIT1_2_PINMUX_H

#include <stdint.h>
#include "pin_name.h"

void hobbit_ioreuse_initial(void);
int32_t pin_mux(pin_name_t pin, uint16_t function);

/*IOMUX0L function definition */


/* IOMUX0H function definition */

#define PA16_UART1_RX     0x00010000
#define PA17_UART1_TX     0x00020000


#define PA20_UART1_RX       0x00020000
#define PA21_UART1_TX       0x00080000

//use for spi eth
#define PA3_SPI0MOSI        0x00000080
#define PA2_SPI0MISO        0x00000020
#define PA4_SPI0SCK         0x00000200
#define PA5_SPI0SSN         0x00000800

#define PA23_SPI1MOSI     0x00800000
#define PA22_SPI1MISO     0x00400000
#define PA21_SPI1SCK      0x00200000
#define PA18_SPI1SSN0     0x00040000




/* IOMUX1L function definition */

/* flag as identification */
#define GPIO_SET_BIT0  0x00000001
#define GPIO_SET_BIT1  0x00000002
#define GPIO_SET_BIT2  0x00000004
#define GPIO_SET_BIT3  0x00000008
#define GPIO_SET_BIT4  0x00000010
#define GPIO_SET_BIT5  0x00000020
#define GPIO_SET_BIT6  0x00000040
#define GPIO_SET_BIT7  0x00000080
#define GPIO_SET_BIT8  0x00000100
#define GPIO_SET_BIT9  0x00000200
#define GPIO_SET_BIT10 0x00000400
#define GPIO_SET_BIT11 0x00000800
#define GPIO_SET_BIT12 0x00001000
#define GPIO_SET_BIT13 0x00002000
#define GPIO_SET_BIT14 0x00004000
#define GPIO_SET_BIT15 0x00008000
#define GPIO_SET_BIT16 0x00010000
#define GPIO_SET_BIT17 0x00020000
#define GPIO_SET_BIT18 0x00040000
#define GPIO_SET_BIT19 0x00080000
#define GPIO_SET_BIT20 0x00100000
#define GPIO_SET_BIT21 0x00200000
#define GPIO_SET_BIT22 0x00400000
#define GPIO_SET_BIT23 0x00800000
#define GPIO_SET_BIT24 0x01000000
#define GPIO_SET_BIT25 0x02000000
#define GPIO_SET_BIT26 0x04000000
#define GPIO_SET_BIT27 0x08000000
#define GPIO_SET_BIT28 0x10000000
#define GPIO_SET_BIT29 0x20000000
#define GPIO_SET_BIT30 0x40000000
#define GPIO_SET_BIT31 0x80000000




/******************************************************************************
 * hobbit1_2 gpio control and gpio reuse function
 * selecting regester adddress
 ******************************************************************************/

#define HOBBIT1_2_GIPO0_PORTCTL_REG 0x50006008
#define HOBBIT1_2_GIPO1_PORTCTL_REG 0x50009008
#define HOBBIT1_2_IOMUX0L_REG       0x50006100
#define HOBBIT1_2_IOMUX0H_REG       0x50006104
#define HOBBIT1_2_IOMUX1L_REG       0x50006108


/*************basic gpio reuse v1.0********************************************
 * UART1(PA16,PA17) for bootrom
 * UART1(PA20,PA21) for console
 ******************************************************************************/
#define GPIO0_REUSE_EN                (0x00000000)
#define GPIO0_REUSE_DIS               (GPIO_SET_BIT16 | GPIO_SET_BIT17)

#define GPIO1_REUSE_EN                (0x00000000)
#define IOMUX0L_FUNCTION_SEL          (0x00000000)
#define IOMUX0H_FUNCTION_SEL          (0x00000000)
#define IOMUX1L_FUNCTION_SEL          (0x00000000)

#endif /* HOBBIT_PINMUX_H */

