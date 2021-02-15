﻿#include "json_reader.h"
#include "map_renderer.h"
#include "svg.h"
#include "test_example_functions.h"

#include <iostream>
#include <sstream>
#include <cassert>

using namespace std::literals;

int main() {
    json_reader::JsonReader js_reader;
    js_reader.Start(std::cin, std::cout);
    //TestTransportCatalogue();
}

/*
{
    "base_requests": [
    {
        "type": "Bus",
            "name" : "114",
            "stops" : ["Morskoy vokzal", "Riverskiy most"] ,
            "is_roundtrip" : false
    },
    {
      "type": "Stop",
      "name" : "Riverskiy most",
      "latitude" : 43.587795,
      "longitude" : 39.716901,
      "road_distances" : {"Morskoy vokzal": 850}
    },
    {
      "type": "Stop",
      "name" : "Morskoy vokzal",
      "latitude" : 43.581969,
      "longitude" : 39.719848,
      "road_distances" : {"Riverskiy most": 850}
    }
    ],
        "stat_requests": [
    { "id": 1, "type" : "Stop", "name" : "Riverskiy most" },
    { "id": 2, "type" : "Bus", "name" : "114" }
        ]
}
////////////////////////////////////////////////////////////////////////
{
    "base_requests": [
    {
        "type": "Bus",
            "name" : "114",
            "stops" : ["Morskoy vokzal", "Riverskiy most"] ,
            "is_roundtrip" : false
    },
    {
      "type": "Stop",
      "name" : "Riverskiy most",
      "latitude" : -1,
      "longitude" : -3,
      "road_distances" : {"Morskoy vokzal": 850}
    },
    {
      "type": "Stop",
      "name" : "Morskoy vokzal",
      "latitude" : 5,
      "longitude" : 36,
      "road_distances" : {"Riverskiy most": 850}
    }
    ],
        "stat_requests": [
    { "id": 1, "type" : "Stop", "name" : "Riverskiy most" },
    { "id": 2, "type" : "Bus", "name" : "114" }
        ]
}
//////////////////////////////////////////////////////////////////
{
  "base_requests": [
    {
      "type": "Stop",
      "name": "Tolstopaltsevo",
      "latitude": 55.611087,
      "longitude": 37.20829,
      "road_distances": {"Marushkino": 3900}
    },
    {
      "type": "Stop",
      "name": "Marushkino",
      "latitude": 55.595884,
      "longitude": 37.209755,
      "road_distances": {"Rasskazovka": 9900, "Marushkino": 100}
    },
    {
      "type": "Bus",
      "name": "256",
      "stops": ["Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"],
      "is_roundtrip": true
    },
    {
      "type": "Bus",
      "name": "750",
      "stops": ["Tolstopaltsevo", "Marushkino", "Marushkino", "Rasskazovka"],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "name": "Rasskazovka",
      "latitude": 55.632761,
      "longitude": 37.333324,
      "road_distances": {"Marushkino": 9500}
    },
    {
      "type": "Stop",
      "name": "Biryulyovo Zapadnoye",
      "latitude": 55.574371,
      "longitude": 37.6517,
      "road_distances": {"Rossoshanskaya ulitsa": 7500, "Biryusinka": 1800, "Universam": 2400}
    },
    {
      "type": "Stop",
      "name": "Biryusinka",
      "latitude": 55.581065,
      "longitude": 37.64839,
      "road_distances": {"Universam": 750}
    },
    {
      "type": "Stop",
      "name": "Universam",
      "latitude": 55.587655,
      "longitude": 37.645687,
      "road_distances": {"Rossoshanskaya ulitsa": 5600, "Biryulyovo Tovarnaya": 900}
    },
    {
      "type": "Stop",
      "name": "Biryulyovo Tovarnaya",
      "latitude": 55.592028,
      "longitude": 37.653656,
      "road_distances": {"Biryulyovo Passazhirskaya": 1300}
    },
    {
      "type": "Stop",
      "name": "Biryulyovo Passazhirskaya",
      "latitude": 55.580999,
      "longitude": 37.659164,
      "road_distances": {"Biryulyovo Zapadnoye": 1200}
    },
    {
      "type": "Bus",
      "name": "828",
      "stops": ["Biryulyovo Zapadnoye", "Universam", "Rossoshanskaya ulitsa", "Biryulyovo Zapadnoye"],
      "is_roundtrip": true
    },
    {
      "type": "Stop",
      "name": "Rossoshanskaya ulitsa",
      "latitude": 55.595579,
      "longitude": 37.605757,
      "road_distances": {}
    },
    {
      "type": "Stop",
      "name": "Prazhskaya",
      "latitude": 55.611678,
      "longitude": 37.603831,
      "road_distances": {}
    }
  ],
  "stat_requests": [
    { "id": 1, "type": "Bus", "name": "256" },
    { "id": 2, "type": "Bus", "name": "750" },
    { "id": 3, "type": "Bus", "name": "751" },
    { "id": 4, "type": "Stop", "name": "Samara" },
    { "id": 5, "type": "Stop", "name": "Prazhskaya" },
    { "id": 6, "type": "Stop", "name": "Biryulyovo Zapadnoye" }
  ]
}
*/