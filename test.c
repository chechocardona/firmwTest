#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
// Auxiliar library from https://github.com/DaveGamble/cJSON
#include "cJSON.c"

//#define PRINTTEST

/*
  Data structure to save randomly generated data that simulates 
  the PAC3200 Power Meter
*/ 
typedef struct powMeter{
    int vl1;
    int vl2;
    int vl3;
    int cl1;
    int cl2;
    int cl3;
    int cE;
    float pf1;
    float pf2;
    float pf3;
    char area;
}powMeter;

// Structures from each process area
powMeter foundry;
powMeter molding;
powMeter press;
powMeter warehouse;

pthread_mutex_t mutex;

// Function to initialize the structure with random values
void initpM(powMeter *pm, int type);
// Function to write random values to powM structures
void writepM(powMeter *pm);
// Reading values function for the pM
powMeter readValues(powMeter *powM);
// Writing values function for the pM
void writeValues(powMeter *powM);
// Thread function to write values every second
void *writeThread(void *arg);
// Print function to check values
void printpM(powMeter pm);
// Thread functions to read foundry, molding, press and warehouse values every 5 seconds
void *readfoundryTh(void *arg);
void *readmoldingTh(void *arg);
void *readpressTh(void *arg);
void *readwarehouseTh(void *arg);
// Function to init a cJSON object
void initJSON(cJSON **object);


int main() 
{
    pthread_t writeTh, readFTh, readMTh, readPTh, readWTh;

    // initialize the mutex
    pthread_mutex_init(&mutex, NULL);

    // Initialize the structure
    initpM(&foundry, 1);
    initpM(&molding, 2);
    initpM(&press, 3);
    initpM(&warehouse, 4);

    // Create threads
    pthread_create(&writeTh, NULL, writeThread, NULL);
    pthread_create(&readFTh, NULL, readfoundryTh, NULL);
    pthread_create(&readMTh, NULL, readmoldingTh, NULL);
    pthread_create(&readPTh, NULL, readpressTh, NULL);
    pthread_create(&readWTh, NULL, readwarehouseTh, NULL);

    // Detach threads
    pthread_detach(writeTh);
    pthread_detach(readFTh);
    pthread_detach(readMTh);
    pthread_detach(readPTh);
    pthread_detach(readWTh);

    while (1) 
    {
        // Check other things like events of alarms, communication with the powerMeter etc.
    }

    return 0;
}

void initpM(powMeter *pm, int type) 
{
    pm->vl1 = 0;
    pm->vl2 = 0;
    pm->vl3 = 0;
    pm->cl1 = 0;
    pm->cl2 = 0;
    pm->cl3 = 0;
    pm->cE =  0;
    pm->pf1 = 0.0;
    pm->pf2 = 0.0;
    pm->pf3 = 0.0;

    if(type==1) pm->area = 'F';
    if(type==2) pm->area = 'M';
    if(type==3) pm->area = 'P';
    if(type==4) pm->area = 'W';
 
}

void writepM(powMeter *pm) 
{
    // I don't know the ranges, just making an assumption
    pm->vl1 = (rand() % 121) + 110;
    pm->vl2 = (rand() % 121) + 110;
    pm->vl3 = (rand() % 121) + 110;
    pm->cl1 = (rand() % 61) + 1;
    pm->cl2 = (rand() % 61) + 1;
    pm->cl3 = (rand() % 61) + 1;
    pm->cE = (rand() % 13801) + 110;
    pm->pf1 = (float)(rand() % 100)/100;
    pm->pf2 = (float)(rand() % 100)/100;
    pm->pf3 = (float)(rand() % 100)/100; 
}

powMeter readValues(powMeter *powM) 
{
    pthread_mutex_lock(&mutex);
    powMeter copy = *powM;
    pthread_mutex_unlock(&mutex);
    return copy;
}

void writeValues(powMeter *powM)
{
    pthread_mutex_lock(&mutex);
    writepM(powM);
    pthread_mutex_unlock(&mutex);
}

void *writeThread(void *arg)
{
    while (1) 
    {
        writeValues(&foundry);
        writeValues(&molding);
        writeValues(&press);
        writeValues(&warehouse);
        sleep(1);
    }
    return NULL;
}

