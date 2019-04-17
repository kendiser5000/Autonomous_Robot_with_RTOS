
#include "ToF.h"
#include "VL53L0X.h"

void ToF_Init(uint32_t port){
	if(!VL53L0X_Init(port, VL53L0X_I2C_ADDR)) { 	// init and wake up VL53L0X
		while(1);
	}
}

int32_t ToF_Measure(uint32_t port){
	VL53L0X_RangingMeasurementData_t measurement;
	
	VL53L0X_getSingleRangingMeasurement(port, &measurement);
	if (measurement.RangeStatus != 4) {
		return measurement.RangeMilliMeter;
	}
	return -1;
}
