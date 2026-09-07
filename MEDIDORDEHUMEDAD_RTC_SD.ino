#include <SPI.h>
#include <SD.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>


#define sensor A0   // Humedad
#define SD_CS 10     // SD
#define sensor_T A1      // Temperatura

#define RTC_CLK 5   // RTC
#define RTC_DAT 6   // RTC
#define RTC_RST 7   // RTC
// Conexión RTC
ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> Rtc(myWire);

// ARCHIVO DONDE SE GUARDARÁ LA INFORMACIÓN EN LA SD
File archivo;
void setup() {
  Serial.begin(9600);

    // ----- RTC --------
    Rtc.Begin();

    if (!Rtc.GetIsRunning()) {
        Serial.println("RTC detenido, iniciando...");
        Rtc.SetIsRunning(true);
    }

    if (Rtc.GetIsWriteProtected()) {
        Rtc.SetIsWriteProtected(false);
    }

    // SOLO LA PRIMERA VEZ DESCOMENTADA Y LUEGO SE VUELVE A COMENTAR
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Rtc.SetDateTime(compiled);

    // ESTA ES UNA CONFIGURACIÓN MANUAL
   // Rtc.SetDateTime(RtcDateTime(2026, 5, 15, 17, 07, 0));

    // ------ SD --------
    pinMode(10, OUTPUT);
    
    if (!SD.begin(SD_CS)) {
        Serial.println("Error SD, no hay conexion");
        while (1);
    }

    if (!SD.exists("Datos.csv")) {
        archivo = SD.open("Datos.csv", FILE_WRITE);
        archivo.println("Fecha,Hora,Temperatura(C),Humedad(%)");
        archivo.close();
    }
    Serial.println("Sistema listo");

}

void loop() {
  // put your main code here, to run repeatedly:
    int humedad = map(analogRead(sensor), 0, 1023, 100, 0);

    RtcDateTime now = Rtc.GetDateTime();

    // Temperatura
    int lecturaADC = analogRead(sensor_T); // Lectura del ADC 
    float voltaje = lecturaADC * (2.58/ 1023.0); // Conversión a voltaje 
    float temperatura = voltaje * 100.0;

    archivo = SD.open("Datos.csv", FILE_WRITE);
    if (archivo) {

        // Fecha
        archivo.print(now.Year());
        archivo.print("-");
        if (now.Month() < 10) archivo.print("0");
        archivo.print(now.Month());
        archivo.print("-");
        if (now.Day() < 10) archivo.print("0");
        archivo.print(now.Day());
        archivo.print(",");

        // Hora
        if (now.Hour() < 10) archivo.print("0");
        archivo.print(now.Hour());
        archivo.print(":");
        if (now.Minute() < 10) archivo.print("0");
        archivo.print(now.Minute());
        archivo.print(":");
        if (now.Second() < 10) archivo.print("0");
        archivo.print(now.Second());
        archivo.print(",");

        archivo.print(temperatura, 1);
        archivo.print(",");

        // Humedad
        archivo.println(humedad);
        archivo.close();
    }

    Serial.print("Fecha: ");
    Serial.print(now.Day()); Serial.print("/");
    Serial.print(now.Month()); Serial.print("/");
    Serial.print(now.Year());

    Serial.print(" | Hora: ");
    Serial.print(now.Hour()); Serial.print(":");
    Serial.print(now.Minute()); Serial.print(":");
    Serial.print(now.Second());

    Serial.print(" | Temperatura: ");
    Serial.print(temperatura);
    Serial.println("°C");

    Serial.print(" | Humedad: ");
    Serial.print(humedad);
    Serial.println("%");

    delay(60000); 
}
