################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../DSP28377D_common/source/F2837xD_CodeStartBranch.asm \
../DSP28377D_common/source/F2837xD_DBGIER.asm \
../DSP28377D_common/source/F2837xD_usDelay.asm 

C_SRCS += \
../DSP28377D_common/source/F2837xD_Adc.c \
../DSP28377D_common/source/F2837xD_CpuTimers.c \
../DSP28377D_common/source/F2837xD_DefaultISR.c \
../DSP28377D_common/source/F2837xD_Dma.c \
../DSP28377D_common/source/F2837xD_ECap.c \
../DSP28377D_common/source/F2837xD_EPwm.c \
../DSP28377D_common/source/F2837xD_EQep.c \
../DSP28377D_common/source/F2837xD_Emif.c \
../DSP28377D_common/source/F2837xD_Gpio.c \
../DSP28377D_common/source/F2837xD_I2C.c \
../DSP28377D_common/source/F2837xD_Ipc.c \
../DSP28377D_common/source/F2837xD_Ipc_Driver.c \
../DSP28377D_common/source/F2837xD_Ipc_Driver_Lite.c \
../DSP28377D_common/source/F2837xD_Ipc_Driver_Util.c \
../DSP28377D_common/source/F2837xD_Mcbsp.c \
../DSP28377D_common/source/F2837xD_PieCtrl.c \
../DSP28377D_common/source/F2837xD_PieVect.c \
../DSP28377D_common/source/F2837xD_Sci.c \
../DSP28377D_common/source/F2837xD_Spi.c \
../DSP28377D_common/source/F2837xD_SysCtrl.c \
../DSP28377D_common/source/F2837xD_TempSensorConv.c \
../DSP28377D_common/source/F2837xD_Upp.c \
../DSP28377D_common/source/F2837xD_can.c \
../DSP28377D_common/source/F2837xD_sci_io.c \
../DSP28377D_common/source/F2837xD_sdfm_drivers.c \
../DSP28377D_common/source/F2837xD_struct.c 

C_DEPS += \
./DSP28377D_common/source/F2837xD_Adc.d \
./DSP28377D_common/source/F2837xD_CpuTimers.d \
./DSP28377D_common/source/F2837xD_DefaultISR.d \
./DSP28377D_common/source/F2837xD_Dma.d \
./DSP28377D_common/source/F2837xD_ECap.d \
./DSP28377D_common/source/F2837xD_EPwm.d \
./DSP28377D_common/source/F2837xD_EQep.d \
./DSP28377D_common/source/F2837xD_Emif.d \
./DSP28377D_common/source/F2837xD_Gpio.d \
./DSP28377D_common/source/F2837xD_I2C.d \
./DSP28377D_common/source/F2837xD_Ipc.d \
./DSP28377D_common/source/F2837xD_Ipc_Driver.d \
./DSP28377D_common/source/F2837xD_Ipc_Driver_Lite.d \
./DSP28377D_common/source/F2837xD_Ipc_Driver_Util.d \
./DSP28377D_common/source/F2837xD_Mcbsp.d \
./DSP28377D_common/source/F2837xD_PieCtrl.d \
./DSP28377D_common/source/F2837xD_PieVect.d \
./DSP28377D_common/source/F2837xD_Sci.d \
./DSP28377D_common/source/F2837xD_Spi.d \
./DSP28377D_common/source/F2837xD_SysCtrl.d \
./DSP28377D_common/source/F2837xD_TempSensorConv.d \
./DSP28377D_common/source/F2837xD_Upp.d \
./DSP28377D_common/source/F2837xD_can.d \
./DSP28377D_common/source/F2837xD_sci_io.d \
./DSP28377D_common/source/F2837xD_sdfm_drivers.d \
./DSP28377D_common/source/F2837xD_struct.d 

