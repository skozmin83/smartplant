package com.smartplant.common;

import com.mongodb.async.client.MongoClient;
import com.mongodb.async.client.MongoClients;
import com.mongodb.async.client.MongoCollection;
import com.mongodb.async.client.MongoDatabase;
import org.apache.commons.configuration2.Configuration;
import org.bson.Document;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.HashMap;
import java.util.Map;

public class MongoAsyncStorage implements AutoCloseable {
    private final Logger logger = LoggerFactory.getLogger(getClass());
    private final Map<String, MongoCollection<Document>> collections = new HashMap<>();
    private final MongoClient mongoClient;
    private final MongoDatabase database;

    public MongoAsyncStorage(Configuration config) {
        mongoClient = MongoClients.create(config.getString("smartplant.mongo.connection-string"));
        database = mongoClient.getDatabase(config.getString("smartplant.mongo.database-name"));
    }

    public void store(String store, Document deviceInfo) {
        getCollection(store).insertOne(deviceInfo, (result, t) -> {
            if (t != null) {
                logger.info("Unable to write a message: {}", deviceInfo, t);
            }
        });
    }

    public void load(String collection, Loader loader) {
        loader.load(getCollection(collection));
    }

    private MongoCollection<Document> getCollection(String store) {
        return collections.computeIfAbsent(store, database::getCollection);
    }

    @Override
    public void close() {
        mongoClient.close();
    }

    public interface Loader {
        void load(MongoCollection<Document> collection);
    }
}