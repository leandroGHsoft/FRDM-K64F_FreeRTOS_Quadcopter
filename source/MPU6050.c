/*
 * MPU6050.c
 *
 *  Created on: 6 sep. 2020
 *      Author: leandro
 */


#include "MPU6050.h"



/*********************************************************************************************
 * @brief Gets the WHO_AM_I value
 *
 * @param I2C RTOS handle
 *
 * @return status flag. Return true if no error
 *********************************************************************************************/
bool MPU6050_ReadSensorWhoAmI(i2c_rtos_handle_t *master_handle)
{
	status_t status;
	uint8_t who_am_i_reg          = MPU6050_WHO_AM_I;
	uint8_t who_am_i_value        = 0x00;
	i2c_master_transfer_t masterXfer;
	memset(&masterXfer, 0, sizeof(masterXfer));

	// START + Slave_address (write_bit); Reg_address
	masterXfer.slaveAddress   = MPU6050_DEVICE_ADDRESS_0;
	masterXfer.direction      = kI2C_Write;
	masterXfer.subaddress     = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data           = &who_am_i_reg;
	masterXfer.dataSize       = 1;
	masterXfer.flags          = kI2C_TransferNoStopFlag;

	status = I2C_RTOS_Transfer(master_handle, &masterXfer);
	if (status != kStatus_Success)
	{
		PRINTF("I2C master: error during write transaction, %d", status);
		return false;
	}


	// START + Slave_address (read_bit); recibo dato
	masterXfer.direction      = kI2C_Read;
	masterXfer.subaddress     = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data           = &who_am_i_value;
	masterXfer.dataSize       = 1;
	masterXfer.flags          = kI2C_TransferRepeatedStartFlag;
	status = I2C_RTOS_Transfer(master_handle, &masterXfer);
	if (status != kStatus_Success)
	{
		PRINTF("I2C master: error during write transaction, %d", status);
		return false;
	}


	return true;
}



/*********************************************************************************************
 * @brief Configure MPU6050 module
 *
 * @param void
 *
 * @return void
 *********************************************************************************************/
bool MPU6050_Configure_Device(i2c_rtos_handle_t *master_handle)
{
	uint8_t databyte  = 0;
	uint8_t write_reg = 0;

	bool success;

	// Configuration register
	write_reg = MPU6050_CONFIG;
	databyte  = 4;
	success = MPU6050_WriteAccelReg(master_handle, MPU6050_DEVICE_ADDRESS_0, write_reg, databyte);
	if (success == false)
	{
		PRINTF("Error in configuration step 1");
	}

	// Power mode and clock source
	write_reg = MPU6050_PWR_MGMT_1;
	databyte  = 0;
	success = MPU6050_WriteAccelReg(master_handle, MPU6050_DEVICE_ADDRESS_0, write_reg, databyte);
	if (success == false)
	{
		PRINTF("Error in configuration step 2");
	}

	// Gyro - +-250°/s
	write_reg = MPU6050_GYRO_CONFIG;
	databyte  = 0;
	success = MPU6050_WriteAccelReg(master_handle, MPU6050_DEVICE_ADDRESS_0, write_reg, databyte);
	if (success == false)
	{
		PRINTF("Error in configuration step 3");
	}


	// Accel - +-2g
	write_reg = MPU6050_ACCEL_CONFIG;
	databyte  = 0;
	success = MPU6050_WriteAccelReg(master_handle, MPU6050_DEVICE_ADDRESS_0, write_reg, databyte);
	if (success == false)
	{
		PRINTF("Error in configuration step 4");
	}

	return true;
}




/*********************************************************************************************
 * @brief Write sensor register
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param register address
 * @param data value address
 *
 * @return status flag. True if success
 *********************************************************************************************/
bool MPU6050_WriteAccelReg(i2c_rtos_handle_t *master_handle, uint8_t device_addr, uint8_t reg_addr, uint8_t value)
{
	i2c_master_transfer_t masterXfer;
	status_t status;
	memset(&masterXfer, 0, sizeof(masterXfer));

	masterXfer.slaveAddress   = device_addr;
	masterXfer.direction      = kI2C_Write;
	masterXfer.subaddress     = reg_addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data           = &value;
	masterXfer.dataSize       = 1;
	masterXfer.flags          = kI2C_TransferDefaultFlag;

	status = I2C_RTOS_Transfer(master_handle, &masterXfer);
	if (status != kStatus_Success)
	{
		PRINTF("I2C master: error during write transaction, %d", status);
		return false;
	}

	return true;
}




/*********************************************************************************************
 * @brief Read accelerometer data
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param XYZ readings
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Read_Accel_Data(i2c_rtos_handle_t *master_handle, uint8_t device_addr, int16_t *xyz_accel)
{
	uint8_t readBuff[6];
	MPU6050_ReadAccelRegs(master_handle, MPU6050_DEVICE_ADDRESS_0, MPU6050_ACCEL_XOUT_H, readBuff, 6);
	xyz_accel[0] = (((int16_t)readBuff[0]) << 8) | readBuff[1];
	xyz_accel[1] = (((int16_t)readBuff[2]) << 8) | readBuff[3];
	xyz_accel[2] = (((int16_t)readBuff[4]) << 8) | readBuff[5];
}




/*********************************************************************************************
 * @brief Read sensor register
 *
 * @param base I2C peripheral base address.
 * @param sensor device address
 * @param register address
 * @param pointer to data read
 * @param data size
 *
 * @return status flag. True if success
 *********************************************************************************************/
