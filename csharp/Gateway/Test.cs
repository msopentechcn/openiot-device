using Amqp;
using Amqp.Framing;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Microsoft.OpenIoT
{
    class Test
    {

        /////////////////  Below information should be customized  /////////////////

        private static string uniqueClientId = "[YourUniqueClientId]";

        private static string receiveAmqpsAddress = "amqps://[sasName]:[sasKey]@[namespace].servicebus.chinacloudapi.cn";
        private static string receiveEntityPath = "/[topicName]/Subscriptions/[subscriptionName]";

        private static string sendAmqpsAddress = "amqps://[sasName]:[sasKey]@[namespace].servicebus.chinacloudapi.cn";
        private static string sendEntityPath = "/[entityPath]";

        // Unique device id, eg. Mac address or UUID
        private static string deviceId = "[YourDeviceId]]";
        // Unique device specification ID, assigned by server admin
        private static string specificationId = "[YourSpecificationId]]";

        ////////////////////// End of customized information //////////////////////

        public static void testReceiver()
        {
            string amqpsAddress = receiveAmqpsAddress;
            string entityName = receiveEntityPath;
            AMQPReceiver<string> r = new AMQPReceiver<string>(amqpsAddress, entityName);
            r.Open(uniqueClientId);
            r.Start();
        }

        public static void testSenderAPI(string alertLevel, string alertMessage)
        {
            string amqpsAddress = sendAmqpsAddress;
            string entityPath = sendEntityPath;

            Gateway gateway = new Gateway(amqpsAddress, entityPath);

            gateway.Register(deviceId, specificationId);
            gateway.Alert(deviceId, alertLevel, alertMessage);
            //gateway.Measurement(deviceId, "cpu.utils", 23.5);
            //gateway.Location(deviceId, 176.223223f, 54.234323f, 1.0012323f);

            gateway.Close();

        }

        public static void testMessageSend(string textMessage)
        {
            string amqpsAddress = sendAmqpsAddress;
            string entityPath = sendEntityPath;

            var amqpConnection = new Connection(new Address(amqpsAddress));

            var amqpSession = new Session(amqpConnection);

            SenderLink amqpSender = new SenderLink(amqpSession,
                   uniqueClientId,       // unique name for all links from this client 
                   entityPath);          // Service Bus entity path 

            var message = new Message();
            message.ApplicationProperties = new ApplicationProperties();
            message.ApplicationProperties[Constants.PayloadKey] = textMessage;

            amqpSender.Send(message);

        }

        public static void Main(String[] args)
        {
            testReceiver();   
        }
    }
}
