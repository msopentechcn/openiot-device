Macaddr=`cat /sys/class/net/eth0/address | tr a-z A-Z`
HardwareId=$Macaddr

Outbound='amqps://device:MpwEStFr%2fI%2bYHvKOxJTtfSFIANoJsps%2b4efvuZm6LOA%3d@iot-dev-ns.servicebus.chinacloudapi.cn/dev'
SpecId='814ba2c4-cefe-41e4-90f3-0f83b22122c8'


./openiot-agent-cli -o $Outbound -i $HardwareId  -s $SpecId -r
