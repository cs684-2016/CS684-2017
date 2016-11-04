################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
EK_TM4C123GXL.obj: ../EK_TM4C123GXL.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="/home/diptesh/ccs_workspace/pwmled_EK_TM4C123GXL_TI_TivaTM4C123GH6PM" --include_path="/opt/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b" --include_path="/opt/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages/ti/sysbios/posix" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE --display_error_number --diag_warning=225 --diag_warning=255 --diag_wrap=off --gen_func_subsections=on --preproc_with_compile --preproc_dependency="EK_TM4C123GXL.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

pwmled.obj: ../pwmled.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="/home/diptesh/ccs_workspace/pwmled_EK_TM4C123GXL_TI_TivaTM4C123GH6PM" --include_path="/opt/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b" --include_path="/opt/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages/ti/sysbios/posix" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE --display_error_number --diag_warning=225 --diag_warning=255 --diag_wrap=off --gen_func_subsections=on --preproc_with_compile --preproc_dependency="pwmled.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: ../pwmled.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"/opt/ti/xdctools_3_32_00_06_core/xs" --xdcpath="/opt/ti/tirtos_tivac_2_16_01_14/packages;/opt/ti/tirtos_tivac_2_16_01_14/products/tidrivers_tivac_2_16_01_13/packages;/opt/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages;/opt/ti/tirtos_tivac_2_16_01_14/products/ndk_2_25_00_09/packages;/opt/ti/tirtos_tivac_2_16_01_14/products/uia_2_00_05_50/packages;/opt/ti/tirtos_tivac_2_16_01_14/products/ns_1_11_00_10/packages;/opt/ti/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tiva:TM4C123GH6PM -r release -c "/opt/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path=\"/home/diptesh/ccs_workspace/pwmled_EK_TM4C123GXL_TI_TivaTM4C123GH6PM\" --include_path=\"/opt/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b\" --include_path=\"/opt/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages/ti/sysbios/posix\" --include_path=\"/opt/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include\" -g --gcc --define=ccs=\"ccs\" --define=PART_TM4C123GH6PM --define=ccs --define=TIVAWARE --display_error_number --diag_warning=225 --diag_warning=255 --diag_wrap=off --gen_func_subsections=on  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/compiler.opt: | configPkg/linker.cmd
configPkg/: | configPkg/linker.cmd


