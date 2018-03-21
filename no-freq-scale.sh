#!/bin/bash
set -e
sudo cpupower frequency-set --governor performance
$@
sudo cpupower frequency-set --governor powersave
