##
##  STM32F429i-DISCO 开发板make设置
##

##############################################################################
# 工程设置
TOPDIR  := ..
OBJS +=
#BINARY = usart_stdio
LDSCRIPT = $(TOPDIR)/buildtool/stm32f429i-discovery.ld

##############################################################################
# 芯片设置

LIBNAME		= opencm3_stm32f4
DEFS		+= -DSTM32F4

FP_FLAGS	:= -mfloat-abi=hard -mfpu=fpv4-sp-d16
ARCH_FLAGS	= -mthumb -mcpu=cortex-m4 $(FP_FLAGS)

################################################################################
# OpenOCD specific variables   OpenOCD调试器配置

OOCD		     := openocd
OOCD_INTERFACE	 := /usr/local/share/openocd/scripts/interface/stlink-v2.cfg
OOCD_TARGET	     := /usr/local/share/openocd/scripts/target/stm32f4x.cfg

################################################################################
OPENCM3_DIR := $(TOPDIR)/libopencm3

include $(TOPDIR)/buildtool/rules.mk 
