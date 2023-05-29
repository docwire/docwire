FROM amd64/debian:bullseye

RUN apt-get update #20211001

RUN echo "Europe/Warsaw" > /etc/timezone && dpkg-reconfigure -f noninteractive tzdata

RUN apt-get install -y locales
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
	sed -i -e 's/# pl_PL.UTF-8 UTF-8/pl_PL.UTF-8 UTF-8/' /etc/locale.gen && \
	dpkg-reconfigure -f noninteractive locales && \
	update-locale LANG=pl_PL.UTF-8
ENV LANG pl_PL.UTF-8

RUN apt-get install -y cmake
RUN apt-get install -y wine
