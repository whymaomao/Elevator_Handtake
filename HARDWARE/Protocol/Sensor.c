#include "Sensor.h"
#include "easyflash.h"
#include "s2j.h"
#include "cJSON.h"
#include "malloc.h"
#include "ef_types.h"
#include "nandflash.h"

extern NAND_DRV nand0_drv;
extern NAND_DRV_CFG cfg;
static void Sensor_Init(Sensor*const me)
{
		unsigned char i = 0;
	
		me->distance = 0;
		me->humidity = 0;
		me->light = 0;
		me->temperature = 0;
		me->date = "";
		for(i = 0; i < 3; i ++)
		{
				me->voltage[i] = 0;
		}
		for( i = 0; i < 3; i ++)
		{
				me->Amper[i] = 0;
		}
}

void Sensor_Clean(Sensor* const me)
{
	Sensor_Init(me);
}


Sensor* Sensor_Create(void)
{
		Sensor* me = (Sensor*)mymalloc(SRAMIN,sizeof(Sensor));
		if(me != NULL)
		{
				Sensor_Init(me);
		}
		return me;
}

void Sensor_Destroy(Sensor*const me)
{
		if(me != NULL)
			myfree(SRAMIN,me);
}

//////////////////////////////////////////////////////////////////

void Sensor_setTemperature(Sensor* const me,float temp)
{
		me->temperature = temp;
}

void Sensor_setHumidity(Sensor*const me, float humidity)
{
		me->humidity = humidity;
}

void Sensor_setLight(Sensor*const me, float light)
{
		me->light = light;
}

void Sensor_setDistance(Sensor* const me, float distance)
{
		me->distance = distance;
}



void Sensor_setDate(Sensor* const me, char* date)
{
		me->date = date;
		//strcpy(me->date,date);
}

void Sensor_setVoltage(Sensor* const me, float voltage[])
{
		unsigned char i = 0;
		for( i = 0; i < 3; i ++)
		{
				me->voltage[i] = voltage[i];
		}
}

void Sensor_setAmper(Sensor* const me, float AMper[])
{
		unsigned char i = 0;
		for(i = 0; i < 3; i ++)
		{
				me->Amper[i] = AMper[i];
		}
}
//////////////////////////////////////////////////////////////////
float Sensor_getTemperature(Sensor* const me)
{
		return me->temperature;
}

float Sensor_getHumidity(Sensor* const me)
{
		return me->humidity;
}

float Sensor_getLight(Sensor* const me)
{
		return me->light;
}
float    Sensor_getDistance(Sensor* const me)
{
		return me->distance;
}



char*  Sensor_getDate(Sensor* const me)
{
		return me->date;
}


///////////////////////////////////////////////////////////////////
static cJSON *struct_to_json(void* struct_obj)
{
		Sensor* me = (Sensor*)struct_obj;
		s2j_create_json_obj(json_Sensor);

		
		s2j_json_set_basic_element(json_Sensor, me, double, temperature);
		s2j_json_set_basic_element(json_Sensor, me, double, humidity);
		s2j_json_set_basic_element(json_Sensor, me, double, light);
		s2j_json_set_basic_element(json_Sensor, me, double, distance);
		s2j_json_set_array_element(json_Sensor,me,double,voltage,3);
		s2j_json_set_array_element(json_Sensor,me,double, Amper,3);
		s2j_json_set_basic_element(json_Sensor, me, string, date);
		return json_Sensor;
}


static void* json_to_struct(cJSON* json_obj)
{
		//create Sensor structure object
		s2j_create_struct_obj(struct_Sensor,Sensor);
		
	 /* deserialize data to Sensor structure object. */
	//
	s2j_struct_get_basic_element(struct_Sensor, json_obj, double, temperature);
	s2j_struct_get_basic_element(struct_Sensor, json_obj, double, humidity);
	s2j_struct_get_basic_element(struct_Sensor, json_obj, double, light);
	s2j_struct_get_basic_element(struct_Sensor, json_obj, double, distance);
	s2j_struct_get_array_element(struct_Sensor, json_obj, double, voltage);
	s2j_struct_get_array_element(struct_Sensor, json_obj, double, Amper);
	s2j_struct_get_basic_element(struct_Sensor, json_obj, string, date);
	return struct_Sensor;
}	

char* Sensor_to_String(Sensor*const me)
{
		cJSON *json_Sensor = struct_to_json(me);
		char *char_value = cJSON_PrintUnformatted(json_Sensor);
		return char_value;
}

void save_Sensor(Sensor* const me,const char* key)
{
		ef_set_struct(key,me,struct_to_json);
		ef_save_env();
		
}

Sensor* get_Stuct(char* char_value)
{
		Sensor* result;
    cJSON *json_value = cJSON_Parse(char_value);
    void *value = NULL;

    if (json_value) {
        value = json_to_struct(json_value);
        cJSON_Delete(json_value);
    }
    return value;   
}

Sensor* get_Sensor(const char*key)
{
		Sensor* result;
		result = (Sensor*)ef_get_struct(key,json_to_struct);
		return result;
}