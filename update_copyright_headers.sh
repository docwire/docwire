docker run --rm --user=$UID -v `pwd`:`pwd` -w `pwd` doctotext_build_env:latest bash -c "headache -c copyright_header.cfg -h copyright_header.txt src/*.{cpp,h} extras/*/*.{cpp,h} tests/*.cpp README"
