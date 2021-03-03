# 42-webserv
Webserver made from scratch in c++

## todo
- [ ] config parser (regexp)
- [ ] webserver
- [ ] cgi handler
- [ ] plugin hooks
- [ ] plugins (5)
- [ ] template language parser
- [ ] terminal commands

## how it works
1. create listener for all ports
2. create worker pool
3. socket listener:
    1. accept
    2. read until EOF or timeout
    3. handle input