bool MPU6050_ReadAccelRegs(i2c_rtos_handle_t *master_handle, uint8_t device_addr, uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize)
{
    i2c_master_transfer_t masterXfer;
    status_t status;
    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress   = device_addr;
    masterXfer.direction      = kI2C_Read;
    masterXfer.subaddress     = reg_addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data           = rxBuff;
    masterXfer.dataSize       = rxSize;
    masterXfer.flags          = kI2C_TransferDefaultFlag;


    status = I2C_RTOS_Transfer(master_handle, &masterXfer);
	if (status != kStatus_Success)
	{
		PRINTF("I2C master: error during read transaction, %d", status);
		return false;
	}

    return true;
}


/*********************************************************************************************
 * @brief Get the Acceleration in g units
 *
 * @param pointer to acceleration vector -- xyz values
 *
 * @return void
 *********************************************************************************************/
void MPU6050_GetgAcceleration(i2c_rtos_handle_t *master_handle, float *accel)
{
	int16_t xyz_accel[3];
	MPU6050_Read_Accel_Data(master_handle, MPU6050_DEVICE_ADDRESS_0, xyz_accel);
	accel[0] = (float)xyz_accel[0] / MPU6050_ACCEL_FACTOR;
	accel[1] = (float)xyz_accel[1] / MPU6050_ACCEL_FACTOR;
	accel[2] = (float)xyz_accel[2] / MPU6050_ACCEL_FACTOR;
}



/*********************************************************************************************
 * @brief Get the angular velocity from Gyro
 *
 * @param I2C master handle
 * @param pointer to angular velocity vector -- xyz values
 *
 * @return void
 *********************************************************************************************/
void MPU6050_GetAngularVelocity(i2c_rtos_handle_t *master_handle, float *omega)
{
	int16_t xyz_gyro[3];
	MPU6050_Read_Gyro_Data(master_handle, MPU6050_DEVICE_ADDRESS_0, xyz_gyro);
	omega[0] = (float)xyz_gyro[0] / MPU6050_GYRO_FACTOR + 2.5287;
	omega[1] = (float)xyz_gyro[1] / MPU6050_GYRO_FACTOR - 1.13;
	omega[2] = (float)xyz_gyro[2] / MPU6050_GYRO_FACTOR - 0.3583;
}


/*********************************************************************************************
 * @brief Read gyroscope data
 *
 * @param I2C master handle
 * @param sensor device address
 * @param XYZ readings
 *
 * @return void
 *********************************************************************************************/
void MPU6050_Read_Gyro_Data(i2c_rtos_handle_t *master_handle, uint8_t device_addr, int16_t *xyz_gyro)
{
	uint8_t readBuff[6];
	MPU6050_ReadAccelRegs(master_handle, MPU6050_DEVICE_ADDRESS_0, MPU6050_GYRO_XOUT_H, readBuff, 6);
	xyz_gyro[0] = (((int16_t)readBuff[0]) << 8) | readBuff[1];
	xyz_gyro[1] = (((int16_t)readBuff[2]) << 8) | readBuff[3];
	xyz_gyro[2] = (((int16_t)readBuff[4]) << 8) | readBuff[5];
}


/*********************************************************************************************
 * @brief Get the X angle from accelerometer
 *
 * @param void
 *
 * @return roll angle
 *********************************************************************************************/
float MPU6050_GetXAngle(i2c_rtos_handle_t *master_handle)
{
	int16_t xyz_accel[3];
	MPU6050_Read_Accel_Data(master_handle, MPU6050_DEVICE_ADDRESS_0, xyz_accel);

	float sum_of_squares = powf((float)xyz_accel[0], 2) + powf((float)xyz_accel[2], 2);
	float root = sqrtf(sum_of_squares);
	float Xangle = atanf(xyz_accel[1] / root) * 57.2957;

	return Xangle - 1.8104;
}


/*********************************************************************************************
 * @brief Get the Y angle from accelerometer
 *
 * @param void
 *
 * @return pitch angle
 *********************************************************************************************/
float MPU6050_GetYAngle(i2c_rtos_handle_t *master_handle)
{
	int16_t xyz_accel[3];
	MPU6050_Read_Accel_Data(master_handle, MPU6050_DEVICE_ADDRESS_0, xyz_accel);

	float sum_of_squares = powf((float)xyz_accel[1], 2) + powf((float)xyz_accel[2], 2);
	float root = sqrtf(sum_of_squares);
	float Yangle = atanf(xyz_accel[0] / root) * 57.2957;

	return Yangle + 3.4232;
}


/*********************************************************************************************
 * @brief Get X and Y angles with complementary filter
 *
 * @param mpu_angles structure
 *
 * @return void
 *********************************************************************************************/
void MPU6050_ComplementaryFilterAngles(i2c_rtos_handle_t *master_handle, struct MPU6050_angles *mpu_angles)
{
	float x_angle = MPU6050_GetXAngle(master_handle);
	float y_angle = MPU6050_GetYAngle(master_handle);

	float omega[3];

	MPU6050_GetAngularVelocity(master_handle, omega);

	mpu_angles->x = (1 - ALPHA) * (mpu_angles->x + omega[0] * mpu_angles->dt) + ALPHA * x_angle;
	mpu_angles->y = (1 - ALPHA) * (mpu_angles->y + omega[1] * mpu_angles->dt) + ALPHA * y_angle;
}
