# This script is just a template and has to be copied and modified per project
# This script should be called from .vscode/tasks.json with
#
#   scripts/Build-Release.ps1            - for Beta builds
#   scripts/Build-Release.ps1 Release    - for Release builds
#
# {
#     "label": "Build-Release",
#     "type": "shell",
#     "command": "scripts/Build-Release.ps1 Release",
#     "args": [],
#     "problemMatcher": [],
#     "group": "test"
# },
# {
#     "label": "Build-Beta",
#     "type": "shell",
#     "command": "scripts/Build-Release.ps1 ",
#     "args": [],
#     "problemMatcher": [],
#     "group": "test"
# }



# set product names, allows mapping of (devel) name in Project to a more consistent name in release
# $settings = scripts/OpenKNX-Build-Settings.ps1

# execute generic pre-build steps
lib/OGM-Common/scripts/setup/reusable/Build-Release-Preprocess.ps1 $args[0]
if (!$?) { exit 1 }

# build firmware based on generated headerfile 
# the following build steps are project specific and must be adopted accordingly
# see comment in Build-Step.ps1 for argument description

# Example call, the following 2 lines might be there multiple times for each firmware which should be built
# ../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_REG1_ETH SmartHomeBridge-Eth uf2
# if (!$?) { exit 1 }


../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OpenKNX_UP1_GW_2XUART firmware-Aircondition-OpenKNX-UP1-GW-2xUART uf2
if (!$?) { exit 1 }

../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_OpenKNX_UP1_WLAN_2XUART firmware-Aircondition-OpenKNX-UP1-WLAN-2xUART esp32
if (!$?) { exit 1 }

 #../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_Adafruit_Feather_ESP32_V2_TP firmware-Aircondition-Adafruit_Feather_ESP32_V2_TP esp32
 #if (!$?) { exit 1 }

 #../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_Adafruit_Feather_ESP32_V2_IP firmware-Aircondition-Adafruit_Feather_ESP32_V2_IP esp32
 #if (!$?) { exit 1 }

 #../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_AZDelivery_DevKit_C_V4_TP firmware-Aircondition-AZDelivery_DevKit_C_V4_TP esp32
 #if (!$?) { exit 1 }

 #../OGM-Common/scripts/setup/reusable/Build-Step.ps1 release_AZDelivery_DevKit_C_V4_IP firmware-Aircondition-AZDelivery_DevKit_C_V4_IP esp32
 #if (!$?) { exit 1 }


# execute generic post-build steps
lib/OGM-Common/scripts/setup/reusable/Build-Release-Postprocess.ps1 $args[0]
if (!$?) { exit 1 }