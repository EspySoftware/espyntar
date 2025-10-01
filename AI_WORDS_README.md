# 🎨 Sistema de Palabras Dinámicas con IA - Espyntar

Esta implementación agrega un sistema de generación dinámica de palabras usando la IA de ChatGPT al juego Espyntar, permitiendo generar palabras temáticas en tiempo real.

## 🚀 Características Nuevas

- **🤖 Generación de Palabras con IA**: Utiliza la API de ChatGPT para generar palabras relacionadas con temáticas específicas
- **🎯 Selección de Temáticas**: Interfaz gráfica para elegir entre diferentes temáticas (animales, deportes, comida, etc.)
- **🔄 Sistema de Respaldo**: Si el servicio IA no está disponible, automáticamente usa la lista estática de palabras
- **💾 Cache Inteligente**: Las palabras generadas se almacenan temporalmente para evitar llamadas repetidas
- **🌐 Arquitectura Cliente-Servidor**: El servicio Python funciona independientemente del juego principal

## 📋 Requisitos

### Software Necesario

- **Python 3.8+** con pip instalado
- **Cuenta de OpenAI** con API key activa
- **MinGW/GCC** para compilar el juego C++
- **Raylib** (ya incluido en el proyecto)

### Dependencias Python

```bash
pip install -r requirements.txt
```

## ⚙️ Configuración

### 1. Configurar API Key de OpenAI

