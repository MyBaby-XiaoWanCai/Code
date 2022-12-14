/*
* Copyright (c) 2013-2018 Arm Limited. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the License); you may
* not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an AS IS BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* -----------------------------------------------------------------------
*
* $Date:        25. May 2018
* $Revision:    V6.3
*
* Project:      Ethernet Physical Layer Transceiver (PHY)
*               Definitions for KSZ8081RNA (KSZ8081RNB compatible)
* -------------------------------------------------------------------- */

#ifndef __PHY_KSZ8041NL_H
#define __PHY_KSZ8041NL_H

#include "Driver_ETH.h"
#include "Driver_ETH_PHY.h"

/* Register Map */
#define REG_BMCR              0x00U     /* Basic Control                      */
#define REG_BMSR              0x01U     /* Basic Status                       */
#define REG_PHYIDR1           0x02U     /* PHY Identifier 1                   */
#define REG_PHYIDR2           0x03U     /* PHY Identifier 2                   */
#define REG_ANAR              0x04U     /* Auto-Negotiation Advertisement     */
#define REG_ANLPAR            0x05U     /* Auto-Neg. Link Partner Ability     */
#define REG_ANER              0x06U     /* Auto-Neg. Expansion                */
#define REG_ANNP              0x07U     /* Auto-Neg. Next Page                */
#define REG_LPNP              0x08U     /* Link Partner Next Page Ability     */

#define REG_MII_CTRL          0x14U     /* MII control                        */
#define REG_RXERCNT           0x15U     /* RXER Counter                       */
#define REG_IRQCS             0x1BU     /* Interrupt Control/Status           */
#define REG_PHYCR1            0x1EU     /* PHY Control 1                      */
#define REG_PHYCR2            0x1FU     /* PHY Control 2                      */

/* Basic Control Register Bitmasks */
#define BMCR_RESET            0x8000U   /* Software Reset                     */
#define BMCR_LOOPBACK         0x4000U   /* Loopback mode                      */
#define BMCR_SPEED_SELECT     0x2000U   /* Speed Select (1=100Mb/s)           */
#define BMCR_ANEG_EN          0x1000U   /* Auto Negotiation Enable            */
#define BMCR_POWER_DOWN       0x0800U   /* Power Down                         */
#define BMCR_ISOLATE          0x0400U   /* Isolate Media interface            */
#define BMCR_RESTART_ANEG     0x0200U   /* Restart Auto Negotiation           */
#define BMCR_DUPLEX_MODE      0x0100U   /* Duplex Mode (1=Full duplex)        */
#define BMCR_COLLISION_TEST   0x0080U   /* Collision Test                     */

/* Basic Status Register Bitmasks */
#define BMSR_100B_T4          0x8000U   /* 100BASE-T4 Capable                 */
#define BMSR_100B_TX_FD       0x4000U   /* 100BASE-TX Full Duplex Capable     */
#define BMSR_100B_TX_HD       0x2000U   /* 100BASE-TX Half Duplex Capable     */
#define BMSR_10B_T_FD         0x1000U   /* 10BASE-T Full Duplex Capable       */
#define BMSR_10B_T_HD         0x0800U   /* 10BASE-T Half Duplex Capable       */
#define BMSR_NO_PREAMBLE      0x0040U   /* Preamble suppression               */
#define BMSR_ANEG_COMPLETE    0x0020U   /* Auto Negotiation Complete          */
#define BMSR_REMOTE_FAULT     0x0010U   /* Remote Fault                       */
#define BMSR_ANEG_ABILITY     0x0008U   /* Auto Negotiation Ability           */
#define BMSR_LINK_STAT        0x0004U   /* Link Status (1=link is up)         */
#define BMSR_JABBER_DETECT    0x0002U   /* Jabber Detect                      */
#define BMSR_EXT_CAPAB        0x0001U   /* Extended Capability                */

/* PHY Identifier Registers Bitmasks */
#define PHY_ID1               0x0022U   /* PHY ID Number 1                    */
#define PHY_ID2               0x1510U   /* PHY ID Number 2                    */

