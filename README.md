# firmwTest

Code that generates random values for 4 structures that represent the 4 process areas of a steel company.

Each structure is defined as: 
```
  typedef struct powMeter{
    int vl1;     // Voltage values
    int vl2;
    int vl3;
    int cl1;     // Current values
    int cl2;
    int cl3;
    int cE;     // Cumulative Energy Imported
    float pf1;  // Power Factor Values
    float pf2;
    float pf3;
    char area;
}powMeter;
```
There are functions to read and write values to the structures

The main function inits: 
  1. A thread that writes random values to each structure.
  2. Four threads that read from each process area and create a cJSON object each.
    
The idea was to send these JSON structures to an Azure IOT Hub using their SDK but this is so far as I could get.

Code was compiled and run on a Linux machine using gcc version 9.3.0 using the command `gcc test.c -pthread`

The output prints something like the following to the console each five seconds:
```
{
	"timestamp":	0,
	"VL1":	219,
	"VL2":	117,
	"VL3":	164,
	"CL1":	30,
	"CL2":	26,
	"CL3":	1,
	"CE":	3300,
	"PF_L1":	0.56000000238418579,
	"PF_L2":	0.10999999940395355,
	"PF_L3":	0.41999998688697815,
	"type":	"telemetry",
	"process":	"warehouse"
}
{
	"timestamp":	0,
	"VL1":	206,
	"VL2":	183,
	"VL3":	159,
	"CL1":	21,
	"CL2":	10,
	"CL3":	41,
	"CE":	13332,
	"PF_L1":	0.25999999046325684,
	"PF_L2":	0.72000002861022949,
	"PF_L3":	0.36000001430511475,
	"type":	"telemetry",
	"process":	"molding"
}
{
	"timestamp":	0,
	"VL1":	171,
	"VL2":	208,
	"VL3":	160,
	"CL1":	55,
	"CL2":	8,
	"CL3":	50,
	"CE":	11535,
	"PF_L1":	0.92000001668930054,
	"PF_L2":	0.49000000953674316,
	"PF_L3":	0.20999999344348907,
	"type":	"telemetry",
	"process":	"foundry"
}
{
	"timestamp":	0,
	"VL1":	142,
	"VL2":	173,
	"VL3":	159,
	"CL1":	9,
	"CL2":	31,
	"CL3":	4,
	"CE":	11777,
	"PF_L1":	0.23000000417232513,
	"PF_L2":	0.67000001668930054,
	"PF_L3":	0.34999999403953552,
	"type":	"telemetry",
	"process":	"press"
}
```
