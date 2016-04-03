require "serialport"
port_str = "/dev/cu.usbmodem1411"
baud_rate = 9600
data_bits = 8
stop_bits = 1
parity = SerialPort::NONE

sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)
sleep(2)

while 1
  hour = format('%02d', Time.now.hour)
  minute = format('%02d', Time.now.min)
  second = format('%02d', Time.now.sec)
  day = format('%02d', Time.now.day)
  month = format('%02d', Time.now.month)
  #sp.write("#{day}")
  sp.write("#{hour}:#{minute}:#{second}:#{day}:#{month}:#{Time.now.year}")
  sleep(1)
end
