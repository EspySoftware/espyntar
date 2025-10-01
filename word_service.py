#!/usr/bin/env python3
"""
Servicio de Python para generar palabras dinámicas usando ChatGPT API
para el juego Espyntar
"""

import json
import logging
import socket
import threading
import time
from typing import List, Dict, Any
import openai
from openai import OpenAI
import os
from dotenv import load_dotenv

# Cargar variables de entorno del archivo .env
load_dotenv()

# Configurar logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class WordGeneratorService:
    """Servicio para generar palabras temáticas usando ChatGPT"""
    
    def __init__(self, api_key: str = None):
        """
        Inicializar el servicio de generación de palabras
        
        Args:
            api_key: Clave de API de OpenAI. Si no se proporciona, se lee de la variable de entorno OPENAI_API_KEY
        """
        self.api_key = api_key or os.getenv('OPENAI_API_KEY')
        if not self.api_key:
            raise ValueError("Se requiere una clave de API de OpenAI. Configúrala en OPENAI_API_KEY o pásala como parámetro.")
        
        # Inicializar cliente de OpenAI
        self.client = OpenAI(api_key=self.api_key)
        
        # Cache para evitar llamadas repetidas con la misma temática
        self.word_cache: Dict[str, List[str]] = {}
        self.cache_max_age = 3600  # 1 hora en segundos
        self.cache_timestamps: Dict[str, float] = {}
        
        # Configuración del servidor
        self.server_socket = None
        self.is_running = False
        self.port = 8888
        
    def generate_words_for_theme(self, theme: str, count: int = 20) -> List[str]:
        """
        Generar palabras relacionadas con una temática específica usando ChatGPT
        
        Args:
            theme: Temática para generar palabras (ej: "animales", "deportes", "comida")
            count: Número de palabras a generar (por defecto 20)
            
        Returns:
            Lista de palabras generadas
        """
        # Verificar cache
        cache_key = f"{theme.lower()}_{count}"
        current_time = time.time()
        
        if (cache_key in self.word_cache and 
            cache_key in self.cache_timestamps and
            current_time - self.cache_timestamps[cache_key] < self.cache_max_age):
            logger.info(f"Devolviendo palabras desde cache para tema: {theme}")
            return self.word_cache[cache_key].copy()
        
        try:
            # Crear prompt para ChatGPT
            prompt = f"""
            Genera exactamente {count} palabras en español relacionadas con la temática: "{theme}".
            
            Requisitos:
            - Las palabras deben ser apropiadas para un juego de dibujar y adivinar
            - Deben ser sustantivos concretos que se puedan dibujar fácilmente
            - Evita palabras abstractas o muy complejas
            - Una sola palabra por línea
            - Sin números, artículos o explicaciones adicionales
            - Formato: una palabra por línea, sin numeración
            
            Ejemplos para "{theme}":
            """
            
            # Agregar algunos ejemplos específicos según la temática
            examples = self._get_theme_examples(theme.lower())
            if examples:
                prompt += "\n".join(examples)
            
            response = self.client.chat.completions.create(
                model="gpt-3.5-turbo",
                messages=[
                    {"role": "system", "content": "Eres un asistente especializado en generar palabras para juegos de dibujar y adivinar. Siempre respondes con palabras simples, concretas y dibujables."},
                    {"role": "user", "content": prompt}
                ],
                max_tokens=500,
                temperature=0.7
            )
            
            # Procesar la respuesta
            words_text = response.choices[0].message.content.strip()
            words = [word.strip().title() for word in words_text.split('\n') if word.strip()]
            
            # Filtrar palabras válidas (solo letras y espacios)
            valid_words = []
            for word in words:
                if word and len(word) > 2 and all(c.isalpha() or c.isspace() for c in word):
                    valid_words.append(word)
            
            # Asegurar que tenemos suficientes palabras
            if len(valid_words) < count // 2:  # Al menos la mitad de las palabras solicitadas
                logger.warning(f"Pocas palabras válidas generadas para {theme}: {len(valid_words)}")
                # Agregar palabras de respaldo si es necesario
                fallback_words = self._get_fallback_words(theme.lower())
                valid_words.extend(fallback_words[:count - len(valid_words)])
            
            # Limitar al número solicitado
            final_words = valid_words[:count]
            
            # Guardar en cache
            self.word_cache[cache_key] = final_words.copy()
            self.cache_timestamps[cache_key] = current_time
            
            logger.info(f"Generadas {len(final_words)} palabras para tema: {theme}")
            return final_words
            
        except Exception as e:
            logger.error(f"Error generando palabras para {theme}: {str(e)}")
            # Devolver palabras de respaldo en caso de error
            return self._get_fallback_words(theme.lower())
    
    def _get_theme_examples(self, theme: str) -> List[str]:
        """Obtener ejemplos específicos para cada temática"""
        examples_map = {
            'animales': ['Perro', 'Gato', 'Elefante', 'León'],
            'deportes': ['Fútbol', 'Baloncesto', 'Natación', 'Tenis'],
            'comida': ['Pizza', 'Hamburguesa', 'Helado', 'Tacos'],
            'naturaleza': ['Árbol', 'Flor', 'Montaña', 'Río'],
            'transporte': ['Coche', 'Avión', 'Bicicleta', 'Barco'],
            'profesiones': ['Doctor', 'Maestro', 'Bombero', 'Policía'],
            'hogar': ['Mesa', 'Silla', 'Cama', 'Sofá'],
            'tecnología': ['Computadora', 'Teléfono', 'Robot', 'Televisión'],
            'música': ['Guitarra', 'Piano', 'Tambor', 'Micrófono'],
            'ropa': ['Camisa', 'Pantalón', 'Zapatos', 'Sombrero']
        }
        
        # Buscar coincidencias parciales
        for key, examples in examples_map.items():
            if key in theme or theme in key:
                return examples
                
        return []
    
    def _get_fallback_words(self, theme: str) -> List[str]:
        """Obtener palabras de respaldo cuando falla la API"""
        fallback_map = {
            'animales': ['Perro', 'Gato', 'León', 'Tigre', 'Elefante', 'Jirafa', 'Caballo', 'Vaca', 'Pollo', 'Pez', 'Pájaro', 'Serpiente', 'Rana', 'Mariposa', 'Abeja'],
            'deportes': ['Fútbol', 'Baloncesto', 'Tenis', 'Natación', 'Correr', 'Saltar', 'Boxeo', 'Golf', 'Béisbol', 'Voleibol', 'Ping Pong', 'Patinar', 'Esquí', 'Surf'],
            'comida': ['Pizza', 'Hamburguesa', 'Helado', 'Pastel', 'Manzana', 'Plátano', 'Pan', 'Queso', 'Huevo', 'Pollo', 'Pescado', 'Ensalada', 'Sopa', 'Taco'],
            'naturaleza': ['Árbol', 'Flor', 'Sol', 'Luna', 'Estrella', 'Río', 'Montaña', 'Océano', 'Playa', 'Bosque', 'Nube', 'Lluvia', 'Viento', 'Fuego'],
            'default': ['Casa', 'Coche', 'Mesa', 'Silla', 'Libro', 'Lápiz', 'Reloj', 'Teléfono', 'Computadora', 'Televisión', 'Guitarra', 'Pelota', 'Corona', 'Corazón']
        }
        
        # Buscar coincidencias parciales
        for key, words in fallback_map.items():
            if key in theme or theme in key:
                return words
                
        return fallback_map['default']
    
    def start_server(self, host: str = 'localhost', port: int = 8888):
        """
        Iniciar el servidor TCP para recibir solicitudes de palabras
        
        Args:
            host: Dirección del servidor
            port: Puerto del servidor
        """
        self.port = port
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        try:
            self.server_socket.bind((host, port))
            self.server_socket.listen(5)
            self.is_running = True
            
            logger.info(f"Servidor de palabras iniciado en {host}:{port}")
            
            while self.is_running:
                try:
                    client_socket, address = self.server_socket.accept()
                    logger.info(f"Nueva conexión desde {address}")
                    
                    # Manejar cliente en un hilo separado
                    client_thread = threading.Thread(
                        target=self._handle_client,
                        args=(client_socket, address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except socket.error as e:
                    if self.is_running:
                        logger.error(f"Error aceptando conexión: {e}")
                        
        except Exception as e:
            logger.error(f"Error iniciando servidor: {e}")
        finally:
            self._cleanup()
    
    def _handle_client(self, client_socket: socket.socket, address: tuple):
        """
        Manejar solicitudes de un cliente específico
        
        Args:
            client_socket: Socket del cliente
            address: Dirección del cliente
        """
        try:
            while True:
                # Recibir datos del cliente
                data = client_socket.recv(1024).decode('utf-8').strip()
                if not data:
                    break
                
                logger.info(f"Solicitud recibida de {address}: {data}")
                
                try:
                    # Parsear solicitud JSON
                    request = json.loads(data)
                    command = request.get('command', '').upper()
                    
                    if command == 'GET_WORDS':
                        theme = request.get('theme', 'general')
                        count = request.get('count', 20)
                        
                        # Generar palabras
                        words = self.generate_words_for_theme(theme, count)
                        
                        # Enviar respuesta
                        response = {
                            'status': 'success',
                            'theme': theme,
                            'words': words,
                            'count': len(words)
                        }
                        
                    elif command == 'PING':
                        response = {'status': 'success', 'message': 'pong'}
                        
                    else:
                        response = {'status': 'error', 'message': f'Comando desconocido: {command}'}
                    
                except json.JSONDecodeError:
                    response = {'status': 'error', 'message': 'Formato JSON inválido'}
                except Exception as e:
                    response = {'status': 'error', 'message': str(e)}
                
                # Enviar respuesta
                response_json = json.dumps(response, ensure_ascii=False) + '\n'
                client_socket.send(response_json.encode('utf-8'))
                
        except Exception as e:
            logger.error(f"Error manejando cliente {address}: {e}")
        finally:
            client_socket.close()
            logger.info(f"Conexión cerrada con {address}")
    
    def stop_server(self):
        """Detener el servidor"""
        self.is_running = False
        if self.server_socket:
            self.server_socket.close()
        logger.info("Servidor detenido")
    
    def _cleanup(self):
        """Limpiar recursos"""
        if self.server_socket:
            self.server_socket.close()
        self.is_running = False


def main():
    """Función principal para ejecutar el servicio"""
    print("""
    ╔══════════════════════════════════════════════════════════════╗
    ║             Servicio de Palabras para Espyntar              ║
    ║                   Powered by ChatGPT                        ║
    ╚══════════════════════════════════════════════════════════════╝
    """)
    
    try:
        # Crear servicio
        service = WordGeneratorService()
        
        # Iniciar servidor
        print("Iniciando servidor...")
        print("Presiona Ctrl+C para detener el servidor")
        
        service.start_server()
        
    except KeyboardInterrupt:
        print("\nDeteniendo servidor...")
        if 'service' in locals():
            service.stop_server()
    except Exception as e:
        logger.error(f"Error fatal: {e}")
        print(f"Error: {e}")


if __name__ == "__main__":
    main()