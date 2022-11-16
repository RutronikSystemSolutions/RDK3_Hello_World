################################################################################
# \file RDK3.mk
#
# \brief
# Define the RDK3 target.
#
################################################################################
# \copyright
# Copyright 2018-2021 Cypress Semiconductor Corporation (an Infineon company) or
# an affiliate of Cypress Semiconductor Corporation
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

ifeq ($(WHICHFILE),true)
$(info Processing $(lastword $(MAKEFILE_LIST)))
endif

# Set the default build recipe for this board if not set by the user
include $(dir $(lastword $(MAKEFILE_LIST)))/locate_recipe.mk

# MCU device selection
#    Changing the device should be done using "make bsp" or "make update_bsp" with the "DEVICE_GEN"
#    variable set to the new MCU. If you change the device manually here you must also update the
#    design.modus file and re-run the device configurator.
DEVICE:=CYB06447BZI-BLD53
# Default target core to CM4 if not already set
CORE?=CM4
# Basic architecture specific components
COMPONENTS+=$(TARGET) CAT1 CAT1A
# Define default type of bootloading method [single, dual]
SECURE_BOOT_STAGE?=single

ifeq ($(CORE),CM4)
# Additional components supported by the target
# single -> CM4 only, dual -> CM0 and CM4
COMPONENTS+=BSP_DESIGN_MODUS PSOC6HAL
#Add secure CM0P image in single stage
ifeq ($(SECURE_BOOT_STAGE), single)
COMPONENTS+=CM0P_SECURE
endif
# Use CyHAL
DEFINES+=CY_USING_HAL

ifeq ($(SECURE_BOOT_STAGE),single)
CY_LINKERSCRIPT_SUFFIX=cm4_dual
CY_SECURE_POLICY_NAME?=policy_single_CM0_CM4
else
CY_LINKERSCRIPT_SUFFIX=cm4
CY_SECURE_POLICY_NAME?=policy_multi_CM0_CM4
endif

else
CY_SECURE_POLICY_NAME=policy_multi_CM0_CM4
endif

#Define the toolchain path
ifeq ($(TOOLCHAIN),ARM)
TOOLCHAIN_PATH=$(CY_COMPILER_ARM_DIR)
else
TOOLCHAIN_PATH=$(CY_COMPILER_GCC_ARM_DIR)
endif

# Python path definition
CY_PYTHON_REQUIREMENT=true

# Check if CM0P Library exists
POST_BUILD_CM0_LIB_PATH=$(call CY_MACRO_FINDLIB,psoc6cm0p)
ifeq ($(POST_BUILD_CM0_LIB_PATH), NotPresent)
# Backward compatibility, try hard-coded paths instead
POST_BUILD_CM0_LIB_PATH=$(CY_INTERNAL_APPLOC)/libs/psoc6cm0p/COMPONENT_CM0P_SECURE
endif


POST_BUILD_BSP_LIB_PATH_INTERNAL=$(CY_TARGET_DIR)
# Check if Target BSP Library exists
#POST_BUILD_BSP_LIB_PATH_INTERNAL=$(call CY_MACRO_FINDLIB,TARGET_RDK3)
#ifeq ($(POST_BUILD_BSP_LIB_PATH_INTERNAL), NotPresent)
# Backward compatibility, try hard-coded paths instead
#POST_BUILD_BSP_LIB_PATH_INTERNAL=$(CY_TARGET_DIR)/TARGET_RDK3
#endif

ifeq ($(OS),Windows_NT)
ifneq ($(CY_WHICH_CYGPATH),)
POST_BUILD_BSP_LIB_PATH=$(shell cygpath -m --absolute $(POST_BUILD_BSP_LIB_PATH_INTERNAL))
else
POST_BUILD_BSP_LIB_PATH=$(abspath $(POST_BUILD_BSP_LIB_PATH_INTERNAL))
endif
else
POST_BUILD_BSP_LIB_PATH=$(abspath $(POST_BUILD_BSP_LIB_PATH_INTERNAL))
endif

# BSP-specific post-build action
CY_BSP_POSTBUILD=$(CY_PYTHON_PATH) $(POST_BUILD_BSP_LIB_PATH)/psoc64_postbuild.py \
				--core $(CORE) \
				--secure-boot-stage $(SECURE_BOOT_STAGE) \
				--policy $(CY_SECURE_POLICY_NAME) \
				--target cyb06xx7 \
				--toolchain-path $(TOOLCHAIN_PATH) \
				--toolchain $(TOOLCHAIN) \
				--build-dir $(CY_CONFIG_DIR) \
				--app-name $(APPNAME) \
				--cm0-app-path $(POST_BUILD_CM0_LIB_PATH) \
				--cm0-app-name psoc6_01_cm0p_secure