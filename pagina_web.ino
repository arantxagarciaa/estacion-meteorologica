<!DOCTYPE html>
<html lang='es'>

<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>ESP8266 estacion meteorologica</title>
    <link rel='stylesheet' href='https://unpkg.com/bulma@0.9.1/css/bulma.min.css'>
    <link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css'>
</head>

<body>
    <section id='app' class='hero is-link is-fullheight'>
        <div class='hero-body'>
            <div class='container'>
                <div class='columns has-text-centered'>
                    <div class='column'>
                        <h1 style='font-size: 2.5rem'>Termómetro</h1>
                        <i class='fa' :class='claseTermometro' style='font-size: 4rem;'></i>
                    </div>
                </div>
                <div class='columns'>
                    <div class='column has-text-centered'>
                        <h2 class='is-size-4 has-text-warning'>Temperatura</h2>
                        <h2 class='is-size-1'>{{temperatura}} °C</h2>
                    </div>
                    <div class='column has-text-centered'>
                        <h2 class='is-size-4 has-text-warning'>Humedad</h2>
                        <h2 class='is-size-1'>{{humedad}} %</h2>
                    </div>
                </div>
                <div class='columns'>
                    <div class='column'>
                        <p>Última lectura: Hace <strong class='has-text-white'>{{ultimaLectura}}</strong> segundo(s)</p>
                        
                    </div>
                </div>
            </div>
        </div>
    </section>
    <script src='https://unpkg.com/vue@2.6.12/dist/vue.min.js'>
    </script>
    <script>
        const INTERVALO_REFRESCO = 5000;
        new Vue({
            el: '#app',
            data: () => ({
                ultimaLectura: 0,
                temperatura: 0,
                humedad: 0,
                estadosol = 
            }),
            mounted() {
                this.refrescarDatos();
            },
            methods: {
                async refrescarDatos() {
                    try {
                        const respuestaRaw = await fetch('./api');
                        const datos = await respuestaRaw.json();
                        this.ultimaLectura = datos.u;
                        this.temperatura = datos.t;
                        this.humedad = datos.h;
                        this.estadosol = datos.e;
                        this.altitud =datos.a;
                        this.presion =datos.p;
                        setTimeout(() => {
                            this.refrescarDatos();
                        }, INTERVALO_REFRESCO);
                    } catch (e) {
                        setTimeout(() => {
                            this.refrescarDatos();
                        }, INTERVALO_REFRESCO);
                    }
                }
            },
            computed: {
                claseTermometro() {
                    if (this.temperatura <= 5) {
                        return 'fa-thermometer-empty';
                    } else if (this.temperatura > 5 && this.temperatura <= 13) {
                        return 'fa-thermometer-quarter';
                    } else if (this.temperatura > 13 && this.temperatura <= 21) {
                        return 'fa-thermometer-half';
                    } else if (this.temperatura > 21 && this.temperatura <= 30) {
                        return 'fa-thermometer-three-quarters';
                    } else {
                        return 'fa-thermometer-full';
                    }
                }
            }
        });
    </script>
</body>

</html>