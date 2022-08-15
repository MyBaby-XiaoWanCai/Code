################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
DSP28377D_common/C_lib/%.obj: ../DSP28377D_common/C_lib/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="E:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/JC/workspace_v10/HV_DCDC_RecentTest/DSP28377D_BSP/include" --include_path="C:/Users/JC/workspace_v10/HV_DCDC_RecentTest/DSP28377D_common/include" --include_path="C:/Users/JC/workspace_v10/HV_DCDC_RecentTest/DSP28377D_headers/include" --include_path="C:/Users/JC/workspace_v10/HV_DCDC_RecentTest/DSP28377D_common" --define=CPU1 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="DSP28377D_common/C_lib/$(basename $(<F)).d_raw" --obj_directory="DSP28377D_common/C_lib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


