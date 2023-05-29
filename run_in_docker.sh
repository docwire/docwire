docker run --rm --user=$UID -v `pwd`:`pwd` -w `pwd` doctotext_build_env:latest bash -c "cd build && ./text_extractor.sh $1 $2 $3"