OBJS += \
./DSP28377D_common/source/F2837xD_Adc.obj \
./DSP28377D_common/source/F2837xD_CodeStartBranch.obj \
./DSP28377D_common/source/F2837xD_CpuTimers.obj \
./DSP28377D_common/source/F2837xD_DBGIER.obj \
./DSP28377D_common/source/F2837xD_DefaultISR.obj \
./DSP28377D_common/source/F2837xD_Dma.obj \
./DSP28377D_common/source/F2837xD_ECap.obj \
./DSP28377D_common/source/F2837xD_EPwm.obj \
./DSP28377D_common/source/F2837xD_EQep.obj \
./DSP28377D_common/source/F2837xD_Emif.obj \
./DSP28377D_common/source/F2837xD_Gpio.obj \
./DSP28377D_common/source/F2837xD_I2C.obj \
./DSP28377D_common/source/F2837xD_Ipc.obj \
./DSP28377D_common/source/F2837xD_Ipc_Driver.obj \
./DSP28377D_common/source/F2837xD_Ipc_Driver_Lite.obj \
./DSP28377D_common/source/F2837xD_Ipc_Driver_Util.obj \
./DSP28377D_common/source/F2837xD_Mcbsp.obj \
./DSP28377D_common/source/F2837xD_PieCtrl.obj \
./DSP28377D_common/source/F2837xD_PieVect.obj \
./DSP28377D_common/source/F2837xD_Sci.obj \
./DSP28377D_common/source/F2837xD_Spi.obj \
./DSP28377D_common/source/F2837xD_SysCtrl.obj \
./DSP28377D_common/source/F2837xD_TempSensorConv.obj \
./DSP28377D_common/source/F2837xD_Upp.obj \
./DSP28377D_common/source/F2837xD_can.obj \
./DSP28377D_common/source/F2837xD_sci_io.obj \
./DSP28377D_common/source/F2837xD_sdfm_drivers.obj \
./DSP28377D_common/source/F2837xD_struct.obj \
./DSP28377D_common/source/F2837xD_usDelay.obj 

ASM_DEPS += \
./DSP28377D_common/source/F2837xD_CodeStartBranch.d \
./DSP28377D_common/source/F2837xD_DBGIER.d \
./DSP28377D_common/source/F2837xD_usDelay.d 

OBJS__QUOTED += \
"DSP28377D_common\source\F2837xD_Adc.obj" \
"DSP28377D_common\source\F2837xD_CodeStartBranch.obj" \
"DSP28377D_common\source\F2837xD_CpuTimers.obj" \
"DSP28377D_common\source\F2837xD_DBGIER.obj" \
"DSP28377D_common\source\F2837xD_DefaultISR.obj" \
"DSP28377D_common\source\F2837xD_Dma.obj" \
"DSP28377D_common\source\F2837xD_ECap.obj" \
"DSP28377D_common\source\F2837xD_EPwm.obj" \
"DSP28377D_common\source\F2837xD_EQep.obj" \
"DSP28377D_common\source\F2837xD_Emif.obj" \
"DSP28377D_common\source\F2837xD_Gpio.obj" \
"DSP28377D_common\source\F2837xD_I2C.obj" \
"DSP28377D_common\source\F2837xD_Ipc.obj" \
"DSP28377D_common\source\F2837xD_Ipc_Driver.obj" \
"DSP28377D_common\source\F2837xD_Ipc_Driver_Lite.obj" \
"DSP28377D_common\source\F2837xD_Ipc_Driver_Util.obj" \
"DSP28377D_common\source\F2837xD_Mcbsp.obj" \
"DSP28377D_common\source\F2837xD_PieCtrl.obj" \
"DSP28377D_common\source\F2837xD_PieVect.obj" \
"DSP28377D_common\source\F2837xD_Sci.obj" \
"DSP28377D_common\source\F2837xD_Spi.obj" \
"DSP28377D_common\source\F2837xD_SysCtrl.obj" \
"DSP28377D_common\source\F2837xD_TempSensorConv.obj" \
"DSP28377D_common\source\F2837xD_Upp.obj" \
"DSP28377D_common\source\F2837xD_can.obj" \
"DSP28377D_common\source\F2837xD_sci_io.obj" \
"DSP28377D_common\source\F2837xD_sdfm_drivers.obj" \
"DSP28377D_common\source\F2837xD_struct.obj" \
"DSP28377D_common\source\F2837xD_usDelay.obj" 

