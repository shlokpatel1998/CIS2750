#ifndef GPX_HELPER_H
#define GPX_HELPER_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>
#include "LinkedListAPI.h"

// Shlok Patel Student # 0979966

//string helper functions

int checkStringEmpty(char * string);

void copyNewString(char * stringToCopy, char ** string);

void catString(char * stringToCat, char ** string);

char * createEmptyString();

void freeString(char * stringToFree);

// Initialize functions

GPXdoc * newGPXdoc();

GPXData * newGPXData(int space);

Waypoint * newWaypoint();

Route * newRoute();

TrackSegment * newTrackSegment();

Track * newTrack();

// Parsing functions

GPXData * parseGPXData(char * name, xmlNode * node);

Waypoint * parseWaypoint(xmlNode * node);

Route * parseRoute(xmlNode * node);

TrackSegment * parseTrackSegment(xmlNode * node);

Track * parseTrack(xmlNode * node);


bool doc_to_tree(xmlNodePtr * root, GPXdoc * doc);

bool is_valid_tree(char * fileName, char * gpxSchemaFile);

bool is_valid_xmlDoc(xmlDoc * doc, char * gpxSchemaFile);

bool is_valid_waypoints(xmlNodePtr * root, GPXdoc * doc);

bool is_valid_routes(xmlNodePtr * root, GPXdoc * doc);

bool is_valid_tracks(xmlNodePtr * root, GPXdoc * doc);

float haversine(double lat1, double lat2, double lon1, double lon2);

void delete_route_dummy(void*data);

void delete_track_dummy(void*data);



#endif

