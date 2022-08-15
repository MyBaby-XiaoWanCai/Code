#include "discharge.h"
#include "main.h"

uint8_t Irange_flag[8] = {I_HIGH, I_HIGH, I_HIGH, I_HIGH, I_HIGH, I_HIGH, I_HIGH, I_HIGH};
uint8_t Vrange_flag[8] = {V_SENSE, V_SENSE, V_SENSE, V_SENSE, V_SENSE, V_SENSE, V_SENSE, V_SENSE};
uint8_t Dir_flag[8] = {BUCK_MODE, BUCK_MODE, BUCK_MODE, BUCK_MODE, BUCK_MODE, BUCK_MODE, BUCK_MODE, BUCK_MODE};

/***************************************************************************************************
**  充放电控制相关引脚初始化
**
**
***************************************************************************************************/
void Discharge_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    //开启相应的时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();


    /*----------------------------------------------------------------------------------------------
    **  Mod_DIR1 --> PE5    |   Mod_EN1A --> PH9    Mod_EN1B --> PD13
    **  Mod_DIR2 --> PE6    |   Mod_EN2A --> PH10   Mod_EN2B --> PD14
    **  Mod_DIR3 --> PI8    |   Mod_EN3A --> PH11   Mod_EN3B --> PD15
    **  Mod_DIR4 --> PC13   |   Mod_EN4A --> PH12   Mod_EN4B --> PG2
    **  Mod_DIR5 --> PI9    |   Mod_EN5A --> PB14   Mod_EN5B --> PG3
    **  Mod_DIR6 --> PF0    |   Mod_EN6A --> PD10   Mod_EN6B --> PG4
    **  Mod_DIR7 --> PF1    |   Mod_EN7A --> PD11   Mod_EN7B --> PG5
    **  Mod_DIR8 --> PF2    |   Mod_EN8A --> PD12   Mod_EN8B --> PG6
    ----------------------------------------------------------------------------------------------*/

    //PB14-->Mod_EN5A
    GPIO_InitStruct.Pin     = GPIO_PIN_14;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;      //推挽输出
    GPIO_InitStruct.Pull    = GPIO_PULLUP;              //上拉
    GPIO_InitStruct.Speed   = GPIO_SPEED_HIGH;          //高速
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //PC13-->Mod_DIR4
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //PD10,11,12-->Mod_EN6A,7A,8A;
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    //PE5,6-->Mod_DIR1,2
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    //PF0,1,2-->Mod_DIR6,7,8
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

//    //PG2,3,4,5,6-->Mod_EN4B,5B,6B,7B,8B
//    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
//    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    //PH9,10,11,12-->Mod_EN1A,2A,3A,4A
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    //PI8,9-->Mod_DIR3,5
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    //设置各IO口初始状态
    //DIR
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET);

    //ENA,ENB
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_9, GPIO_PIN_RESET);   //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_RESET);  //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET);  //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET);  //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);  //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);  //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);  //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);  //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);

    /*----------------------------------------------------------------------------------------------
    **  I1_Range --> PD2    |   V1_Range --> PH13   |   Rev_Port1 --> PG7
    **  I2_Range --> PD3    |   V2_Range --> PH14   |   Rev_Port2 --> PG8
    **  I3_Range --> PD4    |   V3_Range --> PH15   |   Rev_Port3 --> PC6
    **  I4_Range --> PD5    |   V4_Range --> PB5    |   Rev_Port4 --> PC7
    **  I5_Range --> PD6    |   V5_Range --> PB8    |   Rev_Port5 --> PC8
    **  I6_Range --> PD7    |   V6_Range --> PB9    |   Rev_Port6 --> PC9
    **  I7_Range --> PG9    |   V7_Range --> PI4    |   Rev_Port7 --> PA8
    **  I8_Range --> PG10   |   V8_Range --> PI5    |   Rev_Port8 --> PA9
    ----------------------------------------------------------------------------------------------*/

    //PA8,9-->Rev_Port7,8
    GPIO_InitStruct.Pin     = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;      //推挽输出
    GPIO_InitStruct.Pull    = GPIO_PULLUP;              //上拉
    GPIO_InitStruct.Speed   = GPIO_SPEED_HIGH;          //高速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //PB8,9-->V_Range 5,6
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //PC6,7,8,9-->Rev_Port3,4,5,6
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //PD2,3,4,5,6,7-->I_Range 1,2,3,4,5,6
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    //PG7,8-->Rev_Port1,2;PG15,9,10-->I_Range 4,7,8;
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    //PH13,14-->V_Range 1,2  PH15-->V_Range 3
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14| GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    //PI4,5-->V_Range 7,8
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    //I_Range
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);

    //V_Range
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_RESET);

    //Rec_Port
    // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);

    /*----------------------------------------------------------------------------------------------
    **  IO_OE --> PA10
    ----------------------------------------------------------------------------------------------*/
    //PA10-->IO_OE
    GPIO_InitStruct.Pin     = GPIO_PIN_10;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;      //推挽输出
    GPIO_InitStruct.Pull    = GPIO_PULLDOWN;              //上拉
    GPIO_InitStruct.Speed   = GPIO_SPEED_HIGH;          //高速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

    //DAC 模块初始化---------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
}


