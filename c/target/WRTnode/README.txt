Installation Guide:

1. Copy openiot-agent to WRTnode

2. Install openiot-agent
 
#opkg install openiot-agent_1.0-1_ramips_24kec.ipk

3. Config initd to start openiot-agent at system startup

#/etc/init.d/openiot-agent enable
#reboot

4. Check on portal, config device comments to the room number

5. Check heatbeat works

#tail -f /var/log/openiot-agent.log
on portal:
check 'heatbeat' signal, and ticks numbers


Maintainence Guide:

* Stop openiot-agent

#/etc/init.d/openiot-agent stop

* Restart openiot-agent

#/etc/init.d/openiot-agent restart

* Agent related files
- /etc/init.d/openiot-agent: init file
- /usr/lib/qpid*.so.*: ampq protocol client
- /usr/bin/openiot-agent.bin: agent executable
- /usr/bin/openiot-agent.sh: agent script to call the agent executable
