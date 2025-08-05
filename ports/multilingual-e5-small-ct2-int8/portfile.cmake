vcpkg_from_git(
	OUT_SOURCE_PATH SOURCE_PATH
	URL https://huggingface.co/jncraton/multilingual-e5-small-ct2-int8
	REF a0049e03b121150611d7401f11ed7970edc47575
	LFS
)

vcpkg_download_distfile(
	TOKENIZER_MODEL_PATH
    URLS "https://huggingface.co/intfloat/multilingual-e5-small/resolve/d2648e288f5fe1641aeab663a7fa6d1f0d1daff2/sentencepiece.bpe.model"
    FILENAME "spiece.model"
    SHA512 5b4fb04e65d2371cd83fb173a35126e49d2401d1f19f98c981c07dd7f55e8cc32853d804ba8fbc8c8ed6cdbed010bea09975b69f11ac59dd5057ae1dced5d35c
)

vcpkg_download_distfile(
	LICENSE_PATH
	URLS "https://raw.githubusercontent.com/microsoft/unilm/0e31c7c09737df491e7ff74ded19614b884c52b4/LICENSE"
	FILENAME LICENSE-MIT-MS-UNILM.txt
	SHA512 f32e03ca8847c2f16226377644cfd561bed53fe608484a755dd39909265834918c25f8b600b735617fd15caeab41781176c5b17d0fedfa906a3df5b15eb3a922
)

file(INSTALL ${SOURCE_PATH}/ DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} FILES_MATCHING PATTERN "*" PATTERN ".gitattributes" EXCLUDE)
file(INSTALL ${TOKENIZER_MODEL_PATH} DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})
vcpkg_install_copyright(FILE_LIST ${LICENSE_PATH})

file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/include/${PORT})
file(TOUCH ${CURRENT_PACKAGES_DIR}/include/${PORT}/placeholder)
