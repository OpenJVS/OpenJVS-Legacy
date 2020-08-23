#include "sense.h"
#include "jvs.h"
#include "definitions.h"
#include "config.h"
#include "debug.h"

JVSSenseCircuit circuitToUse = SENSE_FLOAT;

JVSStatus setSenseCircuit(JVSSenseCircuit circuitType)
{
	JVSStatus retval = OPEN_JVS_ERR_OK;

	switch (circuitType)
	{
	case SENSE_FLOAT:
	case SENSE_SWITCH:
	{
		circuitToUse = circuitType;
	}
	break;

	default:
	{
		printf("Warning: Invalid config for sync algorithm %u\n", circuitType);
		retval = OPEN_JVS_ERR_INVALID_SYNC_CIRCUIT;
	}
	break;
	}
	return retval;
}

int DebugPinInit(void)
{
	int retval = 0;
	retval = GPIOExport(DEBUG_PIN);

	if (retval != 0)
	{
		printf("Warning: DEBUG_PIN pin %d not available\n", DEBUG_PIN);
	}

	if (retval != 0)
	{
		retval = GPIODirection(DEBUG_PIN, OUT);
		if (retval != 0)
		{
			printf("Warning: DEBUG_PIN pin %d could not be set to output\n", DEBUG_PIN);
		}
	}

	if (retval != 0)
	{
		DebugPinSet(0);
	}
	return retval;
}

void DebugPinSet(uint8_t b)
{
	int error = 0;
	error = GPIOWrite(DEBUG_PIN, b);
}

int initSense(void)
{
	int retval = 0;

	/* Make Pin available */
	switch (circuitToUse)
	{
	case SENSE_SWITCH:
	case SENSE_FLOAT:
		/* GPIO SYNC PINS */
		retval = GPIOExport(SENSE_PIN);
		if (retval != 0)
		{
			printf("Warning: SENSE pin %d not available, change SENSE mode to 0 in the config file\n", SENSE_PIN);
		}
		break;

	SYNC_NONE:
		break;
	}

	/* Config Pin*/
	if (retval == 0)
	{
		switch (circuitToUse)
		{
		case SENSE_SWITCH:
		{
			retval = GPIODirection(SENSE_PIN, OUT);
			if (retval != 0)
			{
				if (getConfig()->debugMode)
					printf("Warning: Sync pin %d could not be set to output\n", SENSE_PIN);
			}
		}
		break;

		case SENSE_NONE:
		case SENSE_FLOAT:
			break;
		}
	}

	setSensePin(false);
	return retval;
}

int setSensePin(bool pull_low)
{
	int error = 0;

	if (pull_low)
	{
		switch (circuitToUse)
		{
		case SENSE_SWITCH:
		{
			error = GPIOWrite(SENSE_PIN, 1);

			if (error != 0)
			{
				if (getConfig()->debugMode)
					printf("Warning: Failed to ground  pin %d\n", SENSE_PIN);
			}
		}
		break;

		case SENSE_FLOAT:
		{
			error = GPIODirection(SENSE_PIN, OUT);

			if (error == 0)
			{
				error = GPIOWrite(SENSE_PIN, 0);
			}

			if (error != 0)
			{
				if (getConfig()->debugMode)
					printf("Warning: Failed to ground  pin %d\n", SENSE_PIN);
			}
		}
		break;

		case SENSE_NONE:
			break;

		default:
		{
			if (getConfig()->debugMode)
				printf("Invalid Sync algorithm net: %u \n", circuitToUse);
		}
		break;
		}

		if (getConfig()->debugMode)
			debug(1, "Floated sense pin\n");
	}
	else
	{
		switch (circuitToUse)
		{
		case SENSE_SWITCH:
		{
			error = GPIOWrite(SENSE_PIN, 0);

			if (error != 0)
			{
				if (getConfig()->debugMode)
					printf("Warning: Failed to pull high pin %d\n", SENSE_PIN);
			}
		}
		break;

		case SENSE_FLOAT:
		{
			error = GPIODirection(SENSE_PIN, IN);

			if (error != 0)
			{
				if (getConfig()->debugMode)
					printf("Warning: Failed to float pin %d\n", SENSE_PIN);
			}
		}
		break;

		case SENSE_NONE:
			break;

		default:
		{
			if (getConfig()->debugMode)
				printf("Invalid Sync algorithm net: %u \n", circuitToUse);
		}
		break;
		}

		if (getConfig()->debugMode)
			debug(1, "Grounded sense pin\n");
	}
	return error;
}

int GPIOExport(int pin)
{
#define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd)
	{
		printf("Failed to open export for writing!\n");
		return (-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return (0);
}

int GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd)
	{
		//fprintf(stderr, "Failed to open unexport for writing!\n");
		return (-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return (0);
}

int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[] = "in\0out";

#define DIRECTION_MAX 35
	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd)
	{
		//fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return (-1);
	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3))
	{
		//fprintf(stderr, "Failed to set direction!\n");
		return (-1);
	}

	close(fd);
	return (0);
}

int GPIORead(int pin)
{
#define VALUE_MAX 30
	char path[VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd)
	{
		//fprintf(stderr, "Failed to open gpio value for reading!\n");
		return (-1);
	}

	if (-1 == read(fd, value_str, 3))
	{
		//fprintf(stderr, "Failed to read value!\n");
		return (-1);
	}

	close(fd);

	return (atoi(value_str));
}

int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd)
	{
		if (getConfig()->debugMode)
			fprintf(stderr, "Failed to open gpio value for writing!\n");
		return (-1);
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1))
	{
		if (getConfig()->debugMode)
			fprintf(stderr, "Failed to write value!\n");
		return (-1);
	}

	close(fd);
	return (0);
}