1. Obtén tu API key en [OpenAI Platform](https://platform.openai.com/api-keys)
2. Edita el archivo `.env`:

```env
OPENAI_API_KEY=tu_clave_de_api_real_aqui
WORD_SERVICE_HOST=localhost
WORD_SERVICE_PORT=8888
```

### 2. Iniciar el Servicio de Palabras

**Opción A: Usando el script automático (Recomendado)**

```bash
start_word_service.bat
```

**Opción B: Manual**

```bash
python word_service.py
```

El servicio se ejecutará en `localhost:8888` y mostrará:

```
╔══════════════════════════════════════════════════════════════╗
║             Servicio de Palabras para Espyntar              ║
║                   Powered by ChatGPT                        ║
╚══════════════════════════════════════════════════════════════╝

Servidor de palabras iniciado en localhost:8888
```

### 3. Compilar y Ejecutar el Juego

```bash
# Compilar (desde el directorio raíz)
mingw32-make RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=main OBJS=main.cpp

# Ejecutar
./main.exe
```

## 🎮 Cómo Usar

### 1. Selección de Temática

1. En la pantalla de inicio, haz clic en **"TEMÁTICA"**
2. Aparecerá un selector con opciones como:

   - **General** (palabras variadas)
   - **Animales** (perro, gato, león, etc.)
   - **Deportes** (fútbol, natación, tenis, etc.)
   - **Comida** (pizza, hamburguesa, helado, etc.)
   - **Naturaleza** (árbol, flor, montaña, etc.)
   - **Y más...**

3. Selecciona tu temática favorita y confirma
4. La temática seleccionada aparecerá en pantalla: "Tema: Animales"

### 2. Jugar con Palabras IA

1. Conecta al servidor como siempre
2. Cuando sea tu turno de dibujar, el juego automáticamente:
   - Solicita palabras de la temática seleccionada al servicio IA
   - Muestra 3 opciones generadas dinámicamente
   - Si el servicio no responde, usa palabras de respaldo

### 3. Cambiar Temáticas

- Puedes cambiar la temática en cualquier momento desde la pantalla de inicio
- Los cambios se aplicarán en la siguiente ronda del juego

## 🔧 Arquitectura del Sistema

```
┌─────────────────────┐    ┌─────────────────────┐    ┌─────────────────────┐
│   Juego Espyntar    │    │  Servicio Python    │    │    API ChatGPT     │
│      (C++)          │◄──►│   (word_service)    │◄──►│     (OpenAI)       │
└─────────────────────┘    └─────────────────────┘    └─────────────────────┘
          │                          │                          │
    ┌─────▼─────┐              ┌─────▼─────┐              ┌─────▼─────┐
    │ Interfaz  │              │   Cache   │              │ Generación│
    │ Temáticas │              │ Palabras  │              │ Inteligente│
    └───────────┘              └───────────┘              └───────────┘
```

### Componentes Principales

#### Lado C++ (Juego)

- **`WordServiceClient`**: Cliente TCP para comunicarse con el servicio Python
- **`ThemeSelector`**: Interfaz gráfica para selección de temáticas
- **`GameConfig`**: Configuración global del juego (singleton)
- **`Games` (modificado)**: Integra el servicio de palabras IA

#### Lado Python (Servicio)

- **`WordGeneratorService`**: Servicio principal con integración ChatGPT
- **Sistema de Cache**: Almacena palabras generadas temporalmente
- **Servidor TCP**: Maneja múltiples conexiones simultáneas
- **Palabras de Respaldo**: Sistema de fallback por temática

## 🛠️ API del Servicio

### Solicitar Palabras

```json
{
  "command": "GET_WORDS",
  "theme": "animales",
  "count": 10
}
```

### Respuesta Exitosa

```json
{
  "status": "success",
  "theme": "animales",
  "words": ["Perro", "Gato", "León", "Tigre", "Elefante"],
  "count": 5
}
```

### Probar Conexión

```json
{
  "command": "PING"
}
```

## 🧪 Pruebas

### Probar el Servicio Python

```bash
# Usar script automático
test_service.bat

# O manual
python test_word_service.py
```

### Ejemplo de Salida de Prueba

```
🎯 Probando temática: animales
✅ Recibidas 10 palabras:
    1. Perro
    2. Gato
    3. León
    4. Tigre
    5. Elefante
    ...

🏓 Probando comando PING...
✅ PING exitoso: pong

🎉 Todas las pruebas completadas
```

## ⚠️ Solución de Problemas

### El servicio no inicia

```bash
# Verificar Python
python --version

# Instalar dependencias
pip install -r requirements.txt

# Verificar API key
echo %OPENAI_API_KEY%
```

### El juego no conecta al servicio

1. Verificar que el servicio Python esté ejecutándose
2. Verificar puerto 8888 disponible
3. Revisar firewall/antivirus

### Palabras no se generan correctamente

1. Verificar API key de OpenAI válida
2. Verificar conexión a internet
3. Revisar logs del servicio Python

### Error de compilación C++

```bash
# Verificar que todos los archivos estén en su lugar
src/headers/WordServiceClient.h
src/components/WordServiceClient.cpp
src/headers/ThemeSelector.h
src/components/ThemeSelector.cpp
src/headers/GameConfig.h
```

## 📝 Logs y Debugging

### Logs del Servicio Python

El servicio muestra información detallada:

```
2024-01-15 10:30:15 - INFO - Servidor de palabras iniciado en localhost:8888
2024-01-15 10:30:25 - INFO - Nueva conexión desde ('127.0.0.1', 52341)
2024-01-15 10:30:25 - INFO - Solicitud recibida: {"command":"GET_WORDS","theme":"animales","count":10}
2024-01-15 10:30:27 - INFO - Generadas 10 palabras para tema: animales
```

### Debugging en C++

El juego muestra mensajes de estado:

```
Connected to word service at localhost:8888
Using AI-generated words for theme: Animales
Selected theme: Deportes
```

## 🔮 Futuras Mejoras

- [ ] **Configuración de Dificultad**: Palabras fáciles, medias o difíciles
- [ ] **Temáticas Personalizadas**: Permitir al usuario crear sus propias temáticas
- [ ] **Múltiples Idiomas**: Soporte para generar palabras en otros idiomas
- [ ] **Análisis de Partidas**: Estadísticas de palabras más/menos adivinadas
- [ ] **IA Local**: Opción de usar modelos locales como Ollama
- [ ] **Interfaz Web**: Panel de administración web para el servicio

## 🤝 Contribuir

1. Fork el repositorio
2. Crea una rama para tu feature: `git checkout -b feature/nueva-funcionalidad`
3. Commit tus cambios: `git commit -am 'Add nueva funcionalidad'`
4. Push a la rama: `git push origin feature/nueva-funcionalidad`
5. Crea un Pull Request

## 📜 Licencia

Este proyecto mantiene la misma licencia que Espyntar original.

## 📞 Soporte

Para reportar problemas o sugerir mejoras:

1. Abrir un issue en GitHub
2. Incluir logs relevantes
3. Describir pasos para reproducir el problema

---

**¡Disfruta jugando Espyntar con palabras generadas por IA! 🎨🤖**
