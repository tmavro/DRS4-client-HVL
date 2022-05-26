import datetime as dt
import serial.tools.list_ports
import cts_connection_serial as connect

ports = list(serial.tools.list_ports.comports())
for p in ports:
    print(p)
    if 'MOXA USB Serial Port' in p.description:
        port = p
        print('Connecting...')

cts = connect.Connection(port=port.device, baud=19200, timeout=0.25)
response = cts.read_set_temp_and_actual()
print('Set temp:', response[0], '  Actual temp:', response[1])


k = 0

temp_list1=[5, 10, 15, 20, 25, 30, 35, 40, 45]
temp_list2=[50, 45, 40, 35, 30, 25, 20, 15, 10]

hourly = dt.datetime.now()
minute = dt.datetime.now()

f = open("bachelorbrostemp.txt", "a")
f.write("date,set_temp,acc_temp\n")
f.close()

while True:
    while k < len(temp_list1):
        if dt.datetime.now() > minute:
            minute = dt.datetime.now() + dt.timedelta(minutes=1)
            f = open("bachelorbrostemp.txt", "a")
            f.close()

        if dt.datetime.now() > hourly:
            f = open("bachelorbrostemp.txt", "a")
            hourly = dt.datetime.now() + dt.timedelta(minutes=120)
            cts.set_temp(temp_list1[k])
            print("Setting temperature to", temp_list1[k], "degrees C at", dt.datetime.now())
            f.write(str(dt.datetime.now()))
            f.write(",")
            f.write(str(cts.read_set_temp_and_actual()[0]))
            f.write(",")
            f.write(str(cts.read_set_temp_and_actual()[1]))
            f.write("\n")
            k += 1
            f.close()
    k = 0

    while k < len(temp_list2):
        if dt.datetime.now() > minute:
            minute = dt.datetime.now() + dt.timedelta(minutes=1)
            f = open("bachelorbrostemp.txt", "a")
            f.close()

        if dt.datetime.now() > hourly:
            f = open("bachelorbrostemp.txt", "a")
            hourly = dt.datetime.now() + dt.timedelta(minutes=120)
            cts.set_temp(temp_list2[k])
            print("Setting temperature to", temp_list2[k], "degrees C at", dt.datetime.now())
            f.write(str(dt.datetime.now()))
            f.write(",")
            f.write(str(cts.read_set_temp_and_actual()[0]))
            f.write(",")
            f.write(str(cts.read_set_temp_and_actual()[1]))
            f.write("\n")
            k += 1
            f.close()
    k=0

