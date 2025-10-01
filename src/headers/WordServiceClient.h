#pragma once
#include <string>
#include <vector>
#include <map>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::stringstream;
using std::vector;

/**
 * Cliente para comunicarse con el servicio de Python que genera palabras usando IA
 */
class WordServiceClient
{
private:
    string host;
    int port;
    SOCKET clientSocket;
    bool connected;

public:
    WordServiceClient(string host = "localhost", int port = 8888);
    ~WordServiceClient();

    // Conexión al servicio
    bool Connect();
    void Disconnect();
    bool IsConnected() const { return connected; }

    // Obtener palabras por temática
    vector<string> GetWordsForTheme(const string &theme, int count = 20);

    // Probar la conexión
    bool Ping();

private:
    // Enviar datos JSON al servicio
    string SendRequest(const string &jsonRequest);

    // Parsear respuesta JSON simple (implementación básica)
    map<string, string> ParseJsonResponse(const string &jsonResponse);
    vector<string> ParseJsonArray(const string &jsonArray);
};