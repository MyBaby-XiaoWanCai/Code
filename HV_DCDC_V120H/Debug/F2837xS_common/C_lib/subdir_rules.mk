################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
F2837xS_common/C_lib/%.obj: ../F2837xS_common/C_lib/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --fp_mode=relaxed --include_path="F:/ccs/HV_DCDC_V120H/F2837xS_BSP/include" --include_path="F:/ccs/HV_DCDC_V120H/F2837xS_common" --include_path="F:/ccs/HV_DCDC_V120H/F2837xS_common/include" --include_path="F:/ccs/HV_DCDC_V120H/F2837xS_headers/include" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_20.2.1.LTS/include" --include_path="F:/ccs/HV_DCDC_V120H/APP/include" --advice:performance=all --define=CPU1 --define=CLA_C --define=_FLASH -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="F2837xS_common/C_lib/$(basename $(<F)).d_raw" --obj_directory="F2837xS_common/C_lib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