/***************************************************************************************************
**  充放电控制执行程序
**
**
***************************************************************************************************/
void Discharge_Control(uint8_t port, uint8_t dir, float V_Set, float I_Set)
{
    uint16_t Vout_number = 0, Iout_number = 0;
    uint32_t temp32;

    // Vout_number = (uint16_t)(V_Set * 65536 / DAC80508_Vref);
    // Iout_number = (uint16_t)(I_Set * 65536 / DAC80508_Vref / DAC80508_Iref);

    switch(port) {
        case DAC80508_CH1: {
            // PORT1_DIS;
            // HAL_Delay(10);
            // REV_PORT1_DIS;

            if(dir == BUCK_MODE) {
                PORT1_DIR = BUCK_SET;
                Dir_flag[0] = BUCK_MODE;
            } else if(dir == BOOST_MODE) {
                PORT1_DIR = BOOST_SET;
                Dir_flag[0] = BOOST_MODE;
            }

            if(I_Set <= I_RANGE) {
                I1_OUT = I_LOW_SET;
                Irange_flag[0] = I_LOW;
            } else if(I_Set <= Iport_MAX) {
                I1_OUT = I_HIGH_SET;
                Irange_flag[0] = I_HIGH;
            } else {
                break;
            }

            if(Dir_flag[0] == BUCK_MODE) {
                if(Vrange_flag[0] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[0].Buck_Vset_sense_K + calibration[0].Buck_Vset_sense_B);
                } else if(Vrange_flag[0] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[0].Buck_Vset_mod_K + calibration[0].Buck_Vset_mod_B);
                }

                if(Irange_flag[0] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[0].Buck_Iset_HK + calibration[0].Buck_Iset_HB);
                } else if(Irange_flag[0] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[0].Buck_Iset_LK + calibration[0].Buck_Iset_LB);
                }
            } else if(Dir_flag[0] == BOOST_MODE) {
                if(Vrange_flag[0] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[0].Boost_Vset_sense_K + calibration[0].Boost_Vset_sense_B);
                } else if(Vrange_flag[0] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[0].Boost_Vset_mod_K + calibration[0].Boost_Vset_mod_B);
                }

                if(Irange_flag[0] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[0].Boost_Iset_HK + calibration[0].Boost_Iset_HB);
                } else if(Irange_flag[0] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[0].Boost_Iset_LK + calibration[0].Boost_Iset_LB);
                }
            }

            DAC80508_SetDacData(0, Vout_number, 0);
            DAC80508_SetDacData(0, Iout_number, 1);

            // REV_PORT1_EN;
            // HAL_Delay(20);
            // PORT1_EN;

            break;
        }

        case DAC80508_CH2: {
            // PORT2_DIS;
            // HAL_Delay(10);
            // REV_PORT2_DIS;

            if(dir == BUCK_MODE) {
                PORT2_DIR = BUCK_SET;
                Dir_flag[1] = BUCK_MODE;
            } else if(dir == BOOST_MODE) {
                PORT2_DIR = BOOST_SET;
                Dir_flag[1] = BOOST_MODE;
            }

            if(I_Set <= I_RANGE) {
                I2_OUT = I_LOW_SET;
                Irange_flag[1] = I_LOW;
            } else if(I_Set <= Iport_MAX) {
                I2_OUT = I_HIGH_SET;
                Irange_flag[1] = I_HIGH;
            } else {
                break;
            }

            if(Dir_flag[1] == BUCK_MODE) {
                if(Vrange_flag[1] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[1].Buck_Vset_sense_K + calibration[1].Buck_Vset_sense_B);
                } else if(Vrange_flag[1] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[1].Buck_Vset_mod_K + calibration[1].Buck_Vset_mod_B);
                }

                if(Irange_flag[1] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[1].Buck_Iset_HK + calibration[1].Buck_Iset_HB);
                } else if(Irange_flag[1] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[1].Buck_Iset_LK + calibration[1].Buck_Iset_LB);
                }
            } else if(Dir_flag[1] == BOOST_MODE) {
                if(Vrange_flag[1] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[1].Boost_Vset_sense_K + calibration[1].Boost_Vset_sense_B);
                } else if(Vrange_flag[1] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[1].Boost_Vset_mod_K + calibration[1].Boost_Vset_mod_B);
                }

                if(Irange_flag[1] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[1].Boost_Iset_HK + calibration[1].Boost_Iset_HB);
                } else if(Irange_flag[1] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[1].Boost_Iset_LK + calibration[1].Boost_Iset_LB);
                }
            }

            DAC80508_SetDacData(1, Vout_number, 0);
            DAC80508_SetDacData(1, Iout_number, 1);

            // REV_PORT2_EN;
            // HAL_Delay(20);
            // PORT2_EN;

            break;
        }

        case DAC80508_CH3: {
            // PORT3_DIS;
            // HAL_Delay(10);
            // REV_PORT3_DIS;

            if(dir == BUCK_MODE) {
                PORT3_DIR = BUCK_SET;
                Dir_flag[2] = BUCK_MODE;
            } else if(dir == BOOST_MODE) {
                PORT3_DIR = BOOST_SET;
                Dir_flag[2] = BOOST_MODE;
            }

            if(I_Set <= I_RANGE) {
                I3_OUT = I_LOW_SET;
                Irange_flag[2] = I_LOW;
            } else if(I_Set <= Iport_MAX) {
                I3_OUT = I_HIGH_SET;
                Irange_flag[2] = I_HIGH;
            } else {
                break;
            }

            if(Dir_flag[2] == BUCK_MODE) {
                if(Vrange_flag[2] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[2].Buck_Vset_sense_K + calibration[2].Buck_Vset_sense_B);
                } else if(Vrange_flag[2] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[2].Buck_Vset_mod_K + calibration[2].Buck_Vset_mod_B);
                }

                if(Irange_flag[2] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[2].Buck_Iset_HK + calibration[2].Buck_Iset_HB);
                } else if(Irange_flag[2] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[2].Buck_Iset_LK + calibration[2].Buck_Iset_LB);
                }
            } else if(Dir_flag[2] == BOOST_MODE) {
                if(Vrange_flag[2] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[2].Boost_Vset_sense_K + calibration[2].Boost_Vset_sense_B);
                } else if(Vrange_flag[2] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[2].Boost_Vset_mod_K + calibration[2].Boost_Vset_mod_B);
                }

                if(Irange_flag[2] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[2].Boost_Iset_HK + calibration[2].Boost_Iset_HB);
                } else if(Irange_flag[2] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[2].Boost_Iset_LK + calibration[2].Boost_Iset_LB);
                }
            }

            DAC80508_SetDacData(2, Vout_number, 0);
            DAC80508_SetDacData(2, Iout_number, 1);

            // REV_PORT3_EN;
            // HAL_Delay(20);
            // PORT3_EN;

            break;
        }

        case DAC80508_CH4: {
            // PORT4_DIS;
            // HAL_Delay(10);
            // REV_PORT4_DIS;

            if(dir == BUCK_MODE) {
                PORT4_DIR = BUCK_SET;
                Dir_flag[3] = BUCK_MODE;
            } else if(dir == BOOST_MODE) {
                PORT4_DIR = BOOST_SET;
                Dir_flag[3] = BOOST_MODE;
            }

            if(I_Set <= I_RANGE) {
                I4_OUT = I_LOW_SET;
                Irange_flag[3] = I_LOW;
            } else if(I_Set <= Iport_MAX) {
                I4_OUT = I_HIGH_SET;
                Irange_flag[3] = I_HIGH;
            } else {
                break;
            }

            if(Dir_flag[3] == BUCK_MODE) {
                if(Vrange_flag[3] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[3].Buck_Vset_sense_K + calibration[3].Buck_Vset_sense_B);
                } else if(Vrange_flag[3] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[3].Buck_Vset_mod_K + calibration[3].Buck_Vset_mod_B);
                }

                if(Irange_flag[3] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[3].Buck_Iset_HK + calibration[3].Buck_Iset_HB);
                } else if(Irange_flag[3] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[3].Buck_Iset_LK + calibration[3].Buck_Iset_LB);
                }
            } else if(Dir_flag[3] == BOOST_MODE) {
                if(Vrange_flag[3] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[3].Boost_Vset_sense_K + calibration[3].Boost_Vset_sense_B);
                } else if(Vrange_flag[3] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[3].Boost_Vset_mod_K + calibration[3].Boost_Vset_mod_B);
                }

                if(Irange_flag[3] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[3].Boost_Iset_HK + calibration[3].Boost_Iset_HB);
                } else if(Irange_flag[3] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[3].Boost_Iset_LK + calibration[3].Boost_Iset_LB);
                }
            }

            DAC80508_SetDacData(3, Vout_number, 0);
            DAC80508_SetDacData(3, Iout_number, 1);

            // REV_PORT4_EN;
            // HAL_Delay(20);
            // PORT4_EN;

            break;
        }

        case DAC80508_CH5: {
            // PORT5_DIS;
            // HAL_Delay(10);
            // REV_PORT5_DIS;

            if(dir == BUCK_MODE) {
                PORT5_DIR = BUCK_SET;
                Dir_flag[4] = BUCK_MODE;
            } else if(dir == BOOST_MODE) {
                PORT5_DIR = BOOST_SET;
                Dir_flag[4] = BOOST_MODE;
            }

            if(I_Set <= I_RANGE) {
                I5_OUT = I_LOW_SET;
                Irange_flag[4] = I_LOW;
            } else if(I_Set <= Iport_MAX) {
                I5_OUT = I_HIGH_SET;
                Irange_flag[4] = I_HIGH;
            } else {
                break;
            }

            if(Dir_flag[4] == BUCK_MODE) {
                if(Vrange_flag[4] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[4].Buck_Vset_sense_K + calibration[4].Buck_Vset_sense_B);
                } else if(Vrange_flag[4] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[4].Buck_Vset_mod_K + calibration[4].Buck_Vset_mod_B);
                }

                if(Irange_flag[4] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[4].Buck_Iset_HK + calibration[4].Buck_Iset_HB);
                } else if(Irange_flag[4] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[4].Buck_Iset_LK + calibration[4].Buck_Iset_LB);
                }
            } else if(Dir_flag[4] == BOOST_MODE) {
                if(Vrange_flag[4] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[4].Boost_Vset_sense_K + calibration[4].Boost_Vset_sense_B);
                } else if(Vrange_flag[4] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[4].Boost_Vset_mod_K + calibration[4].Boost_Vset_mod_B);
                }

                if(Irange_flag[4] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[4].Boost_Iset_HK + calibration[4].Boost_Iset_HB);
                } else if(Irange_flag[4] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[4].Boost_Iset_LK + calibration[4].Boost_Iset_LB);
                }
            }

            DAC80508_SetDacData(4, Vout_number, 0);
            DAC80508_SetDacData(4, Iout_number, 1);

            // REV_PORT5_EN;
            // HAL_Delay(20);
            // PORT5_EN;

            break;
        }

        case DAC80508_CH6: {
            // PORT6_DIS;
            // HAL_Delay(10);
            // REV_PORT6_DIS;

            if(dir == BUCK_MODE) {
                PORT6_DIR = BUCK_SET;
                Dir_flag[5] = BUCK_MODE;
            } else if(dir == BOOST_MODE) {
                PORT6_DIR = BOOST_SET;
                Dir_flag[5] = BOOST_MODE;
            }

            if(I_Set <= I_RANGE) {
                I6_OUT = I_LOW_SET;
                Irange_flag[5] = I_LOW;
            } else if(I_Set <= Iport_MAX) {
                I6_OUT = I_HIGH_SET;
                Irange_flag[5] = I_HIGH;
            } else {
                break;
            }

            if(Dir_flag[5] == BUCK_MODE) {
                if(Vrange_flag[5] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[5].Buck_Vset_sense_K + calibration[5].Buck_Vset_sense_B);
                } else if(Vrange_flag[5] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[5].Buck_Vset_mod_K + calibration[5].Buck_Vset_mod_B);
                }

                if(Irange_flag[5] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[5].Buck_Iset_HK + calibration[5].Buck_Iset_HB);
                } else if(Irange_flag[5] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[5].Buck_Iset_LK + calibration[5].Buck_Iset_LB);
                }
            } else if(Dir_flag[5] == BOOST_MODE) {
                if(Vrange_flag[5] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[5].Boost_Vset_sense_K + calibration[5].Boost_Vset_sense_B);
                } else if(Vrange_flag[5] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[5].Boost_Vset_mod_K + calibration[5].Boost_Vset_mod_B);
                }

                if(Irange_flag[5] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[5].Boost_Iset_HK + calibration[5].Boost_Iset_HB);
                } else if(Irange_flag[5] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[5].Boost_Iset_LK + calibration[5].Boost_Iset_LB);
                }
            }

            DAC80508_SetDacData(5, Vout_number, 0);
            DAC80508_SetDacData(5, Iout_number, 1);

            // REV_PORT6_EN;
            // HAL_Delay(20);
            // PORT6_EN;

            break;
        }

        case DAC80508_CH7: {
            // PORT7_DIS;
            // HAL_Delay(10);
            // REV_PORT7_DIS;

            if(dir == BUCK_MODE) {
                PORT7_DIR = BUCK_SET;
                Dir_flag[6] = BUCK_MODE;
            } else if(dir == BOOST_MODE) {
                PORT7_DIR = BOOST_SET;
                Dir_flag[6] = BOOST_MODE;
            }

            if(I_Set <= I_RANGE) {
                I7_OUT = I_LOW_SET;
                Irange_flag[6] = I_LOW;
            } else if(I_Set <= Iport_MAX) {
                I7_OUT = I_HIGH_SET;
                Irange_flag[6] = I_HIGH;
            } else {
                break;
            }

            if(Dir_flag[6] == BUCK_MODE) {
                if(Vrange_flag[6] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[6].Buck_Vset_sense_K + calibration[6].Buck_Vset_sense_B);
                } else if(Vrange_flag[6] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[6].Buck_Vset_mod_K + calibration[6].Buck_Vset_mod_B);
                }

                if(Irange_flag[6] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[6].Buck_Iset_HK + calibration[6].Buck_Iset_HB);
                } else if(Irange_flag[6] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[6].Buck_Iset_LK + calibration[6].Buck_Iset_LB);
                }
            } else if(Dir_flag[6] == BOOST_MODE) {
                if(Vrange_flag[6] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[6].Boost_Vset_sense_K + calibration[6].Boost_Vset_sense_B);
                } else if(Vrange_flag[6] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[6].Boost_Vset_mod_K + calibration[6].Boost_Vset_mod_B);
                }

                if(Irange_flag[6] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[6].Boost_Iset_HK + calibration[6].Boost_Iset_HB);
                } else if(Irange_flag[6] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[6].Boost_Iset_LK + calibration[6].Boost_Iset_LB);
                }
            }

            DAC80508_SetDacData(6, Vout_number, 0);
            DAC80508_SetDacData(6, Iout_number, 1);

            // REV_PORT7_EN;
            // HAL_Delay(20);
            // PORT7_EN;

            break;
        }

        case DAC80508_CH8: {
            // PORT8_DIS;
            // HAL_Delay(10);
            // REV_PORT8_DIS;

            if(dir == BUCK_MODE) {
                PORT8_DIR = BUCK_SET;
                Dir_flag[7] = BUCK_MODE;
            } else if(dir == BOOST_MODE) {
                PORT8_DIR = BOOST_SET;
                Dir_flag[7] = BOOST_MODE;
            }

            if(I_Set <= I_RANGE) {
                I8_OUT = I_LOW_SET;
                Irange_flag[7] = I_LOW;
            } else if(I_Set <= Iport_MAX) {
                I8_OUT = I_HIGH_SET;
                Irange_flag[7] = I_HIGH;
            } else {
                break;
            }

            if(Dir_flag[7] == BUCK_MODE) {
                if(Vrange_flag[7] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[7].Buck_Vset_sense_K + calibration[7].Buck_Vset_sense_B);
                } else if(Vrange_flag[7] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[7].Buck_Vset_mod_K + calibration[7].Buck_Vset_mod_B);
                }

                if(Irange_flag[7] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[7].Buck_Iset_HK + calibration[7].Buck_Iset_HB);
                } else if(Irange_flag[7] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[7].Buck_Iset_LK + calibration[7].Buck_Iset_LB);
                }
            } else if(Dir_flag[7] == BOOST_MODE) {
                if(Vrange_flag[7] == V_SENSE) {
                    Vout_number = (uint16_t)(V_Set * calibration[7].Boost_Vset_sense_K + calibration[7].Boost_Vset_sense_B);
                } else if(Vrange_flag[7] == V_MOD) {
                    Vout_number = (uint16_t)(V_Set * calibration[7].Boost_Vset_mod_K + calibration[7].Boost_Vset_mod_B);
                }

                if(Irange_flag[7] == I_HIGH) {
                    Iout_number = (uint16_t)(I_Set * calibration[7].Boost_Iset_HK + calibration[7].Boost_Iset_HB);
                } else if(Irange_flag[7] == I_LOW) {
                    Iout_number = (uint16_t)(I_Set * calibration[7].Boost_Iset_LK + calibration[7].Boost_Iset_LB);
                }
            }

            DAC80508_SetDacData(7, Vout_number, 0);
            DAC80508_SetDacData(7, Iout_number, 1);

            // REV_PORT8_EN;
            // HAL_Delay(20);
            // PORT8_EN;

            break;
        }

        default: {
            break;
        }
    }
}