C_DEPS__QUOTED += \
"DSP28377D_common\source\F2837xD_Adc.d" \
"DSP28377D_common\source\F2837xD_CpuTimers.d" \
"DSP28377D_common\source\F2837xD_DefaultISR.d" \
"DSP28377D_common\source\F2837xD_Dma.d" \
"DSP28377D_common\source\F2837xD_ECap.d" \
"DSP28377D_common\source\F2837xD_EPwm.d" \
"DSP28377D_common\source\F2837xD_EQep.d" \
"DSP28377D_common\source\F2837xD_Emif.d" \
"DSP28377D_common\source\F2837xD_Gpio.d" \
"DSP28377D_common\source\F2837xD_I2C.d" \
"DSP28377D_common\source\F2837xD_Ipc.d" \
"DSP28377D_common\source\F2837xD_Ipc_Driver.d" \
"DSP28377D_common\source\F2837xD_Ipc_Driver_Lite.d" \
"DSP28377D_common\source\F2837xD_Ipc_Driver_Util.d" \
"DSP28377D_common\source\F2837xD_Mcbsp.d" \
"DSP28377D_common\source\F2837xD_PieCtrl.d" \
"DSP28377D_common\source\F2837xD_PieVect.d" \
"DSP28377D_common\source\F2837xD_Sci.d" \
"DSP28377D_common\source\F2837xD_Spi.d" \
"DSP28377D_common\source\F2837xD_SysCtrl.d" \
"DSP28377D_common\source\F2837xD_TempSensorConv.d" \
"DSP28377D_common\source\F2837xD_Upp.d" \
"DSP28377D_common\source\F2837xD_can.d" \
"DSP28377D_common\source\F2837xD_sci_io.d" \
"DSP28377D_common\source\F2837xD_sdfm_drivers.d" \
"DSP28377D_common\source\F2837xD_struct.d" 

ASM_DEPS__QUOTED += \
"DSP28377D_common\source\F2837xD_CodeStartBranch.d" \
"DSP28377D_common\source\F2837xD_DBGIER.d" \
"DSP28377D_common\source\F2837xD_usDelay.d" 

C_SRCS__QUOTED += \
"../DSP28377D_common/source/F2837xD_Adc.c" \
"../DSP28377D_common/source/F2837xD_CpuTimers.c" \
"../DSP28377D_common/source/F2837xD_DefaultISR.c" \
"../DSP28377D_common/source/F2837xD_Dma.c" \
"../DSP28377D_common/source/F2837xD_ECap.c" \
"../DSP28377D_common/source/F2837xD_EPwm.c" \
"../DSP28377D_common/source/F2837xD_EQep.c" \
"../DSP28377D_common/source/F2837xD_Emif.c" \
"../DSP28377D_common/source/F2837xD_Gpio.c" \
"../DSP28377D_common/source/F2837xD_I2C.c" \
"../DSP28377D_common/source/F2837xD_Ipc.c" \
"../DSP28377D_common/source/F2837xD_Ipc_Driver.c" \
"../DSP28377D_common/source/F2837xD_Ipc_Driver_Lite.c" \
"../DSP28377D_common/source/F2837xD_Ipc_Driver_Util.c" \
"../DSP28377D_common/source/F2837xD_Mcbsp.c" \
"../DSP28377D_common/source/F2837xD_PieCtrl.c" \
"../DSP28377D_common/source/F2837xD_PieVect.c" \
"../DSP28377D_common/source/F2837xD_Sci.c" \
"../DSP28377D_common/source/F2837xD_Spi.c" \
"../DSP28377D_common/source/F2837xD_SysCtrl.c" \
"../DSP28377D_common/source/F2837xD_TempSensorConv.c" \
"../DSP28377D_common/source/F2837xD_Upp.c" \
"../DSP28377D_common/source/F2837xD_can.c" \
"../DSP28377D_common/source/F2837xD_sci_io.c" \
"../DSP28377D_common/source/F2837xD_sdfm_drivers.c" \
"../DSP28377D_common/source/F2837xD_struct.c" 

ASM_SRCS__QUOTED += \
"../DSP28377D_common/source/F2837xD_CodeStartBranch.asm" \
"../DSP28377D_common/source/F2837xD_DBGIER.asm" \
"../DSP28377D_common/source/F2837xD_usDelay.asm" 