// A lot of duplicated functions, sorry
void *readfoundryTh(void *arg)
{
    while (1) 
    {
        // Initialize a cJSON object
        cJSON *data = cJSON_CreateObject();
        initJSON(&data);

        powMeter f = readValues(&foundry);
                
        #ifdef PRINTTEST
        printpM(f);
        #endif

        // Writing to cJSON object
        // add a timestamp 
        //cJSON_ReplaceItemInObject(data, "timestamp", );
        cJSON_ReplaceItemInObject(data, "VL1", cJSON_CreateNumber(f.vl1));
        cJSON_ReplaceItemInObject(data, "VL2", cJSON_CreateNumber(f.vl2));
        cJSON_ReplaceItemInObject(data, "VL3", cJSON_CreateNumber(f.vl3));
        cJSON_ReplaceItemInObject(data, "CL1", cJSON_CreateNumber(f.cl1));
        cJSON_ReplaceItemInObject(data, "CL2", cJSON_CreateNumber(f.cl2));
        cJSON_ReplaceItemInObject(data, "CL3", cJSON_CreateNumber(f.cl3));
        cJSON_ReplaceItemInObject(data, "PF_L1", cJSON_CreateNumber(f.pf1));
        cJSON_ReplaceItemInObject(data, "PF_L2", cJSON_CreateNumber(f.pf2));
        cJSON_ReplaceItemInObject(data, "PF_L3", cJSON_CreateNumber(f.pf3));
        cJSON_ReplaceItemInObject(data, "CE", cJSON_CreateNumber(f.cE));
        cJSON_AddStringToObject(data, "type", "telemetry");
        cJSON_AddStringToObject(data, "process", "foundry");
        
        // Send JSON object to the cloud - print to the console to test
        char *json_str = cJSON_Print(data);
        printf("%s\n", json_str);
    
        // free the JSON string and cJSON object
        cJSON_free(json_str);
        cJSON_Delete(data);

        sleep(5);
    }
    return NULL;
}

void *readmoldingTh(void *arg)
{
    while (1) 
    {
        // Initialize a cJSON object
        cJSON *data = cJSON_CreateObject();
        initJSON(&data);

        powMeter m = readValues(&molding);
                
        #ifdef PRINTTEST
        printpM(m);
        #endif

        // Writing to cJSON object
        // add a timestamp 
        //cJSON_ReplaceItemInObject(data, "timestamp", );
        cJSON_ReplaceItemInObject(data, "VL1", cJSON_CreateNumber(m.vl1));
        cJSON_ReplaceItemInObject(data, "VL2", cJSON_CreateNumber(m.vl2));
        cJSON_ReplaceItemInObject(data, "VL3", cJSON_CreateNumber(m.vl3));
        cJSON_ReplaceItemInObject(data, "CL1", cJSON_CreateNumber(m.cl1));
        cJSON_ReplaceItemInObject(data, "CL2", cJSON_CreateNumber(m.cl2));
        cJSON_ReplaceItemInObject(data, "CL3", cJSON_CreateNumber(m.cl3));
        cJSON_ReplaceItemInObject(data, "PF_L1", cJSON_CreateNumber(m.pf1));
        cJSON_ReplaceItemInObject(data, "PF_L2", cJSON_CreateNumber(m.pf2));
        cJSON_ReplaceItemInObject(data, "PF_L3", cJSON_CreateNumber(m.pf3));
        cJSON_ReplaceItemInObject(data, "CE", cJSON_CreateNumber(m.cE));
        cJSON_AddStringToObject(data, "type", "telemetry");
        cJSON_AddStringToObject(data, "process", "molding");
        
        // Send JSON object to the cloud - print to the console to test
        char *json_str = cJSON_Print(data);
        printf("%s\n", json_str);
    
        // free the JSON string and cJSON object
        cJSON_free(json_str);
        cJSON_Delete(data);

        sleep(5);
    }
    return NULL;
}