/***************************************************************************************************
**  校准参数读取
**
**
***************************************************************************************************/
void Calibration_Read(void)
{
    uint16_t temp16;
    uint32_t temp32;

    // if(SYS_MODE == SYS_NORMAL_MODE) {
        temp16 = EEPROM_ReadLenByte(0, 2);
        if(temp16 == CALIBRATION_FLAG) {
            for(uint8_t i = 0; i < 8; i++) {
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vset_sense_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vset_sense_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vset_sense_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vset_sense_B, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vset_mod_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vset_mod_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vset_mod_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vset_mod_B, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_iset_HK-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Iset_HK, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_iset_HB-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Iset_HB, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_iset_LK-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Iset_LK, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_iset_LB-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Iset_LB, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vsense_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vsense_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vsense_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vsense_B, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vmod_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vmod_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vmod_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vmod_B, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_iout_HK-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Iout_HK, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_iout_HB-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Iout_HB, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_iout_LK-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Iout_LK, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_iout_LB-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Iout_LB, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vj_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vj_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(buck_vj_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Buck_Vj_B, &temp32, 4);


                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vset_sense_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vset_sense_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vset_sense_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vset_sense_B, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vset_mod_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vset_mod_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vset_mod_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vset_mod_B, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_iset_HK-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Iset_HK, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_iset_HB-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Iset_HB, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_iset_LK-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Iset_LK, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_iset_LB-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Iset_LB, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vsense_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vsense_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vsense_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vsense_B, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vmod_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vmod_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vmod_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vmod_B, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_iout_HK-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Iout_HK, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_iout_HB-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Iout_HB, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_iout_LK-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Iout_LK, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_iout_LB-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Iout_LB, &temp32, 4);

                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vj_K-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vj_K, &temp32, 4);
                temp32 = EEPROM_ReadLenByte((i*CALIBRATION_PORT_LEN+(boost_vj_B-1)*4+2), 4);
                if(temp32 == 0xffffffff)
                    temp32 = 0;
                memcpy(&calibration[i].Boost_Vj_B, &temp32, 4);
            }

            /*
            //--------------------------------------------------------------------------------------
            temp32 = EEPROM_ReadLenByte(((vset_sense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vset_sense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte(((vset_sense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vset_sense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte(((vset_mod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vset_mod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte(((vset_mod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vset_mod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte(((iset_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Iset_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte(((iset_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Iset_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte(((iset_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Iset_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte(((iset_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Iset_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte(((vsense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vsense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte(((vsense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vsense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte(((vmod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vmod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte(((vmod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vmod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte(((iout_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Iout_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte(((iout_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Iout_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte(((iout_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Iout_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte(((iout_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Iout_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte(((vj_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vj_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte(((vj_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[0].Vj_B, &temp32, 4);


            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vset_sense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vset_sense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vset_sense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vset_sense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vset_mod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vset_mod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vset_mod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vset_mod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(iset_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Iset_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(iset_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Iset_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(iset_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Iset_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(iset_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Iset_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vsense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vsense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vsense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vsense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vmod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vmod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vmod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vmod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(iout_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Iout_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(iout_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Iout_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(iout_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Iout_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(iout_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Iout_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vj_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vj_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((CALIBRATION_PORT_LEN+(vj_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[1].Vj_B, &temp32, 4);
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vset_sense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vset_sense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vset_sense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vset_sense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vset_mod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vset_mod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vset_mod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vset_mod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(iset_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Iset_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(iset_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Iset_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(iset_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Iset_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(iset_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Iset_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vsense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vsense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vsense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vsense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vmod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vmod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vmod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vmod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(iout_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Iout_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(iout_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Iout_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(iout_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Iout_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(iout_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Iout_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vj_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vj_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((2*CALIBRATION_PORT_LEN+(vj_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[2].Vj_B, &temp32, 4);
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vset_sense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vset_sense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vset_sense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vset_sense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vset_mod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vset_mod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vset_mod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vset_mod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(iset_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Iset_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(iset_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Iset_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(iset_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Iset_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(iset_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Iset_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vsense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vsense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vsense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vsense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vmod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vmod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vmod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vmod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(iout_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Iout_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(iout_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Iout_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(iout_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Iout_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(iout_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Iout_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vj_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vj_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((3*CALIBRATION_PORT_LEN+(vj_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[3].Vj_B, &temp32, 4);
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vset_sense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vset_sense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vset_sense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vset_sense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vset_mod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vset_mod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vset_mod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vset_mod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(iset_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Iset_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(iset_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Iset_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(iset_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Iset_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(iset_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Iset_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vsense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vsense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vsense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vsense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vmod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vmod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vmod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vmod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(iout_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Iout_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(iout_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Iout_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(iout_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Iout_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(iout_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Iout_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vj_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vj_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((4*CALIBRATION_PORT_LEN+(vj_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[4].Vj_B, &temp32, 4);
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vset_sense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vset_sense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vset_sense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vset_sense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vset_mod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vset_mod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vset_mod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vset_mod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(iset_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Iset_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(iset_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Iset_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(iset_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Iset_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(iset_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Iset_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vsense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vsense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vsense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vsense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vmod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vmod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vmod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vmod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(iout_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Iout_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(iout_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Iout_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(iout_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Iout_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(iout_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Iout_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vj_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vj_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((5*CALIBRATION_PORT_LEN+(vj_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[5].Vj_B, &temp32, 4);
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vset_sense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vset_sense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vset_sense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vset_sense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vset_mod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vset_mod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vset_mod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vset_mod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(iset_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Iset_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(iset_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Iset_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(iset_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Iset_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(iset_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Iset_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vsense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vsense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vsense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vsense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vmod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vmod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vmod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vmod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(iout_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Iout_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(iout_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Iout_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(iout_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Iout_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(iout_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Iout_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vj_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vj_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((6*CALIBRATION_PORT_LEN+(vj_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[6].Vj_B, &temp32, 4);
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vset_sense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vset_sense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vset_sense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vset_sense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vset_mod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vset_mod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vset_mod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vset_mod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(iset_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Iset_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(iset_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Iset_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(iset_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Iset_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(iset_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Iset_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vsense_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vsense_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vsense_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vsense_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vmod_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vmod_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vmod_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vmod_B, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(iout_HK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Iout_HK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(iout_HB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Iout_HB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(iout_LK-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Iout_LK, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(iout_LB-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Iout_LB, &temp32, 4);

            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vj_K-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vj_K, &temp32, 4);
            temp32 = EEPROM_ReadLenByte((7*CALIBRATION_PORT_LEN+(vj_B-1)*4+2), 4);
            if(temp32 == 0xffffffff)
                temp32 = 0;
            memcpy(&calibration[7].Vj_B, &temp32, 4);
            //--------------------------------------------------------------------------------------
            */

            //--------------------------------------------------------------------------------------
            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(bus1_K-1)*4+2), 4);
            // memcpy(&special_calibration.Bus1_K, &temp32, 4);
            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(bus1_B-1)*4+2), 4);
            // memcpy(&special_calibration.Bus1_B, &temp32, 4);

            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(bus2_K-1)*4+2), 4);
            // memcpy(&special_calibration.Bus2_K, &temp32, 4);
            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(bus2_B-1)*4+2), 4);
            // memcpy(&special_calibration.Bus2_B, &temp32, 4);

            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(aux_K-1)*4+2), 4);
            // memcpy(&special_calibration.Aux_K, &temp32, 4);
            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(aux_B-1)*4+2), 4);
            // memcpy(&special_calibration.Aux_B, &temp32, 4);

            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(amb_K-1)*4+2), 4);
            // memcpy(&special_calibration.Amb_K, &temp32, 4);
            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(amb_B-1)*4+2), 4);
            // memcpy(&special_calibration.Amb_B, &temp32, 4);

            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(temperature_K-1)*4+2), 4);
            // memcpy(&special_calibration.Temperature_K, &temp32, 4);
            // temp32 = EEPROM_ReadLenByte((8*CALIBRATION_PORT_LEN+(temperature_B-1)*4+2), 4);
            // memcpy(&special_calibration.Temperature_B, &temp32, 4);


            special_calibration.Bus1_K = 31.91544;
            special_calibration.Bus1_B = 0;
            special_calibration.Bus2_K = 31.91544;
            special_calibration.Bus2_B = 0;
            special_calibration.Aux_K = 20.83333;
            special_calibration.Aux_B = 0;
            special_calibration.Amb_K = 179.34;//107.1429;
            special_calibration.Amb_B = -50;
            special_calibration.Temperature_K = 146.484375;//70.3125;//140.625;
            special_calibration.Temperature_B = -50;      //0
            //--------------------------------------------------------------------------------------

        } else {
            //--------------------------------------------------------------------------------------
            calibration[0].Buck_Vset_sense_K = 5041.230769;       //11090.54
            calibration[0].Buck_Vset_sense_B = 0;
            calibration[0].Buck_Vset_mod_K = 6865.14806;
            calibration[0].Buck_Vset_mod_B = 0;
            calibration[0].Buck_Iset_HK = 735.31392;     //781.25
            calibration[0].Buck_Iset_HB = 0;
            calibration[0].Buck_Iset_LK = 1875.050496;
            calibration[0].Buck_Iset_LB = 0;
            calibration[0].Buck_Vsense_K = 5.498841;     //5.323238
            calibration[0].Buck_Vsense_B = -0.33;     //-0.1597308
            calibration[0].Buck_Vmod_K = 9.7738288;       //8.53971
            calibration[0].Buck_Vmod_B = -0.26655897;       //-0.2329633
            calibration[0].Buck_Iout_HK = 98.01856;         //146.95436
            calibration[0].Buck_Iout_HB = 0;
            calibration[0].Buck_Iout_LK = 38.4386498;      //57.657974
            calibration[0].Buck_Iout_LB = 0;
            calibration[0].Buck_Vj_K = 2.500047;
            calibration[0].Buck_Vj_B = 0;

            calibration[0].Boost_Vset_sense_K = 5041.230769;
            calibration[0].Boost_Vset_sense_B = 0;
            calibration[0].Boost_Vset_mod_K = 6865.14806;
            calibration[0].Boost_Vset_mod_B = 0;
            calibration[0].Boost_Iset_HK = 735.31392;
            calibration[0].Boost_Iset_HB = 0;
            calibration[0].Boost_Iset_LK = 1875.050496;
            calibration[0].Boost_Iset_LB = 0;
            calibration[0].Boost_Vsense_K = 5.498841;
            calibration[0].Boost_Vsense_B = -0.33;
            calibration[0].Boost_Vmod_K = 9.7738288;
            calibration[0].Boost_Vmod_B = -0.26655897;
            calibration[0].Boost_Iout_HK = 98.01856;
            calibration[0].Boost_Iout_HB = 0;
            calibration[0].Boost_Iout_LK = 38.4386498;
            calibration[0].Boost_Iout_LB = 0;
            calibration[0].Boost_Vj_K = 2.500047;
            calibration[0].Boost_Vj_B = 0;
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            calibration[1].Buck_Vset_sense_K = 5041.230769;
            calibration[1].Buck_Vset_sense_B = 0;
            calibration[1].Buck_Vset_mod_K = 6865.14806;
            calibration[1].Buck_Vset_mod_B = 0;
            calibration[1].Buck_Iset_HK = 735.31392;
            calibration[1].Buck_Iset_HB = 0;
            calibration[1].Buck_Iset_LK = 1875.050496;
            calibration[1].Buck_Iset_LB = 0;
            calibration[1].Buck_Vsense_K = 5.498841;
            calibration[1].Buck_Vsense_B = -0.33;
            calibration[1].Buck_Vmod_K = 9.7738288;
            calibration[1].Buck_Vmod_B = -0.26655897;
            calibration[1].Buck_Iout_HK = 98.01856;
            calibration[1].Buck_Iout_HB = 0;
            calibration[1].Buck_Iout_LK = 38.4386498;
            calibration[1].Buck_Iout_LB = 0;
            calibration[1].Buck_Vj_K = 2.500047;
            calibration[1].Buck_Vj_B = 0;

            calibration[1].Boost_Vset_sense_K = 5041.230769;
            calibration[1].Boost_Vset_sense_B = 0;
            calibration[1].Boost_Vset_mod_K = 6865.14806;
            calibration[1].Boost_Vset_mod_B = 0;
            calibration[1].Boost_Iset_HK = 735.31392;
            calibration[1].Boost_Iset_HB = 0;
            calibration[1].Boost_Iset_LK = 1875.050496;
            calibration[1].Boost_Iset_LB = 0;
            calibration[1].Boost_Vsense_K = 5.498841;
            calibration[1].Boost_Vsense_B = -0.33;
            calibration[1].Boost_Vmod_K = 9.7738288;
            calibration[1].Boost_Vmod_B = -0.26655897;
            calibration[1].Boost_Iout_HK = 98.01856;
            calibration[1].Boost_Iout_HB = 0;
            calibration[1].Boost_Iout_LK = 38.4386498;
            calibration[1].Boost_Iout_LB = 0;
            calibration[1].Boost_Vj_K = 2.500047;
            calibration[1].Boost_Vj_B = 0;
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            calibration[2].Buck_Vset_sense_K = 5041.230769;
            calibration[2].Buck_Vset_sense_B = 0;
            calibration[2].Buck_Vset_mod_K = 6865.14806;
            calibration[2].Buck_Vset_mod_B = 0;
            calibration[2].Buck_Iset_HK = 735.31392;
            calibration[2].Buck_Iset_HB = 0;
            calibration[2].Buck_Iset_LK = 1875.050496;
            calibration[2].Buck_Iset_LB = 0;
            calibration[2].Buck_Vsense_K = 5.498841;
            calibration[2].Buck_Vsense_B = -0.33;
            calibration[2].Buck_Vmod_K = 9.7738288;
            calibration[2].Buck_Vmod_B = -0.26655897;
            calibration[2].Buck_Iout_HK = 98.01856;
            calibration[2].Buck_Iout_HB = 0;
            calibration[2].Buck_Iout_LK = 38.4386498;
            calibration[2].Buck_Iout_LB = 0;
            calibration[2].Buck_Vj_K = 2.500047;
            calibration[2].Buck_Vj_B = 0;

            calibration[2].Boost_Vset_sense_K = 5041.230769;
            calibration[2].Boost_Vset_sense_B = 0;
            calibration[2].Boost_Vset_mod_K = 6865.14806;
            calibration[2].Boost_Vset_mod_B = 0;
            calibration[2].Boost_Iset_HK = 735.31392;
            calibration[2].Boost_Iset_HB = 0;
            calibration[2].Boost_Iset_LK = 1875.050496;
            calibration[2].Boost_Iset_LB = 0;
            calibration[2].Boost_Vsense_K = 5.498841;
            calibration[2].Boost_Vsense_B = -0.33;
            calibration[2].Boost_Vmod_K = 9.7738288;
            calibration[2].Boost_Vmod_B = -0.26655897;
            calibration[2].Boost_Iout_HK = 98.01856;
            calibration[2].Boost_Iout_HB = 0;
            calibration[2].Boost_Iout_LK = 38.4386498;
            calibration[2].Boost_Iout_LB = 0;
            calibration[2].Boost_Vj_K = 2.500047;
            calibration[2].Boost_Vj_B = 0;
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            calibration[3].Buck_Vset_sense_K = 5041.230769;
            calibration[3].Buck_Vset_sense_B = 0;
            calibration[3].Buck_Vset_mod_K = 6865.14806;
            calibration[3].Buck_Vset_mod_B = 0;
            calibration[3].Buck_Iset_HK = 735.31392;
            calibration[3].Buck_Iset_HB = 0;
            calibration[3].Buck_Iset_LK = 1875.050496;
            calibration[3].Buck_Iset_LB = 0;
            calibration[3].Buck_Vsense_K = 5.498841;
            calibration[3].Buck_Vsense_B = -0.33;
            calibration[3].Buck_Vmod_K = 9.7738288;
            calibration[3].Buck_Vmod_B = -0.26655897;
            calibration[3].Buck_Iout_HK = 98.01856;
            calibration[3].Buck_Iout_HB = 0;
            calibration[3].Buck_Iout_LK = 38.4386498;
            calibration[3].Buck_Iout_LB = 0;
            calibration[3].Buck_Vj_K = 2.500047;
            calibration[3].Buck_Vj_B = 0;

            calibration[3].Boost_Vset_sense_K = 5041.230769;
            calibration[3].Boost_Vset_sense_B = 0;
            calibration[3].Boost_Vset_mod_K = 6865.14806;
            calibration[3].Boost_Vset_mod_B = 0;
            calibration[3].Boost_Iset_HK = 735.31392;
            calibration[3].Boost_Iset_HB = 0;
            calibration[3].Boost_Iset_LK = 1875.050496;
            calibration[3].Boost_Iset_LB = 0;
            calibration[3].Boost_Vsense_K = 5.498841;
            calibration[3].Boost_Vsense_B = -0.33;
            calibration[3].Boost_Vmod_K = 9.7738288;
            calibration[3].Boost_Vmod_B = -0.26655897;
            calibration[3].Boost_Iout_HK = 98.01856;
            calibration[3].Boost_Iout_HB = 0;
            calibration[3].Boost_Iout_LK = 38.4386498;
            calibration[3].Boost_Iout_LB = 0;
            calibration[3].Boost_Vj_K = 2.500047;
            calibration[3].Boost_Vj_B = 0;
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            calibration[4].Buck_Vset_sense_K = 5041.230769;
            calibration[4].Buck_Vset_sense_B = 0;
            calibration[4].Buck_Vset_mod_K = 6865.14806;
            calibration[4].Buck_Vset_mod_B = 0;
            calibration[4].Buck_Iset_HK = 735.31392;
            calibration[4].Buck_Iset_HB = 0;
            calibration[4].Buck_Iset_LK = 1875.050496;
            calibration[4].Buck_Iset_LB = 0;
            calibration[4].Buck_Vsense_K = 5.498841;
            calibration[4].Buck_Vsense_B = -0.33;
            calibration[4].Buck_Vmod_K = 9.7738288;
            calibration[4].Buck_Vmod_B = -0.26655897;
            calibration[4].Buck_Iout_HK = 98.01856;
            calibration[4].Buck_Iout_HB = 0;
            calibration[4].Buck_Iout_LK = 38.4386498;
            calibration[4].Buck_Iout_LB = 0;
            calibration[4].Buck_Vj_K = 2.500047;
            calibration[4].Buck_Vj_B = 0;

            calibration[4].Boost_Vset_sense_K = 5041.230769;
            calibration[4].Boost_Vset_sense_B = 0;
            calibration[4].Boost_Vset_mod_K = 6865.14806;
            calibration[4].Boost_Vset_mod_B = 0;
            calibration[4].Boost_Iset_HK = 735.31392;
            calibration[4].Boost_Iset_HB = 0;
            calibration[4].Boost_Iset_LK = 1875.050496;
            calibration[4].Boost_Iset_LB = 0;
            calibration[4].Boost_Vsense_K = 5.498841;
            calibration[4].Boost_Vsense_B = -0.33;
            calibration[4].Boost_Vmod_K = 9.7738288;
            calibration[4].Boost_Vmod_B = -0.26655897;
            calibration[4].Boost_Iout_HK = 98.01856;
            calibration[4].Boost_Iout_HB = 0;
            calibration[4].Boost_Iout_LK = 38.4386498;
            calibration[4].Boost_Iout_LB = 0;
            calibration[4].Boost_Vj_K = 2.500047;
            calibration[4].Boost_Vj_B = 0;
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            calibration[5].Buck_Vset_sense_K = 5041.230769;
            calibration[5].Buck_Vset_sense_B = 0;
            calibration[5].Buck_Vset_mod_K = 6865.14806;
            calibration[5].Buck_Vset_mod_B = 0;
            calibration[5].Buck_Iset_HK = 735.31392;
            calibration[5].Buck_Iset_HB = 0;
            calibration[5].Buck_Iset_LK = 1875.050496;
            calibration[5].Buck_Iset_LB = 0;
            calibration[5].Buck_Vsense_K = 5.498841;
            calibration[5].Buck_Vsense_B = -0.33;
            calibration[5].Buck_Vmod_K = 9.7738288;
            calibration[5].Buck_Vmod_B = -0.26655897;
            calibration[5].Buck_Iout_HK = 98.01856;
            calibration[5].Buck_Iout_HB = 0;
            calibration[5].Buck_Iout_LK = 38.4386498;
            calibration[5].Buck_Iout_LB = 0;
            calibration[5].Buck_Vj_K = 2.500047;
            calibration[5].Buck_Vj_B = 0;

            calibration[5].Boost_Vset_sense_K = 5041.230769;
            calibration[5].Boost_Vset_sense_B = 0;
            calibration[5].Boost_Vset_mod_K = 6865.14806;
            calibration[5].Boost_Vset_mod_B = 0;
            calibration[5].Boost_Iset_HK = 735.31392;
            calibration[5].Boost_Iset_HB = 0;
            calibration[5].Boost_Iset_LK = 1875.050496;
            calibration[5].Boost_Iset_LB = 0;
            calibration[5].Boost_Vsense_K = 5.498841;
            calibration[5].Boost_Vsense_B = -0.33;
            calibration[5].Boost_Vmod_K = 9.7738288;
            calibration[5].Boost_Vmod_B = -0.26655897;
            calibration[5].Boost_Iout_HK = 98.01856;
            calibration[5].Boost_Iout_HB = 0;
            calibration[5].Boost_Iout_LK = 38.4386498;
            calibration[5].Boost_Iout_LB = 0;
            calibration[5].Boost_Vj_K = 2.500047;
            calibration[5].Boost_Vj_B = 0;
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            calibration[6].Buck_Vset_sense_K = 5041.230769;
            calibration[6].Buck_Vset_sense_B = 0;
            calibration[6].Buck_Vset_mod_K = 6865.14806;
            calibration[6].Buck_Vset_mod_B = 0;
            calibration[6].Buck_Iset_HK = 735.31392;
            calibration[6].Buck_Iset_HB = 0;
            calibration[6].Buck_Iset_LK = 1875.050496;
            calibration[6].Buck_Iset_LB = 0;
            calibration[6].Buck_Vsense_K = 5.498841;
            calibration[6].Buck_Vsense_B = -0.33;
            calibration[6].Buck_Vmod_K = 9.7738288;
            calibration[6].Buck_Vmod_B = -0.26655897;
            calibration[6].Buck_Iout_HK = 98.01856;
            calibration[6].Buck_Iout_HB = 0;
            calibration[6].Buck_Iout_LK = 38.4386498;
            calibration[6].Buck_Iout_LB = 0;
            calibration[6].Buck_Vj_K = 2.500047;
            calibration[6].Buck_Vj_B = 0;

            calibration[6].Boost_Vset_sense_K = 5041.230769;
            calibration[6].Boost_Vset_sense_B = 0;
            calibration[6].Boost_Vset_mod_K = 6865.14806;
            calibration[6].Boost_Vset_mod_B = 0;
            calibration[6].Boost_Iset_HK = 735.31392;
            calibration[6].Boost_Iset_HB = 0;
            calibration[6].Boost_Iset_LK = 1875.050496;
            calibration[6].Boost_Iset_LB = 0;
            calibration[6].Boost_Vsense_K = 5.498841;
            calibration[6].Boost_Vsense_B = -0.33;
            calibration[6].Boost_Vmod_K = 9.7738288;
            calibration[6].Boost_Vmod_B = -0.26655897;
            calibration[6].Boost_Iout_HK = 98.01856;
            calibration[6].Boost_Iout_HB = 0;
            calibration[6].Boost_Iout_LK = 38.4386498;
            calibration[6].Boost_Iout_LB = 0;
            calibration[6].Boost_Vj_K = 2.500047;
            calibration[6].Boost_Vj_B = 0;
            //--------------------------------------------------------------------------------------

            //--------------------------------------------------------------------------------------
            calibration[7].Buck_Vset_sense_K = 5041.230769;
            calibration[7].Buck_Vset_sense_B = 0;
            calibration[7].Buck_Vset_mod_K = 6865.14806;
            calibration[7].Buck_Vset_mod_B = 0;
            calibration[7].Buck_Iset_HK = 735.31392;
            calibration[7].Buck_Iset_HB = 0;
            calibration[7].Buck_Iset_LK = 1875.050496;
            calibration[7].Buck_Iset_LB = 0;
            calibration[7].Buck_Vsense_K = 5.498841;
            calibration[7].Buck_Vsense_B = -0.33;
            calibration[7].Buck_Vmod_K = 9.7738288;
            calibration[7].Buck_Vmod_B = -0.26655897;
            calibration[7].Buck_Iout_HK = 98.01856;
            calibration[7].Buck_Iout_HB = 0;
            calibration[7].Buck_Iout_LK = 38.4386498;
            calibration[7].Buck_Iout_LB = 0;
            calibration[7].Buck_Vj_K = 2.500047;
            calibration[7].Buck_Vj_B = 0;

            calibration[7].Boost_Vset_sense_K = 5041.230769;
            calibration[7].Boost_Vset_sense_B = 0;
            calibration[7].Boost_Vset_mod_K = 6865.14806;
            calibration[7].Boost_Vset_mod_B = 0;
            calibration[7].Boost_Iset_HK = 735.31392;
            calibration[7].Boost_Iset_HB = 0;
            calibration[7].Boost_Iset_LK = 1875.050496;
            calibration[7].Boost_Iset_LB = 0;
            calibration[7].Boost_Vsense_K = 5.498841;
            calibration[7].Boost_Vsense_B = -0.33;
            calibration[7].Boost_Vmod_K = 9.7738288;
            calibration[7].Boost_Vmod_B = -0.26655897;
            calibration[7].Boost_Iout_HK = 98.01856;
            calibration[7].Boost_Iout_HB = 0;
            calibration[7].Boost_Iout_LK = 38.4386498;
            calibration[7].Boost_Iout_LB = 0;
            calibration[7].Boost_Vj_K = 2.500047;
            calibration[7].Boost_Vj_B = 0;
            //--------------------------------------------------------------------------------------

            special_calibration.Bus1_K = 31.91544;
            special_calibration.Bus1_B = 0;
            special_calibration.Bus2_K = 31.91544;
            special_calibration.Bus2_B = 0;
            special_calibration.Aux_K = 20.83333;
            special_calibration.Aux_B = 0;
            special_calibration.Amb_K = 107.1429;
            special_calibration.Amb_B = 0;
            special_calibration.Temperature_K = 130.9518;
            special_calibration.Temperature_B = 0;
        }

    // }

}

