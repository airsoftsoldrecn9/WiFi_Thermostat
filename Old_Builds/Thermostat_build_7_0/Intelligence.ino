//Functions for Thermostat Intelligence

//Intelligence functions which determine when the thermostat should do what functions

void smart_functions()
{
  if(system_status == 3)
  {
    if(!vacant)
    {
      if((metar_table[0].temperature > 80 && indoor_temp >= 80) || indoor_humidity > humity_cutoff_val) //indoor temperature kept at 80 if outside is above 80 or if humidity above 50%
      {
           if(cond == 2 && out_temp_change == 2) //only if weather is stormy and outside temperature is falling
           {
             AC = false;
             HEAT = false;
           }
           else
           {
             AC = true;
             HEAT = false;
           }
      }
      else if(metar_table[0].temperature < 60 && indoor_temp <= 65) //winter mode
      {
           if(cond != 2 && out_temp_change == 1) //weather is not stormy and outside temperature is rising
           {
               AC = false;
               HEAT = false;
           } 
           else if(cond == 2 && out_temp_change == 2) //weather is stormy and outside temperature is falling
           {
               AC = false;
               HEAT = true;
           }
           else
           {
               AC = false;
               HEAT = false;
           }
      }
      else if(metar_table[0].temperature < 80 && metar_table[0].temperature > 60)
      {
           if(request_temperature <= indoor_temp && indoor_temp > 70 && temp_diff > 2)
           {
             AC = true;
             HEAT = false;
           }
           else if(request_temperature >= indoor_temp && indoor_temp > 70 && temp_diff > 2)
           {
             AC = false;
             HEAT = false;    
           }
           else if(request_temperature >= indoor_temp && indoor_temp < 70 && temp_diff > 2)
           {
             AC = false;
             HEAT = true;
           }
           else if(request_temperature <= indoor_temp && indoor_temp < 70 && temp_diff > 2)
           {
             AC = false;
             HEAT = false;
           }
           //leave the AC and HEAT off otherwise
           else
           {
             AC = false;
             HEAT = false;
           }
      }
    }
    else
    {
      if(indoor_temp > 90)
      {
        AC = true;
        HEAT = false;
      }
      else if(indoor_temp < 40)
      {
        AC = false;
        HEAT = true; 
      }
      else
      {
        AC = false;
        HEAT = false;
      }
    }
  }
  
  if(system_status == 2)
  {
    
    if(!process_deadzone())
    {
      //Serial.println("Deadband not active");
      //AC ON
      if(request_temperature < indoor_temp && indoor_temp > heat_cutoff_val)
      {
        AC = true;
        HEAT = false;
      }
      //HEAT ON
      else if(request_temperature > indoor_temp && indoor_temp < ac_cutoff_val)
      {
        if(indoor_humidity > humity_cutoff_val)
          AC = true;
          
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