/* Auto-Negotiation Advertisement Register Bitmasks */
#define ANAR_NEXT_PAGE        0x8000U   /* Next page capable                  */
#define ANAR_REMOTE_FAULT     0x2000U   /* Remote fault supported             */
#define ANAR_PAUSE            0x0C00U   /* Pause                              */
#define ANAR_100B_T4          0x0200U   /* 100Base-T4 capable                 */
#define ANAR_100B_TX_FD       0x0100U   /* 100MBps full-duplex capable        */
#define ANAR_100B_TX_HD       0x0080U   /* 100MBps half-duplex capable        */
#define ANAR_10B_TX_FD        0x0040U   /* 10MBps full-duplex capable         */
#define ANAR_10B_TX_HD        0x0020U   /* 10MBps half-duplex capable         */
#define ANAR_SELECTOR_FIELD   0x001FU   /* Selector field (0x01==IEEE 802.3)  */

/* Auto-Negotiation Link Partner Ability Register Bitmasks */
#define ANLPAR_NEXT_PAGE      0x8000U   /* Next page capable                  */
#define ANLPAR_ACKNOWLEDGE    0x4000U   /* Acknowledge from partner           */
#define ANLPAR_REMOTE_FAULT   0x2000U   /* Remote fault detected              */
#define ANLPAR_PAUSE          0x0C00U   /* Pause                              */
#define ANLPAR_100B_TX_FD     0x0100U   /* 100MBps full-duplex capable        */
#define ANLPAR_100B_TX_HD     0x0080U   /* 100MBps half-duplex capable        */
#define ANLPAR_10B_TX_FD      0x0040U   /* 10MBps full-duplex capable         */
#define ANLPAR_10B_TX_HD      0x0020U   /* 10MBps half-duplex capable         */
#define ANLPAR_SELECTOR_FIELD 0x001FU   /* Selector field (0x01==IEEE 802.3)  */

/* Auto-Negotiation Expansion Register Bitmasks */
#define ANER_PDF              0x0010U   /* Parallel Detection Fault           */
#define ANER_LPAR_NEXT_PAGE   0x0008U   /* Link Partner Next Page Able        */
#define ANER_NEXT_PAGE        0x0004U   /* Next Page Able                     */
#define ANER_PAGE_RECEIVED    0x0002U   /* Page Received                      */
#define ANER_LPAR_ANEG        0x0001U   /* Link Partner Auto-Negotiation Able */

/* Auto-Negotiation Next Page Register Bitmasks */
#define ANNP_NEXT_PAGE        0x8000U   /* Next Page                          */
#define ANNP_MESSAGE_PAGE     0x2000U   /* Message Page                       */
#define ANNP_ACKNOWLEDGE2     0x1000U   /* Acknowledge2                       */
#define ANNP_TOGGLE           0x0800U   /* Toggle                             */
#define ANNP_MESSAGE_FIELD    0x07FFU   /* Message Field                      */

/* Link Partner Next Page Ability Register Bitmasks */
#define LPNP_NEXT_PAGE        0x8000U   /* Next page                          */
#define LPNP_ACKNOWLEDGE      0x4000U   /* Acknowledge                        */
#define LPNP_MESSAGE_PAGE     0x2000U   /* Message Page                       */
#define LPNP_ACKNOWLEDGE2     0x1000U   /* Acknowledge2                       */
#define LPNP_TOGGLE           0x0800U   /* Toggle                             */
#define LPNP_MESSAGE_FIELD    0x07FFU   /* Message Field                      */

