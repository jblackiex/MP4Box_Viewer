#!/bin/bash

YELLOW='\033[1;33m'
BOLD='\033[1m'
GREEN='\033[1;32m'
DEF_COLOR='\033[0m'

#############################

clear
for i in 1 2 3 ; do \
    echo -e "${YELLOW}📀Welcome to MP4Box Viewer📀${DEF_COLOR}" \
    && sleep 0.4 \
    && clear \
    && echo -e "${GREEN}📀Welcome to MP4Box Viewer📀${DEF_COLOR}" \
    && sleep 0.4 \
    && clear ; done
echo "READY"
sleep 0.3
echo "-"
sleep 0.2
echo "--"
sleep 0.2
echo "---"
sleep 0.5
clear
echo -e "${BOLD}./mp4box created by ${USER}${DEF_COLOR}"
sleep 1.2
clear
