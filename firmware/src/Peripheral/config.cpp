#include "config.h"

int status_Robot = 0; //WAITING_WIFI_CONNECT
int cnt_time_out = 0;
hw_timer_t *timer = NULL;
Timer_Animation Timer_Display;

void LED_ON(void)
{
    digitalWrite(LED_PIN, HIGH);
}

void LED_OFF(void)
{
    digitalWrite(LED_PIN, LOW);
}

int read_BTN(const int _pin)
{
    return digitalRead(_pin);
}

bool push3s_Home(void)
{
    // static int cnt_time = 0;

    // if(read_BTN(HOME_PIN) == PUSH_BUTTON)
    // {
    //     cnt_time++;
    //     if(cnt_time > TIME_OUT_3S)
    //     {
    //         cnt_time = 0;
    //         return false;
    //     }
    // }
    // else
    // {
    //     cnt_time = 0;
    // }
    return true;
}

bool check_timeout(void)
{
    // static int cnt_time = 0;
    cnt_time_out++;
    Serial.printf("cnt time out = %d\r\n", cnt_time_out);
    if(cnt_time_out > TIME_OUT_3S)
    {
        cnt_time_out = 0;
        return false;
    }
    return true;
}

bool check_sensor(const int _sensor_pin)
{
    static int cnt_time = 0;

    if(!read_BTN(_sensor_pin))   //on: Low, off: HIGH
    {
        // cnt_time++;
        // if(cnt_time > 3)
        // {
        //     cnt_time = 0;
            return false;
        // }
    }
    return true;
}



void IRAM_ATTR onTimer(){
    // digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    Timer_Display.count++;
}

void Animation_timer_init(void)
{
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 10000, true);  // ~ 10ms //1000000 ~ 1s
    timerAlarmEnable(timer);
    Timer_Display.time_animation[0] = 0;
}






