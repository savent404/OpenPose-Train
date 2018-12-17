# Test
Use Unix `nc` to send udp message to udpServer

# Test Step

- `cd` to root dir
- Run udpServer in a cmd:
    ``` shell
    python udpServer.py
    ```
- In another cmd, send HandShake msg first
    ``` shell
    cat test/handShakeMsg | nc -u -w1 localhost 9000
    ```
- Server will send a ack to client's port:9001
- send Start transmit msg
    ``` shell
    cat test/startJsonTranmit | nc -u -w1 localhost 9000
    ```
- Server will send a ack to client's port:9001 with a random udp port, client can notice that port number from the ack message. And we now use the server's output log to located the port number.
- send specific file
    ``` shell
    cat JsonContext | nc -u -w1 localhost <specific port>
    ```