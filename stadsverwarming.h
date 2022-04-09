#include "esphome.h"

class Multical602 : public Component, public Switch, public UARTDevice
{
public:
  Sensor *sensor_energy{nullptr};
  Sensor *sensor_volume{nullptr};
  Sensor *sensor_tempin{nullptr};
  Sensor *sensor_tempout{nullptr};
  Sensor *sensor_tempdiff{nullptr};
  Sensor *sensor_power{nullptr};
  Sensor *sensor_flow{nullptr};
  Sensor *sensor_poweract{nullptr};
  TextSensor *sensor_info{nullptr};
  TextSensor *textsensor_status{nullptr};

  std::string m_status;

  UARTDevice *_tx;
  UARTDevice *_rx;

  Multical602(UARTComponent *uart_tx, UARTComponent *uart_rx, Sensor *m__energy, Sensor *m__volume, Sensor *m__tin, Sensor *m__tout,
              Sensor *m__tdiff, Sensor *m__power, Sensor *m__flow, Sensor *m__poweract, TextSensor *m__info, TextSensor *m__status) : sensor_energy(m__energy), sensor_volume(m__volume), sensor_tempin(m__tin), sensor_tempout(m__tout),
                                                                                                                                      sensor_tempdiff(m__tdiff), sensor_power(m__power), sensor_flow(m__flow), sensor_poweract(m__poweract), sensor_info(m__info), textsensor_status(m__status)
  {
    _tx = new UARTDevice(uart_tx);
    _rx = new UARTDevice(uart_rx);
  }

  void setup() override
  {
    // This will be called by App.setup()
  }
  void write_state(bool state) override
  {
    // This will be called every time the user requests a state change.
    run();
  }

  void run()
  {
    ESP_LOGD("multical", "Run");
    byte sendmsg1[] = {175, 163, 177}; //   /#1 with even parity

    byte r = 0;
    byte to = 0;
    byte i;
    byte j;
    char message[255];
    int parityerrors;

    to = 0;
    r = 0;
    i = 0;
    j = 0;
    parityerrors = 0;
    char *tmpstr;
    float m_energy, m_volume, m_tempin, m_tempout, m_tempdiff, m_power;
    long m_hours, m_flow, m_poweract;
    std::string m_info;

    for (int x = 0; x < 3; x++)
    {
      _tx->write(sendmsg1[x]);
    }

    delay(10);

    bool gelukt = false;

    while (_rx->available())
    {

      // receive byte
      r = _rx->read();

      if (r != 0 && r != 120)
      {
        message[i++] = char(r);
        gelukt = true;
      }
    }

    if (gelukt)
    {
      ESP_LOGD("multical", "OK");
      Serial.println("OK: ");
      Serial.println(message);
      Serial.println("");
      Serial.println("");

      message[i] = 0;

      tmpstr = strtok(message, " ");

      if (tmpstr)
      {
        // Convert to kWh
        m_energy = atol(tmpstr) / 1000.000;
        // m_energy = m_energy / 1000.000;
      }
      else
      {
        m_energy = 0;
      }

      tmpstr = strtok(NULL, " ");
      if (tmpstr)
        m_volume = atol(tmpstr) / 100.0;
      else
        m_volume = 0;

      tmpstr = strtok(NULL, " ");
      if (tmpstr)
        m_hours = atol(tmpstr);
      else
        m_hours = 0;

      tmpstr = strtok(NULL, " ");
      if (tmpstr)
        m_tempin = atol(tmpstr) / 100.0;
      else
        m_tempin = 0;

      tmpstr = strtok(NULL, " ");
      if (tmpstr)
        m_tempout = atol(tmpstr) / 100.0;
      else
        m_tempout = 0;

      tmpstr = strtok(NULL, " ");
      if (tmpstr)
        m_tempdiff = atol(tmpstr) / 100.0;
      else
        m_tempdiff = 0;

      tmpstr = strtok(NULL, " ");
      if (tmpstr)
        m_power = atol(tmpstr) / 10.0;
      else
        m_power = 0;

      tmpstr = strtok(NULL, " ");
      if (tmpstr)
        m_flow = atol(tmpstr);
      else
        m_flow = 0;

      tmpstr = strtok(NULL, " ");
      if (tmpstr)
        m_poweract = atol(tmpstr);
      else
        m_poweract = 0;

      tmpstr = strtok(NULL, " ");
      if (tmpstr)
        m_info = atol(tmpstr);
      else
        m_info = "NONE";

      ESP_LOGD("multical", "Energy: %f GJ", m_energy);
      ESP_LOGD("multical", "Volume: %f m3", m_volume);
      ESP_LOGD("multical", "Time: %ld hrs", m_hours);
      ESP_LOGD("multical", "T_in: %f", m_tempin);
      ESP_LOGD("multical", "T_out: %f", m_tempout);
      ESP_LOGD("multical", "T_diff: %f", m_tempdiff);
      ESP_LOGD("multical", "Power: %f", m_power);
      ESP_LOGD("multical", "Flow: %ld l/h", m_flow);
      ESP_LOGD("multical", "Power Act: %ld", m_poweract);
      ESP_LOGD("multical", "Info: %ld", m_info);

      m_status = "OK";

      sensor_energy->publish_state(m_energy);
      sensor_volume->publish_state(m_volume);
      sensor_tempin->publish_state(m_tempin);
      sensor_tempout->publish_state(m_tempout);
      sensor_tempdiff->publish_state(m_tempdiff);
      sensor_power->publish_state(m_power);
      sensor_flow->publish_state(m_flow);
      sensor_poweract->publish_state(m_poweract);
      sensor_info->publish_state(m_info);

      textsensor_status->publish_state(m_status);
    }
    else
    {
      ESP_LOGD("multical", "NIET OK");
      m_status = "ERROR";
      textsensor_status->publish_state(m_status);
    }
  }
};