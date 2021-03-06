//FILE INCLUDES INFORMATION ON DATA PROCESSING AND INTELLIGENCE FOR THE THERMOSTAT

//----------------TEMPERATURE DEADZONE FUNCTION-------------------
boolean process_deadzone()
{
   //Indoor temperature deadzone
    temp_diff = indoor_temp - request_temperature;
    temp_diff = abs(temp_diff);
    if(temp_diff < temp_deadband_val)
      return true;
    else 
      return false; 
}

//-------------------DATA PROCESSOR FUNCTION----------------------
//void barometric_conditions()
//{
//    if(metar_table[0].pressure < 995)
//      cond = 2;
//    else if(metar_table[0].pressure > 1025)
//      cond = 1;
//    else
//      cond = 0;
//}
//void process_data()
//{
//    temp_diff = metar_table[0].temperature - metar_table[1].temperature;
//    //press_diff = metar_table[0].pressure - metar_table[1].pressure;
//	
//    //outside temperature calculation (out_temp_change:  0 - Steady, 1- Rising, 2- Falling)
//    if(metar_table[0].temperature > metar_table[1].temperature && abs(temp_diff) > 3) //outside temperature is rising
//      out_temp_change = 1;
//    else if(metar_table[0].temperature < metar_table[1].temperature && abs(temp_diff) > 3) //outside temperature is dropping
//      out_temp_change = 2;
//    else //equivalent temperature
//      out_temp_change = 0;
//
//    barometric_conditions();
//}

//-------------------END DATA PROCESSOR FUNCTION----------------------

//-------------------THERMOSTAT INTELLIGENCE--------------------------
//Functions for Thermostat Intelligence

//Intelligence functions which determine when the thermostat should do what functions

//function intended to provide a level of prediction for weather events which may remove the need for addtional A/C or heat usage
//encoded return values
//Base suggestions
// 0 -- Suggest status quo 
// 1 -- Suggest AC
// 2 -- Suggest HEAT
// 3 -- Suggest Humidifier
// 4 -- Suggest just FAN

//Combo suggestions
// 5 -- Suggest HEAT and AC (dehumidification)
// 6 -- Suggest HEAT and Humidifer 


byte smart_forcast()
{
  
  if(indoor_temp > request_temperature)
  {
    //indoor temperature is higher than comfort temperature and outside temperature is rising (likely only become more hot)
    if(out_temp_change == 1)
    {
       if(metar_table[0].temperature > ac_cutoff_val)
       {
          return 1; // suggestion is to turn on the AC
       }
    }
    //indoor temperature is higher than comfort temperature and outside temperature is dropping (may cool off)
    if(out_temp_change == 2)
    {
       if(metar_table[0].temperature < heat_cutoff_val)
       {
          if(indoor_humidity < humidity_cutoff_val)
            return 6;
          else
            return 2;
       }
    }
  }
  
  if(indoor_temp < request_temperature)
  {
    //indoor temperature is lower than comfort temperature and outside temperature is rising (likely only to become more hot)
    //if(out_temp_change == 1)
    //{
       //may not need to do anything here so just erase...
    //}
    
    //indoor temperature is lower than comfort temperature and outside temperature is dropping (may cool off)
    if(out_temp_change == 2)
    {
       if(metar_table[0].temperature < heat_cutoff_val)
       {
          return 2;
       }
    }
  }
   
  return 0; //nothing applied so just status quo
}


void smart_functions()
{
  if(system_status == 2 || system_status == 3)
  {
    
    if(!process_deadzone())
    {
      //Serial.println("Deadband not active");
      //AC ON
      if(request_temperature < indoor_temp && indoor_temp > heat_cutoff_val && smart_forcast() == 1)
      {
        AC = true;
        HEAT = false;
      }
      //HEAT ON
      else if(request_temperature > indoor_temp && indoor_temp < ac_cutoff_val && (smart_forcast() == 2 || smart_forcast() == 5))
      {
        if(indoor_humidity > humidity_cutoff_val)
          AC = true;
        else
          AC = false;
          
        HEAT = true;
      }
      //leave the AC and HEAT off otherwise
      else
      {
        AC = false;
        HEAT = false;
      }
    }     
    if(system_status == 0)
    {
       AC = false;
       HEAT = false; 
    }
  }     
}
//-------------------END THERMOSTAT INTELLIGENCE-----------------------
