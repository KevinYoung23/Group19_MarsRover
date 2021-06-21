#from pythonside import *
from query_server import *
import json
import socket
from time import sleep
#from flask import session


# def get_auto_send():
#     # return "test auto"
#     query = 'select rowid, code from auto_send order by rowid desc limit 1'
#     # return query
#     rec = query_db(query)
#     return rec


# def get_manual_send():
#     # return "test auto"
#     query = 'select rowid, code from manual_send order by rowid desc limit 1'
#     # return query
#     rec = query_db(query)[0]
#     #send_id, value = rec

#     return rec


# def a_receive(code):
#     rec = new_code('auto_receive', code)


# def m_receive(code):
#     rec = new_code('manual_receive', code)

# obstacle = True
# if code == '10101010':
#     obstacle = False

# session['obstacle'] = obstacle
# session['receive_manual_id'] = rec

# manual_obj = {
#     'receive_manual_id': session['receive_manual_id'],
#     'obstacle': session['obstacle'],
#     'receive_code': code,
# }
# return manual_obj
# return int(rec)


HOST = "0.0.0.0"
PORT = 65437
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    cur = get_cur()
    s.bind((HOST, PORT))
    s.listen()
    client, addr = s.accept()
    print("after acc")
    mode = "auto"
    with client:
        if mode == "manual":
            while True:
                while True:
                    recvdata = query_table(cur, "manual_receive")
                    senddata = query_table(cur, "manual_send")
                    print("--------get---------")
                    print(senddata)
                    send_id, sendcode = senddata
                    recv_id, recvcode = recvdata
                    if sendcode != None:
                        print(sendcode.encode())
                        if send_id > recv_id:

                            stop = '\n'
                            tosend = str(sendcode)+stop
                            client.send(tosend.encode())
                            print("------send data--------")
                            r1 = client.recv(11)
                            if r1 == b'10000000':
                                print("1000000received")
                                break
                            else:
                                print("did not recviece")
                        else:
                            print("no new send data")
                    else:
                        print("---------wait---------")
                    sleep(2)
                while True:

                    r2 = client.recv(11)
                    print("------rec0-------")
                    print(r2)
                    # if r2 == b'10101010' or r2 == b'11111111':
                    a = edit_table("manual_receive", r2.decode())
                    print(a)
                    print("-------end rec if--------")
                    sleep(2)
                    break

        elif mode == "auto":

            print("-------start auto--------")

            #content = client.recv(32)
            while True:
                r3 = query_table(cur, "auto_send")

                print(r3)
                id, sendcode = r3
                if sendcode != None:
                    # client.send(sendcode.encode())
                    print("-----------after auto send---------")
                    r4 = '\n'
                    tosend1 = str(sendcode)+r4
                    client.send(tosend1.encode())
                    done_message = client.recv(11)
                    if done_message == b'10000000':
                        print("------auto recvice send success------")
                        break
                sleep(2)
            while True:

                r5 = client.recv(11)
                print(r5)
                r6 = edit_table("auto_receive", r5.decode())

                if r5 == b'10101010':
                    print("-------- Done auto Closing connection---------")
                    client.close()
                    break
                sleep(2)

        # def make_connection():
        #     HOST = "0.0.0.0"
        #     PORT = 65433
        #     s = socket.socket()

        #     s.bind(('0.0.0.0', 65443))
        #     s.listen(0)
        #     client, addr = s.accept()
        #     mode = "manual"
        #     if mode == "manual":
        #         while True:
        #             send_data = get_manual_send()
        #             client.send("send:" + send_data.encode())
        #             print(send_data)
        #             r = client.recv(1024)
        #             print("recvc:" + r)
        #             m = m_receive
        #     return s

        # def close_connection(s):
        #     # s.close()
        #     pass

        # def send(s, data):
        #     # s.send(data)
        #     pass

        # if mode == "auto":

        #     while True:
        #         print("111")

        #         print("before")

        #         #content = client.recv(32)
        #         print("after")
        #         r = "110401201"
        #         r2 = '\n'
        #         client.send(r.encode())
        #         print("1122")
        #         client.send(r2.encode())

        #         r = client.recv(1024)
        #         print(r)
        #         client.close()
        #         print("Closing connection")
        #         if r == b'10101010':
        #             break

        # def send(data):
        #     # s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #     # s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        #     # s.bind((HOST, PORT))
        #     # s.listen(1)

        #     # conn, addr = s.accept()
        #     # with conn:
        #     #     print('Connected by', addr)
        #     #     conn.sendall(data.encode())
        #     with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        #         s.bind((HOST, PORT))
        #         s.listen(1)
        #         conn, addr = s.accept()
        #         with conn:
        #             print('Connected by', addr)
        #             while True:
        #                 data = conn.recv(1024)
        #                 if data:
        #                     break
        #                 conn.sendall(data)

        # def recv():
        #     with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        #         print('---------')
        #         print(s)
        #         print('---------')
        #         if s == None:
        #             s.connect((HOST, PORT))
        #             s.listen(0)

        #         conn, addr = s.accept()
        #         with conn:
        #             print('Connected by', addr)
        #             # while True:
        #             # data = conn.recv(1024)
        #             # if not data:
        #             #     break
        #             # conn.sendall(data.encode())