/***************************************************************************************************
**  校准运行
**
**
***************************************************************************************************/
void Calibration_Run(void)
{
    int i = 0;
    if(SYS_MODE == SYS_CALIBRATION_MODE) {
        for(i = 0; i < 8; i++) {
            if(calibration_run[i].run_flag == 1) {
                if(calibration_run[i].status == step_start_setting) {
                    if(calibration_run[i].type == CC) {
                        Discharge_Control(PORT(i), BUCK_MODE, V_MAX, calibration_run[i].current);
                    } else if(calibration_run[i].type == CV) {
                        Discharge_Control(PORT(i), BUCK_MODE, calibration_run[i].voltage, Iport_MAX);
                    } else if(calibration_run[i].type == CC_to_CV) {
                        Discharge_Control(PORT(i), BUCK_MODE, calibration_run[i].voltage, calibration_run[i].current);
                    } else if(calibration_run[i].type == CP) {
                       //进行采集计算相应电流、电压
                        /*
                        code...
                        */
                    } else if(calibration_run[i].type == DCC) {
                        Discharge_Control(PORT(i), BOOST_MODE, V_MIN, calibration_run[i].current);
                    } else if(calibration_run[i].type == DCV) {
                        Discharge_Control(PORT(i), BOOST_MODE, calibration_run[i].voltage, Iport_MAX);
                    } else if(calibration_run[i].type == DCC_to_DCV) {
                        Discharge_Control(PORT(i), BOOST_MODE, calibration_run[i].voltage, calibration_run[i].current);
                    } else if(calibration_run[i].type == DCP) {
                       //进行采集计算相应电流、电压
                        /*
                        code...
                        */
                    } else if(calibration_run[i].type == standby) {
                        PORT_DIS(i);
                    }
                } else if(calibration_run[i].status == step_stop_setting) {
                    PORT_DIS(i);
                } else if(calibration_run[i].status == step_pause_setting) {
                    PORT_DIS(i);
                }
            }

            calibration_run[i].run_flag == 0;
        }
    }
}