void *readpressTh(void *arg)
{
    while (1) 
    {
        // Initialize a cJSON object
        cJSON *data = cJSON_CreateObject();
        initJSON(&data);

        powMeter p = readValues(&press);
                
        #ifdef PRINTTEST
        printpM(p);
        #endif

        // Writing to cJSON object
        // add a timestamp 
        //cJSON_ReplaceItemInObject(data, "timestamp", );
        cJSON_ReplaceItemInObject(data, "VL1", cJSON_CreateNumber(p.vl1));
        cJSON_ReplaceItemInObject(data, "VL2", cJSON_CreateNumber(p.vl2));
        cJSON_ReplaceItemInObject(data, "VL3", cJSON_CreateNumber(p.vl3));
        cJSON_ReplaceItemInObject(data, "CL1", cJSON_CreateNumber(p.cl1));
        cJSON_ReplaceItemInObject(data, "CL2", cJSON_CreateNumber(p.cl2));
        cJSON_ReplaceItemInObject(data, "CL3", cJSON_CreateNumber(p.cl3));
        cJSON_ReplaceItemInObject(data, "PF_L1", cJSON_CreateNumber(p.pf1));
        cJSON_ReplaceItemInObject(data, "PF_L2", cJSON_CreateNumber(p.pf2));
        cJSON_ReplaceItemInObject(data, "PF_L3", cJSON_CreateNumber(p.pf3));
        cJSON_ReplaceItemInObject(data, "CE", cJSON_CreateNumber(p.cE));
        cJSON_AddStringToObject(data, "type", "telemetry");
        cJSON_AddStringToObject(data, "process", "press");
        
        // Send JSON object to the cloud - print to the console to test
        char *json_str = cJSON_Print(data);
        printf("%s\n", json_str);
    
        // free the JSON string and cJSON object
        cJSON_free(json_str);
        cJSON_Delete(data);

        sleep(5);
    }
    return NULL;
}
void *readwarehouseTh(void *arg)
{
    while (1) 
    {
        // Initialize a cJSON object
        cJSON *data = cJSON_CreateObject();
        initJSON(&data);

        powMeter w = readValues(&warehouse);
                
        #ifdef PRINTTEST
        printpM(w);
        #endif

        // Writing to cJSON object
        // add a timestamp 
        //cJSON_ReplaceItemInObject(data, "timestamp", );
        cJSON_ReplaceItemInObject(data, "VL1", cJSON_CreateNumber(w.vl1));
        cJSON_ReplaceItemInObject(data, "VL2", cJSON_CreateNumber(w.vl2));
        cJSON_ReplaceItemInObject(data, "VL3", cJSON_CreateNumber(w.vl3));
        cJSON_ReplaceItemInObject(data, "CL1", cJSON_CreateNumber(w.cl1));
        cJSON_ReplaceItemInObject(data, "CL2", cJSON_CreateNumber(w.cl2));
        cJSON_ReplaceItemInObject(data, "CL3", cJSON_CreateNumber(w.cl3));
        cJSON_ReplaceItemInObject(data, "PF_L1", cJSON_CreateNumber(w.pf1));
        cJSON_ReplaceItemInObject(data, "PF_L2", cJSON_CreateNumber(w.pf2));
        cJSON_ReplaceItemInObject(data, "PF_L3", cJSON_CreateNumber(w.pf3));
        cJSON_ReplaceItemInObject(data, "CE", cJSON_CreateNumber(w.cE));
        cJSON_AddStringToObject(data, "type", "telemetry");
        cJSON_AddStringToObject(data, "process", "warehouse");
        
        // Send JSON object to the cloud - print to the console to test
        char *json_str = cJSON_Print(data);
        printf("%s\n", json_str);
    
        // free the JSON string and cJSON object
        cJSON_free(json_str);
        cJSON_Delete(data);

        sleep(5);
    }
    return NULL;
}

// Just for testing purposes
void printpM(powMeter pm)
{
    printf("area: %c\n", pm.area);
    printf("vl1: %d\t", pm.vl1);
    printf("vl2: %d\t", pm.vl2);
    printf("vl3: %d\n", pm.vl3);
    printf("cl1: %d\t\t", pm.cl1);
    printf("cl2: %d\t\t", pm.cl2);
    printf("cl3: %d\n", pm.cl3);
    printf("pf1: %f\t", pm.pf1);
    printf("pf2: %f\t", pm.pf2);
    printf("pf3: %f\n", pm.pf3);
    printf("cE: %d\n", pm.cE);   
}

void initJSON(cJSON **object)
    {
    // create JSON object with void fields
    cJSON *json = *object;
    cJSON_AddNumberToObject(json, "timestamp", 0);
    cJSON_AddNumberToObject(json, "VL1", 0);
    cJSON_AddNumberToObject(json, "VL2", 0);
    cJSON_AddNumberToObject(json, "VL3", 0);
    cJSON_AddNumberToObject(json, "CL1", 0);
    cJSON_AddNumberToObject(json, "CL2", 0);
    cJSON_AddNumberToObject(json, "CL3", 0);
    cJSON_AddNumberToObject(json, "CE", 0);
    cJSON_AddNumberToObject(json, "PF_L1", 0);
    cJSON_AddNumberToObject(json, "PF_L2", 0);
    cJSON_AddNumberToObject(json, "PF_L3", 0);
}

