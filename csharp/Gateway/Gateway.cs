using Microsoft.OpenIoT.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Microsoft.OpenIoT
{

    class Gateway
    {

        AMQPSender<string> _sender;

        public Gateway(string amqpsAddress, string eventHubName)
        {
            _sender = new AMQPSender<string>(amqpsAddress, eventHubName);
        }

        public void Register(string hardwareId, string specificationToken)
        {
            byte[] payload = Encoder.Register(hardwareId, specificationToken);
            _sender.SendSerialized(payload);
        }

        public void Measurement(string hardwareId, string name, double value)
        {
            byte[] payload = Encoder.Measurement(hardwareId, name, value);
            _sender.SendSerialized(payload);
        }

        public void Alert(string hardwareId, string alertType, string message)
        {
            byte[] payload = Encoder.Alert(hardwareId, alertType, message);
            _sender.SendSerialized(payload);

        }

        public void Location(string hardwareId, double latitude, double longtitude, double elevation)
        {
            byte[] payload = Encoder.Location(hardwareId, latitude, longtitude, elevation);
            _sender.SendSerialized(payload);

        }

        public void Close()
        {
            this._sender.Close();
        }

        public static void Main(String[] args){
            string amqpsAddress = "amqps://yourSasName:yourSasKey@yourNamespace.servicebus.chinacloudapi.cn";
            string eventHubName = "yourEventHubName";

            Gateway gateway = new Gateway(amqpsAddress, eventHubName);

            //gateway.Register("test-id-csharp2", "d2604433-e4eb-419b-97c7-88efe9b2cd41");
            //gateway.Alert("test-id-csharp2", "keepalive","csharp client is alive!");
            //gateway.Measurement("test-id-csharp2", "cpu.utils", 23.5);
            gateway.Location("test-id-csharp2", 176.223223f, 54.234323f, 1.0012323f);
            gateway.Close();
            

        }

    }
}
