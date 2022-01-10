#include <stdio.h>
#include "GPXParser.h"

// Shlok Patel Student # 0979966
//main to run and test functions

int main(int argc, char **argv) {

    GPXdoc * newDoc = createGPXdoc(argv[1]);
    if(newDoc == NULL) {
      fprintf(stderr, "invalid gpx file\n");
      return 0;
    }
    

    char * temp = GPXdocToString(newDoc);
    if(temp != NULL) {
      //printf("%s\n", temp);
      free(temp);
    }

    int r = getNumWaypoints(newDoc);
    printf("numwaypoints: %d\n", r);

    int r1 = getNumRoutes(newDoc);
    printf("numroutes: %d\n", r1);

    int r2 = getNumTracks(newDoc);
    printf("numtracks: %d\n", r2);

    int r3 = getNumSegments(newDoc);
    printf("numsegments: %d\n", r3);

    int r4 = getNumGPXData(newDoc);
    printf("numData: %d\n", r4);
    
   
    deleteGPXdoc(newDoc);

    return 0;
}