/* Interrupt Control/Status Register Bitmasks */
#define IRQCS_JABBER_IE       0x8000U   /* Jabber Interrupt Enable            */
#define IRQCS_RXERR_IE        0x4000U   /* Receive Error Interrupt Enable     */
#define IRQCS_PGRCVD_IE       0x2000U   /* Page Received Interrupt Enable     */
#define IRQCS_PDF_IE          0x1000U   /* Parallel Detect Fault Int. Enable  */
#define IRQCS_LP_ACK_IE       0x0800U   /* Link Partner Ack. Interrupt Enable */
#define IRQCS_LINK_DOWN_IE    0x0400U   /* Link-Down Interrupt Enable         */
#define IRQCS_RFAULT_IE       0x0200U   /* Remote Fault Interrupt Enable      */
#define IRQCS_LINK_UP_IE      0x0100U   /* Link-Up Interrupt Enable           */
#define IRQCS_JABBER_IRQ      0x0080U   /* Jabber Interrupt                   */
#define IRQCS_RXERR_IRQ       0x0040U   /* Receive Error Interrupt            */
#define IRQCS_PGRCVD_IRQ      0x0020U   /* Page Receive Interrupt             */
#define IRQCS_PDF_IRQ         0x0010U   /* Parallel Detect Fault Interrupt    */
#define IRQCS_LP_ACK_IRQ      0x0008U   /* Link Partner Acknowledge Interrupt */
#define IRQCS_LINK_DOWN_IRQ   0x0004U   /* Link-Down Interrupt                */
#define IRQCS_RFAULT_IRQ      0x0002U   /* Remote Fault Interrupt             */
#define IRQCS_LINK_UP_IRQ     0x0001U   /* Link-Up Interrupt                  */

/* PHY Control 1 Register Bitmasks */
#define PHYCR1_LED_MODE       0xC000U   /* LED mode                           */
#define PHYCR1_POLARITY_STAT  0x2000U   /* Polarity Status                    */
#define PHYCR1_MDI_STATE      0x0800U   /* MDI/MDI-X State                    */
#define PHYCR1_REM_LOOPBACK   0x0080U   /* Remote loopback                    */

/* PHY Control 2 Register Bitmasks */
#define PHYCR2_HP_MDIX        0x8000U   /* HP_MDIX                            */
#define PHYCR2_MDI_SELECT     0x4000U   /* MDI/MDI-X Select                   */
#define PHYCR2_PAIR_SWAP_DIS  0x2000U   /* Pair Swap Disable                  */
#define PHYCR2_ENERGY_DETECT  0x1000U   /* Energy Detect                      */
#define PHYCR2_FORCE_LINK     0x0800U   /* Force Link                         */
#define PHYCR2_POWER_SAVING   0x0400U   /* Power Saving                       */
#define PHYCR2_IRQ_LEVEL      0x0200U   /* Interrupt Level                    */
#define PHYCR2_EN_JABBER      0x0100U   /* Enable Jabber                      */
#define PHYCR2_AN_COMPLETE    0x0080U   /* Auto-Negotiation Complete          */
#define PHYCR2_EN_PAUSE       0x0040U   /* Enable Pause (Flow Control)        */
#define PHYCR2_PHY_ISOLATE    0x0020U   /* PHY Isolate                        */
#define PHYCR2_OPERATION_MODE 0x001CU   /* Operation Mode Indication          */
#define PHYCR2_SQE_TEST_EN    0x0002U   /* Enable SQE test                    */
#define PHYCR2_DIS_DATA_SCR   0x0001U   /* Disable Data Scrambling            */

/* PHY Control 1 Operation Mode Bitmasks */
#define PHYCR2_OM_10_HD       0x0004U   /* 10Base-TX Half-duplex bitmask      */
#define PHYCR2_OM_100_HD      0x0008U   /* 100Base-TX Half-duplex bitmask     */
#define PHYCR2_OM_10_FD       0x0014U   /* 10Base-TX Full-duplex bitmask      */
#define PHYCR2_OM_100_FD      0x0018U   /* 100Base-TX Full-duplex bitmask     */

/* PHY Driver State Flags */
#define PHY_INIT              0x01U     /* Driver initialized                */
#define PHY_POWER             0x02U     /* Driver power is on                */

/* PHY Driver Control Structure */
typedef struct phy_ctrl {
  ARM_ETH_PHY_Read_t  reg_rd;           /* PHY register read function        */
  ARM_ETH_PHY_Write_t reg_wr;           /* PHY register write function       */
  uint16_t            bmcr;             /* BMCR register value               */
  uint8_t             flags;            /* Control flags                     */
  uint8_t             rsvd;             /* Reserved                          */
} PHY_CTRL;

#endif /* __PHY_KSZ8041NL_H */
