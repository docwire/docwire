set(MODEL_NAME "granite-4.0-1b")
set(MODEL_QUANT "Q8_0")

set(MODEL_FILE "${MODEL_NAME}-${MODEL_QUANT}.gguf")

vcpkg_download_distfile(
    MODEL_ARCHIVE
    URLS "https://huggingface.co/ibm-granite/granite-4.0-1b-GGUF/resolve/main/${MODEL_FILE}"
    FILENAME "${MODEL_FILE}"
    SHA512 d5a25ed70bcedfed17b318ff53fb4fc9d122dd7ca6309a8b936906587e9482c13dab7708b6f198a129200aa575a24609fc6efab6caf513b1c47d032ea33e9571
)

file(INSTALL
    ${MODEL_ARCHIVE}
    DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT}
)

file(WRITE
    ${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright
    "Model weights from HuggingFace repository ibm-granite/granite-4.0-1b-GGUF."
)
