FROM alpine:latest

RUN apk add --no-cache \
	clang \
	build-base \
	libstdc++

WORKDIR /webserv/

COPY ./resources ./resources
COPY ./src ./src
COPY makefile makefile

RUN make -j

WORKDIR /webserv/build

# executable
ENTRYPOINT ["./not-apache"]

# default arguments. can easily be changed with docker run or another docker file
CMD ["-c", "-v", "-f", "../resources/example-configs/basic.conf"]