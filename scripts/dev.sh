#!/bin/bash
# dev.sh — build MXTune and relaunch AudioPluginHost
#
# First-time setup:
#   1. cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
#   2. cmake --build build --target AudioPluginHost --parallel
#   3. Open AudioPluginHost, load MXTune, route audio, save as dev/mxtune_dev.filtergraph
#   Then use this script for all subsequent iterations.
#
# Usage: ./scripts/dev.sh [au]
#   (no args)  builds VST3
#   au         builds AU (also re-registers the component)

set -e

FORMAT="${1:-vst3}"
BUILD_DIR="$(dirname "$0")/../build"

if [ "$FORMAT" = "au" ]; then
    cmake --build "$BUILD_DIR" --target MXTune_AU --parallel
    killall -9 AudioComponentRegistrar 2>/dev/null || true
else
    cmake --build "$BUILD_DIR" --target MXTune_VST3 --parallel
fi

pkill -x "AudioPluginHost" 2>/dev/null || true
sleep 0.5
open -a AudioPluginHost "$(dirname "$0")/../dev/mxtune_dev.filtergraph"
