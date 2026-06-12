#!/usr/bin/env bash
set -e

# Configurable defaults

MODEL_NAME="${MODEL_NAME:-qwen2-7b-instruct}"
MODEL_QUANT="${MODEL_QUANT:-q4_k_m}"
MODEL_REPO="${MODEL_REPO:-Qwen/Qwen2-7B-Instruct-GGUF}"

# Derived values
MODEL_FILE="${MODEL_NAME}.${MODEL_QUANT}.gguf"
OUTPUT_DIR="${OUTPUT_DIR:-models}"
HF_URL="https://huggingface.co/${MODEL_REPO}/resolve/main/${MODEL_FILE}"

# Checks
if ! command -v wget &> /dev/null && ! command -v curl &> /dev/null; then
    echo "Error: Neither wget nor curl is installed."
    exit 1
fi

mkdir -p "${OUTPUT_DIR}"
cd "${OUTPUT_DIR}"

if [ -f "${MODEL_FILE}" ]; then
    echo "Model already exists: ${OUTPUT_DIR}/${MODEL_FILE}"
    exit 0
fi

echo "Downloading model:"
echo "  Repository : ${MODEL_REPO}"
echo "  File       : ${MODEL_FILE}"
echo "  Destination: ${OUTPUT_DIR}"
echo ""

# Download

if command -v wget &> /dev/null; then
    wget -c "${HF_URL}"
else
    curl -L -C - -o "${MODEL_FILE}" "${HF_URL}"
fi

echo ""
echo "Download complete."
echo "Model saved to: ${OUTPUT_DIR}/${MODEL_FILE}"
