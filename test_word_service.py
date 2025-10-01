#!/usr/bin/env python3
"""
Cliente de prueba para el servicio de palabras de Espyntar
"""

import json
import socket
import sys


def test_word_service(host='localhost', port=8888):
    """Probar el servicio de palabras"""
    
    print(f"Conectando a {host}:{port}...")
    
    try:
        # Crear socket
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((host, port))
        
        print("✅ Conectado al servicio de palabras")
        
        # Probar diferentes temáticas
        themes = [
            'animales',
            'deportes', 
            'comida',
            'naturaleza',
            'transporte',
            'profesiones'
        ]
        
        for theme in themes:
            print(f"\n🎯 Probando temática: {theme}")
            
            # Crear solicitud
            request = {
                'command': 'GET_WORDS',
                'theme': theme,
                'count': 10
            }
            
            # Enviar solicitud
            request_json = json.dumps(request) + '\n'
            client_socket.send(request_json.encode('utf-8'))
            
            # Recibir respuesta
            response_data = client_socket.recv(4096).decode('utf-8').strip()
            response = json.loads(response_data)
            
            if response['status'] == 'success':
                print(f"✅ Recibidas {response['count']} palabras:")
                for i, word in enumerate(response['words'], 1):
                    print(f"   {i:2d}. {word}")
            else:
                print(f"❌ Error: {response.get('message', 'Error desconocido')}")
        
        # Probar comando PING
        print(f"\n🏓 Probando comando PING...")
        ping_request = {'command': 'PING'}
        request_json = json.dumps(ping_request) + '\n'
        client_socket.send(request_json.encode('utf-8'))
        
        response_data = client_socket.recv(1024).decode('utf-8').strip()
        response = json.loads(response_data)
        
        if response['status'] == 'success':
            print(f"✅ PING exitoso: {response['message']}")
        else:
            print(f"❌ PING falló: {response.get('message', 'Error desconocido')}")
            
        client_socket.close()
        print("\n🎉 Todas las pruebas completadas")
        
    except ConnectionRefusedError:
        print("❌ No se pudo conectar al servicio. ¿Está ejecutándose?")
        print("   Ejecuta: python word_service.py")
    except Exception as e:
        print(f"❌ Error: {e}")


if __name__ == "__main__":
    if len(sys.argv) > 2:
        host = sys.argv[1]
        port = int(sys.argv[2])
        test_word_service(host, port)
    else:
        test_word_service()