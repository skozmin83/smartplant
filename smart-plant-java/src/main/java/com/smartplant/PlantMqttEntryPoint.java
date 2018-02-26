package com.smartplant;

import com.smartplant.common.PlantMqttEventSource;
import com.smartplant.common.IPlantEventSource;
import com.smartplant.common.MqttClientFactory;
import com.smartplant.common.PropertiesUtil;
import com.smartplant.server.PlantMqttServer;
import com.smartplant.common.MongoAsyncStorage;
import org.apache.commons.configuration2.Configuration;
import org.bson.Document;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.util.Collections;
import java.util.Date;

public class PlantMqttEntryPoint {
    private static final Logger logger = LoggerFactory.getLogger(PlantMqttEntryPoint.class);

    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Usage java " + PlantMqttEntryPoint.class.getSimpleName() + " <config-name> ");
            System.exit(-1);
        }
        try {
            logger.info("Starting Plant MongoDB and MQTT server with config [" + args[0] + "]. ");
            Configuration config = PropertiesUtil.getConfig(new File(args[0]));

            String collection = config.getString("smartplant.mongo.sensor.mass.collection");
            MongoAsyncStorage storage = new MongoAsyncStorage(config);
//
            MqttClientFactory mqttClientFactory
                    = new MqttClientFactory(config.getString("smartplant.mqtt.controller.broker"),
                                            config.getString("smartplant.mqtt.controller.user"),
                                            config.getString("smartplant.mqtt.controller.pwd"));
            PlantMqttServer server = new PlantMqttServer(Collections.emptyList(), config);
            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                try {
                    mqttClientFactory.close();
                    storage.close();
                    server.close();
                } catch (Exception e) {
                    logger.error("Unable to un-initialize MongoDB and MQTT server. ", e);
                }
            }));
            server.start();

            // subscribe to mqtt
            IPlantEventSource mqttEventSource = new PlantMqttEventSource(mqttClientFactory);
            String deviceTopicBase = config.getString("smartplant.mqtt.devices-base-topic");
            mqttEventSource.subscribe(deviceTopicBase + "#", (topic, msg, ts) -> {
                Document json = Document.parse(new String(msg));
                json = json.append("id", topic.substring(deviceTopicBase.length(), topic.length()));
                json = json.append("ts", ts);
                json = json.append("time", new Date(ts));
                storage.store(collection, json);
            });

            logger.info("Broker started. ");
        } catch (Throwable t) {
            t.printStackTrace();
            System.exit(-2);
        }
    }

}
