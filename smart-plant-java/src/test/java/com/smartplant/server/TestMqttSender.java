package com.smartplant.server;

import com.smartplant.common.MqttClientFactory;
import com.smartplant.common.PropertiesUtil;
import org.apache.commons.configuration2.Configuration;
import org.eclipse.paho.client.mqttv3.IMqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.io.File;

public class TestMqttSender {
    public static void main(String[] args) {
        Configuration config = PropertiesUtil.getConfig(new File("fennechome-mqtt-server-test.properties"));
        MqttClientFactory mqttClientFactory = new MqttClientFactory(
                config.getString("smartplant.mqtt.broker"),
                config.getString("smartplant.mqtt.user"),
                config.getString("smartplant.mqtt.pwd")
        );
        IMqttClient mqttClient = mqttClientFactory.getMqttClient();
        String topicBase = config.getString("smartplant.mqtt.devices-base-topic");

        int step = 0;
        while(true) {
            try {
//                Thread.sleep(300);
                Thread.sleep(1000);
                float tempBase = 23f;
                float sinPart = (float) (Math.sin(Math.toRadians(step++ * 3)) * 3.0f);
                mqttClient = mqttClientFactory.getMqttClient();
                String msg = "{\"t\":" + (tempBase + sinPart) + ",\"h\":59.30,\"v\":2.67,\"sid\":\"dht22-top\"}";
                mqttClient.publish(topicBase + "A0:20:A6:16:A6:34/dht22-top", new MqttMessage(msg.getBytes()));
                System.out.println("Sent: " + msg);
            } catch (Exception e) {
                e.printStackTrace();
                try {
                    mqttClient.close();
                } catch (MqttException e1) {
                    e1.printStackTrace();
                }
            }
//        mqttClient.publish(topicBase + "A0:20:A6:16:A6:34/dht22-bottom", );
//        mqttClient.publish(topicBase + "A0:20:A6:16:A7:0A/dht22-top", );
//        mqttClient.publish(topicBase + "A0:20:A6:16:A7:0A/dht22-bottom", );
        }
    }
}
