#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#####################################################
# Camada Física da Computação
# Prof. Rafael Corsi
#  Abril/2017
#  Aplicação
#
# [ref 1] https://docs.python.org/2/library/socket.html
####################################################


import socket
import sys

message_led_on      = "Turn Led ON ! \n"
message_led_on_ack  = "Led Powered ON ! \n"
message_led_off     = "Turn Led OFF ! \n"
message_led_off_ack = "Led Powered OFF \n"
message_led_status  = "Led Status ? \n"
message_erro        = "O sapo nao lava o pe \n"
message_erro_ack    = "Erro erro erro \n"

def commandTx(s,message):
    print(message)
    s.sendall(message)

def commandRx(s):
    data  = s.recv(128)
    print(data)

def main():
    #create an INET, STREAMing socket
    s = socket.socket(
        socket.AF_INET, socket.SOCK_STREAM)
    # conecta no embarcado
    # - the normal http port
    s.connect(("192.168.0.106", 6666))

    while True:
        command = input(' 1: Led On \n 2: Led Off \n 3: Led Status \n 4: Command error \n ')
        print(command)
        if command == 1 :
            commandTx(s,message_led_on)
            commandRx(s)
        elif command == 2 :
            commandTx(s,message_led_off)
            commandRx(s)
        elif command == 3 :
            commandTx(s,message_led_status)
            commandRx(s)
        elif command == 4 :
            commandTx(s,message_erro)
            commandRx(s)

    # Look for the response
    amount_received = 0
    amount_expected = len(message_ack)

    while amount_received < amount_expected:
        data = s.recv(16)
        amount_received += len(data)
        print("RX : {}".format(data))

if __name__ == "__main__":
    main()
