#!/bin/sh

Macaddr=`cat /sys/class/net/eth0/address | tr a-z A-Z`
FlashAvailable=`df | awk '/rootfs/{print substr($4,0)}'`
FlashUsage=`df | awk '/rootfs/{print substr($5,0)}'`
CpuModel=`cat /proc/cpuinfo | awk '/model/{print substr($4,0) " " substr($5,0) " " substr($6,0) }'`
SystemType=`cat /proc/cpuinfo | awk '/system/{print substr($4,0) " " substr($5,0) " " substr($6,0)}'`
Machine=`cat /proc/cpuinfo | awk '/machine/{print substr($3,0)}'`

echo "MacAddr:         "$Macaddr
echo "FlashAvailable:  "$FlashAvailable
echo "FlashUsage:      "$FlashUsage
echo "CpuModel:        "$CpuModel
echo "SystemType:      "$SystemType
echo "Machine:         "$Machine

Outbound='amqps://device:uWk12lmS00SnItMUzJgRd18oYi%2BfpFqmQGR%2BKbUb1%2BI%3D@iot-dev-ns.servicebus.chinacloudapi.cn/dev'
SpecId='7dfd6d63-5e8d-4380-be04-fc5c73801dfb'
#HardwareId=$Macaddr
HardwareId='test-id-123xxx'

./openiot-agent-cli -o $Outbound -i $HardwareId  -s $SpecId -r

while true
do
    MemTotal=`cat /proc/meminfo | awk '/MemTotal/{print substr($2,0)}'`
    MemFree=`cat /proc/meminfo | awk '/MemFree/{print substr($2,0)}'`
    M=$MemTotal-$MemFree
    MemUsage=$M*100/$MemTotal
    CpuUsage=`top -b -n1 | grep "CPU:" | grep -v grep | awk '{print $2 + $4}'`
    DiskUsage=`df -h | awk '{if ($6 == "/") { print $5 }}' | head -1 | cut -d'%' -f1`

    ./openiot-agent-cli -o $Outbound -i $HardwareId  -s $SpecId -m cpu.utils:10,mem.utils:47,disk.utils:2

    sleep 5

    ./openiot-agent-cli -o $Outbound -i $HardwareId  -s $SpecId -a "health.keepalive:device is alive"

    sleep 5

done
