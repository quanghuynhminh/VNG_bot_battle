/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.android.bluetoothlegatt;

import java.util.HashMap;

/**
 * This class includes a small subset of standard GATT attributes for demonstration purposes.
 */
public class SampleGattAttributes {
    private static HashMap<String, String> attributes = new HashMap();
    public static String CLIENT_CHARACTERISTIC_CONFIG = "00002902-0000-1000-8000-00805f9b34fb";

    //Sensors: 1 services gồm 4 characteristics
    public static String sensor_service_uuid                = "0000FFF6-0000-1000-8000-00805F9B34FB";
    public static String sensor_service_distance_char_uuid  = "0000FFF7-0000-1000-8000-00805F9B34FB";   //NOTIFY-uchar
    public static String sensor_service_left_char_uuid      = "0000FFF8-0000-1000-8000-00805F9B34FB";   //NOTIFY-uchar
    public static String sensor_service_right_char_uuid     = "0000FFF9-0000-1000-8000-00805F9B34FB";   //NOTIFY-uchar
    public static String sensor_service_behind_char_uuid    = "0000FFFA-0000-1000-8000-00805F9B34FB";   //NOTIFY-uchar

    static {
        attributes.put(sensor_service_uuid, "VNG_BOT_SENSORS_SERVICE");

        // Sample Characteristics.
        attributes.put(sensor_service_distance_char_uuid, "Distance Sensor");
        attributes.put(sensor_service_left_char_uuid, "Left Sensor");
        attributes.put(sensor_service_right_char_uuid, "Right Sensor");
        attributes.put(sensor_service_behind_char_uuid, "Behind Sensor");

    }

    public static String lookup(String uuid, String defaultName) {
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }
}
