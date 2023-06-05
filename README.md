# 실시간 암호화폐 정보 확인 및 채팅 프로그램 서비스

### 201820661 사이버보안학과 이응창

## Python3 Prerequisite
> ### PiP
> ```
> pip3 install selenium
> pip3 install -U urllib3 requests

## Chrome Driver Prerequisite
> ### URL
> ```
> https://sites.google.com/chromium.org/driver/
> 경로에 맞게 driver 설정필요

## How to complie
> ### Server
> ```
> $ gcc -o server server.c -lpthread

> ### Client
> ```
> $ gcc -o client client.c -lpthread


## How to execute
> ### Server
> ```
> $ ./server <PORT>

> ### Client
> ```
> $ ./client <Server IP> <PORT>
