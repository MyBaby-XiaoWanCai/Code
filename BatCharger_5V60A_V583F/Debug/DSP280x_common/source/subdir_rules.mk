################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
DSP280x_common/source/%.obj: ../DSP280x_common/source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt -O0 --include_path="E:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/JC/workspace_v10/BatCharger_5V60A_V583F/APP/include" --include_path="C:/Users/JC/workspace_v10/BatCharger_5V60A_V583F/DSP280x_BSP/include" --include_path="C:/Users/JC/workspace_v10/BatCharger_5V60A_V583F/DSP280x_common/include" --include_path="C:/Users/JC/workspace_v10/BatCharger_5V60A_V583F/DSP280x_headers/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="DSP280x_common/source/$(basename $(<F)).d_raw" --obj_directory="DSP280x_common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

DSP280x_common/source/%.obj: ../DSP280x_common/source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt -O0 --include_path="E:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="C:/Users/JC/workspace_v10/BatCharger_5V60A_V583F/APP/include" --include_path="C:/Users/JC/workspace_v10/BatCharger_5V60A_V583F/DSP280x_BSP/include" --include_path="C:/Users/JC/workspace_v10/BatCharger_5V60A_V583F/DSP280x_common/include" --include_path="C:/Users/JC/workspace_v10/BatCharger_5V60A_V583F/DSP280x_headers/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="DSP280x_common/source/$(basename $(<F)).d_raw" --obj_directory="DSP280x_common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


