
#include <ESP8266WiFiMulti.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "data.h" // exporta libreria data.h
#include <SimpleDHT.h>
#include <Wire.h>
#include <Adafruit_BMP180.h>

int pinDHT11 = D0;
int pinFACTORY = D6;
int timeOut = 0;
SimpleDHT11 dht11;
Adafruit_BMP180 bmp;

DHT sensor(PIN_CONEXION_DHT, TIPO_SENSOR);

int ultimaVezLeido = 0;
long intervaloLectura = 5000; // Debería ser mayor que 2000
unsigned long ultimaLecturaExitosa = 0;

float humedad, temperatura = 0;
unsigned long tiempo_actual=0, intervaloLectura=0;
const long tiempo_cancelacion = 500;

ESP8266WiFiMulti wifiMulti;

WiFiClient cliente;//creamos un cliente

void rutaRaiz();
void rutaNoencontrada();
void rutaJson();
// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 100;

WiFiServer servidor(80); // 80 es el puerto de comunicacion para paginas webs

void setup() {
  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);
  WiFiManager wifiManager;
  wifiManager.autoConnect("AP-NAME");
  
  Serial.begin(115200);
  Serial.println("iniciando Multi WiFi");

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  Serial.println("conectado a wifi ...");
   if(!bmp.begin()){
      Serial.println("Error en BMP180");
   }
}

  //configurar rutas
  servidor.on("/",rutaRaiz);
  servidor.on("/api",rutaJson);
  servidor.onNotFound(rutaNoencontrada);

  // Register multi WiFi networks
  wifiMulti.addAP(ssid_1, psswrd_1);
  wifiMulti.addAP(ssid_2, psswrd_2);
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
  // More is possible

  servidor.begin();

}

void loop() {
  
   float luminosidad = analogRead(A0);
 
  // Si el intervalo se ha alcanzado, leer la temperatura
  if (ultimaVezLeido > intervaloLectura)
  {
    float nuevaHumedad = sensor.readHumidity();
    float nuevaTemperatura = sensor.readTemperature();
    // Si los datos son correctos, actualizamos las globales
    if (isnan(nuevaTemperatura) || isnan(nuevaHumedad)|| isnan(nuevaestadoSol))
    {
      indicarErrorDht();
      ultimaVezLeido = 0;
      return;
    }
    ultimaLecturaExitosa = millis();
    humedad = nuevaHumedad;
    temperatura = nuevaTemperatura;
    ultimaVezLeido = 0;
    indicarExitoDht();
  }
  delay(1);
  ultimaVezLeido += 1;
  // Responder las solicitudes entrantes en caso de que haya
  servidor.handleClient();
}

  //imprime si hay error en el sensor dht11
   if(dht11.read(pinDHT11, &temperatura, &humedad, NULL)){
      Serial.print("Error en DHT11");
      return;
   }
    cliente = servidor.available(); //vemos si el cliente esta abilitado

    if(cliente){
    Serial.println("nuevo cliente");
    tiempo_actual = millis();
    tiempo_anterior = tiempo_actual;
    String lineaActual = "";

    while (cliente.conected()&& tiempo_actual - tiempo_anterior && <= tiempo_cancelacion){
      if(cliente.available()){ 
      tiempo_actual = millis();
      char letra = cliente.read();
      if (letra == '/n'){ } // obvia los espacios
      else if (letra !='/r'){
        lineaActual += letra; 
      }
      }
    }

    cliente.stop();
    Serial.printl("cliente desconectado");
    Serial.printl();

    }

  // Maintain WiFi connection
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    Serial.print("WiFi connectado ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi no conectado!");
  }

  //muestra todo el servidor de la pagina web comprimida
  void rutaRaiz()
  {
    servidor.send(200, "text/html","<!DOCTYPE html><html lang='es'><head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <title>Sensor de temperatura </title> <link rel='stylesheet' href='https://unpkg.com/bulma@0.9.1/css/bulma.min.css'> <link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css'></head><body> <section id='app' class='hero is-link is-fullheight'> <div class='hero-body'> <div class='container'> <div class='columns has-text-centered'> <div class='column'> <h1 style='font-size: 2.5rem'>Termómetro</h1> <i class='fa' :class='claseTermometro' style='font-size: 4rem;'></i> </div></div><div class='columns'> <div class='column has-text-centered'> <h2 class='is-size-4 has-text-warning'>Temperatura</h2> <h2 class='is-size-1'>{{temperatura}}°C</h2> </div><div class='column has-text-centered'> <h2 class='is-size-4 has-text-warning'>Humedad</h2> <h2 class='is-size-1'>{{humedad}}%</h2> </div></div><div class='columns'> <div class='column'> <p>Última lectura: Hace <strong class='has-text-white'>{{ultimaLectura}}</strong> segundo(s)</p><p class='is-size-5'><i class='fa fa-code'></i> con <i class='fa fa-heart has-text-danger'></i> por <a target='_blan</a></p></div></div></div></div></section> <script src='https://unpkg.com/vue@2.6.12/dist/vue.min.js'> </script> <script>const INTERVALO_REFRESCO=5000; new Vue({el: '#app', data: ()=> ({ultimaLectura: 0, temperatura: 0, humedad: 0,}), mounted(){this.refrescarDatos();}, methods:{async refrescarDatos(){try{const respuestaRaw=await fetch('./api'); const datos=await respuestaRaw.json(); this.ultimaLectura=datos.u; this.temperatura=datos.t; this.humedad=datos.h; setTimeout(()=>{this.refrescarDatos();}, INTERVALO_REFRESCO);}catch (e){setTimeout(()=>{this.refrescarDatos();}, INTERVALO_REFRESCO);}}}, computed:{claseTermometro(){if (this.temperatura <=5){return 'fa-thermometer-empty';}else if (this.temperatura > 5 && this.temperatura <=13){return 'fa-thermometer-quarter';}else if (this.temperatura > 13 && this.temperatura <=21){return 'fa-thermometer-half';}else if (this.temperatura > 21 && this.temperatura <=30){return 'fa-thermometer-three-quarters';}else{return 'fa-thermometer-full';}}}}); </script></body></html>");
  }
  void rutaNoencontrada()
  {
   servidor.send(404, "text/html","servidor no encontrado");
  }
  delay(1000);

  void rutaJson()//api de la estacion meteorologica
  {
  // Calcular última lectura exitosa en segundos
  unsigned long tiempoTranscurridoEnMilisegundos = millis() - ultimaLecturaExitosa;
  int tiempoTranscurrido = tiempoTranscurridoEnMilisegundos / 1000;
  // Búfer para escribir datos en JSON
  char bufer[50];
  // Crear la respuesta pasando las variables globales
  // La salida será algo como:
  // {"t":14.20,"h":79.20,"l":5.00} 
  sprintf(bufer, "{\"t\":%.2f,\"h\":%.2f,\"u\":%d, "e":"%s}", temperatura, humedad, tiempoTranscurrido, estadoSol);
  // Responder con ese JSON
  servidor.send(200, "application/json", bufer);
}

}
