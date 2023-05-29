docker run --rm -it --user=$UID -v `pwd`:`pwd` -w `pwd` doctotext_build_env:latest bash -c "cd build && LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH valgrind ./text_extractor $1 $2 $3"
