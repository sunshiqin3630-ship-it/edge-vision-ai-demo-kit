#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXAMPLE_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
REPO_DIR="$(cd "${EXAMPLE_DIR}/../.." && pwd)"

ENV_FILE="${1:-${EXAMPLE_DIR}/config/board.env.example}"
if [[ -f "${ENV_FILE}" ]]; then
  # shellcheck disable=SC1090
  source "${ENV_FILE}"
fi

: "${BOARD_QT_MAJOR:=auto}"
: "${BOARD_SYSROOT:=/path/to/board/sysroot}"
: "${BOARD_QT_CMAKE_PREFIX:=/path/to/board/sysroot/usr/lib/cmake}"
: "${BOARD_TOOLCHAIN_PREFIX:=aarch64-linux-gnu-}"

cmake -S "${EXAMPLE_DIR}" -B "${REPO_DIR}/build/qt-vision-viewer-board" \
  -DCMAKE_TOOLCHAIN_FILE="${EXAMPLE_DIR}/cmake/board-linux-toolchain.example.cmake" \
  -DEDGE_VIEWER_QT_MAJOR="${BOARD_QT_MAJOR}" \
  -DEDGE_VIEWER_ENABLE_FFMPEG=ON \
  -DEDGE_VIEWER_ENABLE_V4L2=ON \
  -DCMAKE_PREFIX_PATH="${BOARD_QT_CMAKE_PREFIX}"

cmake --build "${REPO_DIR}/build/qt-vision-viewer-board